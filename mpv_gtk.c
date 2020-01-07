/*

    mpv_gtk.c

    GTK (X Window) Interface (Linux/Unix)

    mp - Programmer Text Editor

    Copyright (C) 1991/2004 Angel Ortega <angel@triptico.com>

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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/stat.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include "mp_core.h"
#include "mp_video.h"
#include "mp_func.h"
#include "mp_iface.h"
#include "mp_lang.h"
#include "mp_synhi.h"
#include "mp_conf.h"

/*******************
	Data
********************/

GtkWidget * window=NULL;
GtkWidget * menu=NULL;
GtkWidget * menu_bar=NULL;
GtkWidget * file_tabs=NULL;
GtkWidget * entry=NULL;
GtkWidget * list=NULL;
GtkWidget * area=NULL;
GtkWidget * scrollbar=NULL;
GtkWidget * status=NULL;
GdkGC * gc=NULL;

#if GTK_MAJOR_VERSION < 2
GdkIC * ic=NULL;
#else
GtkIMContext * im=NULL;
#endif

GdkPixmap * pixmap=NULL;

GdkFont * _font_normal=NULL;
GdkFont * _font_italic=NULL;

/* readline buffer */
static char _mpv_readline_buf[1024];

/* confirmation value for gtk dialogs */
static int _mpv_confirm_value=-1;

/* selected element from the list */
static int _mpv_selected_item=0;

/* font information */
char _mpv_font_face[80]="lucidatypewriter";
int _mpv_font_size=14;
int _mpv_font_width=0;
int _mpv_font_height=0;
char _mpv_font_weight[80]="medium";

/* frame buffer with chars in document window */
static int * _mpv_fb=NULL;
static int * _mpv_fbo=NULL;
static int _mpv_fb_size=0;

/* cursor position */
static int _mpv_x_cursor;
static int _mpv_y_cursor;

/* temporal buffer */
static char _mpv_buffer[1024];

/* flag to use of italic fonts */
int _use_italics=0;

/* set if selection is ours */
int _mpv_selection=0;

/* colors */

static GdkColor _inks[MP_COLOR_PRIVATE];
static GdkColor _papers[MP_COLOR_PRIVATE];

#include "mp.xpm"

static int _use_double_buffer=0;

#ifndef CURSES_MP
#define CURSES_MP "mp"
#endif

/* window geometry */

int _mpv_gtk_xpos=-1;
int _mpv_gtk_ypos=-1;
int _mpv_gtk_width=640;
int _mpv_gtk_height=400;

/* the X11 font in use */
char _x11_font_spec[1024];

/* input context */
static char _mpv_im_char='\0';


/*******************
	Code
*******************/

#include "mpv_unix_common.c"

#if GTK_MAJOR_VERSION >= 2

char * _(const char * str)
{
	static char tmp[1024];
	char * ptr;

	if((ptr=g_locale_to_utf8(str, -1, NULL, NULL, NULL)) != NULL)
	{
		strncpy(tmp, ptr, sizeof(tmp));
		free(ptr);
	}
	else
		tmp[0]='\0';

	return(tmp);
}


char * __(const char * str)
{
	static char tmp[1024];
	char * ptr;

	if((ptr=g_locale_from_utf8(str, -1, NULL, NULL, NULL)) != NULL)
	{
		strncpy(tmp, ptr, sizeof(tmp));
		free(ptr);
	}
	else
		tmp[0]='\0';

	return(tmp);
}

#else

#define _(x) (x)
#define __(x) (x)

#endif


/**
 * mpv_goto - Positions the cursor to start drawing.
 * @x: the x position
 * @y: the y position
 *
 * Positions the cursor to start drawing the document window.
 */
void mpv_goto(int x, int y)
{
	/* just store the coords */
	_mpv_x_cursor=x;
	_mpv_y_cursor=y;
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
	if(_mpv_y_cursor >= _mpv_y_size ||
	   _mpv_x_cursor >= _mpv_x_size)
		return;

	/* fill the frame buffer */
	_mpv_fb[(_mpv_y_cursor * _mpv_x_size) + _mpv_x_cursor]=
		(color << 8)|((unsigned char)c);

	_mpv_x_cursor++;
}


/**
 * mpv_str - Draws a string with color in the document window.
 * @str: the string
 * @color: the color (one of MP_COLOR_ constants)
 *
 * Draws a string, calling mpv_char() for each of their chars.
 */
void mpv_str(char * str, int color)
{
	int * fb;

	fb=&_mpv_fb[(_mpv_y_cursor * _mpv_x_size) + _mpv_x_cursor];

	while(*str)
	{
		*fb=(color << 8)|(*str);

		fb++; _mpv_x_cursor++; str++;
	}
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
	/* dummy! */
}


/**
 * mpv_refresh - Refresh the screen.
 *
 * Orders the underlying system to redraw the screen.
 */
void mpv_refresh(void)
{
	GdkRectangle r;

	r.x=0;
	r.y=0;

#if GTK_MAJOR_VERSION < 2
	r.width=0;
	r.height=0;
#else
	r.width=area->allocation.width;
	r.height=area->allocation.height;
#endif

	gtk_widget_draw(area, &r);
}


/**
 * mpv_title - Sets the string to be drawn as title
 * @str: the string
 *
 * Sets the string to be drawn as title of the document window.
 */
void mpv_title(char * str)
{
	char tmp[2048];

	strncpy(tmp,"mp " VERSION, sizeof(tmp));

	if(str==NULL && _mp_active != NULL)
		str=_mp_active->name;

	if(str!=NULL && *str!='\0')
	{
		strcat(tmp," - ");
		strcat(tmp,str);
	}

	gtk_window_set_title(GTK_WINDOW(window), tmp);
}


/**
 * mpv_status_line - Sets the string to be drawn as status line
 * @str: the string
 *
 * Sets the string to be drawn as the status line.
 */
void mpv_status_line(char * str)
{
	gtk_label_set_text(GTK_LABEL(status), _(str));
}


static char * _mpv_filter_menu_label(char * label)
{
	static char tmp[1024];
	int n;

	for(n=0;*label && *label!='\t';label++)
	{
		if(*label!='&')
			tmp[n++]=*label;
	}
	tmp[n]='\0';

	return(_(tmp));
}


/**
 * mpv_add_menu - Creates a new menu bar.
 * @label: the label
 *
 * Creates a new menu bar.
 */
void mpv_add_menu(char * label)
{
	GtkWidget * menu_item;

	label=L(label);

	if(menu_bar==NULL)
		menu_bar=gtk_menu_bar_new();

	menu=gtk_menu_new();

	label=_mpv_filter_menu_label(label);
	menu_item=gtk_menu_item_new_with_label(label);

	gtk_widget_show(menu_item);

	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item), menu);

	gtk_menu_bar_append(GTK_MENU_BAR(menu_bar), menu_item);
}


static void _mpv_menu_item_callback(char * funcname)
/* menu click callback */
{
	mpi_process('\0', NULL, funcname);

	if(_mpi_exit_requested)
		gtk_main_quit();
}


/**
 * mpv_add_menu_item - Adds a menu item to current menu bar.
 * @funcname: the name of the function to invoke
 *
 * Adds a menu item to the current menu bar.
 */
