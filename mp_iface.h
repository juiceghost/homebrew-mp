/*

    mp_iface.h

    Interface.

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


extern int _mpi_shift;
extern int _mpi_reposition_cursor;
extern int _mpi_macro_index;
extern int _mpi_instant_x;
extern int _mpi_instant_y;
extern int _mpi_mark_column_80;
extern int _mpi_exit_requested;
extern char _mpi_template_file[1024];
extern int _mpi_insert;
extern int _mpi_preread_lines;
extern char _mpi_ctags_cmd[128];
extern char _mpi_status_line_f[128];
extern char _mpi_strftime_f[128];

extern int mpi_mouse;
extern int mpi_monochrome;
extern int mpi_transp_mode;
extern int mpi_move_selecting;

extern int _tmp_word_wrap;
extern int _tmp_auto_indent;

#define MP_SAVE_STATE() _tmp_word_wrap=_mp_word_wrap; \
		     _tmp_auto_indent=_mp_auto_indent; \
		     _mp_word_wrap=_mp_auto_indent=0;

#define MP_RESTORE_STATE() _mp_word_wrap=_tmp_word_wrap; \
			_mp_auto_indent=_tmp_auto_indent;

extern char * MP_LICENSE;

void mpi_draw_all(mp_txt * txt);
int mpi_move_page_up(mp_txt * txt);
int mpi_move_page_down(mp_txt * txt);
int mpi_goto(mp_txt * txt);

int mpi_new(void);
void mpi_open(char * name, int reopen);
int mpi_save_as(mp_txt * txt);
int mpi_save(mp_txt * txt);
void mpi_sync(void);
int mpi_close(mp_txt * txt);

int mpi_history_size(int mode);
int mpi_history_get(int mode, int index, char * buf, int size);
void mpi_history_add(int mode, char * str);

void mpi_exec(mp_txt * txt);
int mpi_seek(mp_txt * txt);
int mpi_seek_next(mp_txt * txt);
int mpi_replace(mp_txt * txt);
int mpi_replace_all(void);
int mpi_grep(void);

int mpi_help(mp_txt * txt);
int mpi_seek_tag(char * tag, int select_only);
int mpi_find_tag(mp_txt * txt);
int mpi_insert_template(void);

int mpi_set_word_wrap(void);
int mpi_set_tab_size(void);
int mpi_completion(mp_txt * txt);
void mpi_current_list(void);

void mpi_record_macro(void);
void mpi_play_macro(void);
int mpi_exec_function(void);

int mpi_process(int c, char * keyname, char * funcname);

int mpi_args_1(int argc, char * argv[]);
int mpi_args_2(int argc, char * argv[]);

void mpi_startup(void);
void mpi_shutdown(void);
