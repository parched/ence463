/**
 * \file shared_displayformat128x64.h
 * \brief Defines the margins and padding for display
 * \author George Xian
 * \version 1.0
 * \date 2014-08-30
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

#ifndef SHARED_DISPLAYFORMAT128x64_H

/* Display parameters */
#define PX_HORZ 128
#define PX_VERT 64

#define CHAR_WIDTH 6

/* Formatting parameter selections */
typedef enum {TEXTALIGN_LEFT, TEXTALIGN_CENTER, TEXTALIGN_RIGHT} TextAlign;

/* General View parameters */
#define UNSELECTED_BRIGHTNESS 6
#define SELECTED_BRIGHTNESS 15

/* ListView parameters */
#define TITLE_PADDINGTOP 5
#define TITLE_TEXTALIGN TEXTALIGN_CENTER
#define TITLE_MARGIN 0

#define TITLE_ITEM_SEP 10
#define ITEM_HEIGHT 10
#define ITEM_TEXTALIGN TEXTALIGN_LEFT
#define ITEM_MARGIN 4

#define OPTION_TEXTALIGN TEXTALIGN_CENTER
#define OPTION_MARGIN 20
#define OPTION_MODFIABLEINDICATOR_MARGIN 4


#endif

