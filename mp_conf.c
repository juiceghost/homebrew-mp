/*

    mp_conf.c

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mp_core.h"
#include "mp_video.h"
#include "mp_conf.h"
#include "mp_lang.h"
#include "mp_func.h"
#include "mp_iface.h"

/*******************
	Data
********************/

#ifndef CONFIG_FILE_NAME
#define CONFIG_FILE_NAME "mprc"
#endif

/* home sweet home */
char _mpc_home[1024]="";

/* the configuration file */
char _mpc_config_file[1024];

/* colors */

#define TEXT_COLOR_DEF		-1
#define TEXT_COLOR_BLACK	0
#define TEXT_COLOR_RED		1
#define TEXT_COLOR_GREEN	2
#define TEXT_COLOR_YELLOW	3
#define TEXT_COLOR_BLUE 	4
#define TEXT_COLOR_MAGENTA	5
#define TEXT_COLOR_CYAN 	6
#define TEXT_COLOR_WHITE	7

struct _mpc_color_desc mpc_color_desc[MP_COLOR_NUM]= {
	{ 0x00000000, 0x00ffffff,
	  TEXT_COLOR_DEF, TEXT_COLOR_DEF,
	  0, 0, 0, 0, "normal" }, /* MP_COLOR_NORMAL */
	{ 0x00ff0000, 0x00ffffff,
	  TEXT_COLOR_RED, TEXT_COLOR_WHITE,
	  0, 0, 1, 0, "selected" }, /* MP_COLOR_SELECTED */
	{ 0x0000aaaa, 0xffffffff,
	  TEXT_COLOR_GREEN, TEXT_COLOR_DEF,
	  1, 0, 0, 0, "comment" }, /* MP_COLOR_COMMENT */
	{ 0x000000ff, 0xffffffff,
	  TEXT_COLOR_BLUE, TEXT_COLOR_DEF,
	  0, 0, 0, 1, "string" }, /* MP_COLOR_STRING */
	{ 0x0000aa00, 0xffffffff,
	  TEXT_COLOR_GREEN, TEXT_COLOR_DEF,
	  0, 0, 0, 1, "token" }, /* MP_COLOR_TOKEN */
	{ 0x00ff6666, 0xffffffff,
	  TEXT_COLOR_RED, TEXT_COLOR_DEF,
	  0, 0, 0, 0, "var" }, /* MP_COLOR_VAR */
	{ 0xffffffff, 0xffffffff,
	  TEXT_COLOR_DEF, TEXT_COLOR_DEF,
	  0, 0, 1, 0, "cursor" }, /* MP_COLOR_CURSOR */
	{ 0x00dddd00, 0xffffffff,
	  TEXT_COLOR_YELLOW, TEXT_COLOR_DEF,
	  0, 0, 0, 1, "caps" }, /* MP_COLOR_CAPS */
	{ 0x008888ff, 0xffffffff,
	  TEXT_COLOR_CYAN, TEXT_COLOR_DEF,
	  0, 1, 0, 0, "local" }, /* MP_COLOR_LOCAL */

	{ 0, 0,
	  TEXT_COLOR_BLUE, TEXT_COLOR_WHITE,
	  0, 0, 1, 1, "title" }, /* MP_COLOR_TEXT_TITLE */
	{ 0, 0,
	  TEXT_COLOR_BLUE, TEXT_COLOR_WHITE,
	  0, 0, 1, 1, "menu_element" }, /* MP_COLOR_TEXT_MENU_ELEM */
	{ 0, 0,
	  TEXT_COLOR_WHITE, TEXT_COLOR_BLACK,
	  0, 0, 0, 1, "menu_selection" }, /* MP_COLOR_TEXT_MENU_SEL */
	{ 0, 0,
	  TEXT_COLOR_BLUE, TEXT_COLOR_BLUE,
	  0, 0, 1, 1, "frame1" }, /* MP_COLOR_TEXT_FRAME1 */
	{ 0, 0,
	  TEXT_COLOR_BLUE, TEXT_COLOR_BLACK,
	  0, 0, 1, 1, "frame2" }, /* MP_COLOR_TEXT_FRAME2 */
	{ 0, 0,
	  TEXT_COLOR_DEF, TEXT_COLOR_DEF,
	  0, 0, 0, 0, "scrollbar" }, /* MP_COLOR_TEXT_SCROLLBAR */
	{ 0, 0,
	  TEXT_COLOR_BLUE, TEXT_COLOR_WHITE,
	  0, 0, 1, 1, "scrollbar_thumb" } /* MP_COLOR_TEXT_SCR_THUMB */
};

