/*
 * Wheel simulation module.
 *
 * Copyright 2014 James Duley
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
