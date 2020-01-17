/*

    mpv_curses.c

    Curses Interface (Linux/Unix)

    mp - Programmer Text Editor

    Copyright (C) 1991-2004 Angel Ortega <angel@triptico.com>
	Copyright (C) 2020 juiceghost <jg@renegade.se>

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

    https://github.com/juiceghost/homebrew-mp

*/

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <curses.h>
#include <signal.h>
#include <ctype.h>
#include <sys/stat.h>
#include <locale.h> /* JG: UTF-8 support */

#include "mp_core.h"
#include "mp_video.h"
#include "mp_synhi.h"
#include "mp_func.h"
#include "mp_iface.h"
#include "mp_lang.h"
#include "mp_conf.h"

/*******************
	Data
********************/

#define START_LINE 1
#define END_LINE 1

/* signal grabbing */
static sigset_t Sigset;

/* standard attributes */
static int _attrs[MP_COLOR_NUM];

/** menu **/

/* menu item */

struct mpv_menu_item
{
	char label[40]; /* label */
	char *funcname; /* funcname */
	int hk;			/* hotkey (not used) */
	int toggle;		/* toggle mark */
	struct mpv_menu_item *next;
	struct mpv_menu_item *prev;
};

/* menu bar */

struct mpv_menu_bar
{
	char label[15];				 /* label */
	struct mpv_menu_item *first; /* first item */
	struct mpv_menu_item *last;  /* last item */
	int items;					 /* item number */
	int xpos;					 /* horizontal position */
	struct mpv_menu_bar *next;
	struct mpv_menu_bar *prev;
};

/* pointers to first and last menu bars */
struct mpv_menu_bar *_mpv_menu = NULL;
struct mpv_menu_bar *_mpv_menu_last = NULL;

/* menu bar horizontal position */
static int _mpv_menu_xpos = 2;

#ifndef NCURSES_VERSION
#define NO_TRANSPARENCY
#endif

#ifndef DEFAULT_INK_COLOR
#define DEFAULT_INK_COLOR COLOR_WHITE
#endif

#ifndef DEFAULT_PAPER_COLOR
#define DEFAULT_PAPER_COLOR COLOR_BLACK
#endif

/* hint text to the left of first line */
char *_mpv_hint_text = NULL;

#ifdef POOR_MAN_BOXES
int _mpv_poor_man_boxes = 1;
#else
int _mpv_poor_man_boxes = 0;
#endif

#define MPBOX_HLINE (_mpv_poor_man_boxes ? '-' : ACS_HLINE)
#define MPBOX_VLINE (_mpv_poor_man_boxes ? '|' : ACS_VLINE)
#define MPBOX_TL_CORNER (_mpv_poor_man_boxes ? '+' : ACS_ULCORNER)
#define MPBOX_TR_CORNER (_mpv_poor_man_boxes ? '+' : ACS_URCORNER)
#define MPBOX_BL_CORNER (_mpv_poor_man_boxes ? '+' : ACS_LLCORNER)
#define MPBOX_BR_CORNER (_mpv_poor_man_boxes ? '+' : ACS_LRCORNER)
#define MPBOX_THUMB (_mpv_poor_man_boxes ? ' ' : ACS_CKBOARD)

/*****************
       Code
******************/

#include "mpv_unix_common.c"

static void _mpv_attrset(int attr)
{
#ifndef ABSOLUTELY_NO_COLORS
	attrset(attr);
#endif
}

static void _mpv_wattrset(WINDOW *w, int attr)
{
#ifndef ABSOLUTELY_NO_COLORS
	wattrset(w, attr);
#endif
}

/**
 * _mpv_sigwinch - SIGWINCH capture function.
 * @s: signal
 *
 * The capture function for the SIGWINCH signal, raised
 * whenever a window resize has been done (from i.e. xterm).
 */
static void _mpv_sigwinch(int s)
{
	/* restart curses */
	mpv_shutdown();
	mpv_startup();

	/* invalidate main window */
	clearok(stdscr, 1);
	refresh();

	/* recalc dimensions */
	_mpv_x_size = COLS - 1;
	_mpv_y_size = LINES - START_LINE - END_LINE;

	/* redraw everything */
	mpv_title(NULL);
	mpi_draw_all(_mp_active);

	/* log new size */
	mp_log("New window size: %dx%d\n", COLS, LINES);

	/* reattach */
	signal(SIGWINCH, _mpv_sigwinch);
}

/**
 * mpv_goto - Positions the cursor to start drawing.
 * @x: the x position
 * @y: the y position
 *
 * Positions the cursor to start drawing the document window.
 */
void mpv_goto(int x, int y)
{
	move(START_LINE + y, x);
}

/**
 * mpv_char - Draws a char with color in the document window.
 * @c: the char
 * @color: the color (one of MP_COLOR_ constants)
 *
 * Draws a char with color in current cursor position of the
 * document window.
 */
void mpv_char(int c, int color)
{
	_mpv_attrset(_attrs[color]);
	addch((unsigned char)c);
}

/**
 * mpv_str - Draws a string with color in the document window.
 * @str: the string
 * @color: the color (one of MP_COLOR_ constants)
 *
 * Draws a string, calling mpv_char() for each of their chars.
 */
void mpv_str(char *str, int color)
{
	_mpv_attrset(_attrs[color]);
	addstr(str);
}

/**
 * mpv_cursor - Positions the hardware cursor.
 * @x: the real x position
 * @y: the real y position
 *
 * Sets the hardware cursor to x, y position.
 */
void mpv_cursor(int x, int y)
{
	static int ox = 0;
	static int oy = 0;

	if (x == -1)
		x = ox;
	if (y == -1)
		y = oy;

	move(START_LINE + y, x);

	ox = x;
	oy = y;
}

