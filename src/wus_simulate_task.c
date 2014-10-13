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

#include "shared_errors.h"

#define SIMULATE_TASK_RATE_HZ 1000

static int roadType = 0;
#define ROAD_RESTORING_FACTOR 1         /**< Road neutral restoring factor. */
#define ROAD_DAMPING_FACTOR 20          /**< Road damping factor. */

static int dampingFactor = 0;          /**< The damping factor (N.s/m). */
static int throttle = 0;               /**< The throttle acceleration (m/s/s). */
static int speed = 0;                  /**< The car speed (m/s). */
static int sprungAcc = 0;              /**< The sprung mass acceleration (m/s/s). */
static int unsprungAcc = 0;            /**< The unsprung mass acceleration (m/s/s). */
static int coilExtension = 0;          /**< The coil extension (mm). */
static int wusStatusEcho = 0;

/* simulation states */
static int zR = 0;                     /**< The road displacement (mm). */
static int zU = 0;                     /**< The unsprung mass displacement (mm). */
static int zS = 0;                     /**< The sprung mass dispalcement (mm). */
static int vR = 0;                     /**< The road velocity (m/s). */
static int vU = 0;                     /**< The unsprung mass velocity (m/s). */
static int vS = 0;                     /**< The sprung mass velocity (m/s). */

static int timeFromLastNoise = 0;      /**< The time since the last noise injection (ticks). */
static int aR = 0;                     /**< The road acceleration (m/s/s). */
static int aRNoise = 0;                /**< The road acceleration noise (m/s/s). */

static char combinedError = 0; /**<current error status */

/**
 * \brief Resets the simulation.
 */
static void resetSimulation();

/**
 * \brief Simulates and updates the state.
 *
 * \param force The applied force from the controller.
 * \param throttle The forward acceleration from the driver.
 * \param dampingFactor The set damping factor.
 * \param roadType The road type.
 * \param dTime The time since the last state.
 *
 * \return The error statuses of the car.
 */
static char simulate(int force, int throttle, int dampingFactor, char roadType, int dTime);

/**
 * \brief Convert throttle message to numeric value
 *
 * \param msg The throttle UART message
 */
int getThrottle(char *msg);

/**
 * \brief Generates a psuedo random number.
 *
 * \return A random number between 0.0 and 1.0 fixed point..
 */
static int getRandom();

/**
 * \brief Reads an incoming UART message.
 *
 * \param uartFrame Pointer to the uartFrame to read.
 */
