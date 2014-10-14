/**
 * \file wus.c
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
#include "wus_simulate_task.h"
#include "shared_guidraw_task.h"
#include "shared_uart_task.h"
#include "shared_button_task.h"
#include "shared_tracenode.h"

#define NUM_ROAD_NODES 200

static const char *placeholder = "test";

static Activity mainActivity;

static TraceView roadSurface;
static TraceNode roadNodes[NUM_ROAD_NODES];
static CircularBufferHandler roadHandler;

static ListView telemetry;
static Options speedOption;
static Item speedItem;
static Options sprungAccOption;
static Item sprungAccItem;
static Options unsprungAccOption;
static Item unsprungAccItem;
static Options coilExtensionOption;
static Item coilExtensionItem;

static ListView wusMessages;
//static Options startOption;
//static Item startItem;
static Options roadTypeOption;
static Item roadTypeItem;
static Options throttleOption;
static Item throttleItem;

static ListView wusStatusEcho;
static Item wusStatusCoilItem;
static Options wusStatusCoilOption;
static Item wusStatusSprungItem;
static Options wusStatusSprungOption;
static Item wusStatusUnsprungItem;
static Options wusStatusUnsprungOption;
static Item speedExceededItem;
static Options speedExceededOption;

static ListView wusStatusEcho2;
static Item powerFailureItem;
static Options powerFailureOption;
static Item watchdogErrorItem;
static Options watchdogErrorOption;


/*-----------------------------------------------------------*/

