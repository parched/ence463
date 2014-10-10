/**
 * \file shared_pwm.c
 * \brief Common module allowing configuation of and reading of PWM outputs.
 * \author Hugh Bisset
 * \version 1.0
 * \date 26/08/2014
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

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pwm.h"
#include "driverlib/sysctl.h"

#include "shared_pwm.h"
#include "shared_parameters.h"

//so that we dont go above 3 Volts
#define MAX_DUTY 1000
#define FREQ_HZ 100000

static unsigned long ulPeriod;

typedef struct  {
    unsigned long pwmOut;
    unsigned long pwmOutBit;
    /*needed as setting the PWM value without initizing the PWM pin will work if the sterllaris previously enabled
     *the pwm pin previously even after you supposedly erased sterllaris to get new program on. A power disconnect
     *fixes this but even then when setting a PWM pin duty cycle without initizing it causes a small (roughly 0.1Volts)
     *PWM noise on the pin.*/
    int enableFlag;
} PwmPin;


static PwmPin pwm1 = {PWM_OUT_1,PWM_OUT_1_BIT,0};
static PwmPin pwm4 = {PWM_OUT_4,PWM_OUT_4_BIT,0};
static PwmPin pwm5 = {PWM_OUT_5,PWM_OUT_5_BIT,0};


/*init the PWM Module*/
void initPwmModule(char pwmORed) {
	//set the PWM frequency to 100000KHz
	ulPeriod = SysCtlClockGet() / FREQ_HZ;

	//set PWM clock to system's clock and enables the PWM perhiferial
	SysCtlPWMClockSet(SYSCTL_PWMDIV_1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM);

	if((pwmORed %2) != 0) { //Sets up PWM1
		PWMGenConfigure(PWM_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
		PWMGenPeriodSet(PWM_BASE, PWM_GEN_0, ulPeriod);
		PWMGenEnable(PWM_BASE, PWM_GEN_0);
		SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
		GPIOPinTypePWM(GPIO_PORTD_BASE, GPIO_PIN_1);
		pwm1.enableFlag = 1;
	}
	if(pwmORed !=1 ) { //Sets up PWM4 and/or PWM5 if needed
		PWMGenConfigure(PWM_BASE, PWM_GEN_2, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
		PWMGenPeriodSet(PWM_BASE, PWM_GEN_2, ulPeriod);
		PWMGenEnable(PWM_BASE, PWM_GEN_2);
		SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
		//dont want to not be able to use a pin if we not using it for pwm
		if(pwmORed == 3 || pwmORed == 2) { //Sets up PWM 4 case
			GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_2);
			pwm4.enableFlag = 1;
		} else if(pwmORed == 4 || pwmORed == 5) { //Sets uo PWM 3 case
			GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_3);
			pwm5.enableFlag = 1;
		} else { //sets up both PWM4 and PWM5 case
			GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_2);
			GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_3);
			pwm4.enableFlag = 1;
			pwm5.enableFlag = 1;
		}
	}
	//Regardless, default the PWM output on all pins to 0 duty cycle else you get junk from PWM from previous programs
	PWMOutputState(PWM_BASE, PWM_OUT_1_BIT | PWM_OUT_4_BIT| PWM_OUT_5_BIT, false);
}


//set the pwm dutycycle for the PWM of (value - minValue) / (maxValue - minValue) for the PWM pin passed to it
void setDuty(char pwmPin, int value, int minValue, int maxValue) {
	PwmPin pwmToChange;
	if(pwmPin == 1 && pwm1.enableFlag == 1) {
		pwmToChange = pwm1;
	} else if(pwmPin == 2 && pwm4.enableFlag == 1) {
		pwmToChange = pwm4;
	} else if(pwmPin == 4 && pwm5.enableFlag == 1) {
		pwmToChange = pwm5;
	} else {
		//bad pwmPin input case
		return;
	}

	//for the case of bad input that is higher than maxValue
	if(value > maxValue) {
		value = maxValue;
	}

	if(value > minValue) {
		unsigned long pulseWidth = ulPeriod * (value - minValue) * DESIRED_MAX_VOLTAGE / ((maxValue - minValue) * REAL_MAX_VOLTAGE);
		PWMPulseWidthSet(PWM_BASE, pwmToChange.pwmOut, pulseWidth);
		PWMOutputState(PWM_BASE, pwmToChange.pwmOutBit, true);
	} else { //disable PWM output in order to get 0V
		PWMOutputState(PWM_BASE, pwmToChange.pwmOutBit, false);
	}
}