/**
 * mpv_refresh - Refresh the screen.
 *
 * Orders the underlying system to redraw the screen.
 */
void mpv_refresh(void)
{
	refresh();
}

/**
 * _mpv_title_status - Draws the title or status line.
 * @y: the y position
 * @str1: first string
 * @str2: second (optional) string
 *
 * Draws the title or status line with str1 and, if defined,
 * concatenates str2. The y variable indicates the vertical
 * position where the line will be drawn. It is filled with
 * spaces to the right margin (internal).
 */
static void _mpv_title_status(int y, char *str1, char *str2)
{
	int n = 0;
	char *ptr;
	static char _default_hint[256];

	move(y, 0);
	_mpv_attrset(_attrs[MP_COLOR_TEXT_TITLE]);

	if (str1 != NULL)
	{
		for (n = 0; *str1 && n < _mpv_x_size; n++, str1++)
			addch(*str1);
	}

	if (str2 != NULL)
	{
		for (ptr = " - "; *ptr && n < _mpv_x_size; n++, ptr++)
			addch(*ptr);
		for (; *str2 && n < _mpv_x_size; n++, str2++)
			addch(*str2);
	}

	if (_mpv_hint_text == NULL)
	{
		char *k;
		char *t;

		k = mpf_get_keyname_by_funcname("menu");
		t = L("menu");

		snprintf(_default_hint, sizeof(_default_hint),
				 "%s %s", k, t);

		_mpv_hint_text = _default_hint;
	}

	if (y != 0)
	{
		for (; n < _mpv_x_size - strlen(_mpv_hint_text) + 1; n++)
			addch(' ');
		addstr(_mpv_hint_text);
	}
	else
	{
		for (; n < _mpv_x_size + 1; n++)
			addch(' ');
	}

	mpv_cursor(-1, -1);
}

/**
 * mpv_title - Sets the string to be drawn as title
 * @str: the string
 *
 * Sets the string to be drawn as title of the document window.
 */
void mpv_title(char *str)
{
	mp_txt *t;
	char files[1024];
	int n, m;
	char *ptr;

	if (str == NULL && _mp_active != NULL)
	{
		str = files;
		if (_mp_active->mod)
			strcpy(files, "*");
		else
			files[0] = '\0';

		strncat(files, _mp_active->name, sizeof(files) - 1);

		if (!_mp_active->sys)
		{
			t = _mp_active->next;
			n = strlen(files);

			m = sizeof(files) - 1;
			if (_mpv_x_size < m)
				m = _mpv_x_size;

			while (n < sizeof(files) - 1)
			{
				if (t == NULL)
					t = _mp_txts;
				if (t == _mp_active)
					break;

				for (ptr = " | "; *ptr && n < m; n++, ptr++)
					files[n] = *ptr;

				if (n < m && t->mod)
					files[n++] = '*';

				for (ptr = t->name; *ptr && n < m; n++, ptr++)
					files[n] = *ptr;

				t = t->next;
			}

			files[n] = '\0';
		}
	}

	_mpv_title_status(0, " mp " VERSION, str);
}

/**
 * mpv_status_line - Sets the string to be drawn as status line
 * @str: the string
 *
 * Sets the string to be drawn as the status line.
 */
void mpv_status_line(char *str)
{
	_mpv_title_status(LINES - 1, str, NULL);
}

/**
 * mpv_zoom - Zooms the document window.
 * @inc: the increment (+1 or -1)
 *
 * Increases / decreases font size of the document window,
 * if applicable.
 */
int mpv_zoom(int inc)
{
	return (0);
}

#define ctrl(k) ((k)&31)

/**
 * _mpv_main_loop - Main processing loop.
 *
 * Main processing loop.
 */
