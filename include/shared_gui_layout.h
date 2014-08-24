/**
 * \file shared_gui_layout.h
 * \brief Common module which declares types used to markup menu layout and hierachy.
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

#include "shared_tracenode.h"

#define ACTIVITY_MAX_PAGES 5	/**<maximum number of pages in a GUI */
#define LISTVIEW_MAX_ITEMS 5	/**<maximum number of contents allowed in a list view menu */
#define TRACEVIEW_POINTS 64     /**<number of data points to store in trace view menu */
#define CONTENT_NAME_SIZE 10	/**<maximum string size allowed for a list view content entry */
#define STROPTION_ITEM_SIZE 5   /**<maximum number of items allowed for contents with string options */

/**
 * \enum ViewType
 *
 * \brief ViewType enum
 */
typedef enum {VIEW_TYPE_TRACE, VIEW_TYPE_LIST} ViewType;

/**
 * \enum ContentType
 *
 * \brief Determines whether ListView content displays number or string options
 */
typedef enum {CONTENT_TYPE_INT, CONTENT_TYPE_STRING} ContentType;

/**
 * \enum ContentType
 *
 * \brief Determines what mode the menu operates in either display only or input.
 */
typedef enum {CONTENT_OUTPUT, CONTENT_INPUT} ContentDirection;

/**
 * \struct Content
 *
 * \brief Declares a ListView content entry and holds relevant context for it.
 */
typedef struct {
	char name[CONTENT_NAME_SIZE];                           /**<name to display for list view entry*/
	ContentType optionType;                                 /**<determines the type of content to display*/
	ContentDirection direction;                             /**<declares whether content allows user to modify its value*/

	int option;					                            /**<context of what option is selected for this content*/
	int minIndex;                                           /**<minimum size allowed for option index*/
	int maxIndex;                                           /**<maximum size allowed for option index*/
	char values[STROPTION_ITEM_SIZE][CONTENT_NAME_SIZE];    /**<declares what value to display at every index (for CONTENT_TYPE_STRING type only)*/

	int (*getter)(void);  			                        /**<getter function to get data for the bound variable*/
	void (*setter)(int);									/**<setter function to set data for the bound variable (for CONTENT_OUTPUT direction only)*/
} Content;

/**
 * \struct ListView
 *
 * \brief Declares a ListView menu and holds relevant context for it.
 */
typedef struct  {
	Content contents[LISTVIEW_MAX_ITEMS];    /**<top down list of ListView contents*/
} ListView;

/**
 * \struct TraceView
 *
 * \brief Declares a TraceView menu and holds relevant context for it.
 */
typedef struct {
	TraceNode* head;			 /**<pointer to head node of the buffer to draw*/
	unsigned int sparseIndex;	 /**<number of data points to skip when drawing the trace*/	
} TraceView;

/**
 * \struct Activity
 *
 * \brief Declares full menu hierachy and layout for a GUI and maintains menu page and cursor context.
 */
typedef struct  {
	void* menus[ACITIVTY_MAX_PAGES];          /**<menu corresponding to each page*/
	ViewType menuTypes[ACITIVTY_MAX_PAGES];   /**<the menu type corresponding to each page*/
	unsigned int pageContext;                 /**<displayed page context*/
	unsigned int cursorContext;               /**<cursor position context*/
	unsigned int noPages;					  /**<number of pages in this activity*/
} Activity;


#endif /* UI_TASK_H_ */

