/**
 * \file wus_simulator.c
 * \brief Wheel unit simulator implementation.
 * \author James Duley
 * \version 1.0
 * \date 2014-09-01
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

#include "wus_simulator.h"

struct SimState {
	int speed;
	int zR;
	int zU;
	int zS;
	int vR;
	int vU;
	int vS;
};

void resetSimulation(SimState *simState) {
}

char simulate(SimState *simState, int force, int throttle, int dampingFactor, char roadType, int dTime) {
	return 0;
}

int getSpeed(const SimState *simState) {
	return simState->speed;
}

int getSprungAcc(const SimState *simState) {
	return 0;
}

int getUnsprungAcc(const SimState *simState) {
	return 0;
}

int getCoilExtension(const SimState *simState) {
	return 0;
}
