/**
 * \file shared_gui_layout.h
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


typedef enum {CONTENT_TYPE_INT, CONTENT_TYPE_STRING} ContentType;

/**
 * \ViewType enum
 */
typedef enum {VIEW_TYPE_TRACE, VIEW_TYPE_LIST} ViewType;

/**
 * \brief content state for each list
 */
typedef struct {
	char name[10];
	ContentType type;
	int index;
	bool selectable;
	char values[10];
} Content;

/**
 * \brief container of contents
 */
typedef struct  {
	Content contents[5];
} ListView;

/**
 * \brief activity state used to define menus
 */
typedef struct  {
	void* menus[10];
	ViewType menuTypes[10]
	int page;
	int cursor;
} Activity;


/**
 * \brief Traceview state used to draw road surface
 */
typedef struct {
	int y[20]:
	int timestep:
}TraceView;

/**
 * \brief Adds a content to the listView passed to it
 *
 * \param content The menue state being added
 * \param menueList The ListView that the Content is being added too.
 */
void addContent(Content content, ListView menueList);



#endif /* UI_TASK_H_ */

