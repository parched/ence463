/**
 * \file wus_simulate_task.h
 * \brief Wheel unit simulation task.
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

#ifndef WUS_SIMULATE_TASK_H
#define WUS_SIMULATE_TASK_H

#include "shared_tracenode.h"

/**
 * \brief The simulation task.
 *
 * \param params Unused parameters.
 */
void vSimulateTask(void *params);

/**
 * \brief Gets the speed for display.
 *
 * \return The speed.
 */
int getDisplaySpeed();

/**
 * \brief Gets the sprung mass acceleration for display.
 *
 * \return The acceleration.
 */
int getDisplaySprungAcc();

/**
 * \brief Gets the unsprung mass acceleration for display.
 *
 * \return The acceleration.
 */
int getDisplayUnsprungAcc();

/**
 * \brief Gets the coil extension for display.
 *
 * \return The coil extension.
 */
int getDisplayCoilExtension();

/**
 * \brief Set the road buffer for writing to.
 *
 * \param roadBuffer A pointer to the buffer.
 */
void setRoadBuffer(CircularBufferHandler *roadBuffer);

/**
 * \brief Get road status.
 *
 */
int getStartStatusDisplay();

/**
 * \brief Get road type status
 *
 */
int getRoadTypeStatusDisplay();

/**
 * \brief Get throttle status
 *
 */
int getThrottleStatusDisplay();

/**
 * \brief Get Coil extension errror involed from ASC
 *
 */
int getCoilExErrorInvoked();

/**
 * \brief Get unsprung acceration error involed from ASC
 *
 */
int getUnsprungAccErrorInvoked();

/**
 * \brief Get sprung accearation error involed from ASC
 *
 */
int getSprungAccErrorInvoked();

/**
 * \brief Get car speed error involed from ASC
 *
 */
int getCarSpeedErrorInvoked();

/**
 * \brief Get power failure error involed from ASC
 *
 */
int getPowerFailureInvoked();

/**
 * \brief Get watchdog timer failure error involed from ASC
 *
 */
int getWatchdogTimerFailureInvoked();

#endif
