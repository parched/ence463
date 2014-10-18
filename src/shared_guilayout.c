/**
 * \file shared_guilayout.c
 * \brief Common module which declares types used to markup menu layout and hierachy.
 * \author Hugh Bisset
 * \version 1.0
 * \date 2014-08-30
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

#include "shared_guilayout.h"

#include "inc/hw_types.h"
#include "shared_displayformat128x96.h"
#include <ustdlib.h>

Options option(int minIndex, int maxIndex)
{
	Options newOptions;
	newOptions.minIndex = minIndex;
	newOptions.maxIndex = maxIndex;
	newOptions.skip = 1;
	return newOptions;
}

Item item(char *name, OptionType optionType, OptionAccess accessType, Options options, int (*getter)(void))
{
	Item item;
	//write the name to the Item object
	ustrncpy(item.name, name, ITEM_NAME_SIZE);

	//set all the other variables of the Item object
	item.optionType = optionType;
	item.accessType = accessType;
	item.options = options;
	item.getter = getter;

	return item;
}

TraceView traceView(char *name, CircularBufferHandler *buffer, int zeroHeight, unsigned int minHorzScale, unsigned int maxHorzScale, int vertScale)
{
	TraceView traceView;

	traceView.buffer = buffer;
	traceView.horzScaleStep = 1;
	traceView.minZoomHorzScale = minHorzScale;
	if (maxHorzScale < minHorzScale)
	{
		maxHorzScale = minHorzScale;
	}
	traceView.maxZoomHorzScale = maxHorzScale;
	traceView.dispHorzScale = traceView.minZoomHorzScale;

	// convert height from bottom of trace to screen position
	if (zeroHeight > TRACE_HEIGHT || zeroHeight < 0)
	{
		// if zero height outside of drawable range, make it center of trace plot
		traceView.zeroLine = TITLE_PADDINGTOP + CHAR_HEIGHT + TITLE_TRACE_SEP + TRACE_HEIGHT / 2;
	}
	else
	{
		// convert pixel height from bottom of plot to pixel index from top of screen
		traceView.zeroLine = PX_VERT - TRACE_MARGIN_BOTTOM - zeroHeight;
	}

	if (zeroHeight == -2)
	{
		traceView.dynamicZero = true;
	}
	else
	{
		traceView.dynamicZero = false;
	}

	traceView.vertScale = vertScale;

	ustrncpy(traceView.name, name, VIEW_NAME_SIZE);

	return traceView;
}

ListView listView(char *name, unsigned int numItems)
{
	ListView newListView;

	newListView.numItems = numItems;
	ustrncpy(newListView.name, name, VIEW_NAME_SIZE);

	return newListView;
}

Activity activity()
{
	Activity newActivity;

	newActivity.numPages = 0;
	newActivity.pageContext = 0; //set default value
	newActivity.cursorContext = 0; //set default value

	return newActivity;
}


int addView(Activity *activity, void *view, ViewType type)
{
	int success = 1; //bad input flag by default
	if (activity->numPages < LISTVIEW_MAX_ITEMS)
	{
		activity->menus[activity->numPages] = view;
		activity->menuTypes[activity->numPages] = type;
		activity->numPages = activity->numPages + 1;
		success = 0; //success, set flag to reflect
	}
	return success;
}
