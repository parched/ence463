/**
 * \file wus_simulate_task.c
 * \brief Wheel unit simulator task.
 * \author James Duley
 * \version 1.0
 * \date 2014-08-21
 */

/* Copyright (C) 
 * 2014 - James Duley
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

#include "wus_simulate_task.h"

#include "FreeRTOS.h"
#include "task.h"

#include <ustdlib.h>

#include "wus_pulse_out.h"
#include "shared_pwm.h"
#include "shared_adc.h"
#include "shared_uart_task.h"
#include "shared_parameters.h"
#include "shared_iqmath.h"
#include "shared_tracenode.h"

#include "shared_errors.h"

#define TICK_RATE_HZ ((long)configTICK_RATE_HZ)   /**< The signed tick rate. */
#define SIMULATE_TASK_RATE_HZ 2000     /**< Task rate, it is borderline stable at 1384Hz. */

#define ROAD_RESTORING_FACTOR 200      /**< Road neutral restoring factor. */
#define ROAD_DAMPING_FACTOR 50          /**< Road damping factor. */

#define NUM_STEPS 3

static int roadType = 0;
static int roadAmplitude = 0;
static _iq dampingFactor = 0;          /**< The damping factor (N.s/mm). */
static _iq force = 0;                  /**< The actuator force (N). */
static _iq throttle = 0;               /**< The throttle acceleration (m/s/s). */
static _iq speed = 0;                  /**< The car speed (m/s). */
static _iq sprungAcc[NUM_STEPS] = {0};              /**< The sprung mass acceleration (m/s/s). */
static _iq unsprungAcc[NUM_STEPS] = {0};            /**< The unsprung mass acceleration (m/s/s). */
static _iq coilExtension = 0;          /**< The coil extension (mm). */
static char wusStatusEcho = 0;         /**< The status the needs to be echoed. */

static CircularBufferHandler *roadBuffer; /**< The road buffer for writing the road to. */

/* simulation states */
static _iq zR[NUM_STEPS] = {0};                     /**< The road displacement (mm). */
static _iq zU[NUM_STEPS] = {0};                     /**< The unsprung mass displacement (mm). */
static _iq zS[NUM_STEPS] = {0};                     /**< The sprung mass dispalcement (mm). */
static _iq vR[NUM_STEPS] = {0};                     /**< The road velocity (mm/s). */
static _iq vU[NUM_STEPS] = {0};                     /**< The unsprung mass velocity (mm/s). */
static _iq vS[NUM_STEPS] = {0};                     /**< The sprung mass velocity (mm/s). */

static _iq halfRoadWavelength = _IQ(100);
static int timeFromLastNoise = 0;      /**< The time since the last noise injection (ticks). */
static _iq aR[NUM_STEPS] = {0};                     /**< The road acceleration (m/s/s). */
static _iq aRNoise = 0;                /**< The road acceleration noise (m/s/s). */

static char combinedError = 0; /**<current error status */
static int startStatus = 0;

static unsigned int i = 0;
static unsigned int j = 1;
static unsigned int k = 2;

/**
 * \brief Resets the simulation.
 */
static void resetSimulation();

/**
 * \brief Converts Road Type to Amplitude and Half-Wavelength
 */
 static void decodeRoadType();

/**
 * \brief Simulates and updates the state.
 *
 * \param dTime The time since the last state.
 */
static void simulate(int dTime);

/**
 * \brief Reads the throttle from a message.
 *
 * \param msg The message to read.
 *
 * \return The throttle.
 */
static _iq getThrottle(char *msg);

/**
 * \brief Generates a psuedo random number.
 *
 * \return A random number between 0.0 and 1.0 fixed point..
 */
static _iq getRandom();

/**
 * \brief Sets the simulation on the bump stops.
 */
static void putSimOnStops();

/**
 * \brief Reads an incoming UART message.
 *
 * \param uartFrame Pointer to the uartFrame to read.
 */
