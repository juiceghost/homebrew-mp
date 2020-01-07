/*

    mpv_win32.c

    Win32 Interface.

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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <commctrl.h>

#include "mp_core.h"
#include "mp_video.h"
#include "mp_func.h"
#include "mp_lang.h"
#include "mp_iface.h"
#include "mp_synhi.h"
#include "mp_res.h"
#include "mp_conf.h"

/*******************
	 Data
 *******************/

/* Win32 necessary globals */

/* the instance */
HINSTANCE hinst;

/* the windows */
HWND hwnd;
HWND hwtabs;

/* font handles */
HFONT _font_normal=NULL;
HFONT _font_italic=NULL;
HFONT _font_underline=NULL;

/* frame buffer with chars in document window */
static int * _mpv_fb=NULL;

/* cursor position */
static int _mpv_x_cursor;
static int _mpv_y_cursor;

/* temporal buffer */
static char _mpv_buffer[1024];

/* font size */
int _mpv_font_size=12;

/* font face */
char _mpv_font_face[80]="Lucida Console";

/* calculated font sizes in local units */
int _mpv_font_height=0;
int _mpv_font_width=0;

/* attributes */
static COLORREF _inks[MP_COLOR_NUM];
static COLORREF _papers[MP_COLOR_NUM];

/* win32.hlp path */
char _mpv_win32help[1024]="c:\\bin\\lcc\\bin\\win32.hlp";

/* dialog box vars */
static char * _mpv_dlg_prompt=NULL;
static char * _mpv_dlg_default=NULL;
static char * _mpv_list_title="";
static mp_txt * _mpv_list_text=NULL;

/* readline buffer */
static char _mpv_readline_buf[1024];

/* title and status line buffers */
static char _mpv_title_buffer[1024];
static char _mpv_status_line_buffer[1024];

/* menu and submenu handlers */
static HMENU _menu=NULL;
static HMENU _smenu=NULL;
static char _menu_label[40]="";

/* height of the tab of files */
int _tab_height=28;

/* microsoft moronity overriding */
int _argc_;
char * _argv_[100];

/* sequential menu id */
int _win32_menu_id=1000;


/*******************
	 Code
 *******************/

/**
 * mpv_strcasecmp - Case ignoring string compare
 * @s1: first string
 * @s2: second string
 *
 * Case ignoring string compare. System dependent
 * (strcasecmp in Unix, stricmp in Win32)
 */
int mpv_strcasecmp(char * s1, char * s2)
{
	return(stricmp(s1,s2));
}


/**
 * mpv_glob - Returns a result of a file globbing
 * @spec: the file pattern
 *
 * Executes a file globbing using @spec as a pattern
 * and returns a mp_txt containing the files matching
 * the glob.
 */
mp_txt * mpv_glob(char * spec)
{
	mp_txt * txt=NULL;
	WIN32_FIND_DATA f;
	HANDLE h;

	if(spec==NULL || *spec=='\0' || strcmp(spec,"*")==0)
		spec="*.*";

	txt=mp_create_sys_txt("<glob>");

	if((h=FindFirstFile(spec,&f))==INVALID_HANDLE_VALUE)
	{
		MessageBox(NULL,"FindFirstFile unexpected error","ERROR",MB_OK);
		return(NULL);
	}

	do
	{
		if(strcmp(f.cFileName,".")==0 ||
		   strcmp(f.cFileName,"..")==0)
			continue;

		if(f.dwFileAttributes &
			FILE_ATTRIBUTE_DIRECTORY)
			continue;

		mp_put_str(txt,f.cFileName,1);
		mp_put_char(txt,'\n',1);
	}
	while(FindNextFile(h,&f));

	FindClose(h);

	return(txt);
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
	/* nothing; the Windows system caret could be used,
	   but I just hate it */
}


/**
 * mpv_refresh - Refresh the screen.
 *
 * Orders the underlying system to redraw the screen.
 */
void mpv_refresh(void)
{
	InvalidateRect(hwnd,NULL,TRUE);
}


/**
 * _mpv_title_status - Sets the window caption
 *
 * Sets the window caption with the title and the status bar.
 */
static void _mpv_title_status(void)
{
	char tmp[2048];

	strncpy(tmp,"mp " VERSION,sizeof(tmp));

	if(_mpv_title_buffer[0]!='\0')
	{
		strcat(tmp," - ");
		strcat(tmp,_mpv_title_buffer);
	}

	if(_mpv_status_line_buffer[0]!='\0')
		strcat(tmp,_mpv_status_line_buffer);

	SetWindowText(hwnd,tmp);
}


/**
 * mpv_title - Sets the string to be drawn as title
 * @str: the string
 *
 * Sets the string to be drawn as title of the document window.
 */
void mpv_title(char * str)
{
	if(str!=NULL)
		strncpy(_mpv_title_buffer,str,sizeof(_mpv_title_buffer));
	else
	if(_mp_active != NULL)
		strncpy(_mpv_title_buffer,_mp_active->name,sizeof(_mpv_title_buffer));
	else
		_mpv_title_buffer[0]='\0';

	_mpv_title_status();
}


