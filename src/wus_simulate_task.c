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

#include "wus_simulator.h"
#include "wus_pulse_out.h"
#include "shared_pwm.h"
#include "shared_adc.h"
#include "shared_uart_task.h"
#include "shared_parameters.h"

#define SIMULATE_TASK_RATE_HZ 1000
#define ROAD_TYPE_MESSAGE_SIZE 2

static SimState wusSimState;
static int roadType = 0;
static int dampingFactor = 0;
static int throttle = 0;
static int speed = 0;
static int sprungAcc = 0;
static int unsprungAcc = 0;
static int coilExtension = 0;
static int wusStatusEcho = 0;


/**
 * \brief Reads an incoming UART message.
 *
 * \param uartFrame Pointer to the uartFrame to read.
 */
void readMessage(UartFrame uartFrame) {
	switch (uartFrame.frameWise.msgType) {
		case 'R':
			roadType = (int) ustrtoul(&uartFrame.frameWise.msg[1], NULL, 10);
			break;
		case 'S':
			resetSimulation(&wusSimState);
			break;
		case 'A':
			throttle = (int) ustrtoul(&uartFrame.frameWise.msg[1], NULL, 10);
			break;
		case 'M':
			wusStatusEcho = (int) ustrtoul(&uartFrame.frameWise.msg[1], NULL, 16);
			break;
	}
}

void vSimulateTask(void *params) {
	int force = 0;
	int dTime = 0;
	char errorCode = 0;

	wusSimState = simState();

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
		errorCode = simulate(&wusSimState, force, throttle, dampingFactor, roadType, dTime);

		speed = getSpeed(&wusSimState);
		sprungAcc = getSprungAcc(&wusSimState);
		unsprungAcc = getUnsprungAcc(&wusSimState);
		coilExtension = getCoilExtension(&wusSimState);

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
	return speed / 10;
}

int getDisplaySprungAcc() {
	return sprungAcc;
}

int getDisplayUnsprungAcc() {
	return unsprungAcc;
}

int getDisplayCoilExtension() {
	return coilExtension;
}

int getWusStatusEcho() {
	return wusStatusEcho;
}
