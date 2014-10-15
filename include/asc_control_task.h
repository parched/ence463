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
void setRideMode(int rideModeIn);

/**
 * \brief Sets active suspension on or off.
 *
 * \param rideType Ride Type.
 */
void setAscOn(int isOn);

/**
 * \brief Sets rideType.
 *
 * \param rideType Ride Type.
 */
void setRoadType(int roadTypeInput);

/**
 * \brief Sets throttle.
 *
 * \param int roadType.
 */
void setThrottle(int throttleInput);

/**
 * \brief Sets the reset state.
 *
 * \param int throttleInput.
 */
void setResetState(int resetStateInput);

/**
 * \brief Sets the invoked coil error state.
 *
 * \param int errorInput.
 */
void setCoilError(int errorInput);

/**
 * \brief Sets the sprung error state.
 *
 * \param int errorInput.
 */
void setSprungError(int errorInput);

/**
 * \brief Sets the unsprung error state.
 *
 * \param int errorInput.
 */
void setUnsprungError(int errorInput);

/**
 * \brief Sets the speed error state.
 *
 * \param int errorInput.
 */
void setSpeedError(int errorInput);

/**
 * \brief Sets the power error state.
 *
 * \param int errorInput.
 */
void setPowerError(int errorInput);

/**
 * \brief Sets the watchdog error state.
 *
 * \param int errorInput.
 */
void setWatchdogError(int errorInput);

/**
 * \brief Gets active suspension on or off.
 *
 * \param rideType Ride Type.
 */
int getDisplayRideMode();

/**
 * \brief Gets the car speed.
 *
 * \return Car Speed (m/s).
 */
int getDisplaySpeed();

/**
 * \brief Gets Sprung Acceration.
 *
 * \return Sprung Acceleration.
 */
int getDisplaySprungAcc();

/**
 * \brief Gets the unsprung acceration
 *
 * \return Unsprung Acceleration.
 */
int getDisplayUnsprungAcc();

/**
 * \brief Gets the coil extension
 *
 * \return Coil Extension.
 */
int getDisplayCoilExtension();

/**
 * \brief Gets the actuator force
 *
 * \return Actuator Force (N).
 */
int getDisplayForce();

/**
 * \brief Gets the damping coeffient
 *
 * \return Damping Coefficient (Nm/s).
 */
int getDisplayDampingCoefficient();

/**
 * \brief Gets the road type.
 *
 * \return The road type.
 */
int getRoadType();

/**
 * \brief Gets throttle.
 *
 * \return throttle
 */
int getThrottle();

/**
 * \brief Gets the resetState
 *
 * \return Reset state
 */
int getResetState();

/**
 * \brief Gets the coil extenstion error status
 *
 * \return Coil extension error state
 */
int getCoilExError();

/**
 * \brief Gets unspurng acceration error state.
 *
 * \return Unsprung acceration error state.
 */
int getUnsprungAccError();

/**
 * \brief Gets the sprung acceration error state
 *
 * \return Sprung acceration error state.
 */
int getSprungAccError();

/**
 * \brief Gets the car speed error state
 *
 * \return Car speed error state.
 */
int getCarSpeedError();

/**
 * \brief Gets Asc on state
 *
 * \return Asc on state.
 */
int getAscOn();

/**
 * \brief Gets coil invoked error
 *
 * \return Involed coil error
 */
int getCoilInvokedError();

/**
 * \brief Gets invoked sprung error
 *
 * \return Invoked sprung error
 */
int getSprungInvokedError();

/**
 * \brief Gets invoked unsprung error
 *
 * \return Invoked unsprung error
 */
int getUnsprungInvokedError();

/**
 * \brief Gets invoked speed error
 *
 * \return Invoked speed error
 */
int getSpeedInvokedError();

/**
 * \brief Gets involed power error
 *
 * \return Invoked power error
 */
int getPowerInvokedError();

/**
 * \brief Gets invoked watchdog error
 *
 * \return Invoked watchdog error
 */
int getWatchdogInvokedError();

/**
 * \brief Gets power error
 *
 * \return Invoked power error
 */
int getPowerError();

/**
 * \brief Gets watchdog error
 *
 * \return Invoked watchdog error
 */
int getWatchdogError();

#endif
