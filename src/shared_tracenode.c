/**
 * \file shared_tracenode.c
 * \brief Helper functions for vector type for use with linked list or circular buffer
 * \author George Xian
 * \version 1.0
 * \date 2014-09-23
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

#include "shared_tracenode.h"

#ifndef NULL
#define NULL                    ((void *)0)
#endif

void createLinkedList(TraceNode* head, unsigned int size)
{
	unsigned int i;
	for (i=0; i<size-1; i++)
	{
		(head+i)->next = head+i+1;
	}
	(head+size)->next = NULL;
}

void createCircularBuffer(TraceNode* head, unsigned int size)
{
	unsigned int i;
	for (i=0; i<size-1; i++)
	{
		(head+i)->next = head+i+1;
	}
	(head+size)->next = head;
}



