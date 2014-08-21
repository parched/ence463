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
#include "shared_adc.h"
#include "shared_uart_task.h"

SimState simState;
char roadType = 0;
int dampingFactor = 0;
int accel = 0;

/**
 * \brief Reads the road type from a message.
 *
 * \param msg The message to read.
 *
 * \return The road type.
 */
char getRoadType(char *msg);

/**
 * \brief Reads the acceleration from a message.
 *
 * \param msg The message to read.
 *
 * \return The acceleration.
 */
int getAccel(char *msg);

/**
 * \brief Reads an incoming UART message.
 *
 * \param uartFrame Pointer to the uartFrame to read.
 */
void readMessage(UartFrame uartFrame) {
	switch (uartFrame.msgType) {
		case 'R':
			roadType = getRoadType(uartFrame.msg);
			break;
		case 'S':
			resetSimulation(&simState);
			break;
		case 'A':
			accel = getAccel(uartFrame.msg);
			break;
		case 'M':
			/* TODO */
			break;
	}
}

void vSimulateTask(void *params) {
	int force = 0;
	int dTime = 0;
	char errorCode = 0;

	initSimulation();
	initAdcModule(ACTUATOR_FORCE_ADC | DAMPING_COEFF_ADC);
	attachOnReceiveCallback(readMessage);

	for (;;) {
		force = getSmoothAdc(ACTUATOR_FORCE_ADC);
		dampingFactor = getSmoothAdc(DAMPING_COEFF_ADC);

		/* TODO: find dTime */
		errorCode = simulate(&simState, force, accel, dampingFactor, roadType, dTime);
		
		if (errorCode != 0) {
			/* TODO */
		}
	}
}