/**
 * mpv_status_line - Sets the string to be drawn as status line
 * @str: the string
 *
 * Sets the string to be drawn as the status line.
 */
void mpv_status_line(char * str)
{
	if(str)
		strncpy(_mpv_status_line_buffer,str,sizeof(_mpv_status_line_buffer));
	else
		_mpv_status_line_buffer[0]='\0';

	_mpv_title_status();
}


/**
 * mpv_add_menu - Creates a new menu bar.
 * @label: the label
 *
 * Creates a new menu bar.
 */
void mpv_add_menu(char * label)
{
	label=L(label);

	if(_menu)
		AppendMenu(_menu,MF_STRING|MF_POPUP,
			(UINT)_smenu,_menu_label);
	else
		_menu=CreateMenu();

	strncpy(_menu_label,label,sizeof(_menu_label));
	_smenu=CreatePopupMenu();
}


/**
 * mpv_add_menu_item - Adds a menu item to current menu bar.
 * @funcname: the name of the function to invoke
 *
 * Adds a menu item to the current menu bar.
 */
void mpv_add_menu_item(char * funcname)
{
	char tmp[1024];
	char * label;
	MENUITEMINFO mi;
	int * i;
	char * keyname;

	label=L(funcname);

	if((keyname=mpf_get_keyname_by_funcname(funcname))!=NULL)
		snprintf(tmp,sizeof(tmp),"%s\t%s",label,keyname);
	else
		strncpy(tmp,label,sizeof(tmp));

	if(tmp[0]=='-')
		AppendMenu(_smenu,MF_SEPARATOR,0,NULL);
	else
	{
		AppendMenu(_smenu,MF_STRING,_win32_menu_id,tmp);

		memset(&mi,'\0',sizeof(mi));
		mi.cbSize=sizeof(mi);
		mi.fMask=MIIM_DATA;
		mi.dwItemData=(unsigned long)funcname;

		SetMenuItemInfo(_smenu,_win32_menu_id,FALSE,&mi);

		if((i=mpf_toggle_function_value(funcname))!=NULL)
		{
			CheckMenuItem(_smenu,_win32_menu_id,
				((*i) ? MF_CHECKED : MF_UNCHECKED));
		}

		_win32_menu_id++;
	}
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
	int n;
	MENUITEMINFO mi;

	/* close the menu, if not closed */
	if(_menu_label[0]!='\0')
		mpv_add_menu("");

	for(n=1000;n < _win32_menu_id;n++)
	{
		memset(&mi,'\0',sizeof(mi));
		mi.cbSize=sizeof(mi);
		mi.fMask=MIIM_DATA;

		if(GetMenuItemInfo(_menu,n,FALSE,&mi))
		{
			if(mi.dwItemData != 0 &&
			   strcmp((char *)mi.dwItemData,funcname)==0)
			{
				CheckMenuItem(_menu, n,
					toggle ? MF_CHECKED :
						 MF_UNCHECKED);

				break;
			}
		}
	}
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
	/* dummy */
	return(0);
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
	char tmp[4096];

	if(msg2==NULL)
		strncpy(tmp,msg,sizeof(tmp));
	else
		sprintf(tmp,msg,msg2);

	MessageBox(hwnd,tmp,"mp " VERSION,
		MB_ICONWARNING|MB_OK);
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
	if(MessageBox(hwnd,prompt,"mp " VERSION,
		MB_ICONQUESTION|MB_YESNO)==IDYES)
		return(1);
	else
		return(0);
}


/**
 * TextDlgProc - Procedure for text input dialog
 * @hwnd: the window handler
 * @msg: the message sent
 * @wparam: the word param
 * @lparam: the long word param
 *
 * Procedure for text input dialog. Called from inside MS Windows.
 */
BOOL CALLBACK TextDlgProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	int ret;

	switch(msg)
	{
	case WM_INITDIALOG:

		SetWindowText(hwnd,"mp " VERSION);

		SetDlgItemText(hwnd,WMP_1STR_LABEL,
			_mpv_dlg_prompt);

		if(_mpv_dlg_default!=NULL)
		{
			SetDlgItemText(hwnd,WMP_1STR_EDIT,
			_mpv_dlg_default);
			SendDlgItemMessage(hwnd,WMP_1STR_EDIT,
				EM_SETSEL, 0, 1000);
		}

		return(TRUE);

	case WM_COMMAND:

		switch(LOWORD(wparam))
		{
		case WMP_OK:
		case WMP_CANCEL:

			if(LOWORD(wparam)==WMP_OK)
			{
				ret=1;
				GetDlgItemText(hwnd,WMP_1STR_EDIT,
					_mpv_readline_buf,
					sizeof(_mpv_readline_buf));
			}
			else
				ret=0;

			EndDialog(hwnd,ret);

			return(TRUE);
		}
	}

	return(FALSE);
}


/**
 * ListDlgProc - Procedure for the list input dialog
 * @hwnd: the window handler
 * @msg: the message sent
 * @wparam: the word param
 * @lparam: the long word param
 *
 * Procedure for list input dialog. Called from inside MS Windows.
 */
