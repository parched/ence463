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
#define ADC_TO_IQ(x) ((x) << (QG - 10)) /** This will need to be changed if QG < 10 */

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

	//

	ADCSequenceStepConfigure(ADC_BASE, ADC_SEQ, 0, ADC_CTL_CH0);
	ADCSequenceStepConfigure(ADC_BASE, ADC_SEQ, 1, ADC_CTL_CH1);
	ADCSequenceStepConfigure(ADC_BASE, ADC_SEQ, 2, ADC_CTL_CH2 | ADC_CTL_IE | ADC_CTL_END);

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


_iq getSmoothAdc(char adc, _iq minValue, _iq maxValue)
{
	unsigned long adcOutput;

	switch(adc)
	{
	case 0x01:
		adcOutput = ADCout[0] & ADC_DATA_MASK; break;
	case 0x02:
		adcOutput = ADCout[1] & ADC_DATA_MASK; break;
	case 0x04:
		adcOutput = ADCout[2] & ADC_DATA_MASK; break;
	default:
		return -1ul;
	}

	return minValue + _IQmpy((maxValue - minValue), ADC_TO_IQ(adcOutput));
}


void adcISR (void)
{
	// Clear ADC Interrupt
	ADCIntClear(ADC_BASE, ADC_SEQ);

	// Get Data from the ADC
	ADCSequenceDataGet(ADC_BASE, ADC_SEQ, ADCout);
}
