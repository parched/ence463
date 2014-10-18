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

static int roadType = 0;
static int roadAmplitude = 0;
static _iq dampingFactor = 0;          /**< The damping factor (N.s/mm). */
static _iq force = 0;                  /**< The actuator force (N). */
static _iq throttle = 0;               /**< The throttle acceleration (m/s/s). */
static _iq speed = 0;                  /**< The car speed (m/s). */
static _iq sprungAcc = 0;              /**< The sprung mass acceleration (m/s/s). */
static _iq unsprungAcc = 0;            /**< The unsprung mass acceleration (m/s/s). */
static _iq coilExtension = 0;          /**< The coil extension (mm). */
static char wusStatusEcho = 0;         /**< The status the needs to be echoed. */

static CircularBufferHandler *roadBuffer; /**< The road buffer for writing the road to. */

/* simulation states */
static _iq zR = 0;                     /**< The road displacement (mm). */
static _iq zU = 0;                     /**< The unsprung mass displacement (mm). */
static _iq zS = 0;                     /**< The sprung mass dispalcement (mm). */
static _iq vR = 0;                     /**< The road velocity (mm/s). */
static _iq vU = 0;                     /**< The unsprung mass velocity (mm/s). */
static _iq vS = 0;                     /**< The sprung mass velocity (mm/s). */

static _iq halfRoadWavelength = _IQ(100);
static int timeFromLastNoise = 0;      /**< The time since the last noise injection (ticks). */
static _iq aR = 0;                     /**< The road acceleration (m/s/s). */
static _iq aRNoise = 0;                /**< The road acceleration noise (m/s/s). */