BOOL CALLBACK ListDlgProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	int ret;
	HWND lst;
	char line[1024];

	switch(msg)
	{
	case WM_INITDIALOG:

		SetWindowText(hwnd, _mpv_list_title);

		lst=GetDlgItem(hwnd, WMP_LIST);
		SendMessage(lst, WM_SETFONT, 
			(WPARAM) GetStockObject(ANSI_FIXED_FONT), 0);

		/* traverses the list, filling the listbox */
		mp_move_bof(_mpv_list_text);

		while(mp_peek_char(_mpv_list_text)!='\0')
		{
			mp_get_str(_mpv_list_text,line,
				sizeof(line),'\n');

			SendMessage(lst, LB_ADDSTRING, 0,
				(LPARAM) line);
		}

		return(TRUE);

	case WM_COMMAND:

		switch(LOWORD(wparam))
		{
		case WMP_OK:
		case WMP_CANCEL:

			if(LOWORD(wparam)==WMP_OK)
				ret=SendDlgItemMessage(hwnd, WMP_LIST,
					LB_GETCURSEL, 0, 0);
			else
				ret=-1;

			EndDialog(hwnd,ret);

			return(TRUE);
		}
	}

	return(FALSE);
}


/**
 * AboutDlgProc - Procedure for the about box
 * @hwnd: the window handler
 * @msg: the message sent
 * @wparam: the word param
 * @lparam: the long word param
 *
 * Procedure for the about box. Called from inside MS Windows.
 */
BOOL CALLBACK AboutDlgProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	char tmp[4096];
	char * ptr;
	int n;

	switch(msg)
	{
	case WM_INITDIALOG:

		SetWindowText(hwnd, L(MSG_ABOUT));

		/* move the license text converting \n to \r\n */
		for(n=0,ptr=tmp;MP_LICENSE[n];n++)
		{
			if(MP_LICENSE[n]=='\n')
				*(ptr++)='\r';
			*(ptr++)=MP_LICENSE[n];
		}
		*ptr='\0';

		SetDlgItemText(hwnd,WMP_LICENSE,tmp);

		SendDlgItemMessage(hwnd,WMP_LICENSE,
			EM_SETSEL, -1, 0);
		SendDlgItemMessage(hwnd,WMP_LICENSE,
			EM_SETREADONLY, 1, 0);

		return(TRUE);

	case WM_COMMAND:

		SendDlgItemMessage(hwnd,WMP_LICENSE,
			EM_SETSEL, -1, 0);

		switch(LOWORD(wparam))
		{
		case WMP_OK:

			EndDialog(hwnd,0);

			return(TRUE);
		}
	}

	return(FALSE);
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
	_mpv_list_title=title;
	_mpv_list_text=txt;

	if(txt->lasty==0)
	{
		/* no lines or just one line: exit */
		return(0);
	}

	return(DialogBox(hinst,"DLGLIST",NULL,ListDlgProc));
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
	OPENFILENAME ofn;

	if(def==NULL)
		_mpv_readline_buf[0]='\0';
	else
		strncpy(_mpv_readline_buf,def,sizeof(_mpv_readline_buf));

	if(type==MPR_OPEN || type==MPR_SAVE)
	{
		_mpv_readline_buf[0]='\0';

		memset(&ofn,'\0',sizeof(OPENFILENAME));
		ofn.lStructSize=sizeof(OPENFILENAME);
		ofn.hwndOwner=hwnd;
		ofn.lpstrFilter="*.*\0*.*\0";
		ofn.nFilterIndex=1;
		ofn.lpstrFile=_mpv_readline_buf;
		ofn.nMaxFile=sizeof(_mpv_readline_buf);
		ofn.lpstrTitle=prompt;
		ofn.lpstrDefExt=(def==NULL ? "" : def);

		if(type==MPR_OPEN)
		{
			ofn.Flags=OFN_PATHMUSTEXIST|OFN_HIDEREADONLY|
				OFN_NOCHANGEDIR|OFN_FILEMUSTEXIST;

			if(GetOpenFileName(&ofn))
				return(_mpv_readline_buf);
		}
		else
		{
			ofn.Flags=OFN_HIDEREADONLY;

			if(GetSaveFileName(&ofn))
				return(_mpv_readline_buf);
		}
	}
	else
	if(type!=MPR_EXEC)
	{
		_mpv_dlg_prompt=prompt;
		_mpv_dlg_default=def;

		if(DialogBox(hinst,"DLG1STRING",NULL,TextDlgProc))
			return(_mpv_readline_buf);
	}

	return(NULL);
}


/**
 * mpv_about - Shows the 'About Minimum Profit...' information.
 *
 * Shows a text or dialog box showing the information
 * about the program, version and such.
 */
void mpv_about(void)
{
	DialogBox(hinst,"ABOUTBOX",NULL,AboutDlgProc);
}


