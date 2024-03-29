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
/*list views*/
static ListView controls;
static ListView statuses;
static ListView statuses2;
static ListView invokeWusErrors;

/*controls options and items*/
static Item roadTypeItem;
static Options roadTypeOption;
static Item rideTypeItem;
static Options rideTypeOption;
static Item throttleItem;
static Options throttleOption;
static Item resetItem;
static Options resetOption;
static Item actControlItem;
static Options actControlOption;
/*status options and items*/
static Item carSpeedItem;
static Options carSpeedOption;
static Item actuatorForceItem;
static Options actuatorForceOption;
static Item coilExtensionItem;
static Options coilExtensionOption;
static Item unsprungAccItem;
static Options unsprungAccOption;
static Item sprungAccItem;
static Options sprungAccOption;
/*Out of Randge options and items*/
static Item wusStatusCoilItem;
static Options wusStatusCoilOption;
static Item wusStatusSprungItem;
static Options wusStatusSprungOption;
static Item wusStatusUnsprungItem;
static Options wusStatusUnsprungOption;
static Item speedExceededItem;
static Options speedExceededOption;
static Item powerFailureItem;
static Options powerFailureOption;
static Item watchdogFauilureItem;
static Options watchdogFailureOption;

/*error to invoke on WUS options and items*/
static Item invokeCoilErrorItem;
static Options invokeCoilErrorOption;
static Item invokeSprungErrorItem;
static Options invokeSprungErrorOption;
static Item invokeUnsprungErrorItem;
static Options invokeUnsprungErrorOption;
static Item invokeSpeedErrorItem;
static Options invokeSpeedErrorOption;
static Item involePowerErrorItem;
static Options involePowerErrorOption;
static Item invokeWatchdogErrorItem;
static Options invokeWatchdogErrorOption;

