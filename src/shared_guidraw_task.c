/**
 * \file asc.c
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

#include "include/FreeRTOS.h"
#include "include/queue.h"
#include "shared_guilayout.h"

#define INPUTEVENT_QUEUE_SIZE 10
#define GUITASK_SLEEP_MS 40

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
}

void changeOption(Activity* activity, HorzDir dir)
{
	const unsigned int page = activity->pageContext;
	if (activity->menuTypes[page] == VIEWTYPE_LIST)
	{
		unsigned int listCursorPos = activity->cursorContext- 1;
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
				if (activity->menus[page]->sparseIndex <= 1)
				{
					// zoom in
					activity->menus[page]->sparseIndex /= 2;
				}
				break;
			case(HORZDIR_LEFT):
				if (activity->menus[page]->sparseIndex >= TRACEVIEW_MAX_ZOOM)
				{
					// zoom out
					activity->menus[page]->sparseIndex *= 2;
				}
				break;
		}
	}
}