/**
 * mpv_help - Shows the available help for a term
 * @term: the term
 * @synhi: the syntax highlighter
 *
 * Shows the available help for the term. The argument
 * filetype is a string describing the file type,
 * taken from the syntax hilighter (this allows to
 * retrieve the help from different sources for C
 * or Perl code, for example).
 * Returns 0 on error or if no help is available
 * for this term.
 */
int mpv_help(char * term, int synhi)
{
	WinHelp(hwnd,_mpv_win32help,
		HELP_KEY,(DWORD) term);

	return(0);
}


/**
 * _mpv_init_fonts - Starts up font stuff.
 * @hdc: the device context
 *
 * Starts up font stuff.
 */
static void _mpv_init_fonts(HDC hdc)
{
	TEXTMETRIC tm;
	RECT rect;
	int n;
	static int f=1;

	/* create fonts */
	n=-MulDiv(_mpv_font_size,GetDeviceCaps(hdc, LOGPIXELSY),72);

	_font_normal=CreateFont(n,0,0,0,0,0,0,
		0,0,0,0,0,0,_mpv_font_face);

	_font_italic=CreateFont(n,0,0,0,0,1,0,
		0,0,0,0,0,0,_mpv_font_face);

	_font_underline=CreateFont(n,0,0,0,0,0,1,
		0,0,0,0,0,0,_mpv_font_face);

	SelectObject(hdc, _font_normal);

	GetTextMetrics(hdc, &tm);

	/* store sizes */
	_mpv_font_height=tm.tmHeight;
	_mpv_font_width=tm.tmAveCharWidth;

	GetClientRect(hwnd, &rect);

	/* calculate the size in chars */
	_mpv_x_size=((rect.right-rect.left)/_mpv_font_width)+1;
	_mpv_y_size=((rect.bottom-rect.top-_tab_height)/_mpv_font_height)+1;

	/* rebuild framebuffer */
	if(_mpv_fb != NULL) free(_mpv_fb);
	_mpv_fb=(int *) malloc(_mpv_x_size * _mpv_y_size * sizeof(int));

	if(f)
	{
		f=0;
		GetWindowRect(hwnd,&rect);
		SetCursorPos(rect.left+200,rect.top+6);
	}
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
	HDC hdc;

	hdc=GetDC(hwnd);

	SelectObject(hdc,GetStockObject(SYSTEM_FONT));

	DeleteObject(_font_normal);
	DeleteObject(_font_italic);
	DeleteObject(_font_underline);

	if(inc>0)
	{
		if(_mpv_font_size < 76)
			_mpv_font_size+=2;
	}
	else
	{
		if(_mpv_font_size > 6)
			_mpv_font_size-=2;
	}

	_mpv_init_fonts(hdc);

	mpi_draw_all(_mp_active);

	ReleaseDC(hwnd,hdc);

	return(0);
}


/**
 * _mpv_paint - Dump the frame buffer to screen.
 * @hwnd: the window handler
 *
 * Dumps the document window frame buffer to the window.
 */
static void _mpv_paint(HWND hwnd)
{
	HDC hdc;
	PAINTSTRUCT ps;
	RECT rect;
	RECT r2;
	int n,m,i;
	int c, color;
	int * fb;

	GetClientRect(hwnd, &rect);
	r2=rect;

	hdc=BeginPaint(hwnd, &ps);

	if(_font_normal==NULL)
		_mpv_init_fonts(hdc);

	SelectObject(hdc, _font_normal);

	SetTextColor(hdc, _inks[MP_COLOR_SELECTED]);
	SetBkColor(hdc, _papers[MP_COLOR_SELECTED]);

	r2.top+=_tab_height;
	r2.bottom=r2.top + _mpv_font_height;

	for(n=0;n < _mpv_y_size;n++)
	{
		r2.left=r2.right=rect.left;

		fb=&_mpv_fb[(n * _mpv_x_size)];

		for(m=0;m < _mpv_x_size;)
		{
			/* get first color */
			color=*fb & 0xff00;

			/* writes into _mpv_buffer while
			   color is the same */
			for(i=0;m<_mpv_x_size &&
				color==(*fb & 0xff00);
				i++,m++,fb++)
			{
				c=*fb & 0xff;
				_mpv_buffer[i]=c;
				r2.right+=_mpv_font_width;
			}

			_mpv_buffer[i]='\0';

			color>>=8;
			SetTextColor(hdc,_inks[color]);
			SetBkColor(hdc,_papers[color]);

			SelectObject(hdc, color==MP_COLOR_COMMENT ?
				_font_italic :
				color==MP_COLOR_LOCAL ? _font_underline :
				_font_normal);

			DrawText(hdc,_mpv_buffer,-1,&r2,DT_SINGLELINE|DT_NOPREFIX);

			r2.left=r2.right;
		}

		r2.top+=_mpv_font_height;
		r2.bottom+=_mpv_font_height;
	}

	EndPaint(hwnd, &ps);
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
	HGLOBAL hclp;
	char * ptr;

	OpenClipboard(NULL);
	hclp=GetClipboardData(CF_TEXT);
	CloseClipboard();

	if(hclp)
	{
		if((ptr=(char *)GlobalLock(hclp))!=NULL)
		{
			mp_lock_clipboard(1);

			/* transfer */
			while(*ptr)
			{
				if(*ptr!='\r')
					mp_insert_char(_mp_clipboard, *ptr);

				ptr++;
			}

			mp_lock_clipboard(0);

			GlobalUnlock(hclp);
		}
	}

	return(1);
}