int main(void)
{
	/* Set the clocking to run from the PLL at 50 MHz.  Assumes 8MHz XTAL,
	   whereas some older eval boards used 6MHz. */
	SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);

	/* Marking up GUI */
	controls = listView("Controls", 5);
	statuses = listView("Status", 5);
	statuses2 = listView("WUS Errors", 6);
	invokeWusErrors = listView("InvokeErr", 6);

	/*controls menu GUI*/
	roadTypeOption = option(10, 33);
	roadTypeOption.skip = 1;
	roadTypeItem = item("Road Type", OPTIONTYPE_INT, OPTIONACCESS_MODIFIABLE, roadTypeOption, getRoadType);
	roadTypeItem.setter = setRoadType;
	setRoadType(10);
	rideTypeOption = option(0, 3);
	rideTypeOption.skip = 1;
	rideTypeOption.values[0] = "SEDATE";
	rideTypeOption.values[1] = "NORMAL";
	rideTypeOption.values[2] = "SPORT";
	rideTypeOption.values[3] = "RALLY";
	rideTypeItem = item("Ride", OPTIONTYPE_STRING, OPTIONACCESS_MODIFIABLE, rideTypeOption, getDisplayRideMode);
	rideTypeItem.setter = setRideMode;
	throttleOption = option(-9, 10);
	throttleOption.skip = 1;
	throttleItem = item("Throttle", OPTIONTYPE_INT, OPTIONACCESS_MODIFIABLE, throttleOption, getThrottle);
	throttleItem.setter = setThrottle;
	resetOption = option(0, 1);
	resetOption.skip = 1;
	resetOption.values[0] = "Off";
	resetOption.values[1] = "On";
	resetItem = item("Reset", OPTIONTYPE_STRING, OPTIONACCESS_MODIFIABLE, resetOption, getResetState);
	resetItem.setter = setResetState;
	actControlOption = option(0, 1);
	actControlOption.skip = 1;
	actControlOption.values[0] = "Off";
	actControlOption.values[1] = "On";
	actControlItem = item("ActState", OPTIONTYPE_STRING, OPTIONACCESS_MODIFIABLE, actControlOption, getAscOn);
	actControlItem.setter = setAscOn;

	/*Status Menu GUI*/
	carSpeedOption = option(-999, 999);
	carSpeedItem = item("Speed", OPTIONTYPE_INT, OPTIONACCESS_READONLY, carSpeedOption, getDisplaySpeed);
	actuatorForceOption = option(-999, 999);
	actuatorForceItem = item("CtrlForce", OPTIONTYPE_INT, OPTIONACCESS_READONLY, actuatorForceOption, getDisplayForce);
	coilExtensionOption = option(-3000, 3000);
	coilExtensionItem = item("CoilEx", OPTIONTYPE_INT, OPTIONACCESS_READONLY, coilExtensionOption, getDisplayCoilExtension);
	unsprungAccOption = option(-3000, 3000);
	unsprungAccItem = item("unsprAc", OPTIONTYPE_INT, OPTIONACCESS_READONLY, unsprungAccOption, getDisplayUnsprungAcc);
	sprungAccOption = option(-3000, 3000);
	sprungAccItem = item("sprAc", OPTIONTYPE_INT, OPTIONACCESS_READONLY, sprungAccOption, getDisplaySprungAcc);

	/*Out of Range Errors Menu GUI*/
	wusStatusCoilOption = option(0, 1);
	wusStatusCoilOption.values[0] = "Ok";
	wusStatusCoilOption.values[1] = "Error";
	wusStatusCoilItem = item("CoilEx", OPTIONTYPE_STRING, OPTIONACCESS_READONLY, wusStatusCoilOption, getCoilExError);
	wusStatusSprungOption = option(0, 1);
	wusStatusSprungOption.values[0] = "Ok";
	wusStatusSprungOption.values[1] = "Error";
	wusStatusSprungItem = item("SprAcc", OPTIONTYPE_STRING, OPTIONACCESS_READONLY, wusStatusSprungOption, getSprungAccError);
	wusStatusUnsprungOption = option(0, 1);
	wusStatusUnsprungOption.values[0] = "Ok";
	wusStatusUnsprungOption.values[1] = "Error";
	wusStatusUnsprungItem = item("UnsprAcc", OPTIONTYPE_STRING, OPTIONACCESS_READONLY, wusStatusUnsprungOption, getUnsprungAccError);
	speedExceededOption = option(0, 1);
	speedExceededOption.values[0] = "Ok";
	speedExceededOption.values[1] = "Error";
	speedExceededItem = item("SpeedEx", OPTIONTYPE_STRING, OPTIONACCESS_READONLY, speedExceededOption, getCarSpeedError);
	watchdogFailureOption = option(0, 1);
	watchdogFailureOption.values[0] = "Ok";
	watchdogFailureOption.values[1] = "Error";
	watchdogFauilureItem = item("WatchdogEx", OPTIONTYPE_STRING, OPTIONACCESS_READONLY, watchdogFailureOption, getWatchdogInvokedError);
	powerFailureOption = option(0, 1);
	powerFailureOption.values[0] = "Ok";
	powerFailureOption.values[1] = "Error";
	powerFailureItem = item("PowerEx", OPTIONTYPE_STRING, OPTIONACCESS_READONLY, powerFailureOption, getPowerError);

	/*invoke errors GUI*/
	invokeCoilErrorOption = option(0, 1);
	invokeCoilErrorOption.skip = 1;
	invokeCoilErrorOption.values[0] = "Off";
	invokeCoilErrorOption.values[1] = "On";
	invokeCoilErrorItem = item("CoilErr", OPTIONTYPE_STRING, OPTIONACCESS_MODIFIABLE, invokeCoilErrorOption, getCoilInvokedError);
	invokeCoilErrorItem.setter = setCoilError;
	invokeSprungErrorOption = option(0, 1);
	invokeSprungErrorOption.skip = 1;
	invokeSprungErrorOption.values[0] = "Off";
	invokeSprungErrorOption.values[1] = "On";
	invokeSprungErrorItem = item("SprErr", OPTIONTYPE_STRING, OPTIONACCESS_MODIFIABLE, invokeSprungErrorOption, getSprungInvokedError);
	invokeSprungErrorItem.setter = setSprungError;
	invokeUnsprungErrorOption = option(0, 1);
	invokeUnsprungErrorOption.skip = 1;
	invokeUnsprungErrorOption.values[0] = "Off";
	invokeUnsprungErrorOption.values[1] = "On";
	invokeUnsprungErrorItem = item("UnsprErr", OPTIONTYPE_STRING, OPTIONACCESS_MODIFIABLE, invokeUnsprungErrorOption, getUnsprungInvokedError);
	invokeUnsprungErrorItem.setter = setUnsprungError;
	invokeSpeedErrorOption = option(0, 1);
	invokeSpeedErrorOption.skip = 1;
	invokeSpeedErrorOption.values[0] = "Off";
	invokeSpeedErrorOption.values[1] = "On";
	invokeSpeedErrorItem = item("SpeedErr", OPTIONTYPE_STRING, OPTIONACCESS_MODIFIABLE, invokeSpeedErrorOption, getSpeedInvokedError);
	invokeSpeedErrorItem.setter = setSpeedError;
	involePowerErrorOption = option(0, 1);
	involePowerErrorOption.skip = 1;
	involePowerErrorOption.values[0] = "Off";
	involePowerErrorOption.values[1] = "On";
	involePowerErrorItem = item("PowErr", OPTIONTYPE_STRING, OPTIONACCESS_MODIFIABLE, involePowerErrorOption, getPowerInvokedError);
	involePowerErrorItem.setter = setPowerError;
	invokeWatchdogErrorOption = option(0, 1);
	invokeWatchdogErrorOption.skip = 1;
	invokeWatchdogErrorOption.values[0] = "Off";
	invokeWatchdogErrorOption.values[1] = "On";
	invokeWatchdogErrorItem = item("WatchdogErr", OPTIONTYPE_STRING, OPTIONACCESS_MODIFIABLE, invokeWatchdogErrorOption, getWatchdogInvokedError);
	invokeWatchdogErrorItem.setter = setWatchdogError;

	/*attach items to ListView*/
	controls.items[0] = roadTypeItem;
	controls.items[1] = rideTypeItem;
	controls.items[2] = throttleItem;
	controls.items[3] = resetItem;
	controls.items[4] = actControlItem;
	statuses.items[0] = carSpeedItem;
	statuses.items[1] = actuatorForceItem;
	statuses.items[2] = coilExtensionItem;
	statuses.items[3] = unsprungAccItem;
	statuses.items[4] = sprungAccItem;
	statuses2.items[0] = wusStatusCoilItem;
	statuses2.items[1] = wusStatusSprungItem;
	statuses2.items[2] = wusStatusUnsprungItem;
	statuses2.items[3] = speedExceededItem;
	statuses2.items[4] = watchdogFauilureItem;
	statuses2.items[5] = powerFailureItem;
	invokeWusErrors.items[0] = invokeCoilErrorItem;
	invokeWusErrors.items[1] = invokeSprungErrorItem;
	invokeWusErrors.items[2] = invokeUnsprungErrorItem;
	invokeWusErrors.items[3] = invokeSpeedErrorItem;
	invokeWusErrors.items[4] = involePowerErrorItem;
	invokeWusErrors.items[5] = invokeWatchdogErrorItem;

	/*init Activity and attach ListViews ot activity*/
	mainActivity = activity();
	addView(&mainActivity, &controls, VIEWTYPE_LIST);
	addView(&mainActivity, &statuses, VIEWTYPE_LIST);
	addView(&mainActivity, &statuses2, VIEWTYPE_LIST);
	addView(&mainActivity, &invokeWusErrors, VIEWTYPE_LIST);
	attachActivity(&mainActivity);

	/* Configure buttons */
	configureButtonEvent(BUTTON_UP, BUTTON_EVENT_RISING_EDGE);
	configureButtonEvent(BUTTON_DOWN, BUTTON_EVENT_RISING_EDGE);
	configureButtonEvent(BUTTON_LEFT, BUTTON_EVENT_RISING_EDGE);
	configureButtonEvent(BUTTON_RIGHT, BUTTON_EVENT_RISING_EDGE);

	/*Continously determines the actuator force needed*/
	xTaskCreate(vControlTask, "Control task", 240, (void *)placeholder, 4, NULL);

	/*Inits UART, continously reads and writes UART messages*/
	xTaskCreate(vUartTask, "UART task", 240, (void *)placeholder, 3, NULL);

	/*Inits button polling and checks for button pushes*/
	xTaskCreate(vButtonPollingTask, "Button polling task", 240, (void *)placeholder, 2, NULL);

	/* Refreshes GUI */
	xTaskCreate(vGuiRefreshTask, "Gui refresh task", 240, (void *)placeholder, 1, NULL);

	/* Start the scheduler so our tasks start executing. */
	vTaskStartScheduler();

	/* If all is well we will never reach here as the scheduler will now be
	   running.  If we do reach here then it is likely that there was insufficient
	   heap available for the idle task to be created. */
	for (;;) ;
}



void vApplicationMallocFailedHook(void)
{
	/* This function will only be called if an API call to create a task, queue
	   or semaphore fails because there is too little heap RAM remaining. */
	for (;;) ;
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook(TaskHandle_t *pxTask, signed char *pcTaskName)
{
	/* This function will only be called if a task overflows its stack.  Note
	   that stack overflow checking does slow down the context switch
	   implementation. */

	for (;;) ;
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
