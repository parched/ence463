/**
 * \file wus_simulator.h
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

#ifndef WUS_SIMULATOR_H
#define WUS_SIMULATOR_H

#define ACC_SPRUNG_EXCEEDED 0x10        /**< Sprung acceleration limit exceeded error. */
#define ACC_UNSPRUNG_EXCEEDED 0x20      /**< Unsprung acceleration limit exceeded error. */
#define COIL_EXTENSION_EXCEEDED 0x40    /**< Coil extension limit exceeded error. */
#define CAR_SPEED_EXCEEDED 0x80         /**< Car speed limit exceeded error. */

/**
 * \struct SimState
 * \brief A state of the simulator.
 */
typedef struct {
	/** \privatesection */
	int speed;  /**< The car speed. */
	int zR;     /**< The road displacement. */
	int zU;     /**< The unsprung mass displacement. */
	int zS;     /**< The sprung mass dispalcement. */
	int vR;     /**< The road velocity. */
	int vU;     /**< The unsprung mass velocity. */
	int vS;     /**< The sprung mass velocity. */
} SimState;

/**
 * \brief Creates a SimState.
 * \public \memberof SimState
 *
 * \return A SimState.
 */
SimState simState();

/**
 * \brief Resets the simulation.
 * \public \memberof SimState
 */
void resetSimulation(SimState *simState);

/**
 * \brief Simulates and updates the state.
 * \public \memberof SimState
 *
 * \param force The applied force from the controller.
 * \param throttle The forward acceleration from the driver.
 * \param dampingFactor The set damping factor.
 * \param roadType The road type.
 * \param dTime The time since the last state.
 *
 * \return The error statuses of the car.
 */
char simulate(SimState *simState, int force, int throttle, int dampingFactor, char roadType, int dTime);

/**
 * \brief Gets the car's current speed.
 * \public \memberof SimState
 *
 * \return The car's speed.
 */
int getSpeed(const SimState *simState);

/**
 * \brief Gets the sprung mass' acceleration.
 * \public \memberof SimState
 *
 * \return The sprung mass' acceleration.
 */
int getSprungAcc(const SimState *simState);

/**
 * \brief Gets the unsprung mass' acceleration.
 * \public \memberof SimState
 *
 * \return The unsprung mass' acceleration.
 */
int getUnsprungAcc(const SimState *simState);

/**
 * \brief Gets the coil's extension.
 * \public \memberof SimState
 *
 * \return The coil's extension.
 */
int getCoilExtension(const SimState *simState);

#endif
