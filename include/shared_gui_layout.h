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


/**
 * \enum ContentType
 *
 * \brief ContentType enum
 */
typedef enum {CONTENT_TYPE_INT, CONTENT_TYPE_STRING} ContentType;

/**
 * \enum ViewType
 *
 * \brief ViewType enum
 */
typedef enum {VIEW_TYPE_TRACE, VIEW_TYPE_LIST} ViewType;

/**
 * \enum ContentType
 *
 * \brief Determines what mode the menue operates in either display only or input.
 */
typedef enum {CONTENT_OUTPUT, CONTENT_INPUT} ContentDirection;

/**
 * \struct Content
 *
 * \brief content state for each list
 */
typedef struct {
	char name[10]; /**<name of menu*/
	ContentType type; /**<determines the type of content to display*/
	int index; /**<index of menu*/
	DisplayType displayMode; /**<display mode, user input or display only*/
	char values[10]; /**<user input variable*/
	int (*binding)(void); /**<data to display on menu*/
} Content;

/**
 * \struct ListView
 *
 * \brief container of contents
 */
typedef struct  {
	Content contents[5]; /**<array of contents*/
} ListView;

/**
 * \struct Activity
 *
 * \brief activity state used to define menus
 */
typedef struct  {
	void* menus[10]; /**<Menus that can be displayed*/
	ViewType menuTypes[10]; /**<type of menu to display*/
	int page; /**<current page, the current page is used to determine the menu to displays*/
	int cursor; /**<the position of the cursor, the cursor is used to select the varialbe you want changed or to scroll down a menu to display different part of the menues*/
} Activity;


/**
 * \struct TraceView
 *
 * \brief Traceview state used to draw road surface
 */
typedef struct {
	int y[20]: /**<road height*/
	int timestep: /**<timestep that the simulation runs at*/
} TraceView;




#endif /* UI_TASK_H_ */

