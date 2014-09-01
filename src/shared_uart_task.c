
#include "shared_uart_task.h"

void vUartTask(void* pvParameters) {
}

void attachOnReceiveCallback(void (*callback)(UartFrame)) {
}

int queueMsgToSend(UartFrame uartFrame) {
	return 0;
}

int getSendQueueAvailSpaces(void) {
	return 0;
}