/**
 * mpv_clipboard_to_system - Copies to the system clipboard
 *
 * Copies the clipboard's content from Minimum Profit to
 * the underlying system's one.
 */
void mpv_clipboard_to_system(void)
{
	int n,c;
	HGLOBAL hclp;
	char * ptr;

	if(_mp_clipboard==NULL) return;

	/* traverses clipboard counting chars */
	mp_move_bof(_mp_clipboard);
	for(n=0;(c=mp_get_char(_mp_clipboard))!='\0';n++)
	{
		if(c=='\n') n++;
	}

	mp_move_bof(_mp_clipboard);

	/* alloc and transfer */
	hclp=GlobalAlloc(GHND, n+1);
	ptr=(char *)GlobalLock(hclp);

	while(n > 0)
	{
		c=mp_get_char(_mp_clipboard);

		if(c=='\n')
		{
			*ptr++='\r';
			n--;
		}

		*ptr++=c;
		n--;
	}

	*ptr='\0';

	GlobalUnlock(hclp);

	OpenClipboard(NULL);
	EmptyClipboard();
	SetClipboardData(CF_TEXT, hclp);
	CloseClipboard();
}


/**
 * mpv_vkey - Converts Windows keys to key names.
 * @c: the key
 *
 * Converts Windows virtual keys to key names.
 */
static void _mpv_vkey(int c)
{
	char * ptr=NULL;
	static int _maximized=0;

	mpi_move_selecting=(GetKeyState(VK_SHIFT) & 0x8000);

	if(GetKeyState(VK_CONTROL) & 0x8000 ||
	   GetKeyState(VK_MENU) & 0x8000)
	{
		switch(c)
		{
		case VK_UP:	ptr="ctrl-cursor-up"; break;
		case VK_DOWN:	ptr="ctrl-cursor-down"; break;
		case VK_LEFT:	ptr="ctrl-cursor-left"; break;
		case VK_RIGHT:	ptr="ctrl-cursor-right"; break;
		case VK_PRIOR:	ptr="ctrl-page-up"; break;
		case VK_NEXT:	ptr="ctrl-page-down"; break;
		case VK_HOME:	ptr="ctrl-home"; break;
		case VK_END:	ptr="ctrl-end"; break;

		case VK_DIVIDE: ptr="ctrl-kp-divide"; break;
		case VK_MULTIPLY: ptr="ctrl-kp-multiply"; break;
		case VK_SUBTRACT: ptr="ctrl-kp-minus"; break;
		case VK_ADD:	ptr="ctrl-kp-plus"; break;
		case VK_RETURN: ptr="ctrl-enter"; break;

		case VK_F1:	ptr="ctrl-f1"; break;
		case VK_F2:	ptr="ctrl-f2"; break;
		case VK_F3:	ptr="ctrl-f3"; break;
		case VK_F4:	ptr="ctrl-f4"; break;
		case VK_F5:	ptr="ctrl-f5"; break;
		case VK_F6:	ptr="ctrl-f6"; break;
		case VK_F7:	ptr="ctrl-f7"; break;
		case VK_F8:	ptr="ctrl-f8"; break;
		case VK_F9:	ptr="ctrl-f9"; break;
		case VK_F10:	ptr="ctrl-f10"; break;
		case VK_F11:	ptr="ctrl-f11"; break;
		case VK_F12:
			SendMessage(hwnd, WM_SYSCOMMAND,
			_maximized ? SC_RESTORE : SC_MAXIMIZE, 0);

			_maximized^=1;

			break;
		}
	}
	else
	{
		switch(c)
		{
		case VK_UP:	ptr="cursor-up"; break;
		case VK_DOWN:	ptr="cursor-down"; break;
		case VK_LEFT:	ptr="cursor-left"; break;
		case VK_RIGHT:	ptr="cursor-right"; break;
		case VK_PRIOR:	ptr="page-up"; break;
		case VK_NEXT:	ptr="page-down"; break;
		case VK_HOME:	ptr="home"; break;
		case VK_END:	ptr="end"; break;

		case VK_TAB:	ptr="tab"; break;
		case VK_RETURN: ptr="enter"; break;
		case VK_BACK:	ptr="backspace"; break;
		case VK_DELETE: ptr="delete"; break;
		case VK_INSERT: ptr="insert"; break;

		case VK_DIVIDE: ptr="kp-divide"; break;
		case VK_MULTIPLY: ptr="kp-multiply"; break;
		case VK_SUBTRACT: ptr="kp-minus"; break;
		case VK_ADD:	ptr="kp-plus"; break;

		case VK_F1:	ptr="f1"; break;
		case VK_F2:	ptr="f2"; break;
		case VK_F3:	ptr="f3"; break;
		case VK_F4:	ptr="f4"; break;
		case VK_F5:	ptr="f5"; break;
		case VK_F6:	ptr="f6"; break;
		case VK_F7:	ptr="f7"; break;
		case VK_F8:	ptr="f8"; break;
		case VK_F9:	ptr="f9"; break;
		case VK_F10:	ptr="f10"; break;
		case VK_F11:	ptr="f11"; break;
		case VK_F12:	ptr="f12"; break;
		}
	}

	if(ptr != NULL)
		mpi_process('\0', ptr, NULL);
}


