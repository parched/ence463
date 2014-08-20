/**
 * \file control.h
 * \brief Active suspension control.
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

#ifndef CONTROL_H
#define CONTROL_H

/**
 * \brief Current controller state.
 */
typedef struct ControlState ControlState;

/**
 * \brief Resets or initialises the controller state.
 *
 * \param controlState A pointer to the controller state.
 */
void resetState(ControlState *controlState);

/**
 * \brief Updates the controller state and gets the required actuator force.
 *
 * \param controlState A pointer to the controller state.
 * \param accSprung The sprung mass' acceleration.
 * \param accUnsprung The unsprung mass' acceleration.
 * \param coilExtension The coil extension.
 * \param wheelSpeed The wheel's rotation speed.
 * \param damping The damping coeffiecient.
 * \param dTime The time passed since the last update.
 *
 * \return The required actuator force.
 */
int getAcutatorForce(ControlState *controlState, int accSprung, int accUnsprung, int coilExtension, int wheelSpeed, int damping, int dTime);

#endif
