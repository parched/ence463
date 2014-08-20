/**
 * \file simulate.h
 * \brief A wheel unit simulator.
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

#ifndef SIMULATE_H
#define SIMULATE_H

/**
 * \brief A state of the simulator.
 */
typedef struct {
	int wheelSpeed;	/**< The wheel rotation speed */
	int zR;        	/**< The road vertical displacement */
	int vR;        	/**< The road vertical velocity */
	int zU;        	/**< The unsprung mass vertical dispalcement */
	int vU;        	/**< The unsprung mass vertical velocity */
	int zS;        	/**< The sprung mass vertical dispalcement */
	int vS;        	/**< The sprung mass vertical velocity */
} SimState;

/**
 * \brief Simulates and updates the outputs.
 *
 * \param force The applied force from the controller.
 * \param accel The forward acceleration from the driver.
 * \param dampingFactor The set damping factor.
 * \param roadType The road type.
 * \param lastState The last state of the simulator.
 * \param dTime The time since the last state.
 *
 * \return The new simulator state.
 */
SimState simulate(int force, int accel, int dampingFactor, char roadType, SimState lastState, int dTime);

#endif
