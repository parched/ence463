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
};

/**
 * \brief Creates a linked list from array of TraceNodes
 *
 * \param head Pointer to the first item in the array
 * \param size Size of the linked list desired
 */
void createLinkedList(TraceNode* head, unsigned int size);

/**
 * \brief Creates a circular buffer from array of TraceNodes
 *
 * \param head Pointer to the first item in the array
 * \param size Size of the circular buffer desired
 */
void createCircularBuffer(TraceNode* head, unsigned int size);


#endif