#define ctrl(ac) ((ac)&31)

/**
 * mpv_akey - Converts ascii keys to key names.
 * @k: the key
 *
 * Converts ascii keys to key names.
 */
static void _mpv_akey(int k)
{
	int c='\0';
	char * ptr=NULL;

	switch(k)
	{
	case ctrl(' '): ptr="ctrl-space"; break;
	case ctrl('a'): ptr="ctrl-a"; break;
	case ctrl('b'): ptr="ctrl-b"; break;
	case ctrl('c'): ptr="ctrl-c"; break;
	case ctrl('d'): ptr="ctrl-d"; break;
	case ctrl('e'): ptr="ctrl-e"; break;
	case ctrl('f'): ptr="ctrl-f"; break;
	case ctrl('g'): ptr="ctrl-g"; break;
	case ctrl('j'): ptr="ctrl-j"; break;
	case ctrl('l'): ptr="ctrl-l"; break;
	case ctrl('n'): ptr="ctrl-n"; break;
	case ctrl('o'): ptr="ctrl-o"; break;
	case ctrl('p'): ptr="ctrl-p"; break;
	case ctrl('q'): ptr="ctrl-q"; break;
	case ctrl('r'): ptr="ctrl-r"; break;
	case ctrl('s'): ptr="ctrl-s"; break;
	case ctrl('t'): ptr="ctrl-t"; break;
	case ctrl('u'): ptr="ctrl-u"; break;
	case ctrl('v'): ptr="ctrl-v"; break;
	case ctrl('w'): ptr="ctrl-w"; break;
	case ctrl('x'): ptr="ctrl-x"; break;
	case ctrl('y'): ptr="ctrl-y"; break;
	case ctrl('z'): ptr="ctrl-z"; break;
	default: if(k >=32 && k <=255) c=k; break;
	}

	if(c!='\0' || ptr!=NULL)
		mpi_process(c, ptr, NULL);
}


static void _mpv_amenu(int item)
{
	MENUITEMINFO mi;

	memset(&mi,'\0',sizeof(mi));
	mi.cbSize=sizeof(mi);
	mi.fMask=MIIM_DATA;

	if(GetMenuItemInfo(_menu,item,FALSE,&mi))
	{
		if(mi.dwItemData != 0)
			mpi_process('\0', NULL, (char *)mi.dwItemData);
	}
	else
	{
		char tmp[100];
		sprintf(tmp,"%d,%d",item,GetLastError());
		MessageBox(NULL,tmp,tmp,MB_OK);
	}
}


#ifndef STDCALL
#define STDCALL __stdcall
#endif

#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL			0x020A
#endif


/**
 * WndProc - Main window callback.
 * @hwnd: the window handler
 * @msg: the message
 * @wparam: the word param
 * @lparam: the long word param
 *
 * Main window callback.
 */