int main(void)
{
	/* Set the clocking to run from the PLL at 50 MHz.  Assumes 8MHz XTAL,
	whereas some older eval boards used 6MHz. */
	SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);

	/* Marking up GUI */
	roadHandler = createCircularBuffer(roadNodes, NUM_ROAD_NODES, BUFFERFULLMODE_OVERWRITE);
	setRoadBuffer(&roadHandler); // pass it to the sim task
	roadSurface = traceView("Surface", &roadHandler, TRACE_ZERO_CENTER, 1, 1, 1);

	/*telemetry GUI*/
	telemetry = listView("Telemetry", 4);
	speedOption = option(-999, 999);
	speedItem = item("Speed", OPTIONTYPE_INT, OPTIONACCESS_READONLY, speedOption, getDisplaySpeed);
	sprungAccOption = option(-9999, 9999);
	sprungAccItem = item("Sp Acc.", OPTIONTYPE_INT, OPTIONACCESS_READONLY, sprungAccOption, getDisplaySprungAcc);
	unsprungAccOption = option(-9999, 9999);
	unsprungAccItem = item("Unsp Acc.", OPTIONTYPE_INT, OPTIONACCESS_READONLY, unsprungAccOption, getDisplayUnsprungAcc);
	coilExtensionOption = option(-9999, 9999);
	coilExtensionItem = item("Coil Ext.", OPTIONTYPE_INT, OPTIONACCESS_READONLY, coilExtensionOption, getDisplayCoilExtension);
	telemetry.items[0] = speedItem;
	telemetry.items[1] = sprungAccItem;
	telemetry.items[2] = unsprungAccItem;
	telemetry.items[3] = coilExtensionItem;

	/*ASC messages GUI*/
	wusMessages = listView("ASC STAT",2);
	/*
	startOption = option(0,1);
	startOption.skip = 1;
	startOption.values[0]  = "Off";
	startOption.values[1]  = "On";
	startItem = item("Reset", OPTIONTYPE_STRING, OPTIONACCESS_READONLY, startOption, getStartStatusDisplay);
	*/
	roadTypeOption = option(0, 1);
	roadTypeOption.skip = 1;
	roadTypeItem = item("Road Type", OPTIONTYPE_INT, OPTIONACCESS_READONLY, roadTypeOption, getRoadTypeStatusDisplay);
	throttleOption = option(-10,10);
	throttleOption.skip = 1;
	throttleItem = item("Throttle", OPTIONTYPE_INT, OPTIONACCESS_READONLY, throttleOption, getThrottleStatusDisplay);
	wusMessages.items[0] = roadTypeItem;
	wusMessages.items[1] = throttleItem;
	//wusMessages.items[2] = startItem;

	/*Invoked errors GUI*/
	wusStatusEcho = listView("ErrInvoked",4);
	wusStatusCoilOption = option(0, 1);
	wusStatusCoilOption.values[0] = "Ok";
	wusStatusCoilOption.values[1] = "Error";
	wusStatusCoilItem = item("CoilEx", OPTIONTYPE_STRING, OPTIONACCESS_READONLY, wusStatusCoilOption, getCoilExErrorInvoked);
	wusStatusSprungOption = option(0, 1);
	wusStatusSprungOption.values[0] = "Ok";
	wusStatusSprungOption.values[1] = "Error";
	wusStatusSprungItem = item("SprAcc", OPTIONTYPE_STRING, OPTIONACCESS_READONLY, wusStatusSprungOption, getSprungAccErrorInvoked);
	wusStatusUnsprungOption = option(0, 1);
	wusStatusUnsprungOption.values[0] = "Ok";
	wusStatusUnsprungOption.values[1] = "Error";
	wusStatusUnsprungItem = item("UnsprAcc", OPTIONTYPE_STRING, OPTIONACCESS_READONLY, wusStatusUnsprungOption, getUnsprungAccErrorInvoked);
	speedExceededOption = option(0,1);
	speedExceededOption.values[0] = "Ok";
	speedExceededOption.values[1] = "Error";
	speedExceededItem = item("SpeedEx", OPTIONTYPE_STRING, OPTIONACCESS_READONLY, speedExceededOption, getCarSpeedErrorInvoked);
	wusStatusEcho.items[0] = wusStatusCoilItem;
	wusStatusEcho.items[1] = wusStatusSprungItem;
	wusStatusEcho.items[2] = wusStatusUnsprungItem;
	wusStatusEcho.items[3] = speedExceededItem;

	wusStatusEcho2 = listView("ErrInvoked2",2);
	powerFailureOption = option(0, 1);
	powerFailureOption.values[0] = "Ok";
	powerFailureOption.values[1] = "Error";
	powerFailureItem = item("CoilEx", OPTIONTYPE_STRING, OPTIONACCESS_READONLY, powerFailureOption, getPowerFailureInvoked);
	watchdogErrorOption = option(0, 1);
	watchdogErrorOption.values[0] = "Ok";
	watchdogErrorOption.values[1] = "Error";
	watchdogErrorItem = item("SprAcc", OPTIONTYPE_STRING, OPTIONACCESS_READONLY, watchdogErrorOption, getWatchdogTimerFailureInvoked);
	wusStatusEcho2.items[0] = powerFailureItem;
	wusStatusEcho2.items[1] = watchdogErrorItem;

	/*attach views to activity*/
	mainActivity = activity();
	addView(&mainActivity, &telemetry, VIEWTYPE_LIST);
	addView(&mainActivity, &roadSurface, VIEWTYPE_TRACE);
	addView(&mainActivity, &wusMessages, VIEWTYPE_LIST);
	addView(&mainActivity, &wusStatusEcho, VIEWTYPE_LIST);
	addView(&mainActivity, &wusStatusEcho2, VIEWTYPE_LIST);
	attachActivity(&mainActivity);

	/* Configure buttons */
	configureButtonEvent(BUTTON_UP, BUTTON_EVENT_RISING_EDGE);
	configureButtonEvent(BUTTON_DOWN, BUTTON_EVENT_RISING_EDGE);
	configureButtonEvent(BUTTON_LEFT, BUTTON_EVENT_RISING_EDGE);
	configureButtonEvent(BUTTON_RIGHT, BUTTON_EVENT_RISING_EDGE);

	/* Simulates the road height position, acceration of the unsprung and sprung spring and the coil extension */
	xTaskCreate(vSimulateTask, "Simulate task", 240,(void*) placeholder , 3, NULL);

	/*Inits UART, continously reads and writes UART messages*/
	//xTaskCreate(vUartTask, "Uart Task", 240,(void*) placeholder , 3, NULL);

	/* Refreshes GUI */
	xTaskCreate(vGuiRefreshTask, "Gui refresh task", 240, (void*) placeholder, 1, NULL);

	/* Polls buttons */
	xTaskCreate(vButtonPollingTask, "Button polling task", 240, (void*) placeholder, 2, NULL);

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


