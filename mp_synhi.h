/*

    mp_synhi.h

    Syntax higlighters.

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

struct mps_wsynhi
{
	char * word;
	int color;
};

#define MAX_WORDS_PER_SYNHI	120

struct mps_synhi
{
	char * type;
	char * mode;
	char * q_start;
	char * q_end;
	char ** c_start;
	char ** c_end;
	char ** magic_1;
	char ** magic_2;
	char ** exts;
	char ** tokens;
	char ** vars;
	char ** helpers;
	char * seps;
	int casesig;
	int numbers;

	int wi;
	struct mps_wsynhi w[MAX_WORDS_PER_SYNHI];
};

extern struct mps_synhi _mps_synhi[];
extern int _draw_quoting;
extern int _in_comment;
extern int _override_synhi;

#define MAX_LOCAL_TOKENS	4096


int mps_is_sep(char c, int synhi);
void mps_auto_synhi(mp_txt * txt);
int mps_word_color(int synhi, char * word, int col, int line);
int mps_quoting(int c, int color, int synhi);
int mps_set_override_mode(char * mode);
void mps_add_local_token(char * token);
char * mps_enumerate_modes(void);
void mps_startup(void);