static void readMessage(UartFrame uartFrame) {
	switch (uartFrame.frameWise.msgType) {
		case 'R':
			roadType = (int) ustrtoul(uartFrame.frameWise.msg, NULL, 10);
			break;
		case 'S':
			resetSimulation();
			break;
		case 'A':
			throttle = getfThrottle(uartFrame.frameWise.msg);
			break;
		case 'M':
			wusStatusEcho = (int) ustrtoul(uartFrame.frameWise.msg, NULL, 16);
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
	if(speed > MAX_SPEED || speed < MIN_SPEED) {
		combinedError = combinedError | CAR_SPEED_EXCEEDED;
	} else {
		combinedError = combinedError & ~CAR_SPEED_EXCEEDED;
	}
	//max sprung acceration check
	if(sprungAcc > MAX_ACC_SPRUNG || sprungAcc < MIN_ACC_SPRUNG) {
		combinedError = combinedError | ACC_SPRUNG_EXCEEDED;
	} else {
		combinedError = combinedError & ~ ACC_SPRUNG_EXCEEDED;
	}
	//max unsprung acceration check
	if(unsprungAcc > MAX_ACC_UNSPRUNG || unsprungAcc < MIN_ACC_UNSPRUNG) {
		combinedError = combinedError | ACC_UNSPRUNG_EXCEEDED;
	} else {
		combinedError = combinedError & ~ ACC_UNSPRUNG_EXCEEDED;
	}

	errorStatusSend.frameWise.msgType = 'W';
	errorStatusSend.frameWise.msg[0] = combinedError;
	queueMsgToSend(errorStatusSend);
}

void vSimulateTask(void *params) {
	int force = 0;
	char errorCode = 0;

	initPulseOut();
	initAdcModule(ACTUATOR_FORCE_ADC | DAMPING_COEFF_ADC);
	initPwmModule(ACC_SPRUNG_PWM | ACC_UNSPRUNG_PWM | COIL_EXTENSION_PWM);
	attachOnReceiveCallback(readMessage);

	// initialize FreeRTOS sleep parameters
	portTickType pxPreviousWakeTime;
	const portTickType xTimeIncrement = configTICK_RATE_HZ / SIMULATE_TASK_RATE_HZ;
	pxPreviousWakeTime = xTaskGetTickCount();

	for (;;) {
		vTaskDelayUntil( &pxPreviousWakeTime, xTimeIncrement);

		force = getSmoothAdc(ACTUATOR_FORCE_ADC, MIN_ACTUATOR_FORCE, MAX_ACTUATOR_FORCE);
		dampingFactor = getSmoothAdc(DAMPING_COEFF_ADC, MIN_DAMPING_COEFF, MAX_DAMPING_COEFF);

		/* TODO: find dTime */
		errorCode = simulate(force, throttle, dampingFactor, roadType, xTimeIncrement);

		setPulseSpeed(speed);
		setDuty(ACC_SPRUNG_PWM, sprungAcc,MIN_ACC_SPRUNG,MAX_ACC_SPRUNG);
		setDuty(ACC_UNSPRUNG_PWM, unsprungAcc,MIN_ACC_UNSPRUNG,MAX_ACC_UNSPRUNG);
		setDuty(COIL_EXTENSION_PWM, coilExtension,MIN_COIL_EXTENSION,MAX_COIL_EXTENSION);

		if (errorCode != 0) {
			/* TODO */
		}
		updateStatus();
	}
}

int getDisplaySpeed() {
	return speed FROM_FP;
}

int getDisplaySprungAcc() {
	return sprungAcc FROM_FP;
}

int getDisplayUnsprungAcc() {
	return unsprungAcc FROM_FP;
}

int getDisplayCoilExtension() {
	return coilExtension FROM_FP;
}


void resetSimulation() {
	speed = 0;
	zR = 0;
	zU = 0;
	zS = 0;
	vR = 0;
	vU = 0;
	vS = 0;
	sprungAcc = 0;
	unsprungAcc = 0;
}

char simulate(int force, int throttle, int dampingFactor, char roadType, int dTime) {
	/* TODO: set the amplitudeFactor according to roadType and halfRoadWavelength and ROAD_FACTORS. */

	//max coil extension check
	if(coilExtension > MAX_COIL_EXTENSION) {
		combinedError = combinedError | COIL_EXTENSION_EXCEEDED;
	} else {
		combinedError = combinedError & ~COIL_EXTENSION_EXCEEDED;
	}

	int amplitudeFactor = 100000;
	int halfRoadWavelength = 250;

	if (speed == 0) {
		aRNoise = 0;
	} else {
		int noisePeroid = configTICK_RATE_HZ * halfRoadWavelength / (speed FROM_FP);

		if (timeFromLastNoise >= noisePeroid) {
			timeFromLastNoise -= noisePeroid;
			aRNoise = (getRandom() + getRandom() - getRandom() - getRandom()) * amplitudeFactor;
		}
		timeFromLastNoise += dTime;
	}

	aR = aRNoise - ROAD_RESTORING_FACTOR * zR - ROAD_DAMPING_FACTOR * vR;

	sprungAcc = (- STIFFNESS_SPRING * (zS - zU) - dampingFactor * (vS - vU) + force ) ON_MASS_SPRUNG;
	unsprungAcc = ( STIFFNESS_SPRING * (zS - zU) + dampingFactor * (vS - vU) - STIFFNESS_TYRE * (zU - zR) - DAMPING_TYRE * (vU - vR) - force ) ON_MASS_UNSPRUNG;

	zR += vR * 1000 * dTime / configTICK_RATE_HZ;
	zU += vU * 1000 * dTime / configTICK_RATE_HZ;
	zS += vS * 1000 * dTime / configTICK_RATE_HZ;
	vR += aR * dTime / configTICK_RATE_HZ;
	vU += unsprungAcc * dTime / configTICK_RATE_HZ;
	vS += sprungAcc * dTime / configTICK_RATE_HZ;

	speed += throttle * dTime / configTICK_RATE_HZ;

	coilExtension = zU - zS;
	
	/* TODO: error check */
	return 0;
}

int getThrottle(char *msg) {
	char intThrottlePartString[2];
	char decThrottlePartString[3];
	ustrncpy(intThrottlePartString, msg, 2);
	ustrncpy(decThrottlePartString, &msg[3], 3);
	int throttleIntPart = (int) ustrtoul(intThrottlePartString, NULL, 10);
	int throttleDecPart = (int) ustrtoul(decThrottlePartString, NULL, 10);
	return (throttleIntPart TO_FP) + (throttleDecPart TO_FP)/1000;
}

int getRandom() {
	static unsigned int b = 12903;

	b = 18000 * (b & 65535) + (b >> 16);

	return b & ((1 TO_FP) - 1);
}