void mpv_add_menu_item(char * funcname)
{
	GtkWidget * menu_item;
	char tmp[1024];
	char * keyname;
	char * label;
	char * signal="activate";
	int * i;

	label=_mpv_filter_menu_label(L(funcname));

	if((keyname=mpf_get_keyname_by_funcname(funcname))!=NULL)
	{
		keyname=L(keyname);
		snprintf(tmp,sizeof(tmp),"%s - [%s]",label,keyname);
		label=tmp;
	}

	if(*label=='-')
		menu_item=gtk_menu_item_new();
	else
	{
		i=mpf_toggle_function_value(funcname);

		if(i!=NULL)
		{
			menu_item=gtk_check_menu_item_new_with_label(label);
			signal="toggled";

			gtk_check_menu_item_set_active(
				GTK_CHECK_MENU_ITEM(menu_item), *i);
		}
		else
			menu_item=gtk_menu_item_new_with_label(label);
	}

	gtk_menu_append(GTK_MENU(menu), menu_item);

	gtk_signal_connect_object(GTK_OBJECT(menu_item), signal,
		GTK_SIGNAL_FUNC(_mpv_menu_item_callback),
		(gpointer) funcname);

	gtk_widget_show(menu_item);
}


/**
 * mpv_check_menu - Checks or unchecks a menu item
 * @funcname: function name which menu item should be (un)checked
 * @toggle: boolean value
 *
 * Checks or unchecks the menu item that executes @funcname.
 */
void mpv_check_menu(char * funcname, int toggle)
{
	/* dummy! */
}


/**
 * mpv_menu - Manages the menu.
 *
 * Manages the menu (drawing it, if applicable).
 * Returns the key associated to the activated menu item,
 * or 0 if none was. 
 */
int mpv_menu(void)
{
	/* dummy! */
	return(0);
}


static void _gtk_really_modal(void)
{
	_mpv_confirm_value=-1;

	while(_mpv_confirm_value==-1)
		gtk_main_iteration();
}


static void _mpv_confirm_yes_callback(GtkWidget * widget, gpointer data)
{
	_mpv_confirm_value=1;
	gtk_widget_destroy(GTK_WIDGET(widget));
}


static void _mpv_confirm_no_callback(GtkWidget * widget, gpointer data)
{
	_mpv_confirm_value=0;
	gtk_widget_destroy(GTK_WIDGET(widget));
}


static void _mpv_confirm_yes_file_callback(GtkWidget * widget, gpointer data)
{
	const char * ptr;

	ptr=gtk_file_selection_get_filename(GTK_FILE_SELECTION(widget));
	strncpy(_mpv_readline_buf,__(ptr),sizeof(_mpv_readline_buf));

	_mpv_confirm_value=1;
	gtk_widget_destroy(GTK_WIDGET(widget));
}


static void _mpv_confirm_yes_entry_callback(GtkWidget * widget, gpointer data)
{
	char * ptr;

	ptr=gtk_editable_get_chars(GTK_EDITABLE(entry),0,-1);
	strncpy(_mpv_readline_buf,__(ptr),sizeof(_mpv_readline_buf));

	g_free(ptr);

	entry=NULL;

	_mpv_confirm_value=1;
	gtk_widget_destroy(GTK_WIDGET(widget));
}


static int _mpv_confirm_key_callback(GtkWidget * widget, GdkEventKey * event)
{
	if(event->string[0] == '\r')
	{
		_mpv_confirm_yes_callback(widget, NULL);
		return(1);
	}
	else
	if(event->string[0] == '\e')
	{
		_mpv_confirm_no_callback(widget, NULL);
		return(1);
	}

	return(0);
}


static gint _mpv_confirm_key_yes_no_callback(GtkWidget * widget, GdkEventKey * event)
{
	char * yes;
	char * no;

	yes=L(MSG_YES);
	no=L(MSG_NO);

	if(tolower(event->string[0]) == tolower(*yes))
	{
		_mpv_confirm_yes_callback(widget, NULL);
		return(1);
	}
	else
	if(tolower(event->string[0]) == tolower(*no))
	{
		_mpv_confirm_no_callback(widget, NULL);
		return(1);
	}

	return(0);
}


static gint _mpv_confirm_key_entry_callback(GtkWidget * widget, GdkEventKey * event)
{
	if(event->string[0] == '\r')
	{
		_mpv_confirm_yes_entry_callback(widget, NULL);
		return(1);
	}
	else
	if(event->string[0] == '\e')
	{
		_mpv_confirm_no_callback(widget, NULL);
		return(1);
	}

	return(0);
}


static gint _mpv_confirm_key_file_callback(GtkWidget * widget, GdkEventKey * event)
{
	if(event->string[0] == '\e')
	{
		_mpv_confirm_no_callback(widget, NULL);
		return(1);
	}

	return(0);
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
	if(_mpv_selection) return(1);

	gtk_selection_convert(area, GDK_SELECTION_PRIMARY,
		gdk_atom_intern("STRING", FALSE), GDK_CURRENT_TIME);

	return(0);
}


/**
 * mpv_clipboard_to_system - Copies to the system clipboard
 *
 * Copies the clipboard's content from Minimum Profit to
 * the underlying system's one.
 */
void mpv_clipboard_to_system(void)
{
	_mpv_selection=gtk_selection_owner_set(area,
		GDK_SELECTION_PRIMARY, GDK_CURRENT_TIME);
}


/**
 * mpv_alert - Alerts the user.
 * @msg: the alert message
 * @msg2: a possible second message.
 *
 * Alerts the user by showing the message and
 * asking for validation.
 */
void mpv_alert(char * msg, char * msg2)
{
	char tmp[8192];
	GtkWidget * dlg;
	GtkWidget * label;
	GtkWidget * button;

	if(msg2==NULL)
		strncpy(tmp,msg,sizeof(tmp));
	else
		sprintf(tmp,msg,msg2);

	dlg=gtk_dialog_new();
	gtk_window_set_title(GTK_WINDOW(dlg),"mp " VERSION);
	gtk_container_border_width(GTK_CONTAINER(GTK_DIALOG(dlg)->vbox),5);

	label=gtk_label_new(_(tmp));
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dlg)->vbox),label,TRUE,TRUE,0);
	gtk_widget_show(label);

	button=gtk_button_new_with_label("OK");
	gtk_signal_connect_object(GTK_OBJECT(button),"clicked",
		GTK_SIGNAL_FUNC(_mpv_confirm_yes_callback),GTK_OBJECT(dlg));
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dlg)->action_area),button,TRUE,TRUE,0);
	gtk_widget_show(button);

	gtk_signal_connect(GTK_OBJECT(dlg),"key_press_event",
		(GtkSignalFunc) _mpv_confirm_key_callback, NULL);

	gtk_window_set_position(GTK_WINDOW(dlg), GTK_WIN_POS_CENTER);

	gtk_widget_show(dlg);
	gtk_grab_add(dlg);
	_gtk_really_modal();
}


/**
 * mpv_confirm - Asks for confirmation.
 * @prompt: the question to ask
 *
 * Asks for confirmation.
 * Returns 1 if choosen 'yes'.
 */
