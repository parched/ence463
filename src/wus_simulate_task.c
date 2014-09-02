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

#include "wus_simulator.h"
#include "wus_pulse_out.h"
#include "shared_pwm.h"
#include "shared_adc.h"
#include "shared_uart_task.h"

SimState wusSimState;
char roadType = 0;
int dampingFactor = 0;
int throttle = 0;

/**
 * \brief Reads the road type from a message.
 *
 * \param msg The message to read.
 *
 * \return The road type.
 */
char getRoadType(char *msg);

/**
 * \brief Reads the throttle from a message.
 *
 * \param msg The message to read.
 *
 * \return The throttle.
 */
int getThrottle(char *msg);

/**
 * \brief Reads an incoming UART message.
 *
 * \param uartFrame Pointer to the uartFrame to read.
 */
void readMessage(UartFrame uartFrame) {
	switch (uartFrame.frameWise.msgType) {
		case 'R':
			roadType = getRoadType(uartFrame.frameWise.msg);
			break;
		case 'S':
			resetSimulation(&wusSimState);
			break;
		case 'A':
			throttle = getThrottle(uartFrame.frameWise.msg);
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

	wusSimState = simState();

	initPulse();
	initAdcModule(ACTUATOR_FORCE_ADC | DAMPING_COEFF_ADC);
	initPwmModule(ACC_SPRUNG_PWM | ACC_UNSPRUNG_PWM | COIL_EXTENSION_PWM);
	attachOnReceiveCallback(readMessage);

	for (;;) {
		force = getSmoothAdc(ACTUATOR_FORCE_ADC);
		dampingFactor = getSmoothAdc(DAMPING_COEFF_ADC);

		/* TODO: find dTime */
		errorCode = simulate(&wusSimState, force, throttle, dampingFactor, roadType, dTime);
		setSpeed(getSpeed(&wusSimState));
		
		setDuty(ACC_SPRUNG_PWM, getSprungAcc(&wusSimState));
		setDuty(ACC_UNSPRUNG_PWM, getUnsprungAcc(&wusSimState));
		setDuty(COIL_EXTENSION_PWM, getCoilExtension(&wusSimState));
		
		if (errorCode != 0) {
			/* TODO */
		}
	}
}

SimState *getSimStatePtr() {
	return &wusSimState;
}

char getRoadType(char *msg) {
	return 0;
}

int getThrottle(char *msg) {
	return 0;
}
