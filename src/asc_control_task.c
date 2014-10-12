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

#include "FreeRTOS.h"
#include "task.h"

#include "asc_pulse_in.h"
#include "shared_pwm.h"
#include "shared_adc.h"
#include "shared_uart_task.h"
#include "shared_parameters.h"

#define CONTROL_TASK_RATE_HZ 1000

#define DAMPING_SEDATE 	100
#define DAMPING_NORMAL 	250
#define DAMPING_SPORT	350
#define DAMPING_RALLY	500

static rideType rideMode = SEDATE;
static int sprungAcc = 0;
static int unsprungAcc = 0;
static int coilExtension = 0;
static int speed = 0;
static int actuatorForce = 0;
static int dampingCoefficient = 0;

/**
 * \brief Calculates the required actuator force.
 *
 * \param dTime The time in ticks since the last calculation.
 *
 * \return The actuator force.
 */
int getControlForce(int dTime);

int getDampingCoefficient (void)
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


void vControlTask(void *params)
{
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
	}
}

int getControlForce(int dTime)
{
	return STIFFNESS_SPRING * coilExtension;
}

/* SETTERS */

void setRideMode(rideType rideModeIn)
{
	rideMode = rideModeIn;
}

/* GETTERS */

rideType getDisplayRideMode()
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