static void readMessage(UartFrame* uartFrame) {
	switch (uartFrame->frameWise.msgType) {
		case 'R':
			uartFrame->frameWise.msg[2] = '\0';
			roadType = (int) ustrtoul(uartFrame->frameWise.msg, NULL, 10);
			decodeRoadType();
			break;
		case 'S':
			resetSimulation();
			break;
		case 'A':
			throttle = getThrottle(uartFrame->frameWise.msg);
			break;
		case 'M':
			wusStatusEcho = uartFrame->frameWise.msg[0];
			break;
	}
}

/**
 * \brief Checks all error shated and sends status to ASC
 *
 */
void updateStatus() {
	UartFrame errorStatusSend;

	//max speed error check
	if(speed >= MAX_SPEED ) {
		combinedError |= CAR_SPEED_EXCEEDED;
	} else {
		combinedError &= ~CAR_SPEED_EXCEEDED;
	}
	//max sprung acceration check
	if(sprungAcc[k] > MAX_ACC_SPRUNG || sprungAcc[k] < MIN_ACC_SPRUNG) {
		combinedError |= ACC_SPRUNG_EXCEEDED;
	} else {
		combinedError &= ~ACC_SPRUNG_EXCEEDED;
	}
	//max unsprung acceration check
	if(unsprungAcc[k] > MAX_ACC_UNSPRUNG || unsprungAcc[k] < MIN_ACC_UNSPRUNG) {
		combinedError |= ACC_UNSPRUNG_EXCEEDED;
	} else {
		combinedError &= ~ACC_UNSPRUNG_EXCEEDED;
	}

	errorStatusSend.frameWise.msgType = 'W';
	errorStatusSend.frameWise.msg[0] = combinedError | wusStatusEcho;
	queueMsgToSend(&errorStatusSend);
}

void vSimulateTask(void *params) {
	initPulseOut();
	initAdcModule(ACTUATOR_FORCE_ADC | DAMPING_COEFF_ADC);
	initPwmModule(ACC_SPRUNG_PWM | ACC_UNSPRUNG_PWM | COIL_EXTENSION_PWM);
	attachOnReceiveCallback(readMessage);

	// initialize FreeRTOS sleep parameters
	portTickType pxPreviousWakeTime;
	const portTickType xTimeIncrement = configTICK_RATE_HZ / SIMULATE_TASK_RATE_HZ;
	pxPreviousWakeTime = xTaskGetTickCount();

	int distanceTravelled = 0;

	for (;;) {
		vTaskDelayUntil( &pxPreviousWakeTime, xTimeIncrement);

		force = getSmoothAdc(ACTUATOR_FORCE_ADC, MIN_ACTUATOR_FORCE, MAX_ACTUATOR_FORCE);
		dampingFactor = getSmoothAdc(DAMPING_COEFF_ADC, MIN_DAMPING_COEFF, MAX_DAMPING_COEFF);

		simulate(xTimeIncrement);

		/* TODO: base this increment on speed */
		distanceTravelled += 100;

		setPulseSpeed(speed);
		setDuty(ACC_SPRUNG_PWM, sprungAcc[k] , MIN_ACC_SPRUNG,MAX_ACC_SPRUNG);
		setDuty(ACC_UNSPRUNG_PWM, unsprungAcc[k] , MIN_ACC_UNSPRUNG,MAX_ACC_UNSPRUNG);
		setDuty(COIL_EXTENSION_PWM, coilExtension , MIN_COIL_EXTENSION,MAX_COIL_EXTENSION);

		circularBufferWrite(roadBuffer, distanceTravelled, _IQint(zR[k]));

		updateStatus();
	}
}

