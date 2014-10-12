/**
 * \file shared_tracenode.h
 * \brief Declares a vector type for use with linked list or circular buffer
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

#ifndef SHARED_TRACENODE
#define SHARED_TRACENODE

typedef enum {BUFFERFULLMODE_BLOCK, BUFFERFULLMODE_OVERWRITE} BufferFullMode;

typedef struct TraceNode TraceNode;

/**
 * \struct TraceNode
 *
 * \brief Vector object to be used for linked list or circular buffer
 */
struct TraceNode
{
	int x;
	int y;
	TraceNode* next;
	TraceNode* prev;
};

/**
 * \struct CircularBufferHandler
 *
 * \brief Handler for circular buffers
 */
typedef struct
{
	TraceNode* lastRead;
	TraceNode* lastWritten;
	unsigned int size;
	BufferFullMode fullMode;
} CircularBufferHandler;

/**
 * \brief Creates a circular buffer from array of TraceNodes
 *
 * \param head Pointer to the first item in the array
 * \param size Size of the circular buffer desired
 * \param mode Desired behavior when buffer is full
 * \return Handler for the circular buffer
 */
CircularBufferHandler createCircularBuffer(TraceNode* head, unsigned int size, BufferFullMode mode);

/**
 * \brief Writes x and y values to chosen circular buffer
 *
 * \param buffer Pointer to the handler for the buffer to modify
 * \param x value to write
 * \param y value to write
 * \return 0 for success, -1 for buffer full
 */
int circularBufferWrite(CircularBufferHandler* buffer, int x, int y);

/**
 * \brief Returns the node of the circular buffer that was last written
 *
 * \param buffer Pointer to the handler for the buffer to modify
 * \return The last written node of the buffer
 */
TraceNode* getLatestNode(CircularBufferHandler* buffer);

/**
 * \brief Read data from the circular buffer
 *
 * \param buffer Pointer to the handler for the buffer to read
 * \return The oldest unread node
 */
TraceNode* circularBufferRead(CircularBufferHandler* buffer);

#endif
