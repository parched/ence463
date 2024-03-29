;   FreeRTOS V7.1.1 - Copyright (C) 2012 Real Time Engineers Ltd.


;   ***************************************************************************
;    *                                                                       *
;    *    FreeRTOS tutorial books are available in pdf and paperback.        *
;    *    Complete, revised, and edited pdf reference manuals are also       *
;    *    available.                                                         *
;    *                                                                       *
;    *    Purchasing FreeRTOS documentation will not only help you, by       *
;    *    ensuring you get running as quickly as possible and with an        *
;    *    in-depth knowledge of how to use FreeRTOS, it will also help       *
;    *    the FreeRTOS project to continue with its mission of providing     *
;    *    professional grade, cross platform, de facto standard solutions    *
;    *    for microcontrollers - completely free of charge!                  *
;    *                                                                       *
;    *    >>> See http://www.FreeRTOS.org/Documentation for details. <<<     *
;    *                                                                       *
;    *    Thank you for using FreeRTOS, and thank you for your support!      *
;    *                                                                       *
;   ***************************************************************************


;   This file is part of the FreeRTOS distribution.

;   FreeRTOS is free software; you can redistribute it and/or modify it under
;   the terms of the GNU General Public License (version 2) as published by the
;   Free Software Foundation AND MODIFIED BY the FreeRTOS exception.
;   >>>NOTE<<< The modification to the GPL is included to allow you to
;   distribute a combined work that includes FreeRTOS without being obliged to
;   provide the source code for proprietary components outside of the FreeRTOS
;   kernel.  FreeRTOS is distributed in the hope that it will be useful, but
;   WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
;   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
;   more details. You should have received a copy of the GNU General Public
;   License and the FreeRTOS license exception along with FreeRTOS; if not it
;   can be viewed here: http://www.freertos.org/a00114.html and also obtained
;   by writing to Richard Barry, contact details for whom are available on the
;   FreeRTOS WEB site.

;   1 tab == 4 spaces!

;   ***************************************************************************
;    *                                                                       *
;    *    Having a problem?  Start by reading the FAQ "My application does   *
;    *    not run, what could be wrong?                                      *
;    *                                                                       *
;    *    http://www.FreeRTOS.org/FAQHelp.html                               *
;    *                                                                       *
;   ***************************************************************************


;   http://www.FreeRTOS.org - Documentation, training, latest information, 
;   license and contact details.

;   http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
;   including FreeRTOS+Trace - an indispensable productivity tool.

;   Real Time Engineers ltd license FreeRTOS to High Integrity Systems, who sell 
;   the code with commercial support, indemnification, and middleware, under 
;   the OpenRTOS brand: http://www.OpenRTOS.com.  High Integrity Systems also
;   provide a safety engineered and independently SIL3 certified version under 
;   the SafeRTOS brand: http://www.SafeRTOS.com.


	.cdecls  C, NOLIST, WARN, "FreeRTOSConfig.h"

	.thumb
	.text

	.global vTaskSwitchContext
	.global pxCurrentTCB
pxCurrentTCBvalue .word pxCurrentTCB

	.global xPortPendSVHandler
	.global vPortSetInterruptMask
	.global vPortClearInterruptMask
	.global vPortSVCHandler
	.global vPortStartFirstTask
	.global vPortEnableVFP


;-----------------------------------------------------------

xPortPendSVHandler:
	mrs r0, psp

	; Get the location of the current TCB.
	ldr r3, pxCurrentTCBvalue
	ldr r2, [r3]

	stmdb r0!, {r4-r11}
	str r0, [r2]

	stmdb sp!, {r3, r14}
	mov r0, #configMAX_SYSCALL_INTERRUPT_PRIORITY
	msr basepri, r0
	bl vTaskSwitchContext
	mov r0, #0
	msr basepri, r0
	ldmia sp!, {r3, r14}

	ldr r1, [r3]
	ldr r0, [r1]
	ldmia r0!, {r4-r11}
	msr psp, r0
	bx r14

;-----------------------------------------------------------

vPortSetInterruptMask:
	push { r0 }
	mov R0, #configMAX_SYSCALL_INTERRUPT_PRIORITY
	msr BASEPRI, R0
	pop { R0 }

	bx r14

;-----------------------------------------------------------

vPortClearInterruptMask:
	PUSH { r0 }
	MOV R0, #0
	MSR BASEPRI, R0
	POP	 { R0 }

	bx r14
;-----------------------------------------------------------

vPortSVCHandler:
	; Get the location of the current TCB.
	ldr r3, pxCurrentTCBvalue
;	ldr r1, [r3]
;	ldr r0, [r1]
	; Pop the core registers.
;	ldmia r0!, {r4-r11, r14}
;	msr psp, r0
;	mov r0, #0
;	msr basepri, r0
;	bx r14

	ldr r1, [r3]
	ldr r0, [r1]
	ldmia r0!, {r4-r11}
	msr psp, r0
	mov r0, #0
	msr	basepri, r0
	orr r14, r14, #13
	bx r14

;-----------------------------------------------------------

vPortStartFirstTask:
; Vector Table offset.
VTABLE .set 0xE000ED08

	; Use the NVIC offset register to locate the stack.
;	MOVW r0, #VTABLE & 0xFFFF
;	MOVT r0, #VTABLE >> 16
;	ldr r0, [r0]
;	ldr r0, [r0]
	; Set the msp back to the start of the stack.
;	msr msp, r0
	; Call SVC to start the first task.
;	cpsie i
;	svc #0

	; Use the NVIC offset register to locate the stack
	MOVW r0, #VTABLE & 0xFFFF
	MOVT r0, #VTABLE >> 16
	ldr r0, [r0]
	ldr r0, [r0]
	; Set the msp back to the start of the stack
	msr msp, r0
	; Call SVC to start the first task
	svc #0


;-----------------------------------------------------------

vPortEnableVFP:
; CAPCR Register address.
CPACR .set 0xE000ED88

	; The FPU enable bits are in the CPACR.
	MOVW     r0, #CPACR & 0xFFFF
	MOVT     r0, #CPACR >> 16
	ldr r1, [r0]

	; Enable CP10 and CP11 coprocessors, then save back.
	orr r1, r1, #( 0xf << 20 )
	str r1, [r0]
	bx  r14


	.end


