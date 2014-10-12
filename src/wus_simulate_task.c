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

#define SIMULATE_TASK_RATE_HZ 1000

#define ACC_SPRUNG_EXCEEDED 0x10        /**< Sprung acceleration limit exceeded error. */
#define ACC_UNSPRUNG_EXCEEDED 0x20      /**< Unsprung acceleration limit exceeded error. */
#define COIL_EXTENSION_EXCEEDED 0x40    /**< Coil extension limit exceeded error. */
#define CAR_SPEED_EXCEEDED 0x80         /**< Car speed limit exceeded error. */

#define ROAD_RESTORING_FACTOR 1         /**< Road neutral restoring factor. */
#define ROAD_DAMPING_FACTOR 20          /**< Road damping factor. */

static int roadType = 0;
static _iq dampingFactor = 0;          /**< The damping factor (N.s/m). */
static _iq throttle = 0;               /**< The throttle acceleration (m/s/s). */
static _iq speed = 0;                  /**< The car speed (m/s). */
static _iq sprungAcc = 0;              /**< The sprung mass acceleration (m/s/s). */
static _iq unsprungAcc = 0;            /**< The unsprung mass acceleration (m/s/s). */
static _iq coilExtension = 0;          /**< The coil extension (mm). */
static int wusStatusEcho = 0;

/* simulation states */
static _iq zR = 0;                     /**< The road displacement (mm). */
static _iq zU = 0;                     /**< The unsprung mass displacement (mm). */
static _iq zS = 0;                     /**< The sprung mass dispalcement (mm). */
static _iq vR = 0;                     /**< The road velocity (m/s). */
static _iq vU = 0;                     /**< The unsprung mass velocity (m/s). */
static _iq vS = 0;                     /**< The sprung mass velocity (m/s). */

static int timeFromLastNoise = 0;      /**< The time since the last noise injection (ticks). */
static _iq aR = 0;                     /**< The road acceleration (m/s/s). */
static _iq aRNoise = 0;                /**< The road acceleration noise (m/s/s). */

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
static char simulate(_iq force, _iq throttle, _iq dampingFactor, char roadType, int dTime);

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
			throttle = getThrottle(uartFrame.frameWise.msg);
			break;
		case 'M':
			wusStatusEcho = (int) ustrtoul(uartFrame.frameWise.msg, NULL, 16);
			break;
	}
}

void vSimulateTask(void *params) {
	_iq force = 0;
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
	}
}

int getDisplaySpeed() {
	return _IQint(speed);
}

int getDisplaySprungAcc() {
	return _IQint(sprungAcc);
}

int getDisplayUnsprungAcc() {
	return _IQint(unsprungAcc);
}

int getDisplayCoilExtension() {
	return _IQint(coilExtension);
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

char simulate(_iq force, _iq throttle, _iq dampingFactor, char roadType, int dTime) {
	/* TODO: set the amplitudeFactor according to roadType and halfRoadWavelength and ROAD_FACTORS. */
	int amplitudeFactor = 100000;
	int halfRoadWavelength = 250;

	if (speed == 0) {
		aRNoise = 0;
	} else {
		int noisePeroid = configTICK_RATE_HZ * halfRoadWavelength / _IQint(speed);

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

_iq getThrottle(char *msg) {
	char intThrottlePartString[3];
	char decThrottlePartString[4];
	ustrncpy(intThrottlePartString, msg, 2);
	ustrncpy(decThrottlePartString, &msg[3], 3);
	int throttleIntPart = (int) ustrtoul(intThrottlePartString, NULL, 10);
	int throttleDecPart = (int) ustrtoul(decThrottlePartString, NULL, 10);
	return _IQ(throttleIntPart) + _IQ(throttleDecPart )/1000;
}

_iq getRandom() {
	static unsigned long b = 12903;

	b = 18000 * (b & 65535) + (b >> 16);

	return b & ((1 << QG) - 1);
}
