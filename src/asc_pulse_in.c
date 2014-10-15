/**
 * \file asc_pulse_in.c
 * \brief Active suspension controller pulse train reader implementation.
 * \author Tom Walsh
 * \version 
 * \date 2014-09-02
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

#define BIT(x) 		(1 << x)
#define PRIORITY(x) (x << 5)

#define PULSE_PIN BIT(7)

#include "asc_pulse_in.h"
#include "shared_pulse.h"

#include "FreeRTOS.h"
#include "task.h"

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"


static volatile TickType_t lastPulseTick;
static volatile TickType_t lastTickDuration;

void isrPortF(void);

void initPulseIn()
{
	// Enable PortF Peripheral
	SysCtlPeripheralEnable (SYSCTL_PERIPH_GPIOF);
	SysCtlDelay (SysCtlClockGet() / 3000);

	// Set Pulse Pin as Input and Configure Rising-Edge 
	GPIOPinTypeGPIOInput (GPIO_PORTF_BASE, PULSE_PIN);
	GPIOPortIntRegister (GPIO_PORTF_BASE, isrPortF);
	GPIOIntTypeSet (GPIO_PORTF_BASE, PULSE_PIN, GPIO_RISING_EDGE);
	GPIOPinIntEnable (GPIO_PORTF_BASE, PULSE_PIN);
	IntPrioritySet (INT_GPIOF, PRIORITY(1));
}

_iq getPulseSpeed()
{
	return _IQ(((long) configTICK_RATE_HZ) * WHEEL_CIRCUMFERENCE_M) / (lastTickDuration * PULSES_PER_REV);
}

void isrPortF(void)
{
	// Clear Interrupt Flag
	GPIOPinIntClear (GPIO_PORTF_BASE, 0xFF);

	// Get Current Time
	TickType_t newPulseTick = xTaskGetTickCountFromISR();

	// Store Pulse Length
	lastTickDuration = newPulseTick - lastPulseTick;

	// Store Current Time
	lastPulseTick = newPulseTick;
}