void decodeRoadType(){
	switch (roadType){
		case 10:
			roadAmplitude = 5;
			halfRoadWavelength = _IQ(0.05);
			break;
		
		case 11:
			roadAmplitude = 10;
			halfRoadWavelength = _IQ(0.07);
			break;

		case 12:
			roadAmplitude = 15;
			halfRoadWavelength = _IQ(0.09);
			break;

		case 13:
			roadAmplitude = 20;
			halfRoadWavelength = _IQ(0.10);
			break;

		case 20:
			roadAmplitude = 25;
			halfRoadWavelength = _IQ(0.11);
			break;

		case 21:
			roadAmplitude = 50;
			halfRoadWavelength = _IQ(0.16);
			break;

		case 22:
			roadAmplitude = 75;
			halfRoadWavelength = _IQ(0.19);
			break;

		case 23:
			roadAmplitude = 100;
			halfRoadWavelength = _IQ(0.22);
			break;

		case 30:
			roadAmplitude = 150;
			halfRoadWavelength = _IQ(0.27);
			break;

		case 31:
			roadAmplitude = 200;
			halfRoadWavelength = _IQ(0.32);			
			break;

		case 32:
			roadAmplitude = 250;
			halfRoadWavelength = _IQ(0.35);
			break;

		case 33:
			roadAmplitude = 300;
			halfRoadWavelength = _IQ(0.39);
			break;
		
		default:
			roadAmplitude = 0;
			halfRoadWavelength = _IQ(100);
			break;
	}
}

int getDisplaySpeed() {
	return _IQint(speed * 36 / 10);
}

int getDisplaySprungAcc() {
	return _IQint(sprungAcc[k]);
}

int getDisplayUnsprungAcc() {
	return _IQint(unsprungAcc[k]);
}

int getDisplayCoilExtension() {
	return _IQint(coilExtension);
}

void setRoadBuffer(CircularBufferHandler *buffer) {
	roadBuffer = buffer;
}

void resetSimulation() {
	speed = 0;
	int n = 0;
	for (n = 0; n < NUM_STEPS; n++) {
		zR[n] = 0;
		zU[n] = 0;
		zS[n] = 0;
		vR[n] = 0;
		vU[n] = 0;
		vS[n] = 0;
		sprungAcc[n] = 0;
		unsprungAcc[n] = 0;
	}
}

void simulate(int dTime) {
	if (speed == 0) {
		aRNoise = 0;
	} else {
		int noisePeroid = TICK_RATE_HZ * halfRoadWavelength / speed;

		if (timeFromLastNoise >= noisePeroid) {
			timeFromLastNoise -= noisePeroid;
			aRNoise = (getRandom() + getRandom() - getRandom() - getRandom()) * roadAmplitude / 4;
		}
		timeFromLastNoise += dTime;
	}

	aR[i] = aRNoise - zR[k] / ROAD_RESTORING_FACTOR -  vR[k] / ROAD_DAMPING_FACTOR;

	_iq suspensionSpringForce = STIFFNESS_SPRING * (zU[k] - zS[k]);
	_iq suspensionDampingForce = _IQmpy(dampingFactor, (vU[k] - vS[k]));
	_iq suspensionForce = suspensionSpringForce + suspensionDampingForce;

	_iq tyreSpringForce = STIFFNESS_TYRE * (zR[k] - zU[k]);
	_iq tyreDampingForce = DAMPING_TYRE * (vR[k] - vU[k]);
	_iq tyreForce = tyreSpringForce + tyreDampingForce;

	_iq sprungForce = suspensionForce + force;
	_iq unsprungForce = tyreForce - suspensionForce - force;

	sprungAcc[i] = ON_MASS_SPRUNG(sprungForce);
	unsprungAcc[i] = ON_MASS_UNSPRUNG(unsprungForce);

	/* Check if on the bump stops */
	if (combinedError & COIL_EXTENSION_EXCEEDED) {
		if ((coilExtension == MAX_COIL_EXTENSION && sprungAcc[i] < unsprungAcc[i])
				|| (coilExtension == MIN_COIL_EXTENSION && sprungAcc[i] > unsprungAcc[i])) {
			/* We are coming off the bump stops */
			combinedError &= ~COIL_EXTENSION_EXCEEDED;
		} else { /* Both masses move as one unit */
			unsprungAcc[i] = ON_MASS_TOTAL(tyreForce);
			sprungAcc[i] = unsprungAcc[i];
		}
	}

	zR[i] += (23 * vR[k] - 16 * vR[j] + 5 * vR[i]) * dTime / (12 * TICK_RATE_HZ);
	zS[i] += (23 * vS[k] - 16 * vS[j] + 5 * vS[i]) * dTime / (12 * TICK_RATE_HZ);
	zU[i] += (23 * vU[k] - 16 * vU[j] + 5 * vU[i]) * dTime / (12 * TICK_RATE_HZ);
	vR[i] += (23 * aR[j] - 16 * aR[k] + 5 * aR[i]) * dTime / (12 * TICK_RATE_HZ / 1000);
	vU[i] += (23 * unsprungAcc[i] - 16 * unsprungAcc[k] + 5 * unsprungAcc[j]) * dTime / (12 * TICK_RATE_HZ / 1000);
	vS[i] += (23 * sprungAcc[i] - 16 * sprungAcc[k] + 5 * sprungAcc[j]) * dTime / (12 * TICK_RATE_HZ / 1000);

	speed += throttle * dTime / TICK_RATE_HZ;

	if (speed < MIN_SPEED) {
		speed = MIN_SPEED;
	} else if (speed > MAX_SPEED) {
		speed = MAX_SPEED;
	}

	coilExtension = zU[i] - zS[i];
	
	// max coil extension check
	if (coilExtension > MAX_COIL_EXTENSION) {
		coilExtension = MAX_COIL_EXTENSION;
		zS[i] = zU[i] + MAX_COIL_EXTENSION;
		putSimOnStops();
	} else if (coilExtension < MIN_COIL_EXTENSION) {
		coilExtension = MIN_COIL_EXTENSION;
		zS[i] = zU[i] + MIN_COIL_EXTENSION;
		putSimOnStops();
	}

	if (++i == NUM_STEPS) i = 0;
	if (++j == NUM_STEPS) j = 0;
	if (++k == NUM_STEPS) k = 0;
}

