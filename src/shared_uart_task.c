/**
 * \file shared_uart_task.c
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

#include "shared_uart_task.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"

#ifndef NULL
#define NULL ( (void *) 0)
#endif

#define SENDMESSAGE_QUEUE_SIZE 10
#define UART_TASK_RATE_HZ 15000
#define UART_BAUD 625000

typedef struct UartFrameNode UartFrameNode;

/**
 * \struct UartFrameNode
 *
 * \brief Wrapper for UartFrame to be used as linked list or circular buffer
 */
struct UartFrameNode {
	UartFrame frame;
	UartFrameNode* next;
};

/**
 * \struct UartFrameBufferHandler
 *
 * \brief Handler for UartFrame queues
 */
typedef struct {
	UartFrameNode* head;
	UartFrameNode* tail;
	unsigned int unsent;
	unsigned int size;
} UartFrameQueueHandler;

/**
 * \brief Creates a queue from an array of UartFrameNodes
 *
 * \param head Pointer to the first item
 * \param size Size of the queue desired
 */
UartFrameQueueHandler createUartFrameQueue(UartFrameNode* head, unsigned int size);

/**
 * \brief Writes the pointer to the UartFrame at the head of the queue
 *
 * \param head The buffer to write the pointer value to
 * \return 0 for success, -1 for empty queue
 */
int uartQueuePop(UartFrame* head);


static uartCallback receivedCallback;
static UartFrameNode uartNodes[SENDMESSAGE_QUEUE_SIZE];
static UartFrameQueueHandler uartQueue;


void vUartTask(void* pvParameters) {
	// setup Uart peripheral
	UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), UART_BAUD, UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE);

	// initialize queue
	uartQueue = createUartFrameQueue(uartNodes, SENDMESSAGE_QUEUE_SIZE);

	// initialize FreeRTOS sleep parameters
	portTickType xLastWakeTime;
	const portTickType xTimeIncrement = configTICK_RATE_HZ / UART_TASK_RATE_HZ;
	xLastWakeTime = xTaskGetTickCount();

	for (;;) {
		vTaskDelayUntil(&xLastWakeTime, xTimeIncrement);

		// process message send queue
		UartFrame* toSend;
		while (uartQueuePop(toSend) == 0) {
			unsigned int msgLen = 0;
			switch(toSend->frameWise.msgType)
			{
				case('W'):
					msgLen = 2;
					break;
				case('R'):
					msgLen = 2;
					break;
				case('S'):
					msgLen = 1;
					break;
				case('A'):
					msgLen = 5;
					break;
				case('M'):
					msgLen = 2;
					break;
				default:
					break;
			}

			unsigned int i = 0;
			for (i=0; i<msgLen; i++) {
				// send characters individually
				UARTCharPut(UART1_BASE, toSend->byteWise[i]);
			}
		}

		// receive messages
		UartFrame buffer;
		unsigned int index = 0;
		while (UARTCharsAvail(UART1_BASE)) {
			buffer.byteWise[index] = (char) UARTCharGet(UART1_BASE);
			index++;
		}
		receivedCallback(&buffer);
	}
}

void attachOnReceiveCallback(void (*callback)(UartFrame*)) {
	receivedCallback = callback;
}

int queueMsgToSend(UartFrame* uartFrame) {
	if (uartQueue.unsent < uartQueue.size) {
		// copy frame to send into buffer
		uartQueue.tail->frame = *uartFrame;

		// advance the queue pointer
		uartQueue.tail = uartQueue.tail->next;
		uartQueue.unsent++;

		return 0;
	} else {
		return -1;		// queue full
	}
}

int getSendQueueAvailSpaces(void) {
	return uartQueue.size - uartQueue.unsent;
}


UartFrameQueueHandler createUartFrameQueue(UartFrameNode* head, unsigned int size) {
	unsigned int i;
	for (i=0; i<size-1; i++)		// handle everything except for the last node
	{
		(head+i)->next = head + i + 1;
	}
	// last node special case
	(head+size-1)->next = head;		// first node

	UartFrameQueueHandler handler;
	handler.head = head;
	handler.tail = head;
	handler.unsent = 0;
	handler.size = size;

	return handler;
}

int uartQueuePop(UartFrame* head) {
	if (uartQueue.unsent > 0) {
		// write the pointer of the frame at the queue head
		head = &(uartQueue.head->frame);

		// advance the queue pointer
		uartQueue.head = uartQueue.head->next;
		uartQueue.unsent--;

		return 0;
	} else {
		head = NULL;
		return -1;		// queue is empty
	}
}