long STDCALL WndProc(HWND hwnd, UINT msg, UINT wparam, LONG lparam)
{
	int x,y;
	LPNMHDR p;
	mp_txt * t;
	char * ptr=NULL;

	switch(msg)
	{
	case WM_KEYDOWN:

		_mpv_vkey(wparam);

		return(0);

	case WM_CHAR:

		_mpv_akey(wparam);

		return(0);

	case WM_VSCROLL:

		switch(LOWORD(wparam))
		{
		case SB_PAGEUP: ptr="move-page-up"; break;
		case SB_PAGEDOWN: ptr="move-page-down"; break;
		case SB_LINEUP: ptr="move-up"; break;
		case SB_LINEDOWN: ptr="move-down"; break;
		}

		if(ptr != NULL)
			mpi_process('\0', NULL, ptr);

		return(0);

	case WM_PAINT:
		_mpv_paint(hwnd);
		return(0);

	case WM_SIZE:

		if(_mpv_font_width && _mpv_font_height)
		{
			_mpv_x_size=(LOWORD(lparam)/_mpv_font_width)+1;
			_mpv_y_size=((HIWORD(lparam)-_tab_height)/_mpv_font_height)+1;

			/* rebuild framebuffer */
			if(_mpv_fb != NULL) free(_mpv_fb);
			_mpv_fb=(int *) malloc(_mpv_x_size * _mpv_y_size * sizeof(int));

			mpi_draw_all(_mp_active);
		}

		MoveWindow(hwtabs,0,0,LOWORD(lparam),_tab_height,TRUE);

		return(0);

	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:

		x=LOWORD(lparam);
		y=HIWORD(lparam) - _tab_height;

		x/=_mpv_font_width;
		y/=_mpv_font_height;

		mp_move_xy(_mp_active,x,y+_mp_active->vy);
		mp_move_bol(_mp_active);
		mp_move_to_visual_column(_mp_active,x);

		mpi_draw_all(_mp_active);

		switch(msg)
		{
		case WM_LBUTTONDOWN: ptr="mouse-left-button"; break;
		case WM_RBUTTONDOWN: ptr="mouse-right-button"; break;
		case WM_MBUTTONDOWN: ptr="mouse-middle-button"; break;
		}

		if(ptr != NULL)
			mpi_process('\0', ptr, NULL);

		return(0);

	case WM_MOUSEWHEEL:

		if((int)wparam > 0)
			ptr="mouse-wheel-up";
		else
			ptr="mouse-wheel-down";

		if(ptr != NULL)
			mpi_process('\0', ptr, NULL);

		return(0);

	case WM_COMMAND:

		_mpv_amenu(LOWORD(wparam));

		return(0);

	case WM_CLOSE:
		mpi_shutdown();

		DestroyWindow(hwnd);
		return(0);

	case WM_DESTROY:
		PostQuitMessage(0);
		return(0);

	case WM_NOTIFY:
		p=(LPNMHDR)lparam;

		if(p->code==TCN_SELCHANGE)
		{
			y=TabCtrl_GetCurSel(hwtabs);

			for(t=_mp_txts,x=0;t!=NULL;t=t->next,x++)
			{
				if(x==y)
				{
					_mp_active=t;
					break;
				}
			}

			mpi_draw_all(_mp_active);
		}

		return(0);
	}

	if(_mpi_exit_requested)
	{
		PostMessage(hwnd,WM_CLOSE,0,0);
		_mpi_exit_requested=0;
	}

	return(DefWindowProc(hwnd,msg,wparam,lparam));
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
	SCROLLINFO si;

	si.cbSize=sizeof(si);
	si.fMask=SIF_ALL;
	si.nMin=1;
	si.nMax=max;
	si.nPage=size;
	si.nPos=pos;

	SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
}


/**
 * mpv_filetabs - Draws the tab set containing the file names
 *
 * Draws the names of the opened files in the form of a tab set.
 */
void mpv_filetabs(void)
{
	TC_ITEM ti;
	mp_txt * t;
	int n;
	char * ptr;

	TabCtrl_DeleteAllItems(hwtabs);

	for(t=_mp_txts,n=0;t!=NULL;t=t->next,n++)
	{
		ti.mask=TCIF_TEXT;

		if((ptr=strrchr(t->name,'\\'))==NULL)
			ptr=t->name;
		else
			ptr++;

		ti.pszText=ptr;

		TabCtrl_InsertItem(hwtabs,n,&ti);

		if(_mp_active==t)
			TabCtrl_SetCurSel(hwtabs,n);
	}
}


static void set_system_language(void)
{
	short s;
	char * ptr;

	s=GetSystemDefaultLangID() & 0x00ff;

	/* try only some 'famous' languages:
	   the complete world language database should
	   be implemented */
	switch(s)
	{
	case 0x01: ptr="ar"; break; /* arabic */
	case 0x02: ptr="bg"; break; /* bulgarian */
	case 0x03: ptr="ca"; break; /* catalan */
	case 0x04: ptr="zh"; break; /* chinese */
	case 0x05: ptr="cz"; break; /* czech */
	case 0x06: ptr="da"; break; /* danish */
	case 0x07: ptr="de"; break; /* german */
	case 0x08: ptr="el"; break; /* greek */
	case 0x09: ptr="en"; break; /* english */
	case 0x0a: ptr="es"; break; /* spanish */
	case 0x0b: ptr="fi"; break; /* finnish */
	case 0x0c: ptr="fr"; break; /* french */
	case 0x0d: ptr="he"; break; /* hebrew */
	case 0x0e: ptr="hu"; break; /* hungarian */
	case 0x0f: ptr="is"; break; /* icelandic */
	case 0x10: ptr="it"; break; /* italian */
	case 0x11: ptr="jp"; break; /* japanese */
	case 0x13: ptr="nl"; break; /* dutch */
	case 0x14: ptr="no"; break; /* norwegian */
	case 0x15: ptr="po"; break; /* polish */
	case 0x16: ptr="pt"; break; /* portuguese */
	case 0x1d: ptr="se"; break; /* swedish */
	default: ptr="en"; break;
	}

	mpl_set_language(ptr);
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

		_inks[n]=((ink & 0x000000ff) << 16)|
			 ((ink & 0x0000ff00)) |
			 ((ink & 0x00ff0000) >> 16);
		_papers[n]=((paper & 0x000000ff) << 16)|
			 ((paper & 0x0000ff00)) |
			 ((paper & 0x00ff0000) >> 16);;
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
	WNDCLASS wc;
	RECT r;

	mp_log("Using Win32 driver.\n");

	InitCommonControls();

	/* register the window */
	wc.style=CS_HREDRAW|CS_VREDRAW;
	wc.lpfnWndProc=WndProc;
	wc.cbClsExtra=0;
	wc.cbWndExtra=0;
	wc.hInstance=hinst;
	wc.hIcon=LoadIcon(hinst,"MP_ICON");
	wc.hCursor=LoadCursor(NULL,IDC_ARROW);
	wc.hbrBackground=NULL;
	wc.lpszMenuName=NULL;
	wc.lpszClassName="minimumprofit3.x";

	if(!RegisterClass(&wc))
		return(0);

	/* create the window */
	hwnd=CreateWindow("minimumprofit3.x","mp " VERSION,
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_VSCROLL,
		CW_USEDEFAULT,CW_USEDEFAULT,
		CW_USEDEFAULT,CW_USEDEFAULT,
		NULL,_menu,hinst,NULL);

	if(hwnd==NULL)
		return(0);

	mpv_add_menu("");

	DrawMenuBar(hwnd);

	ShowWindow(hwnd,SW_SHOW);
	UpdateWindow(hwnd);

	GetClientRect(hwnd,&r);

	hwtabs=CreateWindow(WC_TABCONTROL, "tab",
		WS_CHILD | TCS_TABS | TCS_SINGLELINE | TCS_FOCUSNEVER,
		0, 0, r.right-r.left, _tab_height, hwnd, NULL, hinst, NULL);

	SendMessage(hwtabs, WM_SETFONT, 
		(WPARAM) GetStockObject(ANSI_VAR_FONT), 0);

	ShowWindow(hwtabs,SW_SHOW);
	UpdateWindow(hwtabs);

	mpv_create_colors();

	return(1);
}


