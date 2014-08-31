/**
 * \file shared_button_task.h
 * \brief Common task module which handles push button input events.
 * \author George Xian
 * \version 1.0
 * \date 2014-08-21
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

#ifndef SHARED_BUTTON_TASK_H
#define SHARED_BUTTON_TASK_H

typedef enum {BUTTON_UP, BUTTON_DOWN, BUTTON_LEFT, BUTTON_RIGHT, BUTTON_SELECT} Button;
typedef enum {BUTTON_EVENT_RISING_EDGE, BUTTON_EVENT_FALLING_EDGE} ButtonEvent;

#define BIT(x)		(1 << x)

#define NUM_BUTTONS	5
#define TOTAL_MAX	250
#define TOTAL_MIN	0

#include "shared_guidraw_task.h"

/**
 * \brief Configures chosen button to listen for selected input event
 *
 * \param button Button to configure
 * \param eventType	Event type of listen for
 */
void configureButtonEvent(Button button, ButtonEvent eventType);

/**
 * \brief Function of button polling task to be called by the FreeRTOS kernel
 *
 * \param pvParameters Unused
 */
void vButtonPollingTask(void* pvParameters);

#endif
