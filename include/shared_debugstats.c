/**
 * \file shared_debugstats.c
 * \brief Module to determine CPU usage statistics
 * \author George Xian
 * \version 1.0
 * \date 2014-10-15
 */

/* Copyright (C)
 * 2014 - George Xian
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

#include "shared_debugstats.h"

#include "FreeRTOS.h"
#include "task.h"

#include <stdint.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_timer.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"

void initDebugStats(void)
{
	// Enable and Configure Timer 2 Peripheral
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);
	SysCtlDelay(SysCtlClockGet() / 3000);

	TimerConfigure(TIMER2_BASE, TIMER_CFG_PERIODIC);
	TimerLoadSet(TIMER2_BASE, TIMER_A, 0xFFFFFFFF);
	TimerPrescaleSet(TIMER2_BASE, TIMER_A, 255);
	TimerEnable(TIMER2_BASE, TIMER_A);
}

int getNumberOfTasks(void)
{
	return uxTaskGetNumberOfTasks();
}

int getTasksCPULoad(void)
{
	TaskStatus_t *pxTaskStatusArray;
	volatile UBaseType_t uxArraySize;
	uint32_t ulTotalRunTime;
	unsigned int totalCPUPercentage = 0;
	int ret = -1;

	if (pxTaskStatusArray != NULL)
	{
		uxArraySize = uxTaskGetSystemState(pxTaskStatusArray, uxArraySize, &ulTotalRunTime);

		/* For percentage calculations */
		ulTotalRunTime /= 100UL;

		/* Avoid divide by zero errors */
		if (ulTotalRunTime > 0)
		{
			unsigned int i;
			for (i=0; i<uxArraySize; i++)
			{
				totalCPUPercentage += (pxTaskStatusArray[i].ulRunTimeCounter / ulTotalRunTime);
			}
			ret = totalCPUPercentage /= uxArraySize;
		}
		vPortFree(pxTaskStatusArray);
	}
	return ret;
}