/**
 * mpv_mainloop - Main message processing loop.
 *
 * Main message processing loop.
 */
void mpv_mainloop(void)
{
	MSG msg;

	while(GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}


/**
 * mpv_shutdown - Shuts down the system dependent driver.
 *
 * Shuts down the system dependent driver.
 */
void mpv_shutdown(void)
{
	SendMessage(hwnd,WM_CLOSE,0,0);
}


static void cmd_2_argv(char * cmd)
{
	char d;
	unsigned long int n;
	HKEY hkey;

	_argc_=1;
	d=' ';
	while(*cmd)
	{
		while(*cmd==d || *cmd==' ')
		{
			*cmd='\0';
			cmd++;
		}

		if(*cmd=='"' || *cmd=='\'')
		{
			d=*cmd;
			cmd++;
		}
		else
			d=' ';

		_argv_[_argc_]=cmd;
		_argc_++;

		while(*cmd && *cmd!=' ')
			cmd++;
	}

	/* extract the home directory from the registry */
	if(RegOpenKeyEx(HKEY_CURRENT_USER,
		"Software\\Minimum Profit",0,
		KEY_QUERY_VALUE, &hkey)==ERROR_SUCCESS)
	{
		n=sizeof(_mpc_home);

		RegQueryValueEx(hkey,"Home",NULL,NULL,_mpc_home,&n);
	}
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
	if(strcmp(var,"win32_font_face")==0)
		strncpy(_mpv_font_face, value, sizeof(_mpv_font_face));
	else
	if(strcmp(var,"win32_font_size")==0)
		_mpv_font_size=atoi(value);
	else
	if(strcmp(var,"win32_help_file")==0)
		strncpy(_mpv_win32help, value, sizeof(_mpv_win32help));
}


static void _mpv_usage(void)
{
	char msg[2048];

	strncpy(msg,L(MSG_USAGE_TEXT),sizeof(msg));
	strcat(msg,"\n\n--lang: ");
	strcat(msg,mpl_enumerate_langs());
	strcat(msg,"\n--mode: ");
	strcat(msg,mps_enumerate_modes());

	MessageBox(NULL,msg,"Minimum Profit" VERSION,MB_OK);
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
	return(0);
}


extern int _argc;
extern char ** _argv;

int STDCALL WinMain(HINSTANCE hi, HINSTANCE hpi, LPSTR cmd, int cm)
{
	int r;

	hinst=hi;

	mp_startup();

	cmd_2_argv(cmd);
	set_system_language();

	mpc_startup();

	r=mpi_args_1(_argc_, _argv_);

	if(r < 0) { _mpv_usage(); exit(0); }

	mps_startup();
	mpi_startup();
	mpv_startup();

	r=mpi_args_2(_argc_, _argv_);

	if(r==-1)
	{
		mpv_alert(mps_enumerate_modes(),NULL);
		mpv_shutdown();
		exit(1);
	}

	/* force the reading of the tags file, if one exists */
	mpi_seek_tag(NULL,0);

	if(_mp_active==NULL)
	{
		mp_create_txt(L(MSG_UNNAMED));
		mps_auto_synhi(_mp_active);
	}

	mpv_title(_mp_active->name);
	mpi_draw_all(_mp_active);
	mpv_filetabs();

	mpv_mainloop();

	mpv_shutdown();
	mp_shutdown();

	return(0);
}

