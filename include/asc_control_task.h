/**
 * \file asc_control_task.h
 * \brief The active suspension controller task.
 * \author James Duley
 * \version 1.0
 * \date 2014-08-22
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

#ifndef ASC_CONTROL_TASK_H
#define ASC_CONTROL_TASK_H

typedef enum {SEDATE, NORMAL, SPORT, RALLY} rideType;

/**
 * \brief The active suspension control task.
 *
 * \param params Unused parameters.
 */
void vControlTask(void *params);

/**
 * \brief Sets the car ride type.
 *
 * \param rideType Ride Type.
 */
void setRideType(rideType rideModeIn);

#endif
