/*

    mp_func.c

    Functions & Keys

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

#include "mp_core.h"
#include "mp_video.h"
#include "mp_func.h"
#include "mp_iface.h"
#include "mp_synhi.h"
#include "mp_lang.h"
#include "mp_conf.h"

/*******************
	Data
********************/

/*******************
	Code
********************/

/* mpf_functions */

int mpf_move_up(void) { return(mp_move_up(_mp_active)); }
int mpf_move_down(void) { return(mp_move_down(_mp_active)); }
int mpf_move_left(void) { return(mp_move_left(_mp_active)); }
int mpf_move_right(void) { return(mp_move_right(_mp_active)); }
int mpf_move_word_left(void) { return(mp_move_word_left(_mp_active)); }
int mpf_move_word_right(void) { return(mp_move_word_right(_mp_active)); }
int mpf_move_eol(void) { return(mp_move_eol(_mp_active)); }
int mpf_move_bol(void) { return(mp_move_bol(_mp_active)); }
int mpf_move_eof(void) { return(mp_move_eof(_mp_active)); }
int mpf_move_bof(void) { return(mp_move_bof(_mp_active)); }
int mpf_move_page_up(void) { return(mpi_move_page_up(_mp_active)); }
int mpf_move_page_down(void) { return(mpi_move_page_down(_mp_active)); }
int mpf_goto(void) { return(mpi_goto(_mp_active)); }


int mpf_insert_line(void) { return(mp_insert_line(_mp_active)); }
int mpf_insert_tab(void) { return(mp_insert_tab(_mp_active)); }

int mpf_delete(void)
{
	if(mp_marked(_mp_active))
		return(mp_delete_mark(_mp_active));
	return(mp_delete_char(_mp_active));
}

int mpf_delete_left(void)
{
	if(mp_move_left(_mp_active))
		return(mp_delete_char(_mp_active));
	return(0);
}

int mpf_delete_line(void) { return(mp_delete_line(_mp_active)); }


int mpf_mark(void) { mp_mark(_mp_active); return(1); }
int mpf_unmark(void) { mp_unmark(_mp_active); return(1); }
int mpf_paste_mark(void)
{
	if(! mpv_system_to_clipboard())
		return(0);

	return(mp_paste_mark(_mp_active));
}

int mpf_copy_mark(void)
{
	int ret=mp_copy_mark(_mp_active);
	mp_unmark(_mp_active);

	mpv_clipboard_to_system();
	return(ret);
}

int mpf_cut_mark(void)
{
	int ret=0;

	if(mp_copy_mark(_mp_active))
	{
		mpv_clipboard_to_system();
		ret=mp_delete_mark(_mp_active);
		mp_unmark(_mp_active);
	}

	return(ret);
}


int mpf_seek(void) { return(mpi_seek(_mp_active)); }
int mpf_seek_next(void) { return(mpi_seek_next(_mp_active)); }
int mpf_mark_match(void) { mp_mark_match(_mp_active); return(2); }
int mpf_replace(void) { return(mpi_replace(_mp_active)); }
int mpf_replace_all(void) { return(mpi_replace_all()); }
int mpf_grep(void) { return(mpi_grep()); }

int mpf_next(void)
{
	if((_mp_active=_mp_active->next)==NULL)
		_mp_active=_mp_txts;
	return(2);
}

int mpf_new(void) { return(mpi_new()); }
int mpf_open(void) { mpi_open(NULL,0); return(2); }
int mpf_reopen(void) { mpi_open(NULL,1); return(2); }
int mpf_save_as(void) { return(mpi_save_as(_mp_active)); }
int mpf_save(void) { return(mpi_save(_mp_active)); }
int mpf_sync(void) { mpi_sync(); return(2); }
int mpf_close(void) { return(mpi_close(_mp_active)); }

int mpf_open_under_cursor(void)
{
	char tmp[1024];

	mp_get_word(_mp_active,tmp,sizeof(tmp));
	mpi_open(tmp,0);
	return(2);
}


int mpf_zoom_in(void) { mpv_zoom(1); return(2); }
int mpf_zoom_out(void) { mpv_zoom(-1); return(2); }


