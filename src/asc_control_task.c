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

#include "FreeRTOS.h"
#include "task.h"

#include "asc_controller.h"
#include "asc_pulse_in.h"
#include "shared_pwm.h"
#include "shared_adc.h"
#include "shared_uart_task.h"

#define CONTROL_TASK_RATE_HZ 1000

#define DAMPING_SEDATE 	100
#define DAMPING_NORMAL 	250
#define DAMPING_SPORT	350
#define DAMPING_RALLY	500

static rideType rideMode = SEDATE;

int getDampingCoefficient (void)
{
	switch (rideMode)
	{
		case SEDATE:
			return DAMPING_SEDATE; break;
		case NORMAL:
			return DAMPING_NORMAL; break;
		case SPORT:
			return DAMPING_SPORT; break;
		case RALLY:
			return DAMPING_RALLY; break;
	}
}


void vControlTask(void *params)
{
	// Initialise Variables (Local unless needed elsewhere)
	int sprungAcc = 0;
	int unsprungAcc = 0;
	int coilExtension = 0;
	int speed = 0;

	int actuatorForce = 0;
	int dampingCoefficient = 0;
	int dTime = configTICK_RATE_HZ / CONTROL_TASK_RATE_HZ;

	ascControlState =  controllerState();

	// Initialise Controller Modules
	initPulseIn();
	initAdcModule(ACC_SPRUNG_ADC | ACC_UNSPRUNG_ADC | COIL_EXTENSION_ADC);
	initPwmModule(ACTUATOR_FORCE_PWM | DAMPING_COEFF_PWM);

	// Initialise FreeRTOS Sleep Parameters
	portTickType pxPreviousWakeTime;
	const portTickType xTimeIncrement = configTICK_RATE_HZ / CONTROL_TASK_RATE_HZ;
	pxPreviousWakeTime = xTaskGetTickCount();

	for (;;)
	{
		// Delay until ready
		vTaskDelayUntil(&pxPreviousWakeTime, xTimeIncrement);

		// Get Sensor Values
		sprungAcc = getSmoothADC(ACC_SPRUNG_ADC);
		unsprungAcc = getSmoothADC(ACC_UNSPRUNG_ADC);
		coilExtension = getSmoothADC(COIL_EXTENSION_ADC);
		speed = getPulseSpeed();
		dampingCoefficient = getDampingCoefficient();
		
		//Calculate Control Outputs
		actuatorForce = getActuatorForce(&ascControlState, sprungAcc, unsprungAcc, coilExtension, speed, dampingCoefficient, dTime);

		// Set Control Outputs
		setDuty(ACTUATOR_FORCE_PWM, actuatorForce)
		setDuty(DAMPING_COEFF_PWM, dampingCoefficient)
	}
}


void setRideType(rideType rideModeIn)
{
	rideMode = rideModeIn;
}