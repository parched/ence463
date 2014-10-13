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
#define TRACE_ZERO_CENTER -1    /**<specifies the zero height of the TraceView to be center */


/**
 * \enum ViewType
 *
 * \brief ViewType enum
 */
typedef enum {VIEWTYPE_TRACE, VIEWTYPE_LIST} ViewType;

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
	CircularBufferHandler* buffer;            /**<pointer to circular buffer handler*/

	unsigned int zeroLine;                    /**<position on screen representing y value of 0*/
	unsigned int minZoomHorzScale;			  /**<change in horizontal trace position in pixels per change in x value of TraceNode while zoomed out*/
	unsigned int maxZoomHorzScale;            /**<change in horizontal trace position in pixels per change in x value of TraceNode while zoomed in*/
	int vertScale;                            /**<change in vertical trace position in CHAR_HEIGHT per change in y value of TraceNode*/
	unsigned int horzScaleStep;               /**<change in dispHorzScale with every button press*/
	unsigned int dispHorzScale;               /**<horizontal scale the trace is currently drawn at*/
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
	unsigned int currPage;
} Activity;

/**
 * \brief Constructs an Option with compulsory values as set and context initialized
 *
 * \param minIndex Minimum value this option can be set to
 * \param maxIndex Maximum value this option can be set to
 */
Options option(int minIndex, int maxIndex);

/**
 * \brief Constructs an Item with compulsory values as set and context initialized
 *
 * \param name Label of the item
 * \param optionType Sets whether options scroll through number or string
 * \param accessType Sets whether user can modify the option value
 * \param options Settings for the options available for this Item
 * \param getter Function that retrieves the system variable, binding this option to it
 */
Item item(char *name, OptionType optionType, OptionAccess accessType, Options options, int (*getter)(void));

/**
 * \brief Initailises a ListView and sets the items to a known initial values. Sets numItems =0 if fails to complete
 *
 * \param name Label of the view
 * \param numItems The number of Items that this ListView will have
 */
ListView listView(char *name, unsigned int numItems);

/**
 * \brief Constructs a TraceView with compulsory values as set and context initialized
 *
 * \param name Label of the view
 * \param buffer Pointer to the handler of the circular buffer to draw
 * \param zeroHeight Pixel height the value 0 from bottom of trace plot
 * \param minHorzScale Change in horizontal trace position in pixels per x value of TraceNode while zoomed out
 * \param maxHorzScale Change in horizontal trace position in pixels per x value of TraceNode while zoomed in
 * \param vertScale Change in vertical trace position in CHAR_HEIGHT per y value of TraceNode
 */
TraceView traceView(char *name, CircularBufferHandler* buffer, int zeroHeight, unsigned int minHorzScale, unsigned int maxHorzScale, int vertScale);

/**
 * \brief Constructs an Activity with compulsory values set set and context initialized
 *
 */
Activity activity();

/**
 * \brief Assigns a view to an Activity forcing the user to correctry init a view to an activity.
 * \returns 0 for sucess, 1 for index out of bound
 *
 * \param activity The Activity to attach the view to
 * \param view The View that will be attached to the Activity
 * \param type The type of view
 */
int addView(Activity* activity, void* view, ViewType type);

#endif /* UI_TASK_H_ */