int _mpc_text_interface=0;

mp_txt * _menu_info=NULL;


/*******************
	Code
********************/


static void _mpc_set_variable(char * var, char * value)
{
	if(strcmp(var,"tab_size")==0)
		_mp_tab_size=atoi(value);
	else
	if(strcmp(var,"word_wrap")==0)
		_mp_word_wrap=atoi(value);
	else
	if(strcmp(var,"case_sensitive_search")==0)
		_mp_case_cmp=atoi(value);
	else
	if(strcmp(var,"auto_indent")==0)
		_mp_auto_indent=atoi(value);
	else
	if(strcmp(var,"save_tabs")==0)
	       _mp_save_tabs=atoi(value);
	else
	if(strcmp(var,"col_80")==0)
		_mpi_mark_column_80=atoi(value);
	else
	if(strcmp(var,"cr_lf")==0)
		_mp_cr_lf=atoi(value);
	else
	if(strcmp(var,"preread_lines")==0)
		_mpi_preread_lines=atoi(value);
	else
	if(strcmp(var,"use_regex")==0)
		_mp_regex=atoi(value);
	else
	if(strcmp(var,"template_file")==0)
		strncpy(_mpi_template_file,value,sizeof(_mpi_template_file));
	else
	if(strcmp(var,"lang")==0)
		mpl_set_language(value);
	else
	if(strcmp(var, "ctags_cmd")==0)
		strncpy(_mpi_ctags_cmd,value,sizeof(_mpi_ctags_cmd));
	else
	if(strcmp(var, "status_format")==0)
		strncpy(_mpi_status_line_f,value,sizeof(_mpi_status_line_f));
	else
	if(strcmp(var, "strftime_format")==0)
		strncpy(_mpi_strftime_f,value,sizeof(_mpi_strftime_f));
	else
		mpv_set_variable(var, value);
}


static void _mpc_bind_key(char * args)
{
	char * funcname;

	if((funcname=strrchr(args, ' '))==NULL)
		if((funcname=strrchr(args, '\t'))==NULL)
			return;

	*funcname='\0'; funcname++;
	while(*funcname==' ' || *funcname=='\t') funcname++;

	mpf_bind_key(args, funcname);
}


static void _mpc_define_color(char * colordef, int text)
{
	/* colordef string is:
	   {colorname} {ink_color} {paper_color} [{options}]
	   where options can be
	   italic, underline, reverse, bright

	   - Bright is not used in gui_color, nor italic
	     in text_color
	   - {ink_color} and {paper_color} are RGB
	     definitions in gui_color, and color names
	     in text_color
	*/
	int c,n;
	int in,pn;
	char * text_color_names[]={ "default", "black", "red", "green",
		"yellow", "blue", "magenta", "cyan","white", NULL };
	char * text_color_names_equiv[]= { "ffffff", "000000", "ff0000",
		"00ff00", "ffff00", "0000ff", "ff00ff", "00ffff", "ffffff",
		NULL };
	char * colorname;
	char * ink;
	char * paper;
	char * opts;

	if(_mpc_text_interface != text) return;

	if((colorname=strtok(colordef, " \t"))==NULL) return;

	/* find first the colorname */
	for(c=0;c < MP_COLOR_NUM;c++)
	{
		if(strcmp(colorname,mpc_color_desc[c].colorname)==0)
			break;
	}

	/* bad color name */
	if(c==MP_COLOR_NUM) return;

	if((ink=strtok(NULL, " \t"))==NULL) return;
	if((paper=strtok(NULL, " \t"))==NULL) return;

	if(text)
	{
		in=pn=-1;

		/* find the ink and paper offset */
		for(n=0;text_color_names[n]!=NULL;n++)
		{
			if(strcmp(ink,text_color_names[n])==0)
				in=n;

			if(strcmp(paper,text_color_names[n])==0)
				pn=n;
		}

		/* bad ink or paper name */
		if(in==-1 || pn==-1) return;

		/* store */
		mpc_color_desc[c].ink_text=in-1;
		mpc_color_desc[c].paper_text=pn-1;
	}
	else
	{
		/* try to find if user set a color name, skipping 'default' */
		for(n=1;text_color_names[n]!=NULL;n++)
		{
			if(strcmp(ink,text_color_names[n])==0)
				ink=text_color_names_equiv[n];

			if(strcmp(paper,text_color_names[n])==0)
				paper=text_color_names_equiv[n];
		}

		/* ink and paper are RGB values */
		if(strcmp(ink,"default")==0)
			mpc_color_desc[c].ink_rgb=0xffffffff;
		else
			sscanf(ink, "%x", &mpc_color_desc[c].ink_rgb);

		if(strcmp(paper,"default")==0)
			mpc_color_desc[c].paper_rgb=0xffffffff;
		else
			sscanf(paper, "%x", &mpc_color_desc[c].paper_rgb);
	}

	mpc_color_desc[c].italic=mpc_color_desc[c].underline=
	mpc_color_desc[c].reverse=mpc_color_desc[c].bright=0;

	/* process the options */
	if((opts=strtok(NULL, ""))==NULL) return;

	if(strstr(opts, "italic")!=NULL)
		mpc_color_desc[c].italic=1;
	if(strstr(opts, "underline")!=NULL)
		mpc_color_desc[c].underline=1;
	if(strstr(opts, "reverse")!=NULL)
		mpc_color_desc[c].reverse=1;
	if(strstr(opts, "bright")!=NULL)
		mpc_color_desc[c].bright=1;
}


