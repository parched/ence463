/**
 * \file asc.c
 * \brief file that first runs in the wheel unit simulator
 * \author Hugh Bisset
 * \version 1.0
 * \date 2014-08-21
 */

/* Copyright (C)
 * 2014 - Hugh Bisset
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


/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Stellaris library includes. */
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"


/*Task Modules*/
#include "asc_control_task.h"
#include "shared_guilayout.h"
#include "shared_guidraw_task.h"
#include "shared_uart_task.h"
#include "shared_button_task.h"

const char *placeholder = "test";

/*-----------------------------------------------------------*/

static Activity mainActivity;
static ListView controls;
static Item roadTypeItem;
static Options roadTypeOption;
static Item rideTypeItem;
static Options rideTypeOption;
static Item carSpeedItem;
static Options carSpeedOption;

static Item statusMessageItem;
static Options statusMessageOption;

int main(void)
{
	/* Set the clocking to run from the PLL at 50 MHz.  Assumes 8MHz XTAL,
	whereas some older eval boards used 6MHz. */
	SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);

	/* Marking up GUI */
	controls = listView("Controls", 4);
	roadTypeOption = option(0, 4);
	roadTypeItem = item("Road Type", OPTIONTYPE_INT, OPTIONACCESS_READONLY, roadTypeOption, getDisplaySpeed);
	rideTypeOption = option(0, 4);
	rideTypeItem = item("Ride Type", OPTIONTYPE_INT, OPTIONACCESS_READONLY, rideTypeOption, getDisplaySpeed);
	carSpeedOption = option(-999, 999);
	carSpeedItem = item("Speed", OPTIONTYPE_INT, OPTIONACCESS_READONLY, carSpeedOption, getDisplaySpeed);
	statusMessageOption = option(0, 2);
	statusMessageOption.values[0] = "Kat";
	statusMessageOption.values[1] = "Sew";
	statusMessageItem = item("Speed", OPTIONTYPE_STRING, OPTIONACCESS_READONLY, statusMessageOption, test1);
	controls.items[0] = roadTypeItem;
	controls.items[1] = rideTypeItem;
	controls.items[2] = carSpeedItem;
	controls.items[3] = statusMessageItem;

	mainActivity = activity(1);
	addView(&mainActivity, &controls, VIEWTYPE_LIST, 0);

	attachActivity(&mainActivity);

	/* Configure buttons */
	configureButtonEvent(BUTTON_UP, BUTTON_EVENT_RISING_EDGE);
	configureButtonEvent(BUTTON_DOWN, BUTTON_EVENT_RISING_EDGE);
	configureButtonEvent(BUTTON_LEFT, BUTTON_EVENT_RISING_EDGE);
	configureButtonEvent(BUTTON_RIGHT, BUTTON_EVENT_RISING_EDGE);

	/*Continously determines the actuator force needed*/
	xTaskCreate(vControlTask, "Control task", 240,(void*) placeholder , 1, NULL);

	/*Inits UART, continously reads and writes UART messages*/
	//xTaskCreate(vUartTask, "UART task", 240,(void*) placeholder , 1, NULL);

	/*Inits button polling and checks for button pushes*/
	xTaskCreate(vButtonPollingTask, "Button polling task", 240,(void*) placeholder , 1, NULL);

	/*Inits button polling and checks for button pushes*/
	xTaskCreate(vButtonPollingTask, "Button polling task", 240, (void*) placeholder, 2, NULL);

	/* Refreshes GUI */
    xTaskCreate(vGuiRefreshTask, "Gui refresh task", 240, (void*) placeholder, 1, NULL);

	/* Start the scheduler so our tasks start executing. */
	vTaskStartScheduler();	
	
	/* If all is well we will never reach here as the scheduler will now be
	running.  If we do reach here then it is likely that there was insufficient
	heap available for the idle task to be created. */
	for(;;);
}



void vApplicationMallocFailedHook(void)
{
	/* This function will only be called if an API call to create a task, queue
	or semaphore fails because there is too little heap RAM remaining. */
	for(;;);
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook(xTaskHandle *pxTask, signed char *pcTaskName)
{
	/* This function will only be called if a task overflows its stack.  Note
	that stack overflow checking does slow down the context switch
	implementation. */
	for(;;);
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook(void)
{
	/* idle hook*/
}
/*-----------------------------------------------------------*/

void vApplicationTickHook(void)
{
	/* Tick hook */
}