static void _mpv_main_loop(void)
{
	char *ptr = NULL;
	int k;

	mpi_move_selecting = 0;

	k = getch();

	if (_mpi_shift)
	{
		if (k == '\e')
			_mpi_shift = 0;

		switch (k)
		{
		case '0':
			ptr = "f10";
			break;
		case '1':
			ptr = "f1";
			break;
		case '2':
			ptr = "f2";
			break;
		case '3':
			ptr = "f3";
			break;
		case '4':
			ptr = "f4";
			break;
		case '5':
			ptr = "f5";
			break;
		case '6':
			ptr = "f6";
			break;
		case '7':
			ptr = "f7";
			break;
		case '8':
			ptr = "f8";
			break;
		case '9':
			ptr = "f9";
			break;

		case KEY_LEFT:
			ptr = "ctrl-cursor-left";
			break;
		case KEY_RIGHT:
			ptr = "ctrl-cursor-right";
			break;
		case KEY_DOWN:
			ptr = "ctrl-cursor-down";
			break;
		case KEY_UP:
			ptr = "ctrl-cursor-up";
			break;
		case KEY_END:
			ptr = "ctrl-end";
			break;
		case KEY_HOME:
			ptr = "ctrl-home";
			break;
		case '\r':
			ptr = "ctrl-enter";
			break;
		case KEY_ENTER:
			ptr = "ctrl-enter";
			break;

		case ' ':
			ptr = "ctrl-space";
			break;
		case 'a':
			ptr = "ctrl-a";
			break;
		case 'b':
			ptr = "ctrl-b";
			break;
		case 'c':
			ptr = "ctrl-c";
			break;
		case 'd':
			ptr = "ctrl-d";
			break;
		case 'e':
			ptr = "ctrl-e";
			break;
		case 'f':
			ptr = "ctrl-f";
			break;
		case 'g':
			ptr = "ctrl-g";
			break;
		case 'h':
			ptr = "ctrl-h";
			break;
		case 'i':
			ptr = "ctrl-i";
			break;
		case 'j':
			ptr = "ctrl-j";
			break;
		case 'l':
			ptr = "ctrl-l";
			break;
		case 'm':
			ptr = "ctrl-m";
			break;
		case 'n':
			ptr = "ctrl-n";
			break;
		case 'o':
			ptr = "ctrl-o";
			break;
		case 'p':
			ptr = "ctrl-p";
			break;
		case 'q':
			ptr = "ctrl-q";
			break;
		case 'r':
			ptr = "ctrl-r";
			break;
		case 's':
			ptr = "ctrl-s";
			break;
		case 't':
			ptr = "ctrl-t";
			break;
		case 'u':
			ptr = "ctrl-u";
			break;
		case 'v':
			ptr = "ctrl-v";
			break;
		case 'w':
			ptr = "ctrl-w";
			break;
		case 'x':
			ptr = "ctrl-x";
			break;
		case 'y':
			ptr = "ctrl-y";
			break;
		case 'z':
			ptr = "ctrl-z";
			break;
		default:
			k = 0;
			break;
		}

		_mpi_shift = 0;
	}
	else
	{
		/* Del is rubout */
		if (k == 0x7f)
			k = '\b';

		if (k < 32 || k > 255)
		{
			if (k == '\e')
				_mpi_shift = 1;

			switch (k)
			{
			case KEY_BACKSPACE:
			case '\b':
				ptr = "backspace";
				break;
			case KEY_UP:
				ptr = "cursor-up";
				break;
			case KEY_DOWN:
				ptr = "cursor-down";
				break;
			case KEY_LEFT:
				ptr = "cursor-left";
				break;
			case KEY_RIGHT:
				ptr = "cursor-right";
				break;
			case KEY_NPAGE:
				ptr = "page-down";
				break;
			case KEY_PPAGE:
				ptr = "page-up";
				break;
			case KEY_HOME:
				ptr = "home";
				break;
			case KEY_END:
				ptr = "end";
				break;
			case KEY_IC:
				ptr = "insert";
				break;
			case KEY_DC:
				ptr = "delete";
				break;
			case '\r':
				ptr = "enter";
				break;
			case KEY_ENTER:
				ptr = "enter";
				break;
			case '\t':
				ptr = "tab";
				break;
			case KEY_F(1):
				ptr = "f1";
				break;
			case KEY_F(2):
				ptr = "f2";
				break;
			case KEY_F(3):
				ptr = "f3";
				break;
			case KEY_F(4):
				ptr = "f4";
				break;
			case KEY_F(5):
				ptr = "f5";
				break;
			case KEY_F(6):
				ptr = "f6";
				break;
			case KEY_F(7):
				ptr = "f7";
				break;
			case KEY_F(8):
				ptr = "f8";
				break;
			case KEY_F(9):
				ptr = "f9";
				break;
			case KEY_F(10):
				ptr = "f10";
				break;
			case ctrl(' '):
				ptr = "ctrl-space";
				break;
			case ctrl('a'):
				ptr = "ctrl-a";
				break;
			case ctrl('b'):
				ptr = "ctrl-b";
				break;
			case ctrl('c'):
				ptr = "ctrl-c";
				break;
			case ctrl('d'):
				ptr = "ctrl-d";
				break;
			case ctrl('e'):
				ptr = "ctrl-e";
				break;
			case ctrl('f'):
				ptr = "ctrl-f";
				break;
			case ctrl('g'):
				ptr = "ctrl-g";
				break;
			case ctrl('j'):
				ptr = "ctrl-j";
				break;
			case ctrl('l'):
				ptr = "ctrl-l";
				break;
			case ctrl('n'):
				ptr = "ctrl-n";
				break;
			case ctrl('o'):
				ptr = "ctrl-o";
				break;
			case ctrl('p'):
				ptr = "ctrl-p";
				break;
			case ctrl('q'):
				ptr = "ctrl-q";
				break;
			case ctrl('r'):
				ptr = "ctrl-r";
				break;
			case ctrl('s'):
				ptr = "ctrl-s";
				break;
			case ctrl('t'):
				ptr = "ctrl-t";
				break;
			case ctrl('u'):
				ptr = "ctrl-u";
				break;
			case ctrl('v'):
				ptr = "ctrl-v";
				break;
			case ctrl('w'):
				ptr = "ctrl-w";
				break;
			case ctrl('x'):
				ptr = "ctrl-x";
				break;
			case ctrl('y'):
				ptr = "ctrl-y";
				break;
			case ctrl('z'):
				ptr = "ctrl-z";
				break;

			case KEY_SLEFT:
				mpi_move_selecting = 1;
				ptr = "cursor-left";
				break;
			case KEY_SRIGHT:
				mpi_move_selecting = 1;
				ptr = "cursor-right";
				break;
			case KEY_SHOME:
				mpi_move_selecting = 1;
				ptr = "home";
				break;
			case KEY_SEND:
				mpi_move_selecting = 1;
				ptr = "end";
				break;

			/* probably non-standard */
			case 0476:
				ptr = "kp-divide";
				break;
			case 0477:
				ptr = "kp-multiply";
				break;
			case 0500:
				ptr = "kp-minus";
				break;
			case 0501:
				ptr = "kp-plus";
				break;

#ifdef NCURSES_MOUSE_VERSION

			case KEY_MOUSE:
			{
				MEVENT m;

				getmouse(&m);
				mp_move_xy(_mp_active, m.x,
						   _mp_active->vy + m.y - 1);

				if (m.bstate & BUTTON1_PRESSED)
					ptr = "mouse-left-button";
				else if (m.bstate & BUTTON2_PRESSED)
					ptr = "mouse-middle-button";
				else if (m.bstate & BUTTON3_PRESSED)
					ptr = "mouse-right-button";
				else if (m.bstate & BUTTON4_PRESSED)
					ptr = "mouse-wheel-down";

				break;
			}
#endif
			default:
				k = 0;
				break;
			}
		}
	}

	if (k != '\0' || ptr != NULL)
		mpi_process(k, ptr, NULL);
}

