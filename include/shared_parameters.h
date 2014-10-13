/**
 * \file shared_parameters.h
 * \brief Shared paramters of the system.
 * \author James Duley
 * \version 1.0
 * \date 2014-10-08
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


#ifndef SHARED_PARAMETERS_H
#define SHARED_PARAMETERS_H

#include "shared_iqmath.h"

#define MAX_ACC_SPRUNG _IQ(20)         /**< The maximum sprung acceleration (m/s/s). */
#define MAX_ACC_UNSPRUNG _IQ(1000)     /**< The maximum unsprung acceleration (m/s/s). */
#define MAX_COIL_EXTENSION _IQ(250)    /**< The maximum coil extension (mm). */
#define MAX_ACTUATOR_FORCE _IQ(1000)   /**< The maximum actuator force (N). */
#define MAX_DAMPING_COEFF _IQ(500)     /**< The maximum damping coefficient (N.s/m). */
#define MAX_SPEED _IQ(55.5555)                 /**< The maximum speed (m/s).*/

#define MIN_ACC_SPRUNG _IQ(-20)        /**< The minimum sprung acceleration (m/s/s). */
#define MIN_ACC_UNSPRUNG _IQ(-1000)    /**< The minimum unsprung acceleration (m/s/s). */
#define MIN_COIL_EXTENSION _IQ(-250)   /**< The minimum coil extension (mm). */
#define MIN_ACTUATOR_FORCE _IQ(-1000)  /**< The minimum actuator force (N). */
#define MIN_DAMPING_COEFF _IQ(100)     /**< The minimum damping coefficient (N.s/m). */
#define MIN_SPEED _IQ(0)                     /**< The minimum speed (m/s).*/

#define REAL_MAX_VOLTAGE 33               /**< The actual maximum voltage for the ADC and PWM (0.1V). */
#define DESIRED_MAX_VOLTAGE 30            /**< The desired maximum voltage for the ADC and PWM (0.1V). */

#define ON_MASS_SPRUNG /250               /**< Divide by the sprung mass (kg). */
#define ON_MASS_UNSPRUNG *2/73            /**< Divide by the unsprung mass (kg). */
#define STIFFNESS_SPRING 25               /**< The stiffness of the spring (N/mm). */
#define STIFFNESS_TYRE 100                /**< The stiffness of the tyre (N/mm). */
#define DAMPING_TYRE 50000                /**< The damping coefficient of the tyre (N.s/m). */

#endif /* SHARED_PARAMETERS_H */
