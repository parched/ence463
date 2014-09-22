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

static int ADCout[3];

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

	// Disable Sequence 0-2 before Configuration
	ADCSequenceDisable(ADC_BASE, 0);
	ADCSequenceDisable(ADC_BASE, 1);
	ADCSequenceDisable(ADC_BASE, 2);

	// Configure ADC Processors
	int sequence = 0;
	int step;
	for (step = 0; step < 3; step ++)
	{
		// Check if ADC pin is required
		if (BIT(step) & adcs)
		{
			// Create ADC Config Flags.
			// Only one step per sequence so Sequence End flag set by default.
			int adcConfig = ADC_CTL_END;

			// Configure next ADC processor for a Processor Trigger and 4x Oversampling
			ADCSequenceConfigure(ADC_BASE, sequence, ADC_TRIGGER_TIMER, sequence);
			ADCSoftwareOversampleConfigure(ADC_BASE, sequence, 4);

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
				adcConfig |= ADC_CTL_IE;

				// Configure, Register and Clear Interrupt
				ADCIntEnable 	(ADC_BASE, sequence);
				IntRegister 	(INT_ADC0 + sequence, adcISR);
				IntEnable 		(INT_ADC0 + sequence);
				ADCIntClear 	(ADC_BASE, sequence);
			}

			// Configure ADC Sample Step
			ADCSoftwareOversampleStepConfigure(ADC_BASE, sequence, 0, adcConfig);

			// Enable ADC Sequence
			ADCSequenceEnable(ADC_BASE, sequence);

			sequence ++;
		}
	}
}


int getSmoothAdc(char adc)
{
	char channel;

	switch(adc)
	{
	case 0x01:
		channel = 0; break;
	case 0x02:
		channel = 1; break;
	case 0x04:
		channel = 2; break;
	}

	return ADCout[channel];
}

void adcISR (void)
{
	// Clear ADC Interrupts (All of them, because all lead to this ISR)
	ADCIntClear(ADC_BASE, 0);
	ADCIntClear(ADC_BASE, 1);
	ADCIntClear(ADC_BASE, 2);

	// Get Data from the ADC
	unsigned long ADCraw[3];
	ADCSoftwareOversampleDataGet(ADC_BASE, 0, &ADCraw[0], 4);
	ADCSoftwareOversampleDataGet(ADC_BASE, 1, &ADCraw[1], 4);
	ADCSoftwareOversampleDataGet(ADC_BASE, 2, &ADCraw[2], 4);

	// Mask and Convert to integer
	// All ADC Data in lower 10 bits of 32-bit ADC output
	// according to Stellarisware ADC App Note.
	ADCout[0] = ADCraw[0] & ADC_DATA_MASK;
	ADCout[1] = ADCraw[1] & ADC_DATA_MASK;
	ADCout[2] = ADCraw[2] & ADC_DATA_MASK;
}
