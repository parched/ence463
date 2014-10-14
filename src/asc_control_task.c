/**
 * \file asc_control_task.c
 * \brief Active Suspension Controller task.
 * \author Tom Walsh
 * \version 1.0
 * \date 2014-10-07
 */

/* Copyright (C) 
 * 2014 - Tom Walsh
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

#include "asc_control_task.h"
#include "shared_parameters.h"

#include <ustdlib.h>

#include "FreeRTOS.h"
#include "task.h"

#include "asc_pulse_in.h"
#include "shared_pwm.h"
#include "shared_adc.h"
#include "shared_uart_task.h"
#include "shared_parameters.h"
#include "shared_iqmath.h"

#include "shared_errors.h"


#define CONTROL_TASK_RATE_HZ 1000

#define DAMPING_SEDATE 	100
#define DAMPING_NORMAL 	250
#define DAMPING_SPORT	350
#define DAMPING_RALLY	500

static int isOn;                      /**< Sets whether the active suspension is on or off. */
static rideType rideMode = SEDATE;
static _iq sprungAcc = 0;
static _iq unsprungAcc = 0;
static _iq coilExtension = 0;
static _iq speed = 0;
static _iq actuatorForce = 0;
static _iq dampingCoefficient = 0;

static char wusStatus = 0;
static int roadType = 0;
static _iq throttle = 0;
static int resetState = 0;
static char errorState = 0;

/**
 * \brief Reads an incoming UART message.
 *
 * \param uartFrame Pointer to the uartFrame to read.
 */
static void readMessage(UartFrame uartFrame) {
	switch (uartFrame.frameWise.msgType) {
		case 'W':
			wusStatus = uartFrame.frameWise.msg[0];
			break;
	}
}


/**
 * \brief Gets the damping coefficient.
 *
 * \return The damping coefficient.
 */
static _iq getDampingCoefficient();

/**
 * \brief Calculates the required actuator force.
 *
 * \param dTime The time in ticks since the last calculation.
 *
 * \return The actuator force.
 */
static _iq getControlForce(int dTime);

/**
 * \brief Formats and Transmits data over serial to simulator
 *
 */
static void sendSerialMessages()
{
	// Throttle Transmission
	UartFrame throttleSend;

	throttleSend.frameWise.msgType = 'A'; // Accel Message Type
	usprintf(throttleSend.frameWise.msg, "%2d.000", _IQint(throttle));
	queueMsgToSend(&throttleSend);

	// Road Type Transmission
	UartFrame roadSend;

	roadSend.frameWise.msgType = 'R';	// Road Message Type
	usprintf(roadSend.frameWise.msg, "%2d", roadType);
	queueMsgToSend(&roadSend);

	// Reset Transmission
	if (!resetState)
	{
		UartFrame resetSend;
		resetSend.frameWise.msgType = 'S';	// Reset Message Type
		queueMsgToSend(&resetSend);
	}

	// Status Transmission
	UartFrame statusSend;
	statusSend.frameWise.msgType = 'M';
	statusSend.frameWise.msg[0] = errorState;
	queueMsgToSend(&statusSend);

}

void vControlTask(void *params)
{
	// Initialise Controller Modules
	initPulseIn();
	initAdcModule(ACC_SPRUNG_ADC | ACC_UNSPRUNG_ADC | COIL_EXTENSION_ADC);
	initPwmModule(ACTUATOR_FORCE_PWM | DAMPING_COEFF_PWM);

	attachOnReceiveCallback(readMessage);
	// Initialise FreeRTOS Sleep Parameters
	portTickType pxPreviousWakeTime;
	const portTickType xTimeIncrement = configTICK_RATE_HZ / CONTROL_TASK_RATE_HZ;
	pxPreviousWakeTime = xTaskGetTickCount();

	for (;;)
	{
		// Delay until ready
		vTaskDelayUntil(&pxPreviousWakeTime, xTimeIncrement);

		// Get Sensor Values
		sprungAcc = getSmoothAdc(ACC_SPRUNG_ADC, MIN_ACC_SPRUNG, MAX_ACC_SPRUNG);
		unsprungAcc = getSmoothAdc(ACC_UNSPRUNG_ADC, MIN_ACC_UNSPRUNG, MAX_ACC_UNSPRUNG);
		coilExtension = getSmoothAdc(COIL_EXTENSION_ADC, MIN_COIL_EXTENSION, MAX_COIL_EXTENSION);
		speed = getPulseSpeed();
		dampingCoefficient = getDampingCoefficient();

		// Do control
		actuatorForce = getControlForce(xTimeIncrement);
		
		// Set Control Outputs
		setDuty(ACTUATOR_FORCE_PWM, actuatorForce, MIN_ACTUATOR_FORCE, MAX_ACTUATOR_FORCE);
		setDuty(DAMPING_COEFF_PWM, dampingCoefficient, MIN_DAMPING_COEFF, MAX_DAMPING_COEFF);

		sendSerialMessages();
	}
}

_iq getDampingCoefficient()
{
	switch (rideMode)
	{
		case SEDATE:
			return DAMPING_SEDATE;
		case NORMAL:
			return DAMPING_NORMAL;
		case SPORT:
			return DAMPING_SPORT;
		case RALLY:
			return DAMPING_RALLY;
	}
	
	return -1;
}


_iq getControlForce(int dTime)
{
	if (isOn == 0) {
		return 0;
	}

	return STIFFNESS_SPRING * coilExtension;
}

/* SETTERS */

void setRideMode(int rideModeIn)
{
	rideMode = (rideType) rideModeIn;
}

void setAscOn(int isAscOn)
{
	isOn = isAscOn;
}

void setRoadType(int roadTypeInput) {
	roadType = roadTypeInput;
}

void setThrottle(int throttleInput) {
	throttle = _IQ(throttleInput);
}

void setResetState(int resetStateInput) {
	resetState = resetStateInput;
}

void setErrorState(char errorStateInput)
{
	errorState = errorStateInput;
}

/* GETTERS */

int getDisplayRideMode()
{
	return rideMode;
}

int getDisplaySpeed() 
{
	return speed / 10;
}

int getDisplaySprungAcc() 
{
	return sprungAcc;
}

int getDisplayUnsprungAcc() 
{
	return unsprungAcc;
}

int getDisplayCoilExtension() 
{
	return coilExtension;
}

int getDisplayForce()
{
	return actuatorForce;
}

int getDisplayDampingCoefficient()
{
	return dampingCoefficient;
}

int getRoadType()
{
	return roadType;
}

int getThrottle()
{
	return  _IQint(throttle);
}

int getResetState() {
	return resetState;
}

int getCoilExError() {
	if(wusStatus &  COIL_EXTENSION_EXCEEDED) {
		return 1;
	} else {
		return 0;
	}
}

int getUnsprungAccError() {
	if(wusStatus & ACC_UNSPRUNG_EXCEEDED) {
		return 1;
	} else {
		return 0;
	}
}

int getSprungAccError() {
	if(wusStatus & ACC_SPRUNG_EXCEEDED) {
		return 1;
	} else {
		return 0;
	}
}

int getCarSpeedError() {
	if(wusStatus & CAR_SPEED_EXCEEDED) {
		return 1;
	} else {
		return 0;
	}
}

int getAscOn() {
	return isOn;
}
