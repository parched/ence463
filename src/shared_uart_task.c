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
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"

#ifndef NULL
#define NULL ( (void *) 0)
#endif

#define SENDMESSAGE_QUEUE_SIZE 10
#define UART_TASK_RATE_HZ 1000
#define UART_PERCHAR_HZ 75000	// 13.3us delay between characters
#define UART_BAUD 625000

static uartCallback receivedCallback;
static QueueHandle_t uartSendQueue;

/**
 * \brief Determines the total length of the message including type character
 *
 * \param type Char representing message type (W, R, S, A, M)
 * \return Length of frame including the type character, 0 if did not correspond to a type
 */
unsigned int getMsgLen(char type);

void vUartTask(void* pvParameters) {
	// setup Uart peripheral
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	SysCtlDelay(SysCtlClockGet() / 3000);
	GPIOPinTypeUART(GPIO_PORTD_BASE, GPIO_PIN_3 | GPIO_PIN_2);

	UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), UART_BAUD, UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE);
	UARTEnable(UART1_BASE);

	// initialize queue
	uartSendQueue = xQueueCreate(SENDMESSAGE_QUEUE_SIZE, sizeof(UartFrame));

	// initialize FreeRTOS sleep parameters
	portTickType xLastWakeTime;
	const portTickType xTimeIncrement = configTICK_RATE_HZ / UART_TASK_RATE_HZ;
	xLastWakeTime = xTaskGetTickCount();

	// message decoding variables
	UartFrame buffer;
	unsigned int msgLen;
	unsigned int index = 0;
	char lastChar;

	for (;;) {
		vTaskDelayUntil(&xLastWakeTime, xTimeIncrement);

		// process message send queue
		UartFrame toSend;
		while (xQueueReceive(uartSendQueue, &toSend, 0) == pdTRUE) {
			unsigned int msgLen = getMsgLen(toSend.frameWise.msgType);

			unsigned int i = 0;
			for (i=0; i<msgLen; i++) {
				// send characters individually
				UARTCharPut(UART1_BASE, (unsigned char) toSend.byteWise[i]);
			}
			vTaskDelay(msgLen*configTICK_RATE_HZ / UART_PERCHAR_HZ);		// put delay between messages
		}

		// receive and decode messages
		while (UARTCharsAvail(UART1_BASE)) {
			char recvChar = (char) UARTCharGetNonBlocking(UART1_BASE);
			if (index == 0) {
				// decode message type
				msgLen = getMsgLen(recvChar);
				if (msgLen && lastChar != 'M' && lastChar != 'W') {
					// only valid message types get to advance (msgLen = 0 is invalid)
					buffer.frameWise.msgType = recvChar;
					index++;
				}
			} else {
				if (index >= msgLen && receivedCallback != NULL) {
					// end of message given message type
					receivedCallback(&buffer);
					index = 0;
				} else {
					// adding more characters onto frame
					buffer.byteWise[index] = recvChar;
					index++;
				}
			}

			lastChar = recvChar;		// store the last character
		}
	}
}

void attachOnReceiveCallback(void (*callback)(UartFrame*)) {
	receivedCallback = callback;
}

int queueMsgToSend(UartFrame* uartFrame) {
	if (uartSendQueue == 0) {
		return -2;	// queue has not been created
	}

	if (xQueueSendToBack(uartSendQueue, (void*) uartFrame, 0) == pdTRUE) {
		return 0;
	} else {
		return -1;	// queue full
	}
}

int getSendQueueAvailSpaces(void) {
	return uxQueueMessagesWaiting(uartSendQueue);
}

unsigned int getMsgLen(char type) {
	switch(type)
	{
		case('W'):
			return 2;
		case('R'):
			return 3;
		case('S'):
			return 1;
		case('A'):
			return 7;
		case('M'):
			return 2;
		default:
			return 0;
	}
}
