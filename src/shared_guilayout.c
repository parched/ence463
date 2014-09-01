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


/*inits a TraceView object with the variables passed to it
 * */
TraceView initTraceView(unsigned int sparseIndex, TraceNode *traceNode) {
	TraceView traceView = {NULL,0}; //return this if bad input is detected
	if(sparseIndex > 0) { //checks for bad input
		traceView.head = traceNode; //sets head of traceView to head of TraceNode circular buffer
		traceView.sparseIndex = sparseIndex; //sets the sparseIndex
	}
	return traceView;
}


/*add item one at a time i.e. seperate function calls for seperate items to a ListView. Returns 0 if cannot attach item to ListView*/
int attachItem(char *name, OptionType optionType, OptionAccess accessType, Options options,int getter(),void setter(int),ListView *listView)  {
	int errorFlag = 0; //return this if function could not attach an  item to a ListView
	int i = 0;
	for(i = 0; i < LISTVIEW_MAX_ITEMS; i++) { //finds the next empty Item to attach
		if(listView->items[i].setter == NULL) { //check if item has been declared
			//write the name to the Item object
			int m = 0;
			while(m < (ITEM_NAME_SIZE-1) && name[m] != '\0') {
				listView->items[i].name[m] = name[m];
				m++;
			}
			listView->items[i].name[m] = '\0'; //Null tertermiate the string. If bigger string than ITEM_NAME_SIZE then only take first ITEM_NAME_SIZE-1 characters
			//set all the other variables of the Item object
			listView->items[i].accessType = accessType;
			listView->items[i].optionType = optionType;
			listView->items[i].options = options;
			listView->items[i].getter = getter;
			listView->items[i].setter = setter;
			errorFlag = 1; //success
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
				newOptions.values[i][l] = *(values + (i*ITEM_NAME_SIZE) + l); //uses pointers to get value from 2D char array
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



/*Inits and returns a ListView*/
ListView initListView(unsigned int numItems) {
	ListView newListView;
	newListView.numItems = 0; //check for this for wrong init of ListView
	int i = 0;
	for(i = 0; i < LISTVIEW_MAX_ITEMS; i++) {
		//so we know that the item has been initizialed later on or not.
		newListView.items[i].name[0] = '\0';
		newListView.items[i].getter = NULL;
		newListView.items[i].setter = NULL;
	}
	if(numItems < LISTVIEW_MAX_ITEMS && numItems > 0) { //check for bad input
		newListView.numItems = numItems;
	}
	return newListView;
}
