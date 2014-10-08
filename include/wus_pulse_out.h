/**
 * \file wus_pulse_out.h
 * \brief Wheel unit simulator pulse train module.
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

#ifndef WUS_PULSE_OUT_H
#define WUS_PULSE_OUT_H

/**
 * \brief Initialises the pulse train generator.
 */
void initPulseOut();

/**
 * \brief Sets the car speed.
 *
 * \param speed The speed.
 */
void setPulseSpeed(int speed);

#endif
