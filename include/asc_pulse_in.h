/**
 * \file asc_pulse_in.h
 * \brief Active suspension control pulse train reader module.
 * \author James Duley
 * \version 1.0
 * \date 2014-08-21
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

#ifndef ASC_PULSE_IN_H
#define ASC_PULSE_IN_H

/**
 * \brief Initialises the pulse train reader module.
 */
void initPulseIn();

/**
 * \brief Gets the car speed.
 *
 * \return The car's speed in 0.1 kph.
 */
int getPulseSpeed();

#endif
