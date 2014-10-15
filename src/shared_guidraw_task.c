/**
 * \file shared_guidraw_task.c
 * \brief Contains implementation of the task that draws the gui
 * \author George Xian
 * \version 1.0
 * \date 2014-08-25
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

#include "shared_guidraw_task.h"

#include <ustdlib.h>
#include "inc/hw_types.h"
#include "drivers/rit128x96x4.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "shared_guilayout.h"
#include "shared_displayformat128x96.h"

#define INPUTEVENT_QUEUE_SIZE 10
#define GUI_TASK_RATE_HZ 25
#define OLED_FREQ 1000000

typedef enum {VERTDIR_UP, VERTDIR_DOWN} VertDir;
typedef enum {HORZDIR_LEFT, HORZDIR_RIGHT} HorzDir;
typedef enum {DRAWMODE_TITLE, DRAWMODE_OPTION} DrawMode;

typedef struct
{
	Button button;
	ButtonEvent event;
} InputEvent;

static xQueueHandle inputEventQueue;
static Activity* unitActivity;

static char CLEAR_ROW [PX_HORZ] = "                      ";

/**
 * \brief Moves the cursor up or down
 *
 * \param activity Pointer to the activity that holds the context
 * \param dir Direction to move the cursor
 */
void moveCursor(Activity* activity, VertDir dir);

/**
 * \brief Moves to the left or right page
 *
 * \param activity Pointer to the activity that holds the context
 * \param dir Direction to move the pages
 */
void movePage(Activity* activity, HorzDir dir);

/**
 * \brief Moves ListView Item Option left or right
 *
 * \param activity Pointer to the activity that holds the context
 * \param dir Direction to move the option
 */
void changeOption(Activity* activity, HorzDir dir);

/**
 * \brief Redraws aspects of current view that are read only
 *
 * \param activity Pointer to the activity that holds the context
 */
void refreshReadonlyValues(const Activity* activity);

/**
 * \brief Determines what type of View to redraw and redraws
 *
 * \param activity Pointer to the activity
 */
void redrawView(const Activity* activity);

/**
 * \brief Draws a new ListView upon switching to a different one
 *
 * \param view Pointer to the activity hosting the view
 */
void redrawListView(const Activity* activity);

/**
 * \brief Draws a new TraceView upon switching to a different one
 *
 * \param activity Pointer to the activity hosting the view
 */
void redrawTraceView(const Activity* activity);

/**
 * \brief Draws given string formatted as title in a ListView
 *
 * \param activity Pointer to activity containing title to draw
 */
void drawViewTitle(const Activity* activity);

/**
 * \brief Draws the given Item in a ListView
 *
 * \param item Pointer to Item object to draw
 * \param index The place this item is in the ListView where 0 is top
 * \param selected Is Item selected
 */
void drawListViewItem(const Item* item, unsigned int index, tBoolean selected);

/**
 * \brief Draws the plot area of TraceView
 *
 * \param view Pointer to the TraceView to draw
 * \param selected Is the plot selected
 */
void drawTraceViewPlot(const TraceView* view, tBoolean selected);

/**
 * \brief Draws a single point on the display
 *
 * \param x Horizontal position to draw dot
 * \param y Vertical position to draw dot
 * \param level Brightness to draw dot
 */
void drawPointRtoL(unsigned int x, unsigned int y, char level);

/**
 * \brief Clears the trace plot for a new trace
 *
 */
void clearTracePlot(void);

/**
 * \brief Gets the horizontal position of text given alignment and margins
 *
 * \param str String to draw
 * \param type Type of alignment
 * \param margin Offset given alignment type
 */
unsigned int getHorzAlignment(const char* str, TextAlign align, unsigned int margin);

/**
 * \brief Finds horizontal position of modifiable indicators
 *
 * \param menuType Specify to draw for Title or Option
 * \param dir Which indicator side to find position of
 * \param x Horizontal position of text to surround
 * \param strlen Length of the string
 * \return Horizontal pixel position to draw the modifiable indicator
 */
