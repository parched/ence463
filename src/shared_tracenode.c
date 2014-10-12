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
#define NULL ( (void *) 0)
#endif

CircularBufferHandler createCircularBuffer(TraceNode* head, unsigned int size, BufferFullMode mode)
{
	TraceNode* prev = head + size - 1;	// prev of the first node is the last node

	// setup the other nodes
	unsigned int i;
	for (i=0; i<size-1; i++)			// handle everything except for the last node
	{
		(head+i)->prev = prev;
		(head+i)->next = head + i + 1;

		// advance the prev pointer
		prev = (head+i);
	}
	// last node special case
	(head+size-1)->next = head;			// first node
	(head+size-1)->prev = head+size-2;

	CircularBufferHandler handler;
	handler.lastRead = head->prev->prev;
	handler.lastWritten = head->prev;
	handler.fullMode = mode;
	handler.size = size;

	return handler;
}

int circularBufferWrite(CircularBufferHandler* buffer, int x, int y)
{
	TraceNode* writing = buffer->lastWritten->next;	// node currently being written

	if (buffer->fullMode == BUFFERFULLMODE_BLOCK && writing == buffer->lastRead)
	{
		return -1;		// buffer full
	}
	else
	{
		writing->x = x;
		writing->y = y;
		buffer->lastWritten = writing;	// advance the write pointer

		return 0;
	}
}

TraceNode* getLatestNode(CircularBufferHandler* buffer)
{
	return buffer->lastWritten;
}

TraceNode* circularBufferRead(CircularBufferHandler* buffer)
{
	if (buffer->lastRead->next == buffer->lastWritten->next)
	{
		return NULL;					// no unread data
	}
	else
	{
		TraceNode* reading = buffer->lastRead->next;
		buffer->lastRead = reading;		// advance the read pointer
		return reading;
	}
}


