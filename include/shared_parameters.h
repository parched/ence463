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

#define TO_FP << 16
#define FROM_FP >> 16

#define MAX_ACC_SPRUNG (20 TO_FP)         /**< The sprung acceleration. */
#define MAX_ACC_UNSPRUNG (1000 TO_FP)     /**< The unsprung acceleration. */
#define MAX_COIL_EXTENSION (250 TO_FP)    /**< The coil extension. */
#define MAX_ACTUATOR_FORCE (1000 TO_FP)   /**< The actuator force. */
#define MAX_DAMPING_COEFF (500 TO_FP)     /**< The damping coefficient. */
#define MAX_SPEED (200 TO_FP)             /**< Maximum speed */

#define MIN_ACC_SPRUNG (-20 TO_FP)        /**< The sprung acceleration. */
#define MIN_ACC_UNSPRUNG (-1000 TO_FP)    /**< The unsprung acceleration. */
#define MIN_COIL_EXTENSION (-250 TO_FP)   /**< The coil extension. */
#define MIN_ACTUATOR_FORCE (-1000 TO_FP)  /**< The actuator force. */
#define MIN_DAMPING_COEFF (100 TO_FP)     /**< The damping coefficient. */
#define MIN_SPEED (0 TO_FP)               /**< Maximum speed */

#endif /* SHARED_PARAMETERS_H */