unsigned int getModifiableIndicatorHorzPos(DrawMode menuType, HorzDir dir, unsigned int x, unsigned int strlen);


void attachActivity(Activity* activity)
{
	unitActivity = activity;
}

int queueInputEvent(Button button, ButtonEvent event)
{
	InputEvent inputEvent;
	inputEvent.button = button;
	inputEvent.event = event;

	if (inputEventQueue == 0)
	{
		return -2;	// queue has not been created
	}

	if (xQueueSendToBack(inputEventQueue, (void*) &inputEvent, 0) == pdTRUE)
	{
		return 0;	// queueing success
	}
	else
	{
		return -1;	// queue full
	}
}

void vGuiRefreshTask(void *pvParameters)
{
	// initialize FreeRTOS queue
	inputEventQueue = xQueueCreate(INPUTEVENT_QUEUE_SIZE, sizeof(InputEvent));

	// initialize FreeRTOS sleep parameters
	portTickType xLastWakeTime;
	const portTickType xTimeIncrement = configTICK_RATE_HZ / GUI_TASK_RATE_HZ;
	xLastWakeTime = xTaskGetTickCount();

	// initialize screen
	RIT128x96x4Init(OLED_FREQ);
	redrawView(unitActivity);

	for (;;)
	{
		// wait for next cycle
		vTaskDelayUntil(&xLastWakeTime, xTimeIncrement);

		// updates read only values
		refreshReadonlyValues(unitActivity);

		// process input events
		InputEvent event;
		while (xQueueReceive(inputEventQueue, &event, 0) == pdTRUE)
		{
			if (event.event == BUTTON_EVENT_RISING_EDGE)
			{
				switch(event.button)
				{
					case(BUTTON_UP):
						moveCursor(unitActivity, VERTDIR_UP);
						break;
					case(BUTTON_DOWN):
						moveCursor(unitActivity, VERTDIR_DOWN);
						break;
					case(BUTTON_LEFT):
						if (unitActivity->cursorContext == 0)
						{
							movePage(unitActivity, HORZDIR_LEFT);
						}
						else
						{
							changeOption(unitActivity, HORZDIR_LEFT);
						}
						break;
					case(BUTTON_RIGHT):
						if (unitActivity->cursorContext == 0)
						{
							movePage(unitActivity, HORZDIR_RIGHT);
						}
						else
						{
							changeOption(unitActivity, HORZDIR_RIGHT);
						}
						break;
					default:
						break;
				}
			}
		}
	}
}

void moveCursor(Activity* activity, VertDir dir)
{
	const unsigned int page = activity->pageContext;
	switch(dir)
	{
		case(VERTDIR_UP):
			if (activity->cursorContext > 0)
			{
				// move cursor up if not at the top
				activity->cursorContext--;

				// update display
				const unsigned int listCursorContext = activity->cursorContext-1;
				switch(activity->menuTypes[page])
				{
					case(VIEWTYPE_LIST):
						// dim previous selected item
						drawListViewItem(&(((ListView*) (activity->menus[page]))->items[listCursorContext+1]), listCursorContext+1, false);
						if (activity->cursorContext == 0)
						{
							// user is now selecting the title, brighten title
							drawViewTitle(activity);
						}
						else
						{
							// user is now selecting another item, redraw the item
							drawListViewItem(&(((ListView*) (activity->menus[page]))->items[listCursorContext]), listCursorContext, true);
						}
						break;
					case(VIEWTYPE_TRACE):
						// user has deselected the trace and is now selecting title
						drawViewTitle(activity);
						drawTraceViewPlot(((TraceView*) activity->menus[page]), false);
						break;
				}
			}
			break;
		case(VERTDIR_DOWN):
			if (activity->menuTypes[page] == VIEWTYPE_TRACE)
			{
				if (activity->cursorContext < 1)
				{
					// if in TraceView and not currently in zoom context, enter it
					activity->cursorContext++;

					// user has now deselected the title and is now selecting the trace
					drawViewTitle(activity);
					drawTraceViewPlot(((TraceView*) activity->menus[page]), true);
				}
			}
			else if (activity->menuTypes[page] == VIEWTYPE_LIST)
			{
				if (activity->cursorContext < ((ListView*) activity->menus[page])->numItems)
				{
					// if in ListView and cursor position and not at bottom, go down
					activity->cursorContext++;
					const unsigned int listCursorContext = activity->cursorContext - 1;

					// update display
					if (activity->cursorContext-1 == 0)
					{
						// user was previously selected title, dim title
						drawViewTitle(activity);
					}
					else
					{
						// user was previously selecting another item, dim it
						drawListViewItem(&(((ListView*) activity->menus[page])->items[listCursorContext-1]), listCursorContext-1, false);
					}
					// brighten selected item
					drawListViewItem(&(((ListView*) activity->menus[page])->items[listCursorContext]), listCursorContext, true);
				}
			}
			break;
	}
}

