/**
 * \file shared_uart_task.h
 * \brief Common task module which handles UART message reception and queuing of sending jobs.
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

#ifndef SHARED_UARTTASK_H
#define SHARED_UARTTASK_H

#define UART_FRAME_SIZE 6

/**
 * \struct _UartFrame
 * \brief Structure of the UART message
 */
typedef struct
{
	char msgType;
	char msg[UART_FRAME_SIZE];
} _UartFrame;

/**
 * \union UartFrame
 * \brief Allows UART message to be accessed with structure or as a bit stream
 */
typedef union
{
	_UartFrame frameWise;                   /**< Allows access with structure */
	unsigned char byteWise[UART_FRAME_SIZE + 1]; /**< Allows access as a byte stream */
} UartFrame;

/**
 * \brief Function type of callbacks for this module
 */
typedef void (*uartCallback)(UartFrame *);

/**
 * \brief Function of the UART Task to be called by the FreeRTOS kernel
 *
 * \param pvParameters		Unused
 */
void vUartTask(void *pvParameters);

/**
 * \brief Attach callback to be executed on a UART message receive event
 *
 * \param callback			Callback to execute for receive event
 */
void attachOnReceiveCallback(void (*callback)(UartFrame *));

/**
 * \brief Queues message to be sent out via UART
 * \public \memberof UartFrame
 *
 * \return 0 for success, -1 if queue is full
 */
int queueMsgToSend(UartFrame *uartFrame);

/**
 * \brief Returns no. of slots left on the queue
 *
 * \return Number of spaces left on queue
 */
int getSendQueueAvailSpaces(void);


#endif
