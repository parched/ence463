/**
 * \file wus_pulse_out.c
 * \brief Wheel unit simulator pulse train output implementation.
 * \author James Duley
 * \version 1.0
 * \date 2014-09-02
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

#include "wus_pulse_out.h"

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

#include "FreeRTOS.h"
#include "task.h"

#include "shared_pulse.h"

#define TASK_STACK_DEPTH 100
#define TASK_PROIRITY 5

#define TASK_MIN_RATE_HZ 1

#define PULSE_OUT_PERIPH SYSCTL_PERIPH_GPIOB
#define PULSE_OUT_PORT GPIO_PORTB_BASE
#define PULSE_OUT_PIN GPIO_PIN_0

static volatile int _speed = 0; /**< The internally stored speed. */

/**
 * \brief The pulse output task.
 *
 * \param pvParams Unused.
 */
static void vPulseOutTask(void *pvParams);

void initPulseOut() {
	// Enable peripheral
	SysCtlPeripheralEnable(PULSE_OUT_PERIPH);

	// Set pin direction and configure output settings
	GPIODirModeSet(PULSE_OUT_PORT, PULSE_OUT_PIN, GPIO_DIR_MODE_OUT);
	GPIOPadConfigSet(PULSE_OUT_PORT, PULSE_OUT_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
	GPIOPinTypeGPIOOutput(PULSE_OUT_PORT, PULSE_OUT_PIN);

	xTaskCreate(vPulseOutTask, "Pulse Out", TASK_STACK_DEPTH, NULL, TASK_PROIRITY, NULL);
}

void setPulseSpeed(int speed) {
	_speed = speed;
}

void vPulseOutTask(void *pvParams) {
	unsigned char isPulseHigh = 0;

	// initialise FreeRTOS sleep parameters
	portTickType pxPreviousWakeTime;
	pxPreviousWakeTime = xTaskGetTickCount();

	for (;;) {
		if (_speed == 0) {
			vTaskDelayUntil( &pxPreviousWakeTime, configTICK_RATE_HZ / TASK_MIN_RATE_HZ);
		} else {
			GPIOPinWrite(PULSE_OUT_PORT, PULSE_OUT_PIN, isPulseHigh);

			isPulseHigh = ~isPulseHigh;

			vTaskDelayUntil( &pxPreviousWakeTime, configTICK_RATE_HZ * WHEEL_CIRCUMFERENCE * 36 / (PULSES_PER_REV * _speed * 2));
		}
	}
}