void movePage(Activity* activity, HorzDir dir)
{
	const unsigned int page = activity->pageContext;
	switch(dir)
	{
		case(HORZDIR_LEFT):
			if (page > 0)
			{
				// go to the left page if cursor at top and not at leftmost
				activity->pageContext--;
				redrawView(activity);
			}
			break;
		case(HORZDIR_RIGHT):
			if (page < activity->numPages-1)
			{
				// go to the right page if cursor at top and not at rightmost
				activity->pageContext++;
				redrawView(activity);
			}
			break;
	}
}

void changeOption(Activity* activity, HorzDir dir)
{
	const unsigned int page = activity->pageContext;
	if (activity->menuTypes[page] == VIEWTYPE_LIST)
	{
		unsigned int listCursorPos = activity->cursorContext - 1;
		Item* selectedItem = &(((ListView*) activity->menus[page])->items[listCursorPos]);
		if (selectedItem->accessType == OPTIONACCESS_MODIFIABLE)
		{
			Options* selectedOption = &(selectedItem->options);
			switch(dir)
			{
				case(HORZDIR_LEFT):
					if (selectedItem->getter() > selectedOption->minIndex)
					{
						// if ListView, not at top and selected item is modifiable and not at limit, decrease
						selectedItem->setter(selectedItem->getter() - selectedOption->skip);

						// redraw the item
						drawListViewItem(selectedItem, listCursorPos, true);
					}
					break;
				case(HORZDIR_RIGHT):
					if (selectedItem->getter() < selectedOption->maxIndex)
					{
						// if ListView, not at top and selected item is modifiable and not at limit, increase
						selectedItem->setter(selectedItem->getter() + selectedOption->skip);

						// redraw the item
						drawListViewItem(selectedItem, listCursorPos, true);
					}
					break;
			}
		}
	}
	else if (activity->menuTypes[page] == VIEWTYPE_TRACE)
	{
		TraceView* view = (TraceView*) activity->menus[page];
		switch(dir)
		{
			case(HORZDIR_RIGHT):
				if (view->dispHorzScale > view->minZoomHorzScale)
				{
					// zoom in
					clearTracePlot();
					view->dispHorzScale -= view->horzScaleStep;
				}
				break;
			case(HORZDIR_LEFT):
				if (view->dispHorzScale < view->maxZoomHorzScale)
				{
					// zoom out
					clearTracePlot();
					view->dispHorzScale += view->horzScaleStep;
				}
				break;
		}
	}
}

void redrawView(const Activity* activity)
{
	switch (activity->menuTypes[activity->pageContext])
	{
		case(VIEWTYPE_LIST):
			redrawListView(activity);
			break;
		case(VIEWTYPE_TRACE):
			redrawTraceView(activity);
			break;
	}
}

