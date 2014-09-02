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
TraceView traceView(unsigned int sparseIndex, TraceNode *head) {
	TraceView traceView = {NULL,0}; //return this if bad input is detected
	if(sparseIndex > 0) { //checks for bad input
		traceView.head = head; //sets head of traceView to head of TraceNode circular buffer
		traceView.sparseIndex = sparseIndex; //sets the sparseIndex
	}
	return traceView;
}


/*add item one at a time i.e. seperate function calls for seperate items to a ListView. Returns 0 if cannot attach item to ListView*/
Item item(char *name, OptionType optionType, OptionAccess accessType, Options options,int getter(),void setter(int))  {
	Item item;
	//write the name to the Item object
	int m = 0;
	while(m < (ITEM_NAME_SIZE-1) && name[m] != '\0') {
		item.name[m] = name[m];
		m++;
	}
	item.name[m] = '\0'; //Null tertermiate the string. If bigger string than ITEM_NAME_SIZE then only take first ITEM_NAME_SIZE-1 characters
	//set all the other variables of the Item object
	item.accessType = accessType;
	item.optionType = optionType;
	item.options = options;
	item.getter = getter;
	if(accessType == OPTIONACCESS_MODIFIABLE) {
		item.setter = setter;
	} else {
		item.setter = NULL;
	}
	return item;
}


/*Creates and returns an Options object*/
Options option(int minIndex, int maxIndex, unsigned int skip) {
	Options newOptions;
	newOptions.minIndex = minIndex;
	newOptions.maxIndex = maxIndex;
	newOptions.skip = skip;
	int i = 0;
	for(i = 0; i < ITEM_MAX_OPTIONSTR; i++) {
		newOptions.values[i][0] = '\0';
	}
	return newOptions;
}


/*init a new Activity and pass it back*/
Activity acitivity(unsigned int numPages) {
	Activity newActivity;
	newActivity.numPages = numPages;
	newActivity.pageContext = 0;  //set defualt value
	newActivity.cursorContext= 0;  //set defualt value
	int i = 0;
	for(i = 0; i < ACTIVITY_MAX_PAGES; i++) {
		newActivity.menuTypes[i] = VIEWTYPE_NONE;
		newActivity.menus[i] =  NULL;
	}
	return newActivity;
}



/*Inits and returns a ListView*/
ListView listView(unsigned int numItems, char *name) {
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
		//write the name to the Item object
		int m = 0;
		while(m < (VIEW_NAME_SIZE-1) && name[m] != '\0') {
			newListView.name[m] = name[m];
			m++;
		}
		newListView.name[m] = '\0'; //Null tertermiate the string. If bigger string than ITEM_NAME_SIZE then only take first ITEM_NAME_SIZE-1 characters
	}
	return newListView;
}
