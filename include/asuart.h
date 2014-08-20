/**
 * \file asuart.h
 * \brief Active suspension UART module.
 * \author James Duley
 * \version 1.0
 * \date 2014-08-20
 */

/* Copyright (C) 
 * 2014 - James Duley
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

#ifndef ASUART_H
#define ASUART_H

/**
 * \brief Initalise the active suspension UART.
 */
void initUart();

/**
 * \brief Send a message.
 *
 * \param message A null terminated message string to be sent.
 */
void sendMessage(const char *message);

/**
 * \brief Get a message.
 *
 * \param message Buffer to store message.
 * \param messageSize Size of the buffer.
 *
 * \return 0 for success, 1 for no message, 2 for buffer too small.
 */
char getMessage(char *message, size_t messageSize);

#endif
