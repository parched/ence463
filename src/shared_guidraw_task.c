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
#include "drivers/rit128x96x4.h"
#include "include/FreeRTOS.h"
#include "include/queue.h"
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
void redrawListView(ListView* view);

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
	inputEventQueue = xQueueCreate(INPUTEVENT_QUEUE_SIZE, sizeof(InputEvent));

	RIT128x96x4Init(OLED_FREQ);
	redrawView(unitActivity);

	for (;;)
	{
		// process input events
		InputEvent event;
		while (xQueueReceive(inputEventQueue, (InputEvent*) &event, 0) == pdTRUE)
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
			}
			break;
		case(VERTDIR_DOWN):
			if (activity->menuTypes[page] == VIEWTYPE_TRACE)
			{
				if (activity->cursorContext < 1)
				{
					// if in TraceView and not currently in zoom context, enter it
					activity->cursorContext++;
				}
			}
			else if (activity->menuTypes[page] == VIEWTYPE_LIST)
			{
				unsigned int listCursorPos = activity->cursorContext - 1;
				if (listCursorPos < ((ListView*) activity->menus[page])->numItems)
				{
					// if in ListView and cursor position and not at bottom, go down
					activity->cursorContext++;
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
			}
			break;
		case(HORZDIR_RIGHT):
			if (page < activity->numPages)
			{
				// go to the right page if cursor at top and not at rightmost
				activity->pageContext++;
			}
			break;
	}

	redrawView(activity);
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
					}
					break;
				case(HORZDIR_RIGHT):
					if (selectedItem->getter() < selectedOption->maxIndex)
					{
						// if ListView, not at top and selected item is modifiable and not at limit, increase
						selectedItem->setter(selectedItem->getter() + selectedOption->skip);
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
				}
				break;
			case(HORZDIR_LEFT):
				if (((TraceView*) activity->menus[page])->sparseIndex >= TRACEVIEW_MAX_ZOOM)
				{
					// zoom out
					((TraceView*) activity->menus[page])->sparseIndex *= 2;
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
			redrawListView((ListView*) (activity->menus[unitActivity->pageContext]));
			break;
		case(VIEWTYPE_TRACE):
			//TODO: Implement initial TraceView draw
			break;
	}
}

void redrawListView(ListView* listView)
{
	RIT128x96x4Clear();

	// draw title (is selected when coming to new page)
	unsigned int posX = getHorzAlignment(listView->name, TITLE_TEXTALIGN, TITLE_MARGIN);
	RIT128x96x4StringDraw(listView->name, posX, TITLE_PADDINGTOP, SELECTED_BRIGHTNESS);

	// draw items (none are selected when coming to new page)
	unsigned int posY = TITLE_PADDINGTOP + TITLE_ITEM_SEP;
	unsigned int i;
	for (i=0; i<(listView->numItems); i++)
	{
		// draw item label
		posX = getHorzAlignment(listView->items[i].name, ITEM_TEXTALIGN, ITEM_MARGIN);
		RIT128x96x4StringDraw(listView->items[i].name, posX, posY, UNSELECTED_BRIGHTNESS);

		// draw item option
		char displayStr[ITEM_NAME_SIZE];	// buffer to store the option string
		switch (listView->items[i].optionType)
		{
			case(OPTIONTYPE_INT):
				usprintf(displayStr, "%u", listView->items[i].getter());
				break;
			case(OPTIONTYPE_STRING):
				ustrncpy(displayStr, listView->items[i].options.values[listView->items[i].getter()], ITEM_NAME_SIZE);
				break;
		}
		posX = getHorzAlignment(displayStr, OPTION_TEXTALIGN, OPTION_MARGIN);
		RIT128x96x4StringDraw(displayStr, posX, posY, UNSELECTED_BRIGHTNESS);

		// indicate whether option is modifiable
		if (listView->items[i].accessType == OPTIONACCESS_MODIFIABLE)
		{
			RIT128x96x4StringDraw("<", posX-CHAR_WIDTH-OPTION_MODFIABLEINDICATOR_MARGIN, posY, UNSELECTED_BRIGHTNESS);
			RIT128x96x4StringDraw(">", posX+ITEM_NAME_SIZE*CHAR_WIDTH+OPTION_MODFIABLEINDICATOR_MARGIN, posY, UNSELECTED_BRIGHTNESS);
		}

		posY += i*ITEM_HEIGHT;		// space Items out
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
			pos = PX_HORZ/2 + margin - ustrlen(str)*(CHAR_WIDTH/2);
			break;
		case(TEXTALIGN_RIGHT):
			pos = PX_HORZ - margin - ustrlen(str)*CHAR_WIDTH;
			break;
	}
	return pos;
}

