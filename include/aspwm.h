/**
 * \file aspwm.h
 * \brief Active suspension PWM module.
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

#ifndef ASPWM_H
#define ASPWM_H

#define ACC_SPRUNG_PWM 0x01
#define ACC_UNSPRUNG_PWM 0x02
#define COIL_EXTENSION_PWM 0x04
#define ACTUATOR_FORCE_PWM 0x01
#define DAMPING_COEFF_PWM 0x02

/**
 * \brief Initailises the PWM module.
 *
 * \param pwmOuts The PWM output of which to init, OR of some XXXX_PWM.
 */
void initPwmModule(char pwmOuts);

/**
 * \brief Sets the PWM duty.
 *
 * \param pwmOut The PWM output of which to set, one of XXXX_PWM.
 * \param duty The PWM duty cycle times 1000.
 */
void setPulseWidth(char pwmOut, int duty);

#endif