static void _mpc_add_menu(char * data, int menu_item)
{
	if(_menu_info==NULL)
		_menu_info=mp_create_sys_txt("<menu_tmp_data>");

	if(!menu_item)
		mp_put_char(_menu_info,'/',1);

	mp_put_str(_menu_info,data,1);
	mp_put_char(_menu_info,'\n',1);
}


/**
 * mpc_read_config - Reads and parses the configuration file
 *
 * Reads and parses the configuration file.
 */
static void _mpc_read_config(char * file, int level)
{
	char line[4096];
	FILE * f;
	char * ptr;

	if(level >= 16)
		return;

	if((f=fopen(file,"r"))==NULL)
		return;

	mp_log("Config file: '%s'\n", file);

	while(fgets(line,sizeof(line),f)!=NULL)
	{
		/* chop */
		line[strlen(line)-1]='\0';

		/* strip comments and empty lines */
		if(line[0]=='\0' || line[0]=='#')
			continue;

		ptr=strchr(line,':');
		if(ptr==NULL) ptr=strchr(line,'=');

		if(ptr!=NULL)
		{
			/* it's a variable definition */
			*ptr='\0';
			ptr++;
			while(*ptr==' ') ptr++;

			_mpc_set_variable(line, ptr);
			continue;
		}

		/* find commands */
		if((ptr=strchr(line,' '))==NULL)
			if((ptr=strrchr(line,'\t'))==NULL)
				continue;

		*ptr='\0'; ptr++;
		while(*ptr==' ' || *ptr=='\t') ptr++;

		/* bind key */
		if(strcmp("bind",line)==0)
			_mpc_bind_key(ptr);
		else
		if(strcmp("text_color",line)==0)
			_mpc_define_color(ptr,1);
		else
		if(strcmp("gui_color",line)==0)
			_mpc_define_color(ptr,0);
		else
		if(strcmp("menu",line)==0)
			_mpc_add_menu(ptr, 0);
		else
		if(strcmp("menu_item",line)==0)
			_mpc_add_menu(ptr, 1);
		else
		if(strcmp("source",line)==0)
			_mpc_read_config(ptr, level+1);
	}

	/* fix the template file name if it has a leading ~ */
	if(_mpi_template_file[0]=='~')
	{
		strncpy(line,&_mpi_template_file[1],sizeof(line));
		snprintf(_mpi_template_file,sizeof(_mpi_template_file),
			"%s%s",_mpc_home,line);
	}

	fclose(f);
}


void mpc_startup(void)
{
	char * home;
	int n;

	/* reads first a global config file */
	snprintf(_mpc_config_file, sizeof(_mpc_config_file),
		"/etc/%s", CONFIG_FILE_NAME);

	_mpc_read_config(_mpc_config_file, 0);

	/* take home from the environment variable,
	   unless externally defined */
	if(_mpc_home[0]=='\0' && (home=getenv("HOME"))!=NULL)
		strncpy(_mpc_home,home,sizeof(_mpc_home));

	n=strlen(_mpc_home)-1;
	if(_mpc_home[n]=='/' || _mpc_home[n]=='\\')
		_mpc_home[n]='\0';

	snprintf(_mpc_config_file, sizeof(_mpc_config_file),
		"%s/.%s", _mpc_home, CONFIG_FILE_NAME);

	_mpc_read_config(_mpc_config_file, 0);
}
