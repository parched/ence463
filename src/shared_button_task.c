/**
 * \file shared_button_task.c
 * \brief Common task module which handles push button input events.
 * \author Tom Walsh
 * \version 1.0
 * \date 2014-08-31
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

#include "shared_button_task.h"

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "shared_guidraw_task.h"

#define BIT(x)		(1 << x)

#define TOTAL_MAX	250
#define TOTAL_MIN	0
#define BUTTON_TASK_RATE_HZ 10000

// ButtonSwitch structure contains info on each switch.
typedef struct ButtonSwitch
{
	tBoolean 	enabled;	// Switch is only polled if enabled.
	int 		events;		// Switch triggers events on rising or falling edges.
	int 		total;		// Total of previous switch values for oversampled debouncing.
} ButtonSwitch;

ButtonSwitch ButtonSet[5];


// Configures GPIO Port for Onboard Switches (and LED)
static void buttonGPIOInit(unsigned long mcu_clock)
{
	// Enable PortG Peripheral
	SysCtlPeripheralEnable	(SYSCTL_PERIPH_GPIOG);
	SysCtlDelay 			(mcu_clock / 3000);

	// Set Pin Directions and Configure Input/Output Settings
	GPIODirModeSet 			(GPIO_PORTG_BASE, 0xF8, GPIO_DIR_MODE_IN);
	GPIOPadConfigSet 		(GPIO_PORTG_BASE, 0xF8, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
	GPIOPinTypeGPIOOutput 	(GPIO_PORTG_BASE, BIT(2));
}


// Sets Button Event Parameters
void configureButtonEvent(Button button, ButtonEvent eventType)
{
	ButtonSet[button].enabled 	= true;
	ButtonSet[button].events 	|= BIT(eventType);
	ButtonSet[button].total 	= 0;
}


// Switch Polling Task
void vButtonPollingTask(void* pvParameters)
{
	// Initialise GPIO pins
	unsigned long mcuClock = SysCtlClockGet();
	buttonGPIOInit(mcuClock);

	Button itr;						// Loop Iterator (Button to Process)
	int switchStates 	= 0x00;		// Port G Switch Values
	int tmpTotal 		= 0;		// Temporary Switch Total

	TickType_t xLastWakeTime;

	// Set to 10kHz operation
	const TickType_t xTickIncrement = configTICK_RATE_HZ / BUTTON_TASK_RATE_HZ;

	xLastWakeTime = xTaskGetTickCount();

	for(;;)
	{
		// Sleep for 0.1ms
		vTaskDelayUntil( &xLastWakeTime, xTickIncrement);

		// Read Switches
		switchStates = 0xF8 & ~(GPIOPinRead(GPIO_PORTG_BASE, 0xF8));

		// Iterate through Switches, check for events
		for (itr = BUTTON_UP; itr <= BUTTON_SELECT; itr ++)
		{
			if (ButtonSet[itr].enabled)
			{
				// Get previous total for comparison
				tmpTotal = ButtonSet[itr].total;

				// Increment or Decrement total based on switch state
				if (switchStates & BIT((itr + 3))) tmpTotal++;
				else tmpTotal --;

				if (tmpTotal > TOTAL_MAX) tmpTotal = TOTAL_MAX;
				if (tmpTotal < TOTAL_MIN) tmpTotal = TOTAL_MIN;

				// Check for rising edge, if enabled
				if (tmpTotal == TOTAL_MAX && ButtonSet[itr].total < TOTAL_MAX && ButtonSet[itr].events & BIT(BUTTON_EVENT_RISING_EDGE))
				{
					queueInputEvent(itr, BUTTON_EVENT_RISING_EDGE);
				}

				// Check for falling edge, if enabled
				if (tmpTotal == TOTAL_MIN && ButtonSet[itr].total > TOTAL_MIN && ButtonSet[itr].events & BIT(BUTTON_EVENT_FALLING_EDGE))
				{
					queueInputEvent(itr, BUTTON_EVENT_FALLING_EDGE);
				}

				// Store new total value
				ButtonSet[itr].total = tmpTotal;
			}
		}
	}
}