void putSimOnStops() {
	combinedError |= COIL_EXTENSION_EXCEEDED;
	vU[i] = WEIGHT_BY_MASSES(vS[i],vU[i]);
	vS[i] = vU[i];
}

_iq getThrottle(char *msg) {
	int throttleInt = 1000*(msg[1]-'0') + 100*(msg[3] - '0') + 10*(msg[4] - '0') + (msg[5] - '0');
	if (msg[0] == '-') {
		// negative case
		throttleInt = -throttleInt;
	} else if (msg[0] >= '1' && msg[0] <= '9') {
		// 10 case
		throttleInt = 10000*(msg[0]-'0');
	}

	return _IQ(throttleInt)/1000;
}

_iq getRandom() {
	static unsigned long b = 12903;

	b = 18000 * (b & 65535) + (b >> 16);

	return b & ((1 << QG) - 1);
}

int getStartStatusDisplay() {
	return startStatus;
}

int getRoadTypeStatusDisplay() {
	return roadType;
}

int getThrottleStatusDisplay() {
	return _IQint(throttle);
}


int getCoilExErrorInvoked() {
	if(wusStatusEcho &  COIL_EXTENSION_EXCEEDED) {
		return 1;
	} else {
		return 0;
	}
}

int getUnsprungAccErrorInvoked() {
	if(wusStatusEcho & ACC_UNSPRUNG_EXCEEDED) {
		return 1;
	} else {
		return 0;
	}
}

int getSprungAccErrorInvoked() {
	if(wusStatusEcho & ACC_SPRUNG_EXCEEDED) {
		return 1;
	} else {
		return 0;
	}
}

int getCarSpeedErrorInvoked() {
	if(wusStatusEcho & CAR_SPEED_EXCEEDED) {
		return 1;
	} else {
		return 0;
	}
}

int getPowerFailureInvoked() {
	if(wusStatusEcho & POWER_FAILURE) {
		return 1;
	} else {
		return 0;
	}
}

int getWatchdogTimerFailureInvoked() {
	if(wusStatusEcho & WATCHDOG_TIMER) {
		return 1;
	} else {
		return 0;
	}
}