int mpf_toggle_insert(void) { _mpi_insert=!_mpi_insert; return(0); }
int mpf_toggle_case(void) { _mp_case_cmp ^= 1; return(0); }
int mpf_toggle_save_tabs(void) { _mp_save_tabs ^= 1; return(0); }
int mpf_toggle_cr_lf(void) { _mp_cr_lf ^= 1; return(0); }
int mpf_toggle_auto_indent(void) { _mp_auto_indent ^= 1; return(0); }
int mpf_toggle_column_80(void) { _mpi_mark_column_80 ^= 1; return(1); }
int mpf_toggle_regex(void) { _mp_regex ^= 1; return(0); }


int mpf_help(void) { return(mpi_help(_mp_active)); }
int mpf_exec_command(void) { mpi_exec(_mp_active); return(1); }
int mpf_document_list(void) { mpi_current_list(); return(2); }
int mpf_find_tag(void) { return(mpi_find_tag(_mp_active)); }
int mpf_insert_template(void) { return(mpi_insert_template()); }
int mpf_completion(void) { return(mpi_completion(_mp_active)); }


int mpf_edit_templates_file(void)
{
	mp_create_txt(_mpi_template_file);

	if(mp_load_file(_mp_active,_mpi_template_file)!=-1)
		mps_auto_synhi(_mp_active);
	else
		mp_put_str(_mp_active,L(MSG_EMPTY_TEMPLATE),1);

	_mp_active->mod=0;

	return(2);
}

int mpf_edit_config_file(void)
{
	mp_create_txt(_mpc_config_file);

	if(mp_load_file(_mp_active,_mpc_config_file)!=-1)
		mps_auto_synhi(_mp_active);
	else
		mp_put_str(_mp_active,L(MSG_EMPTY_CONFIG_FILE),1);

	_mp_active->mod=0;

	return(2);
}


int mpf_set_word_wrap(void) { return(mpi_set_word_wrap()); }
int mpf_set_tab_size(void) { return(mpi_set_tab_size()); }

int mpf_play_macro(void) { mpi_play_macro(); return(2); }
int mpf_record_macro(void) { mpi_record_macro(); return(2); }

int mpf_menu(void) { return(mpv_menu()); }

int mpf_about(void) { mpv_about(); return(2); }
int mpf_exit(void) { _mpi_exit_requested=1; return(2); }

int mpf_key_help(void);

int mpf_mouse_position(void) { return(2); }

int mpf_exec_function(void) { return(mpi_exec_function()); }

int mpf_show_clipboard(void) { mp_show_sys_txt(_mp_clipboard); return(2); }
int mpf_show_log(void) { mp_show_sys_txt(_mp_log); return(2); }

/* the functions */

struct _mpf_functions
{
	char * funcname;
	int (* func)(void);
};

