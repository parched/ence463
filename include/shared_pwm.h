
/**
 * \file shared_pwm.h
 * \brief Common module allowing configuation of and reading of PWM outputs.
 * \author James Duley
 * \version 1.0
 * \date 2014-08-20
 */

/* Copyright (C)
 * 2014 - James Duley
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

#ifndef SHARED_PWM_H
#define SHARED_PWM_H

#define ACC_SPRUNG_PWM 1
#define ACC_UNSPRUNG_PWM 2
#define COIL_EXTENSION_PWM 4
#define DAMPING_PWM 1
#define ACTUACTOR_PWM 4


/**
 * \brief Initailises the PWM module.
 *
 * \param pwmOuts The bitwise OR of the PWM outputs init.
 */
void initPwmModule(int pwmORed);

/**
 * \brief Sets the PWM duty.
 *
 * \param pwmOut The PWM output of which to set.
 * \param duty The PWM duty cycle times 1000.
 */
void setDuty(int pwmPin, int duty);

#endif
