/**
 * \file shared_adc.c
 * \brief Common module allowing configuration and reading of ADC input.
 * \author Tom Walsh
 * \version 1.0
 * \date 2014-09-01
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

#include "shared_adc.h"
#include "shared_parameters.h"

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_timer.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"

#define BIT(x) 			(1 << x)
#define ADC_FREQ_HZ 	50000
#define ADC_DATA_MASK	0x3FF
#define ADC_SEQ			0
#define ADC_PRIORITY	0
#define ADC_MAX			(1023 * DESIRED_MAX_VOLTAGE / REAL_MAX_VOLTAGE)

static unsigned long ADCout[8];

void adcISR (void);

static void initAdcTimer(void)
{
	// Initialise Timer 1 (Used to trogger ADC) as a Periodic Timer
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
	TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC_UP);

	// Set Timer 1 Load to 50 ksps (ADC Sample Rate)
	TimerLoadSet(TIMER1_BASE, TIMER_A, SysCtlClockGet() / ADC_FREQ_HZ - 1);

	// Enable Timer 1 ADC Trigger
	TimerControlTrigger(TIMER1_BASE, TIMER_A, true);
}

void initAdcModule(char adcs)
{
	// Determine Number of Enabled ADC Pins
	int maxSteps = 0;
	if (adcs == 0x01 || adcs == 0x02 || adcs == 0x04)
		maxSteps = 1;
	else if (adcs == 0x03 || adcs == 0x05 || adcs == 0x06)
		maxSteps = 2;
	else if (adcs == 0x07)
		maxSteps = 3;

	// Configure Timer for ADC Triggering
	initAdcTimer();

	// Enable ADC Peripheral
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC);
	SysCtlDelay(SysCtlClockGet() / 3000);

	// Set ADC Speed to 500ksps Max
	SysCtlADCSpeedSet(SYSCTL_ADCSPEED_500KSPS);

	// Disable Sequence 0 before Configuration
	ADCSequenceDisable(ADC_BASE, ADC_SEQ);

	// Configure  ADC processor for a Timer Trigger and 8x Oversampling
	ADCSequenceConfigure(ADC_BASE, ADC_SEQ, ADC_TRIGGER_TIMER, ADC_PRIORITY);
	ADCHardwareOversampleConfigure(ADC_BASE, 8);

	// Configure ADC Processors
	int step;
	for (step = 0; step < 3; step ++)
	{
		// Check if ADC pin is required
		if (BIT(step) & adcs)
		{
			// Create ADC Config Flags.
			int adcConfig = 0;

			// Select ADC Input
			switch (step)
			{
				case 0:
					adcConfig |= ADC_CTL_CH0; break;
				case 1:
					adcConfig |= ADC_CTL_CH1; break;
				case 2:
					adcConfig |= ADC_CTL_CH2; break;
			}

			// Check if an interrupt should be configured
			if (step == maxSteps - 1)
			{
				adcConfig |= ADC_CTL_IE | ADC_CTL_END;
			}

			// Configure ADC Sample Step
			ADCSequenceStepConfigure(ADC_BASE, ADC_SEQ, step, adcConfig);
		}
	}

	// Enable ADC Sequence
	ADCSequenceEnable(ADC_BASE, ADC_SEQ);

	// Configure, Register and Clear Interrupt
	ADCIntEnable 	(ADC_BASE, ADC_SEQ);
	IntRegister 	(INT_ADC0, adcISR);
	IntEnable 		(INT_ADC0);
	ADCIntClear 	(ADC_BASE, ADC_SEQ);
	IntMasterEnable	();

	// Enable ADC Trigger Timer
	TimerEnable(TIMER1_BASE, TIMER_A);
}


int getSmoothAdc(char adc, int minValue, int maxValue)
{
	int adcOutput = -1;

	switch(adc)
	{
	case 0x01:
		adcOutput = (int) ADCout[0] & ADC_DATA_MASK; break;
	case 0x02:
		adcOutput = (int) ADCout[1] & ADC_DATA_MASK; break;
	case 0x04:
		adcOutput = (int) ADCout[2] & ADC_DATA_MASK; break;
	}

	return minValue + ((maxValue - minValue) * adcOutput / ADC_MAX);
}


void adcISR (void)
{
	// Clear ADC Interrupt
	ADCIntClear(ADC_BASE, ADC_SEQ);

	// Get Data from the ADC
	ADCSequenceDataGet(ADC_BASE, ADC_SEQ, ADCout);
}