/**
 * mpv_add_menu - Creates a new menu bar.
 * @label: the label
 *
 * Creates a new menu bar.
 */
void mpv_add_menu(char *label)
{
	struct mpv_menu_bar *m;
	int n;

	label = L(label);
	m = (struct mpv_menu_bar *)malloc(sizeof(struct mpv_menu_bar));

	if (m == NULL)
		return;

	for (n = 0; *label; label++)
	{
		if (*label != '&')
			m->label[n++] = *label;
	}
	m->label[n] = '\0';

	m->first = m->last = NULL;
	m->xpos = _mpv_menu_xpos;

	m->next = NULL;

	if (_mpv_menu == NULL)
	{
		_mpv_menu = m;
		m->prev = NULL;
	}
	else
	{
		_mpv_menu_last->next = m;
		m->prev = _mpv_menu_last;
	}

	/* this is the last, by now */
	_mpv_menu_last = m;

	_mpv_menu_xpos += (strlen(m->label) + 3);
}

/**
 * mpv_add_menu_item - Adds a menu item to current menu bar.
 * @funcname: the name of the function to invoke
 *
 * Adds a menu item to the current menu bar.
 */
void mpv_add_menu_item(char *funcname)
{
	struct mpv_menu_item *i;
	int n, m, l;
	char tmp[1024];
	char *keyname;
	char *label;
	int *t;

	label = L(funcname);

	i = (struct mpv_menu_item *)malloc(sizeof(struct mpv_menu_item));

	if (i == NULL)
		return;

	i->funcname = funcname;
	i->next = NULL;

	if ((t = mpf_toggle_function_value(funcname)) != NULL)
		i->toggle = *t;
	else
		i->toggle = 0;

	if ((keyname = mpf_get_keyname_by_funcname(funcname)) != NULL)
	{
		keyname = L(keyname);
		snprintf(tmp, sizeof(tmp), "%s\t%s", label, keyname);
		label = tmp;
	}

	/* calculate tabulator size */
	l = (sizeof(i->label) - strlen(label)) - 2;

	for (n = 0; label[n] != '\t' && label[n] && n < sizeof(i->label); n++)
		i->label[n] = label[n];

	for (m = n; l > 0; l--, m++)
		i->label[m] = ' ';

	if (label[n] == '\t')
		n++;
	else
		m--;
	for (; label[n]; n++, m++)
		i->label[m] = label[n];
	i->label[m] = '\0';

	if (_mpv_menu_last->first == NULL)
	{
		_mpv_menu_last->first = i;
		i->prev = NULL;
	}
	else
	{
		_mpv_menu_last->last->next = i;
		i->prev = _mpv_menu_last->last;
	}

	_mpv_menu_last->last = i;

	_mpv_menu_last->items++;
}

/**
 * mpv_check_menu - Checks or unchecks a menu item
 * @funcname: function name which menu item should be (un)checked
 * @toggle: boolean value
 *
 * Checks or unchecks the menu item that executes @funcname.
 */
void mpv_check_menu(char *funcname, int toggle)
{
	struct mpv_menu_bar *m;
	struct mpv_menu_item *i;

	for (m = _mpv_menu; m != NULL; m = m->next)
	{
		for (i = m->first; i != NULL; i = i->next)
		{
			if (i->funcname != NULL &&
				strcmp(i->funcname, funcname) == 0)
			{
				i->toggle = toggle;
				return;
			}
		}
	}
}

static void _mpv_box(WINDOW *w, int tx, int ty)
{
	int x, y;

	wmove(w, 0, 0);
	waddch(w, MPBOX_TL_CORNER);
	for (x = 1; x < tx - 1; x++)
		waddch(w, MPBOX_HLINE);
	waddch(w, MPBOX_TR_CORNER);

	for (y = 1; y < ty - 1; y++)
	{
		_mpv_wattrset(w, _attrs[MP_COLOR_TEXT_FRAME1]);
		wmove(w, y, 0);
		waddch(w, MPBOX_VLINE);
		_mpv_wattrset(w, _attrs[MP_COLOR_TEXT_FRAME2]);
		wmove(w, y, tx - 1);
		waddch(w, MPBOX_VLINE);
	}

	waddch(w, MPBOX_BL_CORNER);
	for (x = 1; x < tx - 1; x++)
		waddch(w, MPBOX_HLINE);
	waddch(w, MPBOX_BR_CORNER);
}

/**
 * mpv_menu - Manages the menu.
 *
 * Manages the menu (drawing it, if applicable).
 */