static char combinedError = 0; /**<current error status */
static int startStatus = 0;

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
static void readMessage(UartFrame *uartFrame)
{
	switch (uartFrame->frameWise.msgType)
	{
	case 'R':
		uartFrame->frameWise.msg[2] = '\0';
		roadType = (int)ustrtoul(uartFrame->frameWise.msg, NULL, 10);
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
void updateStatus()
{
	UartFrame errorStatusSend;

	//max speed error check
	if (speed >= MAX_SPEED)
	{
		combinedError |= CAR_SPEED_EXCEEDED;
	}
	else
	{
		combinedError &= ~CAR_SPEED_EXCEEDED;
	}
	//max sprung acceration check
	if (sprungAcc > MAX_ACC_SPRUNG || sprungAcc < MIN_ACC_SPRUNG)
	{
		combinedError |= ACC_SPRUNG_EXCEEDED;
	}
	else
	{
		combinedError &= ~ACC_SPRUNG_EXCEEDED;
	}
	//max unsprung acceration check
	if (unsprungAcc > MAX_ACC_UNSPRUNG || unsprungAcc < MIN_ACC_UNSPRUNG)
	{
		combinedError |= ACC_UNSPRUNG_EXCEEDED;
	}
	else
	{
		combinedError &= ~ACC_UNSPRUNG_EXCEEDED;
	}

	errorStatusSend.frameWise.msgType = 'W';
	errorStatusSend.frameWise.msg[0] = combinedError | wusStatusEcho;
	queueMsgToSend(&errorStatusSend);
}

void vSimulateTask(void *params)
{
	initPulseOut();
	initAdcModule(ACTUATOR_FORCE_ADC | DAMPING_COEFF_ADC);
	initPwmModule(ACC_SPRUNG_PWM | ACC_UNSPRUNG_PWM | COIL_EXTENSION_PWM);
	attachOnReceiveCallback(readMessage);

	// initialize FreeRTOS sleep parameters
	TickType_t pxPreviousWakeTime;
	const TickType_t xTimeIncrement = configTICK_RATE_HZ / SIMULATE_TASK_RATE_HZ;
	pxPreviousWakeTime = xTaskGetTickCount();

	int distanceTravelled = 0;

	for (;;)
	{
		vTaskDelayUntil( &pxPreviousWakeTime, xTimeIncrement);

		force = getSmoothAdc(ACTUATOR_FORCE_ADC, MIN_ACTUATOR_FORCE, MAX_ACTUATOR_FORCE);
		dampingFactor = getSmoothAdc(DAMPING_COEFF_ADC, MIN_DAMPING_COEFF, MAX_DAMPING_COEFF);

		simulate(xTimeIncrement);

		/* TODO: base this increment on speed */
		distanceTravelled += 100;

		setPulseSpeed(speed);
		setDuty(ACC_SPRUNG_PWM, sprungAcc, MIN_ACC_SPRUNG, MAX_ACC_SPRUNG);
		setDuty(ACC_UNSPRUNG_PWM, unsprungAcc, MIN_ACC_UNSPRUNG, MAX_ACC_UNSPRUNG);
		setDuty(COIL_EXTENSION_PWM, coilExtension, MIN_COIL_EXTENSION, MAX_COIL_EXTENSION);

		circularBufferWrite(roadBuffer, distanceTravelled, _IQint(zR));

		updateStatus();
	}
}

void decodeRoadType()
{
	switch (roadType)
	{
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

int getDisplaySpeed()
{
	return _IQint(speed * 36 / 10);
}

int getDisplaySprungAcc()
{
	return _IQint(sprungAcc);
}

int getDisplayUnsprungAcc()
{
	return _IQint(unsprungAcc);
}

int getDisplayCoilExtension()
{
	return _IQint(coilExtension);
}

void setRoadBuffer(CircularBufferHandler *buffer)
{
	roadBuffer = buffer;
}

void resetSimulation()
{
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

void simulate(int dTime)
{
	if (speed == 0)
	{
		aRNoise = 0;
	}
	else
	{
		int noisePeroid = TICK_RATE_HZ * halfRoadWavelength / speed;

		if (timeFromLastNoise >= noisePeroid)
		{
			timeFromLastNoise -= noisePeroid;
			aRNoise = (getRandom() + getRandom() - getRandom() - getRandom()) * roadAmplitude / 4;
		}
		timeFromLastNoise += dTime;
	}

	aR = aRNoise - zR / ROAD_RESTORING_FACTOR - vR / ROAD_DAMPING_FACTOR;

	_iq suspensionSpringForce = STIFFNESS_SPRING * (zU - zS);
	_iq suspensionDampingForce = _IQmpy(dampingFactor, (vU - vS));
	_iq suspensionForce = suspensionSpringForce + suspensionDampingForce;

	_iq tyreSpringForce = STIFFNESS_TYRE * (zR - zU);
	_iq tyreDampingForce = DAMPING_TYRE * (vR - vU);
	_iq tyreForce = tyreSpringForce + tyreDampingForce;

	_iq sprungForce = suspensionForce + force;
	_iq unsprungForce = tyreForce - suspensionForce - force;

	sprungAcc = ON_MASS_SPRUNG(sprungForce);
	unsprungAcc = ON_MASS_UNSPRUNG(unsprungForce);

	/* Check if on the bump stops */
	if (combinedError & COIL_EXTENSION_EXCEEDED)
	{
		if ((coilExtension == MAX_COIL_EXTENSION && sprungAcc < unsprungAcc)
		    || (coilExtension == MIN_COIL_EXTENSION && sprungAcc > unsprungAcc))
		{
			/* We are coming off the bump stops */
			combinedError &= ~COIL_EXTENSION_EXCEEDED;
		}
		else /* Both masses move as one unit */
		{
			unsprungAcc = ON_MASS_TOTAL(tyreForce);
			sprungAcc = unsprungAcc;
		}
	}

	zR += vR * dTime / TICK_RATE_HZ;
	zU += vU * dTime / TICK_RATE_HZ;
	zS += vS * dTime / TICK_RATE_HZ;
	vR += aR * dTime / (TICK_RATE_HZ / 1000);
	vU += unsprungAcc * dTime / (TICK_RATE_HZ / 1000);
	vS += sprungAcc * dTime / (TICK_RATE_HZ / 1000);

	speed += throttle * dTime / TICK_RATE_HZ;

	if (speed < MIN_SPEED)
	{
		speed = MIN_SPEED;
	}
	else if (speed > MAX_SPEED)
	{
		speed = MAX_SPEED;
	}

	coilExtension = zU - zS;

	// max coil extension check
	if (coilExtension > MAX_COIL_EXTENSION)
	{
		coilExtension = MAX_COIL_EXTENSION;
		zS = zU + MAX_COIL_EXTENSION;
		putSimOnStops();
	}
	else if (coilExtension < MIN_COIL_EXTENSION)
	{
		coilExtension = MIN_COIL_EXTENSION;
		zS = zU + MIN_COIL_EXTENSION;
		putSimOnStops();
	}
}

void putSimOnStops()
{
	combinedError |= COIL_EXTENSION_EXCEEDED;
	vU = WEIGHT_BY_MASSES(vS, vU);
	vS = vU;
}

_iq getThrottle(char *msg)
{
	int throttleInt = 1000 * (msg[1] - '0') + 100 * (msg[3] - '0') + 10 * (msg[4] - '0') + (msg[5] - '0');
	if (msg[0] == '-')
	{
		// negative case
		throttleInt = -throttleInt;
	}
	else if (msg[0] >= '1' && msg[0] <= '9')
	{
		// 10 case
		throttleInt = 10000 * (msg[0] - '0');
	}

	return _IQ(throttleInt) / 1000;
}

_iq getRandom()
{
	static unsigned long b = 12903;

	b = 18000 * (b & 65535) + (b >> 16);

	return b & ((1 << QG) - 1);
}

int getStartStatusDisplay()
{
	return startStatus;
}

int getRoadTypeStatusDisplay()
{
	return roadType;
}

int getThrottleStatusDisplay()
{
	return _IQint(throttle);
}


int getCoilExErrorInvoked()
{
	if (wusStatusEcho & COIL_EXTENSION_EXCEEDED)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int getUnsprungAccErrorInvoked()
{
	if (wusStatusEcho & ACC_UNSPRUNG_EXCEEDED)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int getSprungAccErrorInvoked()
{
	if (wusStatusEcho & ACC_SPRUNG_EXCEEDED)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int getCarSpeedErrorInvoked()
{
	if (wusStatusEcho & CAR_SPEED_EXCEEDED)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int getPowerFailureInvoked()
{
	if (wusStatusEcho & POWER_FAILURE)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int getWatchdogTimerFailureInvoked()
{
	if (wusStatusEcho & WATCHDOG_TIMER)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
