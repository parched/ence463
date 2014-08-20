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

#define ACC_SPRUNG_EXCEEDED 0x10
#define ACC_UNSPRUNG_EXCEEDED 0x20
#define COIL_EXTENSION_EXCEEDED 0x40
#define CAR_SPEED_EXCEEDED 0x80

/**
 * \brief A state of the simulator.
 */
typedef struct SimState SimState;

/**
 * \brief Resets the simulation.
 *
 * \param simState
 */
void resetSimulation(SimState *simState);

/**
 * \brief Simulates and updates the outputs.
 *
 * \param force The applied force from the controller.
 * \param accel The forward acceleration from the driver.
 * \param dampingFactor The set damping factor.
 * \param roadType The road type.
 * \param simState A pointer to the state of the simulator.
 * \param dTime The time since the last state.
 *
 * \return The status of the car.
 */
char simulate(int force, int accel, int dampingFactor, char roadType, SimState *simState, int dTime);

/**
 * \brief Gets the car's current speed.
 *
 * \param simState A pointer the state of the simulator.
 *
 * \return The car's speed.
 */
int getCarSpeed(const SimState *simState);

/**
 * \brief Gets the wheel rotation speed.
 *
 * \param simState A pointer the state of the simulator.
 *
 * \return The wheel's rotation speed.
 */
int getWheelSpeed(const SimState *simState);

/**
 * \brief Gets the sprung mass' acceleration.
 *
 * \param simState A pointer the state of the simulator.
 *
 * \return The sprung mass' acceleration.
 */
int getSprungAcc(const SimState *simState);

/**
 * \brief Gets the unsprung mass' acceleration.
 *
 * \param simState A pointer the state of the simulator.
 *
 * \return The unsprung mass' acceleration.
 */
int getUnsprungAcc(const SimState *simState);

/**
 * \brief Gets the coil's extension.
 *
 * \param simState A pointer the state of the simulator.
 *
 * \return The coil's extension.
 */
int getCoilExtension(const SimState *simState);

#endif