int mpv_menu(void)
{
	int k = 0, ok;
	char tmp[1024];
	struct mpv_menu_bar *m;
	struct mpv_menu_item *i;
	struct mpv_menu_item *i2;
	WINDOW *w;
	int x, y, c, cc, sy;
	char *funcname = NULL;

	tmp[0] = '\0';
	for (m = _mpv_menu; m != NULL; m = m->next)
	{
		strcat(tmp, "   ");
		strcat(tmp, m->label);
	}

	_mpv_hint_text = L(MSG_CANCELHINT);
	_mpv_title_status(0, tmp, NULL);
	m = _mpv_menu;

	ok = 0;
	while (!ok)
	{
		i = m->first;

		w = newwin(m->items + 2, sizeof(i->label), 1, m->xpos);
		if (w == NULL)
			return (0);

		_mpv_wattrset(w, _attrs[MP_COLOR_TEXT_FRAME1]);
		_mpv_box(w, sizeof(i->label), m->items + 2);
		wrefresh(w);

		while (!ok)
		{
			/* draw items */
			for (sy = y = 1, i2 = m->first; i2 != NULL; y++, i2 = i2->next)
			{
				if (i2 == i)
				{
					c = _attrs[MP_COLOR_TEXT_MENU_SEL];
					sy = y;
				}
				else
					c = _attrs[MP_COLOR_TEXT_MENU_ELEM];

				/* if '-', then item is a separator */
				if (i2->label[0] == '-')
				{
					wmove(w, y, 1);
					_mpv_wattrset(w, _attrs[MP_COLOR_TEXT_FRAME1]);

					/* waddch(w,ACS_LTEE); */
					for (x = 0; x < sizeof(i2->label) - 2; x++)
						waddch(w, MPBOX_HLINE);
					/* waddch(w,ACS_RTEE); */
				}
				else
				{
					wmove(w, y, 1);
					_mpv_wattrset(w, c);

					for (x = 0; x < sizeof(i->label) && (cc = i2->label[x]); x++)
					{
						if (cc == '&')
						{
							/* _mpv_wattrset(w,_menu_act_attr); */
							cc = i2->label[++x];
						}
						else
							_mpv_wattrset(w, c);

						waddch(w, (unsigned char)cc);
					}

					/* draws toggle mark */
					if (i2->toggle)
						waddch(w, '*');
					else
						waddch(w, ' ');
				}
			}

			wmove(w, sy, 1);
			wrefresh(w);
			k = getch();

			/* possible keys */
			if (k == 0x1b || k == ctrl('A') || k == ctrl('N') || k == ctrl(' '))
			{
				k = 0;
				ok = 1;
			}
			else if (k == KEY_RIGHT)
			{
				if ((m = m->next) == NULL)
					m = _mpv_menu;
				break;
			}
			else if (k == KEY_LEFT)
			{
				if ((m = m->prev) == NULL)
					m = _mpv_menu_last;
				break;
			}
			else if (k == KEY_DOWN)
			{
				if ((i = i->next) == NULL)
					i = m->first;

				/* this assumes a separator can't be
				   the last item of a menu, so don't do it */
				if (i->label[0] == '-')
					i = i->next;
			}
			else if (k == KEY_UP)
			{
				if ((i = i->prev) == NULL)
					i = m->last;

				/* this assumes a separator can't be
				   the last item of a menu, so don't do it */
				if (i->label[0] == '-')
					i = i->prev;
			}
			else if (k == '\r' || k == KEY_ENTER)
			{
				funcname = i->funcname;
				ok = 1;
			}
			else
				ok = 1;
		}

		delwin(w);
		touchwin(stdscr);
		refresh();
	}

	_mpv_hint_text = NULL;

	if (funcname != NULL)
		mpi_process('\0', NULL, funcname);

	return (2);
}

/**
 * _mpv_prompt - Asks yes or no.
 * @prompt: the prompt
 * @prompt2: an optional second prompt.
 *
 * Asks yes or no.
 * Returns 1 if 'yes' choosen.
 */
static int _mpv_prompt(char *prompt, char *prompt2)
{
	int c;
	char *yes;
	char *no;

	move(LINES - 1, 0);
	_mpv_attrset(_attrs[MP_COLOR_TEXT_TITLE]);

	addstr(prompt);
	if (prompt2)
		addstr(prompt2);

	yes = L(MSG_YES);
	no = L(MSG_NO);

	for (;;)
	{
		c = getch();

		if (toupper(c) == *yes || toupper(c) == *no ||
			c == '\r' || c == KEY_ENTER)
		{
			mpv_status_line(NULL);
			break;
		}
	}

	if (toupper(c) == *no)
		return (0);

	return (1);
}

/**
 * mpv_confirm - Asks for confirmation.
 * @prompt: the question to ask
 *
 * Asks for confirmation.
 * Returns 1 if choosen 'yes'.
 */
int mpv_confirm(char *prompt)
{
	return (_mpv_prompt(prompt, L(MSG_YESNO)));
}

/**
 * mpv_alert - Alerts the user.
 * @msg: the alert message
 * @msg2: a possible second message.
 *
 * Alerts the user by showing the message and
 * asking for validation.
 */
void mpv_alert(char *msg, char *msg2)
{
	char tmp[4096];

	if (msg2 == NULL)
		strncpy(tmp, msg, sizeof(tmp));
	else
		sprintf(tmp, msg, msg2);

	_mpv_prompt(tmp, L(MSG_ENTER));
}

/**
 * mpv_list - Manages a selection list
 * @title: the title or caption of the list
 * @txt: the text containing the list to show
 *
 * Shows a unique element selection list to the user.
 * The list must be previously built into txt.
 * Returns the selected element (the number of the
 * line where the element is) or -1 on cancellation.
 */