struct _mpf_functions mpf_functions[]=
{
	{ "move-up",		 mpf_move_up },
	{ "move-down",		 mpf_move_down },
	{ "move-left",		 mpf_move_left },
	{ "move-right", 	 mpf_move_right },
	{ "move-word-left",	 mpf_move_word_left },
	{ "move-word-right",	 mpf_move_word_right },
	{ "move-eol",		 mpf_move_eol },
	{ "move-bol",		 mpf_move_bol },
	{ "move-eof",		 mpf_move_eof },
	{ "move-bof",		 mpf_move_bof },
	{ "move-page-up",	 mpf_move_page_up },
	{ "move-page-down",	 mpf_move_page_down },
	{ "goto",		 mpf_goto },
	{ "insert-line",	 mpf_insert_line },
	{ "insert-tab", 	 mpf_insert_tab },
	{ "delete",		 mpf_delete },
	{ "delete-left",	 mpf_delete_left },
	{ "delete-line",	 mpf_delete_line },
	{ "mark",		 mpf_mark },
	{ "unmark",		 mpf_unmark },
	{ "copy",		 mpf_copy_mark },
	{ "paste",		 mpf_paste_mark },
	{ "cut",		 mpf_cut_mark },
	{ "seek",		 mpf_seek },
	{ "seek-next",		 mpf_seek_next },
	{ "replace",		 mpf_replace },
	{ "replace-all",	 mpf_replace_all },
	{ "next",		 mpf_next },
	{ "new",		 mpf_new },
	{ "open",		 mpf_open },
	{ "reopen",		 mpf_reopen },
	{ "save",		 mpf_save },
	{ "save-as",		 mpf_save_as },
	{ "close",		 mpf_close },
	{ "open-under-cursor",	 mpf_open_under_cursor },
	{ "zoom-in",		 mpf_zoom_in },
	{ "zoom-out",		 mpf_zoom_out },
	{ "toggle-insert",	 mpf_toggle_insert },
	{ "toggle-case",	 mpf_toggle_case },
	{ "toggle-save-tabs",	 mpf_toggle_save_tabs },
	{ "toggle-cr-lf",	 mpf_toggle_cr_lf },
	{ "toggle-auto-indent",  mpf_toggle_auto_indent },
	{ "toggle-column-80",	 mpf_toggle_column_80 },
	{ "toggle-regex",		mpf_toggle_regex },
	{ "help",		 mpf_help },
	{ "exec-command",	 mpf_exec_command },
	{ "document-list",	 mpf_document_list },
	{ "find-tag",		 mpf_find_tag },
	{ "insert-template",	 mpf_insert_template },
	{ "completion", 	 mpf_completion },
	{ "edit-templates-file", mpf_edit_templates_file },
	{ "edit-config-file",	 mpf_edit_config_file },
	{ "set-word-wrap",	 mpf_set_word_wrap },
	{ "set-tab-size",	 mpf_set_tab_size },
	{ "menu",		 mpf_menu },
	{ "about",		 mpf_about },
	{ "exit",		 mpf_exit },
	{ "play-macro", 	 mpf_play_macro },
	{ "record-macro",	 mpf_record_macro },
	{ "key-help",		 mpf_key_help },
	{ "mouse-position",	 mpf_mouse_position },
	{ "exec-function",	 mpf_exec_function },
	{ "sync",		 mpf_sync },
	{ "grep",		 mpf_grep },
	{ "mark-match", 	 mpf_mark_match },
	{ "show-clipboard",	 mpf_show_clipboard },
	{ "show-log",		 mpf_show_log },
	{ NULL, NULL }
};

/* the keys */

struct _mpf_keys
{
	char * keyname;
	int (* func)(void);
};

struct _mpf_keys mpf_keys[]=
{
	{ "cursor-up",		mpf_move_up },
	{ "cursor-down",	mpf_move_down },
	{ "cursor-left",	mpf_move_left },
	{ "cursor-right",	mpf_move_right },
	{ "page-up",		mpf_move_page_up },
	{ "page-down",		mpf_move_page_down },
	{ "home",		mpf_move_bol },
	{ "end",		mpf_move_eol },
	{ "ctrl-cursor-up",	NULL },
	{ "ctrl-cursor-down",	NULL },
	{ "ctrl-cursor-left",	mpf_move_word_left },
	{ "ctrl-cursor-right",	mpf_move_word_right },
	{ "ctrl-page-up",	NULL },
	{ "ctrl-page-down",	NULL },
	{ "ctrl-home",		mpf_move_bof },
	{ "ctrl-end",		mpf_move_eof },
	{ "insert",		mpf_toggle_insert },
	{ "delete",		mpf_delete },
	{ "backspace",		mpf_delete_left },
	{ "escape",		NULL },
	{ "enter",		mpf_insert_line },
	{ "tab",		mpf_insert_tab },
	{ "kp-minus",		NULL },
	{ "kp-plus",		NULL },
	{ "kp-multiply",	NULL },
	{ "kp-divide",		NULL },
	{ "ctrl-kp-minus",	mpf_zoom_out },
	{ "ctrl-kp-plus",	mpf_zoom_in },
	{ "ctrl-kp-multiply",	NULL },
	{ "ctrl-kp-divide",	NULL },
	{ "f1", 		mpf_help },
	{ "f2", 		mpf_save },
	{ "f3", 		mpf_open },
	{ "f4", 		mpf_close },
	{ "f5", 		mpf_new },
	{ "f6", 		mpf_next },
	{ "f7", 		mpf_play_macro },
	{ "f8", 		mpf_unmark },
	{ "f9", 		mpf_mark },
	{ "f10",		mpf_record_macro },
	{ "f11",		mpf_zoom_out },
	{ "f12",		mpf_zoom_in },
	{ "ctrl-f1",		NULL },
	{ "ctrl-f2",		NULL },
	{ "ctrl-f3",		NULL },
	{ "ctrl-f4",		NULL },
	{ "ctrl-f5",		NULL },
	{ "ctrl-f6",		NULL },
	{ "ctrl-f7",		NULL },
	{ "ctrl-f8",		NULL },
	{ "ctrl-f9",		NULL },
	{ "ctrl-f10",		mpf_record_macro },
	{ "ctrl-f11",		NULL },
	{ "ctrl-f12",		NULL },
	{ "ctrl-enter", 	mpf_open_under_cursor },
	{ "ctrl-a",		mpf_menu },
	{ "ctrl-b",		mpf_seek },
	{ "ctrl-c",		mpf_copy_mark },
	{ "ctrl-d",		mpf_copy_mark },
	{ "ctrl-e",		mpf_move_bof },
	{ "ctrl-f",		mpf_find_tag },
	{ "ctrl-g",		mpf_goto },
	{ "ctrl-h",		mpf_delete_left },
	{ "ctrl-i",		mpf_insert_tab },
	{ "ctrl-j",		mpf_move_word_left },
	{ "ctrl-k",		mpf_move_word_right },
	{ "ctrl-l",		mpf_seek_next },
	{ "ctrl-m",		mpf_insert_line },
	{ "ctrl-n",		mpf_grep },
	{ "ctrl-o",		mpf_document_list },
	{ "ctrl-p",		mpf_paste_mark },
	{ "ctrl-q",		mpf_cut_mark },
	{ "ctrl-r",		mpf_replace },
	{ "ctrl-s",		mpf_completion },
	{ "ctrl-t",		mpf_cut_mark },
	{ "ctrl-u",		mpf_insert_template },
	{ "ctrl-v",		mpf_paste_mark },
	{ "ctrl-w",		mpf_move_eof },
	{ "ctrl-x",		mpf_exit },
	{ "ctrl-y",		mpf_delete_line },
	{ "ctrl-z",		NULL },
	{ "ctrl-space", 	mpf_menu },
	{ "mouse-left-button",	mpf_mouse_position },
	{ "mouse-right-button", mpf_mark },
	{ "mouse-middle-button",mpf_paste_mark },
	{ "mouse-wheel-up",	mpf_move_page_up },
	{ "mouse-wheel-down",	mpf_move_page_down },
	{ "mouse-wheel-left",	NULL },
	{ "mouse-wheel-right",	NULL },
	{ NULL, NULL }
};