int mpv_confirm(char * prompt)
{
	GtkWidget * dlg;
	GtkWidget * label;
	GtkWidget * ybutton;
	GtkWidget * nbutton;

	dlg=gtk_dialog_new();
	gtk_window_set_title(GTK_WINDOW(dlg),"mp " VERSION);
	gtk_container_border_width(GTK_CONTAINER(GTK_DIALOG(dlg)->vbox),5);

	label=gtk_label_new(_(prompt));
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dlg)->vbox),label,TRUE,TRUE,0);
	gtk_widget_show(label);

	ybutton=gtk_button_new_with_label(_(L(MSG_YES_STRING)));
	gtk_signal_connect_object(GTK_OBJECT(ybutton),"clicked",
		GTK_SIGNAL_FUNC(_mpv_confirm_yes_callback),GTK_OBJECT(dlg));
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dlg)->action_area),ybutton,TRUE,TRUE,0);
	gtk_widget_show(ybutton);

	nbutton=gtk_button_new_with_label(_(L(MSG_NO_STRING)));
	gtk_signal_connect_object(GTK_OBJECT(nbutton),"clicked",
		GTK_SIGNAL_FUNC(_mpv_confirm_no_callback),GTK_OBJECT(dlg));
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dlg)->action_area),nbutton,TRUE,TRUE,0);
	gtk_widget_show(nbutton);

	gtk_signal_connect(GTK_OBJECT(dlg),"key_press_event",
		(GtkSignalFunc) _mpv_confirm_key_yes_no_callback, NULL);

	gtk_window_set_position(GTK_WINDOW(dlg), GTK_WIN_POS_CENTER);

	gtk_widget_show(dlg);
	gtk_grab_add(dlg);
	_gtk_really_modal();

	return(_mpv_confirm_value);
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
char * mpv_readline(int type, char * prompt, char * def)
{
	GtkWidget * dlg;
	GtkWidget * label;
	GtkWidget * ybutton;
	GtkWidget * nbutton;
	GtkWidget * combo;
	GList * combo_items=NULL;
	char * hist[10];
	int n;

	if(type==MPR_OPEN || type==MPR_SAVE)
	{
		dlg=gtk_file_selection_new(prompt);

		if(def != NULL)
			gtk_file_selection_set_filename(GTK_FILE_SELECTION(dlg),def);

		gtk_signal_connect_object(
			GTK_OBJECT(GTK_FILE_SELECTION(dlg)->ok_button),"clicked",
			GTK_SIGNAL_FUNC(_mpv_confirm_yes_file_callback),GTK_OBJECT(dlg));
		gtk_signal_connect_object(
			GTK_OBJECT(GTK_FILE_SELECTION(dlg)->cancel_button),"clicked",
			GTK_SIGNAL_FUNC(_mpv_confirm_no_callback),GTK_OBJECT(dlg));
		gtk_signal_connect(GTK_OBJECT(dlg),"key_press_event",
			(GtkSignalFunc) _mpv_confirm_key_file_callback, NULL);

		gtk_window_set_position(GTK_WINDOW(dlg), GTK_WIN_POS_CENTER);

		gtk_widget_show(dlg);
		gtk_grab_add(dlg);
		_gtk_really_modal();

		if(_mpv_confirm_value)
		{
			struct stat s;

			/* simulate abort if user types ENTER
			   without writing nothing
			   or opens a directory */
			if(stat(_mpv_readline_buf,&s)!=-1 &&
			   s.st_mode & S_IFDIR)
				return(NULL);

			_mpv_strip_cwd(_mpv_readline_buf,
				sizeof(_mpv_readline_buf));

			return(_mpv_readline_buf);
		}
	}
	else
	{
		dlg=gtk_dialog_new();
		gtk_window_set_title(GTK_WINDOW(dlg),"mp " VERSION);
		gtk_container_border_width(GTK_CONTAINER(GTK_DIALOG(dlg)->vbox),5);

		label=gtk_label_new(_(prompt));
		gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dlg)->vbox),label,TRUE,TRUE,0);
		gtk_widget_show(label);

		combo=gtk_combo_new();
		entry=GTK_COMBO(combo)->entry;
		gtk_widget_set_usize(combo, 300, -1);
		gtk_combo_set_use_arrows_always(GTK_COMBO(combo), TRUE);
		gtk_combo_set_case_sensitive(GTK_COMBO(combo), TRUE);

		/* store the history into combo_items */
		for(n=0;n < 10;n++)
		{
			char tmp[512];

			if(! mpi_history_get(type,
				mpi_history_size(type) - n,
				tmp, sizeof(tmp)))
				break;

			if(tmp[0]=='\0')
				hist[n]=NULL;
			else
			{
				hist[n]=(char *)malloc(sizeof(tmp)+1);
				strcpy(hist[n],_(tmp));
			}

			combo_items=g_list_append(combo_items, hist[n]);
		}

		for(;n < 10;n++) hist[n]=NULL;

		if(def != NULL && *def != '\0')
		{
			gtk_entry_set_text(GTK_ENTRY(entry),def);
			combo_items=g_list_prepend(combo_items, def);
		}

		gtk_combo_set_popdown_strings(GTK_COMBO(combo), combo_items);
		g_list_free(combo_items);

		gtk_editable_select_region(GTK_EDITABLE(entry),0,1000);

		gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dlg)->vbox),combo,TRUE,TRUE,0);
		gtk_widget_show(combo);

		ybutton=gtk_button_new_with_label(_(L(MSG_OK_STRING)));
		gtk_signal_connect_object(GTK_OBJECT(ybutton),"clicked",
			GTK_SIGNAL_FUNC(_mpv_confirm_yes_entry_callback), GTK_OBJECT(dlg));
		gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dlg)->action_area),
			ybutton,TRUE,TRUE,0);
		gtk_widget_show(ybutton);

		nbutton=gtk_button_new_with_label(_(L(MSG_CANCEL_STRING)));
		gtk_signal_connect_object(GTK_OBJECT(nbutton),"clicked",
			GTK_SIGNAL_FUNC(_mpv_confirm_no_callback),GTK_OBJECT(dlg));
		gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dlg)->action_area),
			nbutton,TRUE,TRUE,0);
		gtk_widget_show(nbutton);

		gtk_signal_connect(GTK_OBJECT(dlg),"key_press_event",
			(GtkSignalFunc) _mpv_confirm_key_entry_callback, NULL);

		gtk_window_set_position(GTK_WINDOW(dlg), GTK_WIN_POS_CENTER);

		gtk_widget_show(dlg);
		gtk_grab_add(dlg);
		gtk_widget_grab_focus(entry);

		_gtk_really_modal();

		for(n=0;n < 10;n++)
		{
			if(hist[n]!=NULL)
				free(hist[n]);
		}

		if(_mpv_confirm_value)
		{
			mpi_history_add(type, _mpv_readline_buf);
			return(_mpv_readline_buf);
		}
	}

	return(NULL);
}


