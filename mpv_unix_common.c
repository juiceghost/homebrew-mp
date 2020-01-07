/*

    mpv_unix_common.c

    Code common to Unix versions (curses / X).

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

#if !defined(WITHOUT_GLOB)
#include <glob.h>
#endif

/*******************
	Data
********************/

/*******************
	Code
********************/

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
	return(strcasecmp(s1,s2));
}


/**
 * _mpv_strip_cwd - Strips current working directory
 * @buf: buffer containing a file name
 * @size: size of the buffer
 *
 * If the file name contained in @buf begins with
 * the current working directory, strips it (effectively
 * converting an absolute pathname to relative). If
 * the file name path is different from the
 * current working directory, nothing is done.
 */
void _mpv_strip_cwd(char * buf, int size)
{
	char tmp[1024];
	int l;
	char * ptr;

	if(getcwd(tmp,sizeof(tmp))==NULL)
		return;

	l=strlen(tmp);
	if(strncmp(tmp,buf,l)==0)
	{
		ptr=&buf[l];
		if(*ptr=='/') ptr++;

		strncpy(tmp,ptr,sizeof(tmp));
		strncpy(buf,tmp,size);
	}
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
	struct stat s;

#if !defined(WITHOUT_GLOB)
	int n;
	glob_t globbuf;
	char * ptr;

	if(spec[0]=='\0') spec="*";

	globbuf.gl_offs=1;
	if(glob(spec,GLOB_MARK,NULL,&globbuf))
		return(NULL);

	txt=mp_create_sys_txt("<glob>");

	MP_SAVE_STATE();

	for(n=0;globbuf.gl_pathv[n]!=NULL;n++)
	{
		ptr=globbuf.gl_pathv[n];

		if(stat(ptr,&s)==-1) continue;
		if(s.st_mode & S_IFDIR) continue;

		mp_put_str(txt,ptr,1);
		mp_put_char(txt,'\n',1);
	}

	globfree(&globbuf);

	mp_move_left(txt);
	mp_delete_char(txt);
	mp_move_bof(txt);

	MP_RESTORE_STATE();

#else /* WITHOUT_GLOB */

	/* no glob: simulate it piping from ls */

	FILE * f;
	char tmp[1024];

	if(*spec=='\0' || strcmp(spec,"*")==0)
		strcpy(tmp,"ls");
	else
		snprintf(tmp,sizeof(tmp),"ls %s",spec);

	if((f=popen(tmp,"r"))==NULL)
		return(NULL);

	txt=mp_create_sys_txt("<glob>");

	MP_SAVE_STATE();

	while(fgets(tmp,sizeof(tmp),f)!=NULL)
	{
		tmp[strlen(tmp)-1]='\0';

		if(tmp[0]=='\0') continue;
		if(stat(tmp,&s)==-1) continue;
		if(s.st_mode & S_IFDIR) continue;

		mp_put_str(txt,tmp,1);
		mp_put_char(txt,'\n',1);
	}

	if(pclose(f)==-1)
	{
		mp_delete_sys_txt(txt);
		txt=NULL;
	}

	mp_move_left(txt);
	mp_delete_char(txt);
	mp_move_bof(txt);

	MP_RESTORE_STATE();

#endif /* WITHOUT_GLOB */

	return(txt);
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
	char tmp[1024];
	FILE * f;
	mp_txt * txt;
	char ** ptr;

	sprintf(tmp,L(MSG_HELP),term);

	if(synhi==0 || (txt=mp_create_txt(tmp))==NULL)
		return(0);

	if((ptr=_mps_synhi[synhi - 1].helpers)!=NULL)
	{
		for(;*ptr!=NULL;ptr++)
		{
			snprintf(tmp,sizeof(tmp),*ptr,term);

			if((f=popen(tmp,"r"))!=NULL)
			{
				mp_load_open_file(txt,f);

				if(!pclose(f))
					break;
			}
		}
	}

	if(ptr==NULL || *ptr==NULL)
	{
		mp_delete_txt(txt);
		mpv_alert(L(MSG_NOHELPFOR),term);
		return(0);
	}

	mps_auto_synhi(txt);
	txt->type=MP_TYPE_READ_ONLY;
	txt->mod=0;

	return(1);
}