/* toggle functions and its value */
struct
{
	char * funcname;
	int * toggle;
} _toggle_functions[]=
{
	{ "toggle-case", &_mp_case_cmp },
	{ "toggle-save-tabs", &_mp_save_tabs },
	{ "toggle-cr-lf", &_mp_cr_lf },
	{ "toggle-auto-indent", &_mp_auto_indent },
	{ "toggle-column-80", &_mpi_mark_column_80 },
	{ "toggle-regex", &_mp_regex },
	{ NULL, NULL }
};

	
/**
 * mpf_get_func_by_keyname - Gets a function by the key bound to it
 * @key_name: name of the key
 *
 * Returns a pointer to the function bound to the @key_name, or
 * NULL if no function is bound to that key name.
 */
mp_funcptr mpf_get_func_by_keyname(char * key_name)
{
	int n;

	if(key_name==NULL) return(NULL);

	for(n=0;mpf_keys[n].keyname!=NULL;n++)
	{
		if(strcmp(key_name, mpf_keys[n].keyname)==0)
			return(mpf_keys[n].func);
	}

	return(NULL);
}


/**
 * mpf_get_func_by_funcname - Gets a function by its name
 * @func_name: name of the function
 *
 * Returns a pointer to the function named @func_name,
 * or NULL if no function has that name.
 */
mp_funcptr mpf_get_func_by_funcname(char * func_name)
{
	int n;

	if(func_name==NULL) return(NULL);

	for(n=0;mpf_functions[n].funcname!=NULL;n++)
	{
		if(strcmp(func_name, mpf_functions[n].funcname)==0)
			return(mpf_functions[n].func);
	}

	return(NULL);
}


/**
 * mpf_get_keyname_by_funcname - Returns the key bound to a function
 * @func_name: name of the function
 *
 * Returns the name of the key bound to the function called
 * @func_name, or NULL of none is.
 */
char * mpf_get_keyname_by_funcname(char * func_name)
{
	int n;
	mp_funcptr func=NULL;

	if((func=mpf_get_func_by_funcname(func_name))==NULL)
		return(NULL);

	/* search now a key with that function */
	for(n=0;mpf_keys[n].keyname!=NULL;n++)
	{
		if(mpf_keys[n].func == func)
			return(mpf_keys[n].keyname);
	}

	return(NULL);
}