int mpv_list(char *title, mp_txt *txt)
{
	int c, ok, ret;
	char *funcname;
	mp_txt *tmp;

	_mpv_hint_text = L(MSG_CANCELHINT);
	mpv_title(title);

	mp_move_bof(txt);

	if (txt->lasty == 0)
	{
		/* no lines or just one line: exit */
		_mpv_hint_text = NULL;
		return (0);
	}

	txt->type = MP_TYPE_LIST;
	txt->mod = 0;

	tmp = _mp_active;
	_mp_active = txt;

	mpi_draw_all(_mp_active);

	ret = -1;

	ok = 0;
	while (!ok)
	{
		c = getch();

		funcname = NULL;

		switch (c)
		{
		case KEY_LEFT:
		case KEY_UP:
			funcname = "move-up";
			break;
		case KEY_RIGHT:
		case KEY_DOWN:
			funcname = "move-down";
			break;
		case KEY_NPAGE:
			funcname = "move-page-down";
			break;
		case KEY_PPAGE:
			funcname = "move-page-up";
			break;
		case KEY_HOME:
			funcname = "move-bof";
			break;
		case KEY_END:
			funcname = "move-eof";
			break;

		case '\r':
		case KEY_ENTER:
			ret = _mp_active->y;
			/* falls thru */

		case KEY_F(4):
		case '\e':
			ok = 1;
			break;
		}

		mpi_process('\0', NULL, funcname);
	}

	_mpv_hint_text = NULL;

	_mp_active = tmp;

	return (ret);
}

/**
 * _mpv_open_file_list - Shows a list of files to open
 * @rcpt: buffer to receive the selected file
 *
 * Asks for a file scheme and shows a list of files
 * matching it.
 * Returns the pushed key to exit ('\r' or '\e').
 * If no or just one file is matched, '\r' is also returned.
 * The selected file will be written into the rcpt
 * buffer.
 * This is an ugly hack: it will be rewritten
 * sooner or later.
 */
static int _mpv_open_file_list(char *rcpt)
{
	mp_txt *txt;
	int c;
	int l;

	MP_SAVE_STATE();

	if ((txt = mpv_glob(rcpt)) == NULL)
		return ('\r');

	if ((l = mpv_list("file list", txt)) != -1)
	{
		mp_move_xy(txt, 0, l);
		mp_get_str(txt, rcpt, 1024, '\n');
		c = '\r';
	}
	else
		c = '\e';

	mp_delete_sys_txt(txt);
	return (c);
}

/**
 * mpv_readline - Ask for user input.
 * @type: the type of input asked (one of MPR_ constants)
 * @prompt: the prompt
 * @def: the default value
 *
 * Asks for user input. The expected data type is
 * described in type.
 * Returns a pointer to a static buffer with the
 * data entered by the user, or NULL if user cancelled.
 */
char *mpv_readline(int type, char *prompt, char *def)
{
	static char tmp[1024];
	int c, cursor;
	int n, history;

	move(LINES - 1, 0);
	_mpv_attrset(_attrs[MP_COLOR_TEXT_TITLE]);

	/* clean line */
	for (n = 0; n < COLS; n++)
		addch(' ');
	move(LINES - 1, 0);

	if (def == NULL)
		tmp[0] = '\0';
	else
		strncpy(tmp, def, sizeof(tmp));

	cursor = strlen(tmp);

	addstr(prompt);
	addstr(tmp);

	history = mpi_history_size(type) + 1;

	for (;;)
	{
		c = getch();

		if (c == '\r' || c == KEY_ENTER || c == '\e')
			break;

		if ((c == KEY_BACKSPACE || c == '\b' || c == 0x7f) && cursor > 0) /* JG Fixed filename save on mac */
		{
			addstr("\b \b");
			tmp[--cursor] = '\0';
			refresh();
		}
		else if (c == ctrl('U'))
		{
			while (cursor--)
			{
				addstr("\b \b");
			}
			tmp[cursor = 0] = '\0';
			refresh();
		}
		else if (c == KEY_UP)
		{
			if (history > 0)
				history--;

			mpi_history_get(type, history, tmp, sizeof(tmp));

			while (cursor--)
			{
				addstr("\b \b");
			}
			addstr(tmp);
			cursor = strlen(tmp);
			refresh();
		}
		else if (c == KEY_DOWN)
		{
			if (history < mpi_history_size(type))
				history++;

			mpi_history_get(type, history, tmp, sizeof(tmp));

			while (cursor--)
			{
				addstr("\b \b");
			}
			addstr(tmp);
			cursor = strlen(tmp);
			refresh();
		}
		else if (c == '\t')
		{
			tmp[cursor++] = '*';
			tmp[cursor] = '\0';
			c = '\r';
			break;
		}
		else if (c >= 32 && c <= 255 && cursor < 56)
		{
			addch(c);
			tmp[cursor++] = c;
			tmp[cursor] = '\0';
			refresh();
		}
	}

	if (c == '\e')
		return (NULL);

	if (type == MPR_OPEN)
	{
		if (tmp[0] == '\0')
			strncpy(tmp, "*", sizeof(tmp));
		else
		{
			struct stat s;

			if (!stat(tmp, &s))
			{
				/* add a globber if it's a directory */
				if (s.st_mode & S_IFDIR)
					strcat(tmp, "/*");
			}
		}

		if (strchr(tmp, '*') != NULL || strchr(tmp, '?') != NULL)
			c = _mpv_open_file_list(tmp);
	}

	mpv_status_line(NULL);

	if (c == '\e')
		return (NULL);

	/* store line in history */
	mpi_history_add(type, tmp);

	return (tmp);
}

/**
 * mpv_system_to_clipboard - Copies from the system clipboard
 *
 * Copies the clipboard's content from the underlying system
 * to Minimum Profit's internal one. A returning value of 0
 * means the actual pasting is already done (or done from a
 * system callback), so it won't be necessary to do it.
 */
int mpv_system_to_clipboard(void)
{
	return (1);
}

/**
 * mpv_clipboard_to_system - Copies to the system clipboard
 *
 * Copies the clipboard's content from Minimum Profit to
 * the underlying system's one.
 */
void mpv_clipboard_to_system(void)
{
}

