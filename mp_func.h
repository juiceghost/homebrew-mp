/*

    mp_func.h

    Functions (bindable to keys)

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

typedef int (* mp_funcptr)(void);

mp_funcptr mpf_get_func_by_keyname(char * keyname);
mp_funcptr mpf_get_func_by_funcname(char * funcname);

char * mpf_get_keyname_by_funcname(char * funcname);
char * mpf_get_funcname_by_keyname(char * keyname);
int mpf_bind_key(char * keyname, char * funcname);
int * mpf_toggle_function_value(char * funcname);