/**
 * mpf_get_funcname_by_keyname - Returns the function name bound to a key
 * @key_name: name of the key
 *
 * Returns the name of the function bound to the @key_name,
 * or NULL otherwise.
 */
char * mpf_get_funcname_by_keyname(char * key_name)
{
	int n;
	mp_funcptr func=NULL;

	if((func=mpf_get_func_by_keyname(key_name))==NULL)
		return(NULL);

	/* search now a funcname with that function */
	for(n=0;mpf_functions[n].funcname!=NULL;n++)
	{
		if(mpf_functions[n].func == func)
			return(mpf_functions[n].funcname);
	}

	return(NULL);
}


/**
 * mpf_bind_key - Binds a key to a function
 * @key_name: name of the key to be bound
 * @func_name: name of the function
 *
 * Binds a key to a function. @func_name is the name of the
 * function to be assigned, or <none> if the key is to be
 * unbounded. @key_name is the name of the key, or <all>
 * if the function is to be bound to ALL keys (mainly thought
 * to be used in combination with <none> to clean all key
 * bindings). Returns 0 if @key_name
 * or @func_name is not a defined one, or 1 if the key was bound.
 */
int mpf_bind_key(char * key_name, char * func_name)
{
	int n,c;
	mp_funcptr func;

	if(strcmp(func_name,"<none>")==0)
		func=NULL;
	else
	if((func=mpf_get_func_by_funcname(func_name))==NULL)
		return(0);

	/* search now a key with that keyname */
	for(n=c=0;mpf_keys[n].keyname!=NULL;n++)
	{
		if(strcmp(key_name, "<all>")==0 ||
			strcmp(mpf_keys[n].keyname, key_name)==0)
		{
			mpf_keys[n].func=func;
			c++;
		}
	}

	return(c);
}


/**
 * mpf_key_help - Shows the key binding help to the user
 *
 * Shows the key binding help to the user.
 */
int mpf_key_help(void)
{
	int n,m;
	mp_txt * txt;
	char * ptr;

	MP_SAVE_STATE();

	txt=mp_create_txt(L(MSG_KEYHELP));

	for(n=0;(ptr=mpf_keys[n].keyname)!=NULL;n++)
	{
		mp_put_str(txt,"  ",1);
		mp_put_str(txt,ptr,1);

		for(m=0;m < 22 - strlen(ptr);m++)
			mp_put_char(txt,' ',1);

		if((ptr=mpf_get_funcname_by_keyname(ptr))==NULL)
			ptr=L("<none>");

		mp_put_str(txt,ptr,1);

		for(m=0;m < 22 - strlen(ptr);m++)
			mp_put_char(txt,' ',1);

		ptr=L(ptr);
		mp_put_str(txt,ptr,1);

		mp_put_char(txt,'\n',1);
	}

	mp_put_char(txt,'\n',1);
	mp_put_str(txt,L("Unlinked functions"),1);
	mp_put_char(txt,'\n',1);
	mp_put_char(txt,'\n',1);

	for(n=0;(ptr=mpf_functions[n].funcname)!=NULL;n++)
	{
		if(mpf_get_keyname_by_funcname(ptr)!=NULL)
			continue;

		mp_put_str(txt,"  ",1);
		mp_put_str(txt,ptr,1);

		for(m=0;m < 22 - strlen(ptr);m++)
			mp_put_char(txt,' ',1);

		ptr=L(ptr);
		mp_put_str(txt,ptr,1);

		mp_put_char(txt,'\n',1);
	}

	mp_move_bof(txt);
	txt->type=MP_TYPE_READ_ONLY;
	txt->mod=0;

	MP_RESTORE_STATE();

	return(2);
}


/**
 * mpf_toggle_function_value - Returns the value of a toggle function
 * @func_name: name of the function
 *
 * Returns a pointer to an integer containing the boolean value
 * of a toggle function, or NULL if @func_name does not exist.
 */
int * mpf_toggle_function_value(char * func_name)
{
	int n;

	for(n=0;_toggle_functions[n].funcname!=NULL;n++)
	{
		if(strcmp(_toggle_functions[n].funcname,func_name)==0)
			return(_toggle_functions[n].toggle);
	}

	return(NULL);
}