/**
 * mpv_about - Shows the 'About Minimum Profit...' information.
 *
 * Shows a text or dialog box showing the information
 * about the program, version and such.
 */
void mpv_about(void)
{
	mp_txt *txt;

	txt = mp_create_txt(L(MSG_ABOUT));

	mp_put_str(txt, MP_LICENSE, 1);

	mp_move_bof(txt);
	txt->type = MP_TYPE_READ_ONLY;
	txt->mod = 0;
}

/**
 * mpv_notify - The notify function.
 * @str: the str
 *
 * The notify function for mp_set_notify().
 */
static void mpv_notify(char *str)
{
	mpv_status_line(str);
	mpv_refresh();
}

#define R3(a, b, c) (((a) * (b)) / (c))

/**
 * mpv_scrollbar - Draws / updates the scrollbar.
 * @pos: current position
 * @size: vertical size
 * @max: maximum value
 *
 * Draws / updates the scrollbar. pos is current position
 * (the line where the cursor is), size is the number of
 * lines of the document window and max is the maximum
 * value (the total number of lines in current text).
 */
void mpv_scrollbar(int pos, int size, int max)
{
	int n;
	int l, u, a;

	if (max < size)
	{
		l = 0;
		u = _mpv_y_size;
		a = 1;
	}
	else
	{
		l = R3(pos, size, max);
		u = R3(pos + size, size, max);
		a = 0;
	}

	for (n = 0; n < _mpv_y_size; n++)
	{
		move(START_LINE + n, _mpv_x_size);

		if (n >= l && n <= u && !a)
		{
			_mpv_attrset(_attrs[MP_COLOR_TEXT_SCR_THUMB]);
			addch(MPBOX_THUMB);
		}
		else
		{
			_mpv_attrset(_attrs[MP_COLOR_TEXT_SCROLLBAR]);
			addch(MPBOX_VLINE);
		}
	}
}

/**
 * mpv_filetabs - Draws the tab set containing the file names
 *
 * Draws the names of the opened files in the form of a tab set.
 */
void mpv_filetabs(void)
{
}

#ifdef IEXTEN
#define TTY_MODE ISIG | ICANON | ECHO | IEXTEN
#else /* not IEXTEN */
#define TTY_MODE ISIG | ICANON | ECHO
#endif /* not IEXTEN */

/**
 * _mpv_xonxoff - Sets/unsets terminal magic
 * @state: 1: set state, 0 reset state
 *
 * Sets/unsets Unix terminal parameters.
 */
static void _mpv_xonxoff(int state)
{
	static struct termios _ioval;
	static int orig_state = -1;

	/* Get current settings */
	tcgetattr(0, &_ioval);

	/* Store XON/XOFF state */
	if (orig_state == -1)
	{
		orig_state = (_ioval.c_iflag & (IXON | IXOFF));
	}

	if (state)
	{

		_ioval.c_lflag &= ~TTY_MODE;
		_ioval.c_iflag &= ~(IXON | IXOFF);

		tcsetattr(0, TCSANOW, &_ioval);
		return;
	}
	else
	{
		/* Paranoid? */
		if (orig_state == -1)
		{
			return;
		}

		_ioval.c_lflag |= TTY_MODE;
		_ioval.c_iflag |= orig_state;

		tcsetattr(0, TCSANOW, &_ioval);
		return;
	}
}

/**
 * mpv_create_colors - Creates the colors defined in the color database
 *
 * Creates the colors defined in the color database, probably
 * set from the configuration file.
 */
void mpv_create_colors(void)
{
	int n;
	int di, dp;

#ifndef NO_TRANSPARENCY
	use_default_colors();
#else
	mpi_transp_mode = 0;
#endif

	di = DEFAULT_INK_COLOR;
	dp = DEFAULT_PAPER_COLOR;

	if (mpi_transp_mode)
		di = dp = -1;
	else
	{
		/* if transparent mode is disabled and
		   normal ink or paper is -1,
		   set as default */
		if (mpc_color_desc[0].ink_text == -1)
			mpc_color_desc[0].ink_text = di;
		if (mpc_color_desc[0].paper_text == -1)
			mpc_color_desc[0].paper_text = dp;
	}

	for (n = 0; n < MP_COLOR_NUM; n++)
	{
		int i, p;

		i = mpc_color_desc[n].ink_text;
		p = mpc_color_desc[n].paper_text;

		if (n != 0 && (i == -1 || p == -1))
		{
			if (i == -1)
				i = mpc_color_desc[0].ink_text;
			if (p == -1)
				p = mpc_color_desc[0].paper_text;
		}

		if (mpi_monochrome)
			init_pair(n + 1, di, dp);
		else
			init_pair(n + 1, i, p);

		_attrs[n] = (COLOR_PAIR(n + 1) | (mpc_color_desc[n].reverse ? A_REVERSE : 0) | (mpc_color_desc[n].bright ? A_BOLD : 0) | (mpc_color_desc[n].underline ? A_UNDERLINE : 0));
	}
}

/**
 * mpv_startup - Starts up the system dependent driver.
 *
 * Starts up the system dependent driver.
 * Returns 1 if succesful.
 */
