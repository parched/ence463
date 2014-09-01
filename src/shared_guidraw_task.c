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
#include "include/FreeRTOS.h"
#include "include/queue.h"
#include "include/task.h"
#include "shared_guilayout.h"
#include "shared_displayformat128x64.h"

#define INPUTEVENT_QUEUE_SIZE 10
#define GUITASK_SLEEP_MS 40
#define OLED_FREQ 1000000

typedef enum {VERTDIR_UP, VERTDIR_DOWN} VertDir;
typedef enum {HORZDIR_LEFT, HORZDIR_RIGHT} HorzDir;

typedef struct
{
	Button button;
	ButtonEvent event;
} InputEvent;

xQueueHandle inputEventQueue;
Activity* unitActivity;

char CLEAR_ROW [PX_HORZ] = "                      ";

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
 * \brief Determines what type of View to redraw
 *
 * \param activity Pointer to the activity
 */
void redrawView(Activity* activity);

/**
 * \brief Draws a new ListView upon switching to a different one
 *
 * \param view Pointer to the ListView to draw
 */
void redrawListView(Activity* view);

/**
 * \brief Draws given string formatted as title in a ListView
 *
 * \param activity Pointer to activity containing title to draw
 */
void drawListViewTitle(Activity* activity);

/**
 * \brief Draws the given Item in a ListView
 *
 * \param item Pointer to Item object to draw
 * \param index The place this item is in the ListView where 0 is top
 * \param selected Is Item selected
 */
void drawListViewItem(Item* item, unsigned int index, tBoolean selected);

/**
 * \brief Gets the horizontal position of text given alignment and margins
 *
 * \param str String to draw
 * \param type Type of alignment
 * \param margin Offset given alignment type
 */
unsigned int getHorzAlignment(char* str, TextAlign align, unsigned int margin);


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
	const portTickType xFrequency = GUITASK_SLEEP_MS*portTICK_RATE_MS;
	xLastWakeTime = xTaskGetTickCount();

	// initialize screen
	RIT128x96x4Init(OLED_FREQ);
	redrawView(unitActivity);

	for (;;)
	{
		// wait for next cycle
		vTaskDelayUntil( &xLastWakeTime, xFrequency);

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
							drawListViewTitle(activity);
						}
						else
						{
							// user is now selecting another item, redraw the item
							drawListViewItem(&(((ListView*) (activity->menus[page]))->items[listCursorContext]), listCursorContext, true);
						}
						break;
					case(VIEWTYPE_TRACE):
						drawListViewTitle(activity);
						//TODO: dim the trace as it is no longer being selected
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

					// update display
					drawListViewTitle(activity);
					//TODO: Brighten trace as it is now being selected
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
						drawListViewTitle(activity);
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
		switch(dir)
		{
			case(HORZDIR_RIGHT):
				if (((TraceView*) activity->menus[page])->sparseIndex <= 1)
				{
					// zoom in
					((TraceView*) activity->menus[page])->sparseIndex /= 2;
					//TODO: Redraw trace
				}
				break;
			case(HORZDIR_LEFT):
				if (((TraceView*) activity->menus[page])->sparseIndex >= TRACEVIEW_MAX_ZOOM)
				{
					// zoom out
					((TraceView*) activity->menus[page])->sparseIndex *= 2;
					//TODO: Redraw trace
				}
				break;
		}
	}
}

void redrawView(Activity* activity)
{
	switch (activity->menuTypes[activity->pageContext])
	{
		case(VIEWTYPE_LIST):
			redrawListView(activity);
			break;
		case(VIEWTYPE_TRACE):
			//TODO: Implement initial TraceView draw
			break;
	}
}

void redrawListView(Activity* activity)
{
	RIT128x96x4Clear();

	// draw title (is selected when coming to new page)
	drawListViewTitle(activity);

	// draw items (none are selected when coming to new page)
	ListView* listView = (ListView*) activity->menus[activity->pageContext];
	unsigned int i;
	for (i=0; i<(listView->numItems); i++)
	{
		drawListViewItem(&(listView->items[i]), i, false);
	}
}

void drawListViewTitle(Activity* activity)
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
			RIT128x96x4StringDraw("<", posX-CHAR_WIDTH-OPTION_MODFIABLEINDICATOR_MARGIN, TITLE_PADDINGTOP, SELECTED_BRIGHTNESS);
		}
		if (activity->pageContext < activity->numPages-1)
		{
			RIT128x96x4StringDraw(">", posX+ustrlen(titleStr)*CHAR_WIDTH+OPTION_MODFIABLEINDICATOR_MARGIN, TITLE_PADDINGTOP, SELECTED_BRIGHTNESS);
		}
	}
	else
	{
		// title not selected, draw dim
		RIT128x96x4StringDraw(titleStr, posX, TITLE_PADDINGTOP, UNSELECTED_BRIGHTNESS);
		if (activity->pageContext > 0)
		{
			RIT128x96x4StringDraw("<", posX-CHAR_WIDTH-OPTION_MODFIABLEINDICATOR_MARGIN, TITLE_PADDINGTOP, UNSELECTED_BRIGHTNESS);
		}
		if (activity->pageContext < activity->numPages-1)
		{
			RIT128x96x4StringDraw(">", posX+ustrlen(titleStr)*CHAR_WIDTH+OPTION_MODFIABLEINDICATOR_MARGIN, TITLE_PADDINGTOP, UNSELECTED_BRIGHTNESS);
		}
	}
}

void drawListViewItem(Item* item, unsigned int index, tBoolean selected)
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
				RIT128x96x4StringDraw(">", posX+OPTION_NAME_SIZE*CHAR_WIDTH+OPTION_MODFIABLEINDICATOR_MARGIN, posY, SELECTED_BRIGHTNESS);
			}
			if (item->getter() > item->options.minIndex)
			{
				RIT128x96x4StringDraw("<", posX-CHAR_WIDTH-OPTION_MODFIABLEINDICATOR_MARGIN, posY, SELECTED_BRIGHTNESS);
			}
		}
		else
		{
			if (item->getter() < item->options.maxIndex)
			{
				RIT128x96x4StringDraw(">", posX+OPTION_NAME_SIZE*CHAR_WIDTH+OPTION_MODFIABLEINDICATOR_MARGIN, posY, UNSELECTED_BRIGHTNESS);
			}
			if (item->getter() > item->options.minIndex)
			{
				RIT128x96x4StringDraw("<", posX-CHAR_WIDTH-OPTION_MODFIABLEINDICATOR_MARGIN, posY, UNSELECTED_BRIGHTNESS);
			}
		}
	}
}

unsigned int getHorzAlignment(char* str, TextAlign align, unsigned int margin)
{
	unsigned int pos;
	switch (align)
	{
		case(TEXTALIGN_LEFT):
			pos = margin;
			break;
		case(TEXTALIGN_CENTER):
			pos = PX_HORZ/2 + margin - (ustrlen(str)/2)*CHAR_WIDTH;
			break;
		case(TEXTALIGN_RIGHT):
			pos = PX_HORZ - margin - ustrlen(str)*CHAR_WIDTH;
			break;
	}
	return pos;
}