static void _mpv_select_row_callback(GtkCList * list, gint row,
	gint column, GdkEventButton * event, gpointer data)
{
	_mpv_selected_item=row;
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
int mpv_list(char * title, mp_txt * txt)
{
	GtkWidget * dlg;
	GtkWidget * label;
	GtkWidget * ybutton;
	GtkWidget * nbutton;
	GtkWidget * scrolled;
	char line[1024];
	int n;

	mp_move_bof(txt);

	if(txt->lasty==0)
	{
		/* no lines or just one line: exit */
		return(0);
	}

	dlg=gtk_dialog_new();
	gtk_window_set_title(GTK_WINDOW(dlg),"mp " VERSION);
	gtk_container_border_width(GTK_CONTAINER(GTK_DIALOG(dlg)->vbox),5);

	label=gtk_label_new(_(title));
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dlg)->vbox),label,TRUE,TRUE,0);
	gtk_widget_show(label);

	scrolled=gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_set_usize(scrolled, 450, 250);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),
		GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dlg)->vbox),scrolled,TRUE,TRUE,0);
	gtk_widget_show(scrolled);

	list=gtk_clist_new(1);
	gtk_clist_set_selection_mode(GTK_CLIST(list), GTK_SELECTION_BROWSE);
	gtk_clist_set_column_title(GTK_CLIST(list),0,"");
	gtk_clist_column_titles_hide(GTK_CLIST(list));
	gtk_container_add(GTK_CONTAINER(scrolled),list);

	gtk_widget_show(list);

	/* traverse the list, adding the elements */

	n=0;
	_mpv_selected_item=0;

	while(mp_peek_char(txt)!='\0')
	{
		char * args[1];

		mp_get_str(txt,line,sizeof(line),'\n');

		args[0]=_(line);
		gtk_clist_append(GTK_CLIST(list), args);
		n++;
	}

	gtk_signal_connect(GTK_OBJECT(list), "select-row",
		GTK_SIGNAL_FUNC(_mpv_select_row_callback), NULL);

	ybutton=gtk_button_new_with_label(_(L(MSG_OK_STRING)));
	gtk_signal_connect_object(GTK_OBJECT(ybutton),"clicked",
		GTK_SIGNAL_FUNC(_mpv_confirm_yes_callback), GTK_OBJECT(dlg));
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dlg)->action_area),
		ybutton,TRUE,TRUE,0);
	gtk_widget_show(ybutton);

	nbutton=gtk_button_new_with_label(_(L(MSG_CANCEL_STRING)));
	gtk_signal_connect_object(GTK_OBJECT(nbutton),"clicked",
		GTK_SIGNAL_FUNC(_mpv_confirm_no_callback),GTK_OBJECT(dlg));
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dlg)->action_area),
		nbutton,TRUE,TRUE,0);
	gtk_widget_show(nbutton);

	gtk_signal_connect(GTK_OBJECT(dlg),"key_press_event",
		(GtkSignalFunc) _mpv_confirm_key_callback, NULL);

/*	gtk_signal_connect_object(GTK_OBJECT(dlg),"key_press_event",
		GTK_SIGNAL_FUNC(_mpv_confirm_key_callback),GTK_OBJECT(dlg));
*/
	gtk_window_set_position(GTK_WINDOW(dlg), GTK_WIN_POS_CENTER);

	gtk_widget_show(dlg);
	gtk_grab_add(dlg);
	gtk_widget_grab_focus(list);
	_gtk_really_modal();

	if(_mpv_confirm_value)
		return(_mpv_selected_item);

	return(-1);
}


static GdkFont * _mpv_get_font(char * fontface, char * slant, int size)
{
	if(fontface==NULL)
		strncpy(_x11_font_spec, "fixed", sizeof(_x11_font_spec));
	else
		snprintf(_x11_font_spec,sizeof(_x11_font_spec),
		"-*-%s-%s-%s-*-*-%d-*-*-*-*-*-iso8859-*",
		fontface, _mpv_font_weight, slant, size);

	return(gdk_font_load(_x11_font_spec));
}


static void _mpv_calc_font_width_and_height(void)
{
	if(_font_normal == NULL) return;

	_mpv_font_width=gdk_char_width(_font_normal,'m');
	_mpv_font_height=gdk_string_height(_font_normal,"lg,_|'");
	_mpv_font_height += (_mpv_font_height / 3);

	_mpv_x_size=(area->allocation.width / _mpv_font_width)+1;
	_mpv_y_size=(area->allocation.height / _mpv_font_height)+1;

	/* rebuild framebuffer */
	if(_mpv_fb != NULL) { free(_mpv_fb); free(_mpv_fbo); }

	_mpv_fb_size=_mpv_x_size * _mpv_y_size * sizeof(int);
	_mpv_fb=(int *) malloc(_mpv_fb_size);
	_mpv_fbo=(int *) malloc(_mpv_fb_size);
	memset(_mpv_fb, '\0', _mpv_fb_size);
	memset(_mpv_fbo, '\0', _mpv_fb_size);

	/* (re)build pixmap */
	if(pixmap != NULL) gdk_pixmap_unref(pixmap);

	pixmap=gdk_pixmap_new(area->window,
		area->allocation.width, _mpv_font_height,-1);
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
	static int _min_size=6;
	static int _max_size=128;
	GdkFont * fn=NULL;
	GdkFont * fi=NULL;
	int s=0;

	if(_use_italics)
	{
		for(s=_mpv_font_size+inc;s >= _min_size && s <= _max_size;)
		{
			fn=_mpv_get_font(_mpv_font_face, "r", s);

			if(fn==NULL) continue;

			fi=_mpv_get_font(_mpv_font_face, "i", s);

			if(fi==NULL)
			{
				gdk_font_unref(fn);
				fn=NULL;
			}
			else
				break;

			s+=inc;
		}
	}

	if(fi==NULL)
	{
		_use_italics=0;

		/* no fonts? may be the selected one cannot be
		   set to italic; retry */
		for(s=_mpv_font_size+inc;fn==NULL && s >= _min_size && s <= _max_size;)
		{
			if((fn=_mpv_get_font(_mpv_font_face, "r", s))!=NULL)
				break;

			s+=inc;
		}
	}

	/* if the font could not be set, try a default
	   fallback font ('fixed') */
	if(fn==NULL)
		fn=_mpv_get_font(NULL, "r", s);

	/* if the font could not be set and there is no
	   previous one, we have a serious problem */
	if(fn==NULL)
	{
		if(_font_normal==NULL)
		{
			printf("Error: font '%s' could not be set\n", _mpv_font_face);
			exit(0);
		}
		else
		{
			if(inc==-1)
				_min_size=_mpv_font_size;
			else
				_max_size=_mpv_font_size;
		}
	}
	else
	{
		_mpv_font_size=s;
		if(_font_normal!=NULL) gdk_font_unref(_font_normal);
		if(_font_italic!=NULL) gdk_font_unref(_font_italic);

		_font_normal=fn;
		_font_italic=fi;

		_mpv_calc_font_width_and_height();
		mp_log("Using X11 font '%s'\n", _x11_font_spec);
	}

	return(0);
}


static void _mpv_value_changed_callback(GtkAdjustment * adj, gpointer * data)
{
	if((int)adj->value != _mp_active->y + 1)
	{
		mp_move_xy(_mp_active,0,(int)adj->value - 1);
		_mp_active->vy=_mp_active->y;

		mpi_draw_all(_mp_active);
	}
}


/**
 * mpv_scrollbar - Draws / updates the scrollbar.
 * @pos: current position
 * @size: vertical size
 * @max: maximum value
 *
 * Draws / updates the scrollbar. @pos is current position
 * (the line where the cursor is), @size is the number of
 * lines of the document window and @max is the maximum
 * value (the total number of lines in current text).
 */
void mpv_scrollbar(int pos, int size, int max)
{
	GtkAdjustment * adjustment;

	adjustment=gtk_range_get_adjustment(GTK_RANGE(scrollbar));

	if(adjustment->upper==(gfloat) max &&
	   adjustment->page_size==(gfloat)size &&
	   adjustment->value==(gfloat)pos)
		return;

	gtk_signal_disconnect_by_func(GTK_OBJECT(adjustment),
		(GtkSignalFunc) _mpv_value_changed_callback, NULL);

	adjustment->upper=(gfloat)max - 1;
	adjustment->page_size=(gfloat)size;
	adjustment->value=(gfloat)pos - 1;

	gtk_range_set_adjustment(GTK_RANGE(scrollbar),adjustment);

	gtk_adjustment_changed(adjustment);
	gtk_adjustment_value_changed(adjustment);

	gtk_signal_connect(
		GTK_OBJECT(gtk_range_get_adjustment(GTK_RANGE(scrollbar))),
		"value_changed",
		(GtkSignalFunc) _mpv_value_changed_callback, NULL);
}


