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
#include "inc/hw_ints.h"
#include "inc/hw_timer.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include "FreeRTOS.h"
#include "task.h"

#include "shared_pulse.h"
#include "shared_parameters.h"

#define PULSE_OUT_TASK_STACK_DEPTH 100
#define PULSE_OUT_TASK_PROIRITY 5

#define PULSE_OUT_TASK_MIN_RATE_HZ 1
#define TIMER_FALLBACK_RATE_HZ 5

#define PULSE_OUT_PERIPH SYSCTL_PERIPH_GPIOB
#define PULSE_OUT_PORT GPIO_PORTB_BASE
#define PULSE_OUT_PIN GPIO_PIN_0

static volatile int psuedoSpeed = 0; /**< The internally stored speed. */
static volatile unsigned char isPulseHigh = 0;
/**
 * \brief The pulse output task.
 *
 * \param pvParams Unused.
 */
//static void vPulseOutTask(void *pvParams);

static void isrTimer0 (void);

static void initPulseTimer(void)
{
	// Enable and Configure Timer Peripheral
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	SysCtlDelay(SysCtlClockGet()/3000);

	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

	// Initialise Timer Load
	TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet() / TIMER_FALLBACK_RATE_HZ);

	// Configure Timer Interrupt
	TimerIntRegister(TIMER0_BASE, TIMER_A, isrTimer0);
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

	// Enable Timer Stalling
	TimerControlStall(TIMER0_BASE, TIMER_A, true);

	// Enable Timer
	TimerEnable(TIMER0_BASE, TIMER_A);
}


void isrTimer0 (void)
{
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	if(psuedoSpeed == 0)
	{
		TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet() / TIMER_FALLBACK_RATE_HZ);
	}
	else
	{
		GPIOPinWrite(PULSE_OUT_PORT, PULSE_OUT_PIN, isPulseHigh);
		isPulseHigh = ~isPulseHigh;

		TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet() / psuedoSpeed / PULSES_PER_REV);
	}
}

void initPulseOut() {
	// Enable peripheral
	SysCtlPeripheralEnable(PULSE_OUT_PERIPH);
	SysCtlDelay(SysCtlClockGet() / 3000);

	// Set pin direction and configure output settings
	GPIODirModeSet(PULSE_OUT_PORT, PULSE_OUT_PIN, GPIO_DIR_MODE_OUT);
	GPIOPadConfigSet(PULSE_OUT_PORT, PULSE_OUT_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
	GPIOPinTypeGPIOOutput(PULSE_OUT_PORT, PULSE_OUT_PIN);

	initPulseTimer();
	//xTaskCreate(vPulseOutTask, "Pulse Out", PULSE_OUT_TASK_STACK_DEPTH, NULL, PULSE_OUT_TASK_PROIRITY, NULL);
}

void setPulseSpeed(_iq speed) {
	psuedoSpeed = _IQint(speed);
}
/*
void vPulseOutTask(void *pvParams) {
	unsigned char isPulseHigh = 0;

	// initialise FreeRTOS sleep parameters
	portTickType pxPreviousWakeTime;
	pxPreviousWakeTime = xTaskGetTickCount();

	for (;;) {
		if (psuedoSpeed == 0) {
			vTaskDelayUntil( &pxPreviousWakeTime, configTICK_RATE_HZ / PULSE_OUT_TASK_MIN_RATE_HZ);
		} else {
			GPIOPinWrite(PULSE_OUT_PORT, PULSE_OUT_PIN, isPulseHigh);

			isPulseHigh = ~isPulseHigh;

			vTaskDelayUntil( &pxPreviousWakeTime, configTICK_RATE_HZ * WHEEL_CIRCUMFERENCE_M / psuedoSpeed);
		}
	}
}
*/
