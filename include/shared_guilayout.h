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

#define ACTIVITY_MAX_PAGES 5    /**<maximum number of pages in a GUI */

#define LISTVIEW_MAX_ITEMS 5    /**<maximum number of items allowed in a ListView */

#define VIEW_NAME_SIZE 12		/**<maximum string size allowed for a View title */
#define ITEM_NAME_SIZE 10       /**<maximum string size allowed for a ListView Item */
#define ITEM_MAX_OPTIONSTR 5    /**<maximum number of items allowed for Items with string Options */
#define OPTION_NAME_SIZE 6		/**<maximum string size allowed for string Options */

#define TRACEVIEW_POINTS 64     /**<number of data points to store in TraceView menu */
#define TRACEVIEW_MAX_ZOOM 8	/**<maximum sparse index allowed */


/**
 * \enum ViewType
 *
 * \brief ViewType enum
 */
typedef enum {VIEWTYPE_TRACE, VIEWTYPE_LIST, VIEWTYPE_NONE} ViewType;

/**
 * \enum ContentType
 *
 * \brief Determines whether ListView content displays number or string options
 */
typedef enum {OPTIONTYPE_INT, OPTIONTYPE_STRING} OptionType;

/**
 * \enum ContentType
 *
 * \brief Determines what mode the menu operates in either display only or input.
 */
typedef enum {OPTIONACCESS_READONLY, OPTIONACCESS_MODIFIABLE} OptionAccess;

/**
 * \struct Option
 *
 * \brief Declares options available for an Item and holds relevant context for it.
 */
typedef struct {
    int minIndex;                                           /**<minimum size allowed for option index*/
    int maxIndex;                                           /**<maximum size allowed for option index*/
    unsigned int skip;										/**<amount to increment/decrement when modified*/
    char* values[ITEM_MAX_OPTIONSTR];                       /**<declares what value to display at every index (for OPTIONTYPE_STRING type only)*/
} Options;

/**
 * \struct Content
 *
 * \brief Declares a ListView content entry and holds relevant context for it.
 */
typedef struct {
	char name[ITEM_NAME_SIZE];                              /**<name to display for this ListView item*/
	OptionType optionType;                                  /**<determines the type of content to display*/
	OptionAccess accessType;                                /**<declares whether content allows user to modify its value*/

    Options options;                                        /**<options available for this item*/
	int (*getter)(void);                                    /**<getter function to get data for the bound variable*/
	void (*setter)(int);                                    /**<setter function to set data for the bound variable (for OPTIONACCESS_MODIFIABLE only)*/
} Item;

/**
 * \struct ListView
 *
 * \brief Declares a ListView menu and holds relevant context for it.
 */
typedef struct  {
	char name[VIEW_NAME_SIZE];                /**<name of the view*/
	Item items[LISTVIEW_MAX_ITEMS];           /**<top down list of ListView contents*/
    unsigned int numItems;                    /**<number of items in this ListView*/
} ListView;

/**
 * \struct TraceView
 *
 * \brief Declares a TraceView menu and holds relevant context for it.
 */
typedef struct {
	char name[VIEW_NAME_SIZE];                /**<name of the view*/
	TraceNode* head;                          /**<pointer to head node of the buffer to draw*/
	unsigned int sparseIndex;                 /**<number of data points to skip when drawing the trace*/
} TraceView;

/**
 * \struct Activity
 *
 * \brief Declares full menu hierarchy and layout for a GUI and maintains menu page and cursor position context.
 */
typedef struct  {
	void* menus[ACTIVITY_MAX_PAGES];          /**<menu corresponding to each page*/
	ViewType menuTypes[ACTIVITY_MAX_PAGES];   /**<the menu type corresponding to each page*/
	unsigned int pageContext;                 /**<displayed page context*/
	unsigned int cursorContext;               /**<cursor position context*/
	unsigned int numPages;                    /**<number of pages in this activity*/
} Activity;

/**
 * \brief Returns an initilied TraceView which includes attaching the head of a ciruclar buffer of TraceNodes to it
 *
 * \param sparseIndex The amount of TraceNodes to skip when drawing the road surface
 * param circularBufferSize The size of the circular buffer size of TraceNodes, must be less than MAX_CIRC_BUFF_SIZE and greater than 0
 */
TraceView traceView(unsigned int sparseIndex, TraceNode *head);

/**
 * \brief Creates An Item
 *
 * \param name The name of the item.
 * \param optionType Determines whereever display a string or number
 * \param OptionAccess determines wherever to display only or allow user input
 * \param options The bitwise OR of the PWM outputs init.
 * \param getter Attach an item getter function
 * \param setter Attach an item setter function
 */
Item item(char *name, OptionType optionType, OptionAccess accessType, Options options,int getter(),void setter(int));

/**
 * \brief Creates and returns an Option
 *
 * \param minIndex The size of the min Index
 * param manIndex The size of the man Index
 */
Options option(int minIndex, int maxIndex, unsigned int skip);

/**
 * \brief Initilizes an activity
 *
 * \param numPages Number of pages for this activity
 */
Activity acitivity(unsigned int numPages);

/**
 * \brief Initailises a ListView and sets the items to a known initial values. Sets numItems =0 if fails to complete
 *
 * \param numItems The number of Items that this ListView will have
 * \param name name of ListView
 */
ListView listView(unsigned int numItems, char *name);


#endif /* UI_TASK_H_ */