/**
 * mpv_about - Shows the 'About Minimum Profit...' information.
 *
 * Shows a text or dialog box showing the information
 * about the program, version and such.
 */
void mpv_about(void)
{
	GtkWidget * dlg;
	GtkWidget * icon;
	GtkWidget * label;
	GtkWidget * button;
	GdkPixmap * pixmap;
	GdkPixmap * mask;

	dlg=gtk_dialog_new();
	gtk_window_set_title(GTK_WINDOW(dlg),"mp " VERSION);
	gtk_container_border_width(GTK_CONTAINER(GTK_DIALOG(dlg)->vbox),5);

	pixmap=gdk_pixmap_create_from_xpm_d(window->window,
		&mask, NULL, mp_xpm);
	icon=gtk_pixmap_new(pixmap, mask);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dlg)->vbox),icon,TRUE,TRUE,0);
	gtk_widget_show(icon);

	label=gtk_label_new(_(MP_LICENSE));
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dlg)->vbox),label,TRUE,TRUE,0);
	gtk_widget_show(label);

	button=gtk_button_new_with_label("OK");
	gtk_signal_connect_object(GTK_OBJECT(button),"clicked",
		GTK_SIGNAL_FUNC(_mpv_confirm_yes_callback),GTK_OBJECT(dlg));
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dlg)->action_area),button,TRUE,TRUE,0);
	gtk_widget_show(button);

	gtk_signal_connect(GTK_OBJECT(dlg),"key_press_event",
		(GtkSignalFunc) _mpv_confirm_key_callback, NULL);

	gtk_window_set_position(GTK_WINDOW(dlg), GTK_WIN_POS_CENTER);

	gtk_widget_show(dlg);
	gtk_grab_add(dlg);
	_gtk_really_modal();
}


static void _mpv_paint(void)
{
	int n,m,i;
	int * fb;
	int c,color,x;
	GdkFont * font;
	int yp,sw,sp;

	for(n=0;n < _mpv_y_size;n++)
	{
		i=n * _mpv_x_size;

		fb=&_mpv_fb[i];

		/* avoid drawing a line that didn't change */
		if(memcmp((char *)fb, (char *)&_mpv_fbo[i],
			_mpv_x_size * sizeof(int))==0)
			continue;

		yp=n * _mpv_font_height;

		for(x=m=0;m < _mpv_x_size;)
		{
			/* get first color */
			color=*fb & 0xff00;

			/* writes into _mpv_buffer while
			   color is the same */
			for(i=0,sp=1;m<_mpv_x_size &&
				color==(*fb & 0xff00);
				i++,m++,fb++)
			{
				c=*fb & 0xff;
				_mpv_buffer[i]=c;
				if(c!=' ') sp=0;
			}

			_mpv_buffer[i]='\0';

			color>>=8;

			if(mpc_color_desc[color].italic && _font_italic != NULL)
				font=_font_italic;
			else
				font=_font_normal;

			sw=gdk_string_width(font,_mpv_buffer);

			gdk_gc_set_foreground(gc, &_papers[color]);

			if(! _use_double_buffer)
			{
				gdk_draw_rectangle(area->window, gc,
					TRUE, x, yp, sw, _mpv_font_height);

				if(!sp)
				{
					gdk_gc_set_foreground(gc, &_inks[color]);

					gdk_draw_string(area->window, font, gc,
					x, yp + _mpv_font_height - (_mpv_font_height / 3),
					_mpv_buffer);

					if(mpc_color_desc[color].underline)
					gdk_draw_line(area->window, gc,
					x, yp + _mpv_font_height - 4,
					x + sw, yp + _mpv_font_height - 4);
				}
			}
			else
			{
				if(sp)
				{
					gdk_draw_rectangle(area->window, gc,
					TRUE, x, yp, sw, _mpv_font_height);
				}
				else
				{
					gdk_draw_rectangle(pixmap, gc,
					TRUE, 0, 0, sw, _mpv_font_height);

					gdk_gc_set_foreground(gc, &_inks[color]);

					gdk_draw_string(pixmap, font, gc,
					0, _mpv_font_height - (_mpv_font_height / 3),
					_mpv_buffer);

					if(mpc_color_desc[color].underline)
					gdk_draw_line(pixmap, gc,
					0, _mpv_font_height - 4,
					sw, _mpv_font_height - 4);

					/* finally transfer */
					gdk_draw_pixmap(area->window, gc,
					pixmap, 0, 0,
					x, yp, sw, _mpv_font_height);
				}
			}

			x+=sw;
		}
	}

	/* copy this framebuffer into old */
	memcpy(_mpv_fbo, _mpv_fb, _mpv_fb_size);
}


