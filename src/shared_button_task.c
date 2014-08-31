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

#define BIT(x) (1 << x)

#include "shared_button_task.h"

// ButtonSwitch structure contains info on each switch.
typedef struct ButtonSwitch
{
	int enabled;	// Switch is only polled if enabled.
	int events;		// Switch triggers events on rising or falling edges.
	int total;		// Total of previous switch values for oversampled debouncing.
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
	ButtonSet[button].enabled = 1;
	ButtonSet[button].events |= 1 << eventType;
}


// Switch Polling Task
void vButtonPollingTask(void* pvParameters);
{
	unsigned long mcuClock = SysCtlClockGet();
	buttonGPIOInit(mcuClock);

	while(1)
	{
		//TODO: Poll Enabled Buttons
		//TODO: Check for Events
		//TODO: Sleep Task
	}
}