int mpv_startup(void)
{
	mp_log("Using curses/ncurses driver\n");

	_mpv_xonxoff(1);
	setlocale(LC_ALL, ""); /* JG UTF-8 support */
	initscr();
	start_color();

#ifdef NCURSES_MOUSE_VERSION
	if (mpi_mouse)
		mousemask(BUTTON1_PRESSED | BUTTON2_PRESSED |
					  BUTTON3_PRESSED | BUTTON4_PRESSED,
				  NULL);

	mp_log("Ncurses mouse version: %d\n", NCURSES_MOUSE_VERSION);
#endif

	keypad(stdscr, TRUE);
	nonl();
	cbreak();
	noecho();

	mpv_create_colors();

	_mpv_x_size = COLS - 1;
	_mpv_y_size = LINES - START_LINE - END_LINE;

	signal(SIGWINCH, _mpv_sigwinch);

	mp_set_notify(mpv_notify);
	mpv_status_line(NULL);

	fclose(stderr);

	/* ignore unwanted signals */
	sigemptyset(&Sigset);
#ifdef STRICT_SIGNALS
	sigaddset(&Sigset, SIGTERM);
	sigaddset(&Sigset, SIGHUP);
#endif
	sigaddset(&Sigset, SIGTSTP);
	sigaddset(&Sigset, SIGINT);
	sigaddset(&Sigset, SIGIOT);
	sigaddset(&Sigset, SIGQUIT);
	sigprocmask(SIG_BLOCK, &Sigset, 0);

	/* metainfo */

#ifdef NO_TRANSPARENCY
	mp_log("Transparency mode disabled\n");
#endif
#ifdef WITHOUT_GLOB
	mp_log("Globbing mode disabled\n");
#endif
#ifdef NCURSES_VERSION
	mp_log("Ncurses version: %s\n", NCURSES_VERSION);
#else
	mp_log("Using plain curses library\n");
#endif
#ifdef POOR_MAN_BOXES
	mp_log("Poor man boxes active by default\n");
#endif

	mp_log("Default colors - ink: %d paper: %d\n",
		   DEFAULT_INK_COLOR, DEFAULT_PAPER_COLOR);

	return (1);
}

/**
 * mpv_shutdown - Shuts down the system dependent driver.
 *
 * Shuts down the system dependent driver.
 */
void mpv_shutdown(void)
{
	sigprocmask(SIG_UNBLOCK, &Sigset, 0);

	endwin();

	_mpv_xonxoff(0);
}

/**
 * _mpv_usage - Prints the usage help.
 *
 * Prints the usage help.
 */
static void _mpv_usage(void)
{
	printf("Minimum Profit " VERSION " - Programmer Text Editor\n");
	printf("Copyright (C) 1991-2003 Angel Ortega <angel@triptico.com>\n");
	printf("%s\n", __DATE__ " " __TIME__);
	printf("This software is covered by the GPL license. NO WARRANTY.\n\n");

	printf("%s\n", L(MSG_USAGE_TEXT));

	printf("--lang: %s\n", mpl_enumerate_langs());
	printf("--mode: %s\n", mps_enumerate_modes());
	printf("\nConfig file: %s\n", _mpc_config_file);
}

/**
 * mpv_set_variable - Sets a driver-dependent variable
 * @var: the name of the variable to be set
 * @value: the new value
 *
 * Sets a variable from the configuration file that is
 * driver-specific.
 */
void mpv_set_variable(char *var, char *value)
{
	if (strcmp(var, "mouse") == 0)
		mpi_mouse = atoi(value);
	else if (strcmp(var, "hardware_cursor") == 0)
		_mpi_reposition_cursor = atoi(value);
	else if (strcmp(var, "monochrome") == 0)
		mpi_monochrome = atoi(value);
	else if (strcmp(var, "transparent") == 0)
		mpi_transp_mode = atoi(value);
	else if (strcmp(var, "poor_man_boxes") == 0)
		_mpv_poor_man_boxes = atoi(value);
}

/**
 * mpv_args_1 - Driver command line argument processing, 1st pass
 * @n: pointer to integer containing argument being processed
 * @argc: argument count
 * @argv: the arguments
 *
 * First pass to argument processing, before anything is
 * initialised. Called from within mpi_args_1().
 */
int mpv_args_1(int *n, int argc, char *argv[])
{
	return (0);
}

/**
 * mpv_args_2 - Driver command line argument processing, 2nd pass
 * @n: pointer to integer containing argument being processed
 * @argc: argument count
 * @argv: the arguments
 *
 * Second pass to argument processing, after everything is
 * initialised. Called from within mpi_args_2().
 */
int mpv_args_2(int *n, int argc, char *argv[])
{
	return (0);
}

int main(int argc, char *argv[])
{
	int r;

	mp_startup();

	_mpc_text_interface = 1;
	mpc_startup();

	r = mpi_args_1(argc, argv);

	if (r == -1)
	{
		_mpv_usage();
		exit(0);
	}
	if (r == -2)
	{
		printf(VERSION "\n");
		exit(0);
	}

	mps_startup();
	mpi_startup();
	mpv_startup();

	r = mpi_args_2(argc, argv);

	if (r == -1)
	{
		mpv_shutdown();
		printf("%s\n", L(MSG_BADMODE));
		printf("%s\n", mps_enumerate_modes());
		exit(1);
	}

	/* force the reading of the tags file, if one exists */
	mpi_seek_tag(NULL, 0);

	/* create empty text if no file is open */
	if (_mp_active == NULL)
	{
		mp_create_txt(L(MSG_UNNAMED));
		mps_auto_synhi(_mp_active);
	}

	mpv_title(NULL);
	mpi_draw_all(_mp_active);

	/* bark if bad tags */
	if (r == -2)
	{
		mpv_alert(L(MSG_TAGNOTFOUND), "");
		mpi_draw_all(_mp_active);
	}

	/* main loop */

	while (!_mpi_exit_requested)
		_mpv_main_loop();

	/* close everything */
	mpi_shutdown();

	mpv_title(NULL);
	mpv_refresh();

	mpv_shutdown();
	mp_shutdown();

	return (0);
}