static gint _mpv_key_callback(GtkWidget * widget, GdkEventKey * event)
{
	char * ptr=NULL;
	int c;

#if GTK_MAJOR_VERSION >= 2
	gtk_im_context_filter_keypress(im, event);
#endif

	c=(int)(unsigned char)event->string[0];

	mpi_move_selecting=event->state & GDK_SHIFT_MASK;

	if(event->state & (GDK_CONTROL_MASK|GDK_MOD1_MASK))
	{
		switch(event->keyval)
		{
		case GDK_Up:	ptr="ctrl-cursor-up"; break;
		case GDK_Down:	ptr="ctrl-cursor-down"; break;
		case GDK_Left:	ptr="ctrl-cursor-left"; break;
		case GDK_Right: ptr="ctrl-cursor-right"; break;
		case GDK_Prior: ptr="ctrl-page-up"; break;
		case GDK_Next:	ptr="ctrl-page-down"; break;
		case GDK_Home:	ptr="ctrl-home"; break;
		case GDK_End:	ptr="ctrl-end"; break;

		case GDK_KP_Add: ptr="ctrl-kp-plus"; break;
		case GDK_KP_Subtract: ptr="ctrl-kp-minus"; break;
		case GDK_KP_Multiply: ptr="ctrl-kp-multiply"; break;
		case GDK_KP_Divide: ptr="ctrl-kp-divide"; break;

		case GDK_F1:	ptr="ctrl-f1"; break;
		case GDK_F2:	ptr="ctrl-f2"; break;
		case GDK_F3:	ptr="ctrl-f3"; break;
		case GDK_F4:	ptr="ctrl-f4"; break;
		case GDK_F5:	ptr="ctrl-f5"; break;
		case GDK_F6:	ptr="ctrl-f6"; break;
		case GDK_F7:	ptr="ctrl-f7"; break;
		case GDK_F8:	ptr="ctrl-f8"; break;
		case GDK_F9:	ptr="ctrl-f9"; break;
		case GDK_F10:	ptr="ctrl-f10"; break;
		case GDK_F11:	ptr="ctrl-f11"; break;
		case GDK_F12:	ptr="ctrl-f12"; break;
		case GDK_Return: ptr="ctrl-enter"; break;

		case 'a':	ptr="ctrl-a"; break;
		case 'b':	ptr="ctrl-b"; break;
		case 'c':	ptr="ctrl-c"; break;
		case 'd':	ptr="ctrl-d"; break;
		case 'e':	ptr="ctrl-e"; break;
		case 'f':	ptr="ctrl-f"; break;
		case 'g':	ptr="ctrl-g"; break;
		case 'h':	ptr="ctrl-h"; break;
		case 'i':	ptr="ctrl-i"; break;
		case 'j':	ptr="ctrl-j"; break;
		case 'l':	ptr="ctrl-l"; break;
		case 'm':	ptr="ctrl-m"; break;
		case 'n':	ptr="ctrl-n"; break;
		case 'o':	ptr="ctrl-o"; break;
		case 'p':	ptr="ctrl-p"; break;
		case 'q':	ptr="ctrl-q"; break;
		case 'r':	ptr="ctrl-r"; break;
		case 's':	ptr="ctrl-s"; break;
		case 't':	ptr="ctrl-t"; break;
		case 'u':	ptr="ctrl-u"; break;
		case 'v':	ptr="ctrl-v"; break;
		case 'w':	ptr="ctrl-w"; break;
		case 'x':	ptr="ctrl-x"; break;
		case 'y':	ptr="ctrl-y"; break;
		case 'z':	ptr="ctrl-z"; break;
		}
	}
	else
	if(event->keyval > 256)
	{
		switch(event->keyval)
		{
		case GDK_Up:	ptr="cursor-up"; break;
		case GDK_Down:	ptr="cursor-down"; break;
		case GDK_Left:	ptr="cursor-left"; break;
		case GDK_Right: ptr="cursor-right"; break;
		case GDK_Prior: ptr="page-up"; break;
		case GDK_Next:	ptr="page-down"; break;
		case GDK_Home:	ptr="home"; break;
		case GDK_End:	ptr="end"; break;

		case GDK_KP_Add: ptr="kp-plus"; break;
		case GDK_KP_Subtract: ptr="kp-minus"; break;
		case GDK_KP_Multiply: ptr="kp-multiply"; break;
		case GDK_KP_Divide: ptr="kp-divide"; break;

		case GDK_F1:	ptr="f1"; break;
		case GDK_F2:	ptr="f2"; break;
		case GDK_F3:	ptr="f3"; break;
		case GDK_F4:	ptr="f4"; break;
		case GDK_F5:	ptr="f5"; break;
		case GDK_F6:	ptr="f6"; break;
		case GDK_F7:	ptr="f7"; break;
		case GDK_F8:	ptr="f8"; break;
		case GDK_F9:	ptr="f9"; break;
		case GDK_F10:	ptr="f10"; break;
		case GDK_F11:	ptr="f11"; break;
		case GDK_F12:	ptr="f12"; break;

		case GDK_Insert: ptr="insert"; break;
		case GDK_BackSpace: ptr="backspace"; break;
		case GDK_Delete: ptr="delete"; break;
		case GDK_Return: ptr="enter"; break;
		case GDK_Tab:	 ptr="tab"; break;
		case GDK_Escape: ptr="escape"; break;
		}
	}
	else
	if(c < 32 || c > 255 || c==127)
		c='\0';

	if(_mpv_im_char != '\0')
	{
		c=_mpv_im_char;
		_mpv_im_char='\0';
	}

	if(c!='\0' || ptr!=NULL)
		mpi_process(c, ptr, NULL);

	if(_mpi_exit_requested)
		gtk_main_quit();

/*	  gdk_draw_string(area->window, _font_normal, area->style->black_gc,
		50, 50, "Minimum Profit");
*/
/*	  for(n=0;n < 100;n++)
	{
		char tmp[100];

		sprintf(tmp,"línea %d - %d",n,_mpv_font_width);
		gdk_draw_string(area->window, _font_normal, area->style->black_gc,
			0, _mpv_font_height + (n * _mpv_font_size), tmp);
	}
*/
/*	  mpv_scrollbar(_mp_active->y,_mpv_y_size,_mp_active->lasty+1);
*/
	return(0);
}


static gint _mpv_mouse_callback(GtkWidget * widget,
	GdkEventButton * event, gpointer data)
{
	int x,y;
	char * keyname=NULL;

	/* mouse instant positioning */
	x=((int)event->x) / _mpv_font_width;
	y=((int)event->y) / _mpv_font_height;

	mp_move_xy(_mp_active,x,y+_mp_active->vy);
	mp_move_bol(_mp_active);
	mp_move_to_visual_column(_mp_active,x);

	switch(event->button)
	{
	case 1: keyname="mouse-left-button"; break;
	case 2: keyname="mouse-middle-button"; break;
	case 3: keyname="mouse-right-button"; break;
	case 4: keyname="mouse-wheel-up"; break;
	case 5: keyname="mouse-wheel-down"; break;
	}


	if(keyname != NULL)
		mpi_process('\0', keyname, NULL);

	mpi_draw_all(_mp_active);

	return(0);
}


static gint _mpv_configure_event_callback(GtkWidget * widget,
	GdkEventConfigure * event)
{
	static GdkEventConfigure o;

	if(memcmp(&o,event,sizeof(o))==0)
		return(TRUE);
	memcpy(&o,event,sizeof(o));

	_mpv_calc_font_width_and_height();

	if(_mp_active)
		mpi_draw_all(_mp_active);

	return(TRUE);
}


static gint _mpv_expose_event_callback(GtkWidget * widget,
	GdkEventExpose * event)
{
	if(event->area.width!=0 && event->area.height!=0)
		memset(_mpv_fbo, '\0', _mpv_fb_size);

	_mpv_paint();

#if GTK_MAJOR_VERSION < 2

	/* get back the processing of dead keys */
	if(ic!=NULL)
		gdk_im_begin(ic, widget->window);

#endif

	return(FALSE);
}


#if GTK_MAJOR_VERSION >= 2
static void _mpv_commit_callback(GtkIMContext * i, char * str, gpointer u)
{
	str=__(str);

	_mpv_im_char=*str;

	free(str);
}
#endif

static void _mpv_realize_callback(GtkWidget * widget)
{
#if GTK_MAJOR_VERSION < 2

	/* I don't undestand this stupid load of crap */
	/* borrowed (more or less) from the gtk_entry code */
	/* all this shit is just to receive dead key
	   composed characters correctly */
	GdkICAttr * attr;
	GdkEventMask mask;
	GdkIMStyle s_style=GDK_IM_PREEDIT_NONE |
			   GDK_IM_PREEDIT_NOTHING |
			   GDK_IM_STATUS_NONE |
			   GDK_IM_STATUS_NOTHING;

	if(gdk_im_ready() && (attr=gdk_ic_attr_new())!=NULL)
	{
		attr->style=gdk_im_decide_style(s_style);
		attr->client_window=widget->window;

		ic=gdk_ic_new(attr, (GdkICAttributesType)GDK_IC_ALL_REQ);

		mask=gdk_window_get_events(widget->window);
		mask |= gdk_ic_get_events(ic);
		gdk_window_set_events(widget->window, mask);

		gdk_im_begin(ic, widget->window);
	}

#else /* GTK > 2 */

	im=gtk_im_multicontext_new();
	g_signal_connect(im, "commit",
			 G_CALLBACK(_mpv_commit_callback), NULL);
	gtk_im_context_set_client_window(im, widget->window);

#endif
}


static void _mpv_filetabs_callback(GtkNotebook * notebook,
		GtkNotebookPage * page, gint pg_num,
		gpointer data)
{
	int n;
	mp_txt * txt;

	for(n=0,txt=_mp_txts;txt!=NULL && n < 100;n++,txt=txt->next)
	{
		if(n==pg_num)
		{
			if(_mp_active != txt)
				_mp_active=txt;
		}
	}

	gtk_widget_grab_focus(area);
	mpi_draw_all(_mp_active);
}


/**
 * mpv_filetabs - Draws the tab set containing the file names
 *
 * Draws the names of the opened files in the form of a tab set.
 */
