/*

    mp_conf.h

    Configuration file.

    mp - Programmer Text Editor

    Copyright (C) 1991-2004 Angel Ortega <angel@triptico.com>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

    http://www.triptico.com

*/

extern char _mpc_home[1024];
extern char _mpc_config_file[1024];
extern int _mpc_text_interface;
extern mp_txt * _menu_info;

#define MP_COLOR_NORMAL 	0
#define MP_COLOR_SELECTED	1
#define MP_COLOR_COMMENT	2
#define MP_COLOR_STRING 	3
#define MP_COLOR_TOKEN		4
#define MP_COLOR_VAR		5
#define MP_COLOR_CURSOR 	6
#define MP_COLOR_CAPS		7
#define MP_COLOR_LOCAL		8

#define MP_COLOR_TEXT_TITLE	9
#define MP_COLOR_TEXT_MENU_ELEM 10
#define MP_COLOR_TEXT_MENU_SEL	11
#define MP_COLOR_TEXT_FRAME1	12
#define MP_COLOR_TEXT_FRAME2	13
#define MP_COLOR_TEXT_SCROLLBAR 14
#define MP_COLOR_TEXT_SCR_THUMB 15

#define MP_COLOR_PRIVATE	MP_COLOR_TEXT_TITLE
#define MP_COLOR_NUM		MP_COLOR_TEXT_SCR_THUMB+1

struct _mpc_color_desc
{
	unsigned int ink_rgb;
	unsigned int paper_rgb;
	int ink_text;
	int paper_text;
	int italic;
	int underline;
	int reverse;
	int bright;
	char * colorname;
};

extern struct _mpc_color_desc mpc_color_desc[MP_COLOR_NUM];

void mpc_startup(void);
