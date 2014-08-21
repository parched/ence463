/**
 * \file shared_button_task.h
 * \brief Active suspension button configuration and task module
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
typedef enum {BUTTON_EVENT_RISING_EDGE} ButtonEvent;

/**
 * \brief Configures chosen button to listen for selected input event
 *
 * \params button			Button to configure
 * \params eventType		Event type of listen for
 */
void configureButtonEvent(Button button, ButtonEvent eventType);

/**
 * \brief Function of button polling task to be called by the FreeRTOS kernel
 *
 * \params pvParameters		Unused
 */
void vButtonPollingTask(void* pvParameters);

#endif