void mpv_filetabs(void)
{
	int n;
	mp_txt * t;
	GtkWidget * l;
	char * ptr;

	gtk_signal_disconnect_by_func(GTK_OBJECT(file_tabs),
		(GtkSignalFunc) _mpv_filetabs_callback, NULL);

	/* delete possible previous tabs */
	for(n=0;n < 100;n++)
		gtk_notebook_remove_page(GTK_NOTEBOOK(file_tabs),0);

	for(t=_mp_txts,n=0;t!=NULL && n < 100;t=t->next,n++)
	{
		GtkWidget * f;

		if((ptr=strrchr(t->name,'/'))==NULL)
			ptr=t->name;
		else
			ptr++;

		l=gtk_label_new(_(ptr));
		gtk_widget_show(l);

		f=gtk_frame_new(NULL);
		gtk_widget_show(f);

		gtk_notebook_append_page(GTK_NOTEBOOK(file_tabs),
			f,l);

		if(_mp_active == t)
			gtk_notebook_set_page(GTK_NOTEBOOK(file_tabs), n);
	}

	gtk_signal_connect(GTK_OBJECT(file_tabs),"switch_page",
		(GtkSignalFunc) _mpv_filetabs_callback, NULL);

	gtk_widget_grab_focus(area);
}


static gint _mpv_selection_clear_callback(GtkWidget * widget,
	GdkEventSelection * event, gpointer data)
{
	_mpv_selection=0;

	return(TRUE);
}


static void _mpv_selection_get_callback(GtkWidget * widget,
	GtkSelectionData * sel, guint info, guint tm, gpointer data)
{
	char * ptr;
	int n,c;

	if(!_mpv_selection) return;
	if(_mp_clipboard == NULL) return;

	/* counts first the number of bytes in the clipboard */
	mp_move_bof(_mp_clipboard);
	for(n=0;(c=mp_get_char(_mp_clipboard))!='\0';n++);

	/* get buffer */
	if((ptr=(char *)malloc(n))==NULL) return;

	/* transfer */
	mp_move_bof(_mp_clipboard);
	for(n=0;(c=mp_get_char(_mp_clipboard))!='\0';n++)
		ptr[n]=c;

	/* pastes into primary selection */
	gtk_selection_data_set(sel, GDK_SELECTION_TYPE_STRING, 8,
		ptr, n);

	free(ptr);
}


static void _mpv_selection_received_callback(GtkWidget * widget,
	GtkSelectionData * sel, guint tm, gpointer data)
{
	int n;

	mp_lock_clipboard(1);

	for(n=0;n < sel->length;n++)
	{
		mp_put_char(_mp_clipboard,sel->data[n],1);
		mp_put_char(_mp_active,sel->data[n],1);
	}

	mp_lock_clipboard(0);

	mpi_draw_all(_mp_active);
}


#if GTK_MAJOR_VERSION >= 2

static gint _mpv_scroll_callback(GtkWidget * widget, GdkEventScroll * event)
{
	char * keyname=NULL;

	switch(event->direction)
	{
	case GDK_SCROLL_UP: keyname="mouse-wheel-up"; break;
	case GDK_SCROLL_DOWN: keyname="mouse-wheel-down"; break;
	case GDK_SCROLL_LEFT: keyname="mouse-wheel-left"; break;
	case GDK_SCROLL_RIGHT: keyname="mouse-wheel-right"; break;
	default: keyname=NULL;
	}

	if(keyname != NULL)
		mpi_process('\0', keyname, NULL);

	mpi_draw_all(_mp_active);

	return(0);
}

#endif


/**
 * mpv_create_colors - Creates the colors defined in the color database
 *
 * Creates the colors defined in the color database, probably
 * set from the configuration file.
 */
void mpv_create_colors(void)
{
	int n;
	int ink,paper;

	for(n=0;n < MP_COLOR_PRIVATE;n++)
	{
		ink=mpc_color_desc[n].ink_rgb;
		paper=mpc_color_desc[n].paper_rgb;

		if(ink==0xffffffff) ink=mpc_color_desc[0].ink_rgb;
		if(paper==0xffffffff) paper=mpc_color_desc[0].paper_rgb;

		if(mpc_color_desc[n].reverse)
		{
			int i=paper;
			paper=ink; ink=i;
		}

		_inks[n].pixel=0;
		_inks[n].blue=(ink & 0x000000ff) << 8;
		_inks[n].green=(ink & 0x0000ff00);
		_inks[n].red=(ink & 0x00ff0000) >> 8;

		gdk_color_alloc(gdk_colormap_get_system(), &_inks[n]);

		_papers[n].pixel=0;
		_papers[n].blue=(paper & 0x00000000ff) << 8;
		_papers[n].green=(paper & 0x0000ff00);
		_papers[n].red=(paper & 0x00ff00ff) >> 8;

		gdk_color_alloc(gdk_colormap_get_system(), &_papers[n]);
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
	GtkWidget * vbox;
	GtkWidget * hbox;

	mp_log("Using GTK+ driver (%d.%d.%d)\n",
		GTK_MAJOR_VERSION, GTK_MINOR_VERSION, GTK_MICRO_VERSION);

	window=gtk_window_new(GTK_WINDOW_TOPLEVEL);

	gtk_signal_connect(GTK_OBJECT(window), "delete_event",
		(GtkSignalFunc) gtk_main_quit, NULL);

	/* file tabs */
	file_tabs=gtk_notebook_new();
	gtk_notebook_set_tab_pos(GTK_NOTEBOOK(file_tabs), GTK_POS_TOP);
	GTK_WIDGET_UNSET_FLAGS(file_tabs,GTK_CAN_FOCUS);
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(file_tabs),0);

	vbox=gtk_vbox_new(FALSE, 2);
	gtk_container_add(GTK_CONTAINER(window), vbox);

	gtk_box_pack_start(GTK_BOX(vbox),menu_bar,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(vbox),file_tabs,FALSE,FALSE,0);

	/* horizontal box holding the text and the scrollbar */
	hbox=gtk_hbox_new(FALSE, 2);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,TRUE,TRUE,0);

	/* the Minimum Profit area */
	area=gtk_drawing_area_new();
	gtk_box_pack_start(GTK_BOX(hbox),area,TRUE,TRUE,0);
	gtk_widget_set_usize(GTK_WIDGET(area),400,300);
	gtk_widget_set_events(GTK_WIDGET(area),GDK_BUTTON_PRESS_MASK);

	gtk_signal_connect(GTK_OBJECT(area),"configure_event",
		(GtkSignalFunc) _mpv_configure_event_callback, NULL);

	gtk_signal_connect(GTK_OBJECT(area),"expose_event",
		(GtkSignalFunc) _mpv_expose_event_callback, NULL);

	gtk_signal_connect(GTK_OBJECT(area), "realize",
		(GtkSignalFunc) _mpv_realize_callback, NULL);

	gtk_signal_connect(GTK_OBJECT(window),"key_press_event",
		(GtkSignalFunc) _mpv_key_callback, NULL);

	gtk_signal_connect(GTK_OBJECT(area),"button_press_event",
		(GtkSignalFunc) _mpv_mouse_callback, NULL);

	gtk_signal_connect(GTK_OBJECT(file_tabs),"switch_page",
		(GtkSignalFunc) _mpv_filetabs_callback, NULL);

	gtk_signal_connect(GTK_OBJECT(area), "selection_clear_event",
		GTK_SIGNAL_FUNC(_mpv_selection_clear_callback), NULL);

	gtk_signal_connect(GTK_OBJECT(area), "selection_get",
		GTK_SIGNAL_FUNC(_mpv_selection_get_callback), NULL);

	gtk_signal_connect(GTK_OBJECT(area), "selection_received",
		GTK_SIGNAL_FUNC(_mpv_selection_received_callback), NULL);

