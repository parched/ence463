/**
 * \file shared_debugstats.c
 * \brief Module to determine CPU usage statistics
 * \author George Xian
 * \version 1.0
 * \date 2014-10-15
 */

/* Copyright (C)
 * 2014 - George Xian
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

/**
 * \brief Initialize timer for debugging
 */
void initDebugStats(void);

/**
 * \brief Get number of tasks currently running
 *
 * \return Number of tasks currently running
 */
int getNumberOfTasks(void);

/**
 * \brief Get combined CPU load
 *
 * \return CPU load in percentage if valid, -1 for error
 */
int getTasksCPULoad(void);
