/**
 * \file wus_graphics.h
 * \brief Active suspension simulator user interface module.
 * \author Hugh Bisset
 * \version 1.0
 * \date 2014-08-21
 */

/* Copyright (C)
 * 2014 - Hugh Bisset
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


#ifndef UI_TASK_H_
#define UI_TASK_H_


/**
 * \brief Sets up the user input task and continously updates display
 *
 * \param params unused
 */

typedef enum {CONTENT_TYPE_INT, } ContentType;

typedef struct {
	char name[10];
	ContentType type;
	int index;
	bool selectable;
	char values[10];
} Content;

typedef struct  {
	Content contents[5];

} ListView;


typedef struct  {
	void* menus
	int page;
	int curser;
} GUI;



#endif /* UI_TASK_H_ */