#if GTK_MAJOR_VERSION >= 2

	gtk_signal_connect(GTK_OBJECT(area), "scroll_event",
		GTK_SIGNAL_FUNC(_mpv_scroll_callback), NULL);

#endif

	gtk_selection_add_target(area, GDK_SELECTION_PRIMARY,
		GDK_SELECTION_TYPE_STRING, 1);

	/* the scrollbar */
	scrollbar=gtk_vscrollbar_new(NULL);
	gtk_box_pack_start(GTK_BOX(hbox),scrollbar,FALSE,FALSE,0);

	gtk_signal_connect(
		GTK_OBJECT(gtk_range_get_adjustment(GTK_RANGE(scrollbar))),
		"value_changed",
		(GtkSignalFunc) _mpv_value_changed_callback, NULL);

	/* the status bar */
	status=gtk_label_new("mp " VERSION);
	gtk_box_pack_start(GTK_BOX(vbox), status, FALSE, FALSE, 0);
	gtk_misc_set_alignment(GTK_MISC(status), 0, 0.5);
	gtk_label_set_justify(GTK_LABEL(status), GTK_JUSTIFY_LEFT);

	gtk_widget_show_all(window);

	if(_mpv_gtk_xpos >= 0 && _mpv_gtk_ypos >= 0)
		gdk_window_move(GTK_WIDGET(window)->window,
			_mpv_gtk_xpos, _mpv_gtk_ypos);

	if(_mpv_gtk_width > 0 && _mpv_gtk_height > 0)
	{
		/* fix ridiculously small values */
		if(_mpv_gtk_width < 150) _mpv_gtk_width=150;
		if(_mpv_gtk_height < 150) _mpv_gtk_height=150;

		gtk_window_set_default_size(GTK_WINDOW(window),
			_mpv_gtk_width, _mpv_gtk_height);

		gdk_window_resize(GTK_WIDGET(window)->window,
			_mpv_gtk_width, _mpv_gtk_height);
	}

	gc=gdk_gc_new(area->window);

	_mpv_font_size--;
	mpv_zoom(1);

	/* colors */
	mpv_create_colors();

	fclose(stderr);

	mp_log("Text mode fallback program: '%s'\n",CURSES_MP);

	mp_log("X11 geometry set to %dx%d+%d+%d\n", _mpv_gtk_width, _mpv_gtk_height,
		_mpv_gtk_xpos, _mpv_gtk_ypos);

	return(0);
}


/**
 * mpv_shutdown - Shuts down the system dependent driver.
 *
 * Shuts down the system dependent driver.
 */
void mpv_shutdown(void)
{
	/* dummy */
}


/**
 * _mpv_usage - Prints the usage help.
 *
 * Prints the usage help.
 */
void _mpv_usage(void)
{
	printf("Minimum Profit " VERSION " (Gtk Interface) - Programmer Text Editor\n");
	printf("Copyright (C) 1991-2003 Angel Ortega <angel@triptico.com>\n");
	printf("%s\n", __DATE__ " " __TIME__);
	printf("This software is covered by the GPL license. NO WARRANTY.\n\n");

	printf("%s\n", L(MSG_USAGE_TEXT));

	printf("--lang: %s\n",mpl_enumerate_langs());
	printf("--mode: %s\n",mps_enumerate_modes());
	printf("\nConfig file: %s\n",_mpc_config_file);
}


/**
 * mpv_set_variable - Sets a driver-dependent variable
 * @var: the name of the variable to be set
 * @value: the new value
 *
 * Sets a variable from the configuration file that is
 * driver-specific.
 */
void mpv_set_variable(char * var, char * value)
{
	if(strcmp(var,"gtk_font_face")==0)
		strncpy(_mpv_font_face, value, sizeof(_mpv_font_face));
	else
	if(strcmp(var,"gtk_font_size")==0)
		_mpv_font_size=atoi(value);
	else
	if(strcmp(var,"gtk_font_weight")==0)
		strncpy(_mpv_font_weight, value, sizeof(_mpv_font_weight));
	else
	if(strcmp(var,"gtk_use_italics")==0)
		_use_italics=atoi(value);
	else
	if(strcmp(var,"gtk_use_double_buffer")==0)
		_use_double_buffer=atoi(value);
	else
	if(strcmp(var,"gtk_width")==0)
		_mpv_gtk_width=atoi(value);
	else
	if(strcmp(var,"gtk_height")==0)
		_mpv_gtk_height=atoi(value);
	else
	if(strcmp(var,"gtk_xpos")==0)
		_mpv_gtk_xpos=atoi(value);
	else
	if(strcmp(var,"gtk_ypos")==0)
		_mpv_gtk_ypos=atoi(value);
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
int mpv_args_1(int * n, int argc, char * argv[])
{
	if(strcmp(argv[*n],"-geometry")==0 ||
	   strcmp(argv[*n],"--geometry")==0)
	{
		int x,y,tx,ty;

		if(*n >= argc-1)
			return(1);

		(*n)++;
		x=y=tx=ty=-1;

		if(sscanf(argv[*n],"%dx%d+%d+%d",
			&tx,&ty,&x,&y)==4)
		{
			_mpv_gtk_width=tx;
			_mpv_gtk_height=ty;
			_mpv_gtk_xpos=x;
			_mpv_gtk_ypos=y;
		}
		else
		if(sscanf(argv[*n],"%dx%d",
			&tx,&ty)==2)
		{
			_mpv_gtk_width=tx;
			_mpv_gtk_height=ty;
		}
		else
		if(sscanf(argv[*n],"+%d+%d",
			&x,&y)==2)
		{
			_mpv_gtk_xpos=x;
			_mpv_gtk_ypos=y;
		}
	}

	return(0);
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
int mpv_args_2(int * n, int argc, char * argv[])
{
	/* skips the -geometry definition */
	if(strcmp(argv[*n],"-geometry")==0 ||
	   strcmp(argv[*n],"--geometry")==0)
	{
		if(*n >= argc-1)
			return(1);

		(*n)++;
	}

	return(0);
}


int main(int argc, char * argv[])
/* main */
{
	int r;

	mp_startup();

	mpc_startup();

	r=mpi_args_1(argc, argv);

	if(r==-1) { _mpv_usage(); exit(0); }
	if(r==-2) { printf(VERSION "\n"); exit(0); }

	/* opens GTK, and falls back to text mp */
	if(!gtk_init_check(&argc,&argv))
	{
		execvp(CURSES_MP,argv);
		printf("ERROR: Bad X Display\n");
		exit(1);
	}

	mps_startup();
	mpi_startup();
	mpv_startup();

	r=mpi_args_2(argc, argv);

	if(r==-1)
	{
		mpv_shutdown();
		printf("%s\n",L(MSG_BADMODE));
		printf("%s\n",mps_enumerate_modes());
		exit(1);
	}

	/* force the reading of the tags file, if one exists */
	mpi_seek_tag(NULL,0);

	/* create empty text if no file is open */
	if(_mp_active==NULL)
	{
		mp_create_txt(L(MSG_UNNAMED));
		mps_auto_synhi(_mp_active);
	}

	mpv_title(NULL);
	mpi_draw_all(_mp_active);
	mpv_filetabs();

	/* bark if bad tags */
	if(r==-2)
		mpv_alert(L(MSG_TAGNOTFOUND),"");

	/* main loop */
	gtk_main();

	/* close everything */
	mpi_shutdown();

	mpv_shutdown();
	mp_shutdown();

	return(0);
}