void refreshReadonlyValues(const Activity* activity)
{
	unsigned int i;
	if (activity->menuTypes[activity->pageContext] == VIEWTYPE_LIST)
	{
		ListView* listView = (ListView*) activity->menus[activity->pageContext];
		// Redraw items that are read-only only
		for (i=0; i<listView->numItems; i++)
		{
			if (listView->items[i].accessType == OPTIONACCESS_READONLY)
			{
				tBoolean selected = (activity->cursorContext-1) == i;
				drawListViewItem(&(((ListView*) listView)->items[i]), i, selected);
			}
		}
	}
	else if (activity->menuTypes[activity->pageContext] == VIEWTYPE_TRACE)
	{
		// Redraw trace view plot only
		tBoolean selected = (activity->cursorContext) > 0;
		TraceView* traceView = (TraceView*) activity->menus[activity->pageContext];
		drawTraceViewPlot(traceView, selected);
	}
}

void redrawListView(const Activity* activity)
{
	RIT128x96x4Clear();

	// draw title (is selected when coming to new page)
	drawViewTitle(activity);

	// draw items (none are selected when coming to new page)
	ListView* listView = (ListView*) activity->menus[activity->pageContext];
	unsigned int i;
	for (i=0; i<(listView->numItems); i++)
	{
		drawListViewItem(&(listView->items[i]), i, false);
	}
}

void redrawTraceView(const Activity* activity)
{
	RIT128x96x4Clear();

	// draw title (is selected when coming to new page)
	drawViewTitle(activity);

	// draw plot (not selected when coming to new page
	drawTraceViewPlot((TraceView*) activity->menus[activity->pageContext], false);
}

void drawViewTitle(const Activity* activity)
{
	RIT128x96x4StringDraw(CLEAR_ROW, 0, TITLE_PADDINGTOP, 0);

	// draw title
	char titleStr[VIEW_NAME_SIZE];
	ustrncpy(titleStr, ((ListView*) (activity->menus[activity->pageContext]))->name, VIEW_NAME_SIZE);
	unsigned int posX = getHorzAlignment(titleStr, TITLE_TEXTALIGN, TITLE_MARGIN);

	if (activity->cursorContext == 0)
	{
		// title selected, draw bright
		RIT128x96x4StringDraw(titleStr, posX, TITLE_PADDINGTOP, SELECTED_BRIGHTNESS);
		if (activity->pageContext > 0)
		{
			RIT128x96x4StringDraw("<", getModifiableIndicatorHorzPos(DRAWMODE_OPTION, HORZDIR_LEFT, posX, ustrlen(titleStr)), TITLE_PADDINGTOP, SELECTED_BRIGHTNESS);
		}
		if (activity->pageContext < activity->numPages-1)
		{
			RIT128x96x4StringDraw(">", getModifiableIndicatorHorzPos(DRAWMODE_OPTION, HORZDIR_RIGHT, posX, ustrlen(titleStr)), TITLE_PADDINGTOP, SELECTED_BRIGHTNESS);
		}
	}
	else
	{
		// title not selected, draw dim
		RIT128x96x4StringDraw(titleStr, posX, TITLE_PADDINGTOP, UNSELECTED_BRIGHTNESS);
		if (activity->pageContext > 0)
		{
			RIT128x96x4StringDraw("<", getModifiableIndicatorHorzPos(DRAWMODE_OPTION, HORZDIR_LEFT, posX, ustrlen(titleStr)), TITLE_PADDINGTOP, UNSELECTED_BRIGHTNESS);
		}
		if (activity->pageContext < activity->numPages-1)
		{
			RIT128x96x4StringDraw(">", getModifiableIndicatorHorzPos(DRAWMODE_OPTION, HORZDIR_RIGHT, posX, ustrlen(titleStr)), TITLE_PADDINGTOP, UNSELECTED_BRIGHTNESS);
		}
	}
}

