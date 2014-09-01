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
#include <stdlib.h>

#include "include/FreeRTOS.h"
#include "include/task.h"
#include "demo_code\basic_io.h"

//could/should/maybe write own function and minimize need to import string.h.
#include <string.h>

//circular buffer size
#define MAX_CIRC_BUFF_SIZE 300


/*allows for multiple TraceViews if needed in the future. Set circular buffer
 *of TraceNodes and gets a TraceView stuct pointing to the head of the circular buffer. Note
 *this function only allocates a memory block for a circular buffer. It does not dynmatically create/delete TraceNodes
 *as needed. Unless someone repetively calls this function there will be no memory leaks. A check could be
 *be written for this but probally unnessary */
TraceView initTraceView(unsigned int sparseIndex, int circularBufferSize) {
	TraceView traceView = {NULL,0}; //return this if bad input is detected. Up to user of function to check for this
	if(circularBufferSize > 0 && circularBufferSize < MAX_CIRC_BUFF_SIZE && sparseIndex > 0) { //checks for bad input
		//creates circular buffer of TraceNodes
		TraceNode *newTraceNode;
		newTraceNode = (TraceNode *)malloc(sizeof(TraceNode)*circularBufferSize);
		int i = 0;
		for(i = 0; i < (circularBufferSize-1); i++) {
			(newTraceNode+i)->next = (newTraceNode + i + 1);
			//dont want junk for all the TraceNodes defined
			(newTraceNode+i)->x = 0;
			(newTraceNode+i)->y = 0;
		}
		(newTraceNode+i)->next = newTraceNode;
		//dont want junk for all the TraceNodes defined
		(newTraceNode+i)->x = 0;
		(newTraceNode+i)->y = 0;
		traceView.head = newTraceNode; //sets head of traceView to head of TraceNode circular buffer
		traceView.sparseIndex = sparseIndex; //sets the sparseIndex
	} else {
		/*bad error case. Prints to usb that the function was declared incorrectly. Used for testing only, may delete this
		 *  unless I can think of something else to do here. Requires importing FreeRTOS libray
		 */
		vPrintString("initTraceView declared incorrectly");
	}
	return traceView;
}


/*add item one at a time i.e. seperate function calls for seperate items to a ListView. Returns 0 if cannot attach item to ListView*/
int attachItem(char *name, OptionType optionType, OptionAccess accessType, Options options,int getter(),void setter(int),ListView *listView)  {
	int errorFlag = 0; //return this if function could not attach an  item to a ListView
	int i = 0;
	for(i = 0; i < LISTVIEW_MAX_ITEMS; i++) {
		if(listView->items[i].setter == NULL) { //check if item has been declared.
			strcpy (listView->items[i].name, name); //null termination? overflow checking?
			listView->items[i].accessType = accessType;
			listView->items[i].optionType = optionType;
			listView->items[i].options = options;
			listView->items[i].getter = getter;
			listView->items[i].setter = setter;
			errorFlag = 1;
			break;
		}
	}
	return errorFlag;
}


/*Doesnt check the size of the string passed into it yet. You pass in &values[0][0] assumes that you passed in all
 * nessary strings*/
Options createOption(int minIndex, int maxIndex, char *values) {
	Options newOptions;
	newOptions.minIndex = minIndex;
	newOptions.maxIndex = maxIndex;
	int i = 0;
	int l = 0;
	for(i = 0; i < ITEM_MAX_OPTIONSTR; i++) {
		for(l = 0; l < ITEM_NAME_SIZE; l++) {
			if(*(values + (i*ITEM_NAME_SIZE) + l) != '\0') {
				newOptions.values[i][l] = *(values + (i*ITEM_NAME_SIZE) + l); //pointer magic
			} else {
				newOptions.values[i][l] = '\0'; //null terminate the string
				break; //only read what needs to be read
			}
		}
	}
	return newOptions;
}


/*init a new Activity and pass it back*/
Activity initAcitivity(void* menus[], ViewType menuTypes[],  unsigned int numPages) {
	Activity newActivity;
	newActivity.numPages = numPages;
	newActivity.pageContext = 0;  //set defualt value
	newActivity.cursorContext= 0;  //set defualt value
	int i = 0;
	for(i = 0; i < ACTIVITY_MAX_PAGES; i++) {
		if(i < numPages) {
			newActivity.menuTypes[i] = menuTypes[i];
			newActivity.menus[i] =  menus[i];
		} else {
			newActivity.menuTypes[i] = VIEWTYPE_NONE;
			newActivity.menus[i] =  NULL;
		}
	}
	return newActivity;
}



//what happens to Items not used? Two cases this can occur. 1) User does not declare Item. 2) The number of pages declared
//is less than the maximum allowared therefore the extra items will still be created if not used. In this implementation
//it is up to the user to check for nonillialized items with something like ListView.items[i].getter == NULL
ListView initListView(unsigned int numItems) {
	ListView newListView;
	newListView.numItems = 0; //check for this for wrong init of ListView
	int i = 0;
	for(i = 0; i < LISTVIEW_MAX_ITEMS; i++) {
		//so we know that the item has been initizialed later on or not. Will improve but for now
		newListView.items[i].name[0] = '\0';
		newListView.items[i].getter = NULL;
		newListView.items[i].setter = NULL;
	}
	if(numItems < LISTVIEW_MAX_ITEMS && numItems > 0) { //check for bad input
		newListView.numItems = numItems;
	}
	return newListView;
}
