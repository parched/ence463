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
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"
#include "driverlib/interrupt.h"

#define BIT(x) (1 << x)

void adcISR (void);


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

	// Enable ADC Peripheral
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC);
	SysCtlDelay(SysCtlClockGet() / 3000);

	// Set ADC Speed to 500ksps Max
	SysCtlADCSpeedSet(SYSCTL_ADCSPEED_500KSPS);

	// Disable Sequence 0 before Configuration
	ADCSequenceDisable(ADC_BASE, 0);

	// Configure Sequence 0 for a Processor Trigger and 8x Oversampling
	ADCSequenceConfigure(ADC_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
	ADCSoftwareOversampleConfigure(ADC_BASE, 0, 8);

	// Configure ADC Sequence Steps
	int stepItr;
	int stepNum = 0;
	for (stepItr = 0; stepItr < 3; stepItr ++)
	{
		int ADCconfig 	= 0;
		int ADCinput	= BIT(stepItr);

		// Check if ADC pin is required
		if (ADCinput & adcs)
		{
			// Enable Channel
			switch (ADCinput)
			{
				case BIT(0):
					ADCconfig = ADC_CTL_CH0; break;
				case BIT(1):
					ADCconfig = ADC_CTL_CH1; break;
				case BIT(2):
					ADCconfig = ADC_CTL_CH2; break;
			}

			// Enable Interrupt on last Step
			if (stepNum >= maxSteps - 1)
				ADCconfig |= ADC_CTL_IE | ADC_CTL_END;

			// Configure Sequence Step
			ADCSoftwareOversampleStepConfigure(ADC_BASE, 0, stepNum, ADCconfig);
			stepNum ++;
		}
	}

	// Enable ADC Sequence
	ADCSequenceEnable(ADC_BASE, 0);

	// Enable ADC Interrupts, register and enable ISR
	ADCIntEnable(ADC_BASE, 0);
	IntRegister(INT_ADC0SS0, adcISR);
	IntEnable(INT_ADC0SS0);

	// Clear ADC Interrupt
	ADCIntClear(ADC_BASE, 0);
}


int getSmoothAdc(char adc)
{
	unsigned long adcValues[3];
	ADCSoftwareOversampleDataGet(ADC_BASE, 0, adcValues, 8);

	//TODO: Store Data

	return 0;
}

void adcISR (void)
{
	ADCIntClear(ADC_BASE, 0);

	//TODO: Schedule ADC Data read
}
