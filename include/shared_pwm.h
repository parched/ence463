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

#include "shared_iqmath.h"

#define ACC_SPRUNG_PWM 0x01      /**< The sprung acceleration PWM. */
#define ACC_UNSPRUNG_PWM 0x02    /**< The unsprung acceleration PWM. */
#define COIL_EXTENSION_PWM 0x04  /**< The coil extension PWM. */
#define ACTUATOR_FORCE_PWM 0x01  /**< The actuator force PWM. */
#define DAMPING_COEFF_PWM 0x02   /**< The damping coefficient PWM. */

/**
 * \brief Initailises the PWM module.
 *
 * \param pwmOuts The bitwise OR of the PWM outputs init.
 */
void initPwmModule(char pwmOuts);

/**
 * \brief set the pwm dutycycle for the PWM of (value - minValue) / (maxValue - minValue) for the PWM pin passed to it
 *
 * \param pwmPin The PWM output of which to set.
 * \param value must be inbetween minValue and maxValue.
 * \param minValue minimum value for value
 * \param maxValue maximum value for value
 */
void setDuty(char pwmPin, _iq value, _iq minValue, _iq maxValue);

#endif
