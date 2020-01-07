/*

    mp_video.h

    Video driver definitions.

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

extern int _mpv_x_size;
extern int _mpv_y_size;

/* mpv_readline types */

#define MPR_OPEN		0
#define MPR_SAVE		1
#define MPR_SEEK		2
#define MPR_GOTO		3
#define MPR_REPLACETHIS 	4
#define MPR_REPLACEWITH 	5
#define MPR_TAG 		6
#define MPR_EXEC		7
#define MPR_WORDWRAP		8
#define MPR_TABSIZE		9
#define MPR_EXECFUNCTION	10
#define MPR_GREPFILES		11
#define MPR_LAST		MPR_GREPFILES

int mpv_strcasecmp(char * s1, char * s2);
mp_txt * mpv_glob(char * spec);

void mpv_goto(int x, int y);
void mpv_char(int c, int color);
void mpv_str(char * str, int color);
void mpv_cursor(int x, int y);
void mpv_refresh(void);

void mpv_title(char * str);
void mpv_status_line(char * str);

void mpv_add_menu(char * label);
void mpv_add_menu_item(char * funcname);
void mpv_check_menu(char * funcname, int toggle);
int mpv_menu(void);

void mpv_alert(char * msg, char * msg2);
int mpv_confirm(char * prompt);
int mpv_list(char * title, mp_txt * txt);
char * mpv_readline(int type, char * prompt, char * def);

int mpv_system_to_clipboard(void);
void mpv_clipboard_to_system(void);

void mpv_about(void);
int mpv_help(char * term, int synhi);
int mpv_zoom(int inc);

void mpv_scrollbar(int pos, int size, int max);
void mpv_filetabs(void);

void mpv_set_variable(char * var, char * value);
int mpv_args_1(int * n, int argc, char * argv[]);
int mpv_args_2(int * n, int argc, char * argv[]);

int mpv_startup(void);
void mpv_shutdown(void);