void drawListViewItem(const Item* item, unsigned int index, tBoolean selected)
{
	// draw item label
	unsigned int posX = getHorzAlignment(item->name, ITEM_TEXTALIGN, ITEM_MARGIN);
	unsigned int posY = TITLE_PADDINGTOP + TITLE_ITEM_SEP + index*ITEM_HEIGHT;
	RIT128x96x4StringDraw(CLEAR_ROW, 0, posY, 0);
	if (selected)
	{
		RIT128x96x4StringDraw(item->name, posX, posY, SELECTED_BRIGHTNESS);
	}
	else
	{
		RIT128x96x4StringDraw(item->name, posX, posY, UNSELECTED_BRIGHTNESS);
	}

	// draw item option
	char buffer[OPTION_NAME_SIZE];
	char* displayStr;
	switch (item->optionType)
	{
		case(OPTIONTYPE_INT):
			usprintf(buffer, "%4d\0", item->getter());
			displayStr = buffer;
			break;
		case(OPTIONTYPE_STRING):
			displayStr = item->options.values[item->getter()];
			break;
	}
	posX = getHorzAlignment(displayStr, OPTION_TEXTALIGN, OPTION_MARGIN);
	if (selected)
	{
		RIT128x96x4StringDraw(displayStr, posX, posY, SELECTED_BRIGHTNESS);
	}
	else
	{
		RIT128x96x4StringDraw(displayStr, posX, posY, UNSELECTED_BRIGHTNESS);
	}

	// indicate whether option is modifiable
	if (item->accessType == OPTIONACCESS_MODIFIABLE)
	{
		if (selected)
		{
			if (item->getter() < item->options.maxIndex)
			{
				RIT128x96x4StringDraw(">", getModifiableIndicatorHorzPos(DRAWMODE_OPTION, HORZDIR_RIGHT, posX, ustrlen(displayStr)), posY, SELECTED_BRIGHTNESS);
			}
			if (item->getter() > item->options.minIndex)
			{
				RIT128x96x4StringDraw("<", getModifiableIndicatorHorzPos(DRAWMODE_OPTION, HORZDIR_LEFT, posX, ustrlen(displayStr)), posY, SELECTED_BRIGHTNESS);
			}
		}
		else
		{
			if (item->getter() < item->options.maxIndex)
			{
				RIT128x96x4StringDraw(">", getModifiableIndicatorHorzPos(DRAWMODE_OPTION, HORZDIR_RIGHT, posX, ustrlen(displayStr)), posY, UNSELECTED_BRIGHTNESS);
			}
			if (item->getter() > item->options.minIndex)
			{
				RIT128x96x4StringDraw("<", getModifiableIndicatorHorzPos(DRAWMODE_OPTION, HORZDIR_LEFT, posX, ustrlen(displayStr)), posY, UNSELECTED_BRIGHTNESS);
			}
		}
	}
}

void drawTraceViewPlot(const TraceView* view, tBoolean selected)
{
	unsigned char brightness = selected ? SELECTED_BRIGHTNESS : UNSELECTED_BRIGHTNESS;

	TraceNode* plotting = getLatestNode(view->buffer);
	int headX = plotting->x;		// latest node appears rightmost of the trace

	static int prevYPos[128];
	int dispPosX = (plotting->x - headX)/((int) view->dispHorzScale) + (PX_HORZ-2);
	int dispPosY = view->zeroLine - (plotting->y*CHAR_HEIGHT)/(view->vertScale);
	// draw until screen is full or up to one being written,
	// note: return value of getLatestNode() will change while buffer is being drawn
	do {
		if ((dispPosY > (TITLE_PADDINGTOP+CHAR_HEIGHT+TITLE_TRACE_SEP)) && (dispPosY < (TITLE_PADDINGTOP+CHAR_HEIGHT+TITLE_TRACE_SEP+TRACE_HEIGHT)))
		{
			drawPointRtoL(dispPosX, prevYPos[dispPosX], 0);				// clear the previous dot in this x position
			prevYPos[dispPosX] = dispPosY;								// store the current dot position to clear next time
			drawPointRtoL(dispPosX, dispPosY, brightness);
		}

		plotting = plotting->prev;			// draw the previous node

		dispPosX = (plotting->x - headX)/((int) view->dispHorzScale) + (PX_HORZ-1);
		dispPosY = view->zeroLine - (plotting->y*CHAR_HEIGHT)/(view->vertScale);
	} while(plotting != NULL && dispPosX >= 0 && plotting != getLatestNode(view->buffer));

}

