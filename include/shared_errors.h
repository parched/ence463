/**
 * \file shared_errors.h
 * \brief file List of shared error codes common to WUS and ASC
 * \author Hugh Bisset
 * \version 1.0
 * \date 2014-10-12
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

#ifndef SHARED_ERRORS_H_
#define SHARED_ERRORS_H_

#define ACC_SPRUNG_EXCEEDED 0x10        /**< Sprung acceleration limit exceeded error. */
#define ACC_UNSPRUNG_EXCEEDED 0x20      /**< Unsprung acceleration limit exceeded error. */
#define COIL_EXTENSION_EXCEEDED 0x40    /**< Coil extension limit exceeded error. */
#define CAR_SPEED_EXCEEDED 0x80         /**< Car speed limit exceeded error. */

#endif /* SHARED_ERRORS_H_ */