void drawPointRtoL(unsigned int x, unsigned int y, char level)
{
	if (level > MAX_BRIGHT_LEVEL)
	{
		level = MAX_BRIGHT_LEVEL;	// limit brightness
	}
	unsigned char dot[1];
	dot[0] = level | (level << 4);

	RIT128x96x4ImageDraw(dot, x, y, 2, 1);	// draw do
}

void clearTracePlot(void)
{
	unsigned int i=0;
	for (i=TITLE_PADDINGTOP+CHAR_HEIGHT+TITLE_TRACE_SEP; i<TITLE_PADDINGTOP+CHAR_HEIGHT+TITLE_TRACE_SEP+TRACE_HEIGHT; i+=CHAR_HEIGHT)
	{
		unsigned int j=0;
		for (j=0; j<PX_HORZ; j+=CHAR_WIDTH)
		{
			RIT128x96x4StringDraw(" ", j, i, 0);
		}
	}
}

unsigned int getHorzAlignment(const char* str, TextAlign align, unsigned int margin)
{
	unsigned int pos;
	switch (align)
	{
		case(TEXTALIGN_LEFT):
			pos = margin;
			break;
		case(TEXTALIGN_CENTER):
			pos = (PX_HORZ - ustrlen(str)*CHAR_WIDTH)/2 + margin;
			break;
		case(TEXTALIGN_RIGHT):
			pos = PX_HORZ - margin - ustrlen(str)*CHAR_WIDTH;
			break;
	}
	return pos;
}

unsigned int getModifiableIndicatorHorzPos(DrawMode menuType, HorzDir dir, unsigned int x, unsigned int strlen)
{
	strlen--;		// account for null byte

	unsigned int margin;
	unsigned int nameSize;
	tBoolean wrap;

	switch(menuType)
	{
		case(DRAWMODE_OPTION):
			margin = OPTION_MODIFIABLEINDICATOR_MARGIN;
			nameSize = OPTION_NAME_SIZE;
			wrap = OPTION_MODIFIABLEINDICATOR_WRAP;
			break;
		case(DRAWMODE_TITLE):
			margin = TITLE_MARGIN;
			nameSize = VIEW_NAME_SIZE;
			wrap = TITLE_WRAP;
			break;
	}

	switch(OPTION_TEXTALIGN)
	{
		case(TEXTALIGN_LEFT):
			switch(dir)
			{
				case(HORZDIR_LEFT):
					return (x - margin - CHAR_WIDTH);
				case(HORZDIR_RIGHT):
					if (wrap)
					{
						return (x + strlen*CHAR_WIDTH + margin);
					}
					else
					{
						return (x + nameSize*CHAR_WIDTH + margin);
					}
			}
		case(TEXTALIGN_CENTER):
			switch(dir)
			{
				case(HORZDIR_LEFT):
					if (wrap)
					{
						return (x - margin - CHAR_WIDTH);
					}
					else
					{
						return (x + CHAR_WIDTH*(strlen - nameSize)/2 - margin - CHAR_WIDTH);
					}
				case(HORZDIR_RIGHT):
					if (OPTION_MODIFIABLEINDICATOR_WRAP)
					{
						return (x + CHAR_WIDTH*strlen + margin);
					}
					else
					{
						return (x + CHAR_WIDTH*(strlen + nameSize)/2 + margin);
					}
			}
		case(TEXTALIGN_RIGHT):
			switch(dir)
			{
				case(HORZDIR_LEFT):
					if (wrap)
					{
						return (x - margin - CHAR_WIDTH);
					}
					else
					{
						return (x + CHAR_WIDTH*(strlen - nameSize) + margin);
					}
				case(HORZDIR_RIGHT):
					return (x + CHAR_WIDTH*strlen + margin);
			}
	}
	return 0;
}

