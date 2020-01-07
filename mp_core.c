/*

    mp_core.c

    Text editing engine.

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
#include <ctype.h>
#include <stdarg.h>

#include "mp_core.h"

#ifndef WITHOUT_REGEX
#include "gnu_regex.h"
#endif

/*********************
	  Data
**********************/

/**
 * _mp_txts - Chained list of texts
 *
 * The chained list of texts. This is where all the document
 * texts are stored.
 */
mp_txt * _mp_txts=NULL;

/**
 * _mp_active - Pointer to the active text
 *
 * Pointer to the active text. Assumed to be always non-NULL.
 */
mp_txt * _mp_active=NULL;

/**
 * _mp_clipboard - Pointer to the clipboard
 *
 * Pointer to a text containing the clipboard. Used for
 * copying and pasting.
 */
mp_txt * _mp_clipboard=NULL;

/**
 * _mp_tab_size - Size (in spaces) of a tab character
 *
 * Size in spaces of each tab column position.
 */
int _mp_tab_size=DEFAULT_TAB_SIZE;

/* the word separators */
char _mp_separators[40]=" \r\n:)=!;,-<>()[]|+&\"\t";

/**
 * _mp_word_wrap - Column where the word wrapping is done.
 *
 * Any word crossing the column boundary marked by this
 * variable will be sent to the next line. A value of 0
 * means no word wrapping (so right margin expands forever).
 */
int _mp_word_wrap=0;

/**
 * _mp_save_tabs - Tab saving flag.
 *
 * If this flag is set, a string of spaces that crosses
 * a tab column boundary will be saved as a tab. Otherwise,
 * the spaces themselves will be saved.
 */
int _mp_save_tabs=1;

/**
 * _mp_auto_indent - Auto indentation flag.
 *
 * If this flag is set, a new line will have the same spaces
 * at the beginning as the previous one, if any. Otherwise, a
 * new line will start always exactly at the first column.
 */
int _mp_auto_indent=AUTO_INDENT;

/**
 * _mp_case_cmp - Case sensitive compare flag.
 *
 * When this flag is set, searches are made case sensitive.
 */
int _mp_case_cmp=1;

/* LF->CR/LF flag */
int _mp_cr_lf=0;

/* # of chars to indent */
int _mp_indent=0;

/* notify function */
static void mp_notify_stub(char *);
void (* _mp_notify)(char *)=mp_notify_stub;

/* temporal txt */
mp_txt _mp_tmp_txt;

/* the logger */
mp_txt * _mp_log=NULL;

/* use regular expressions in seeks */
int _mp_regex=0;


/*******************
	Code
********************/


static void * mp_malloc(int size)
{
	return(malloc(size));
}


static void mp_free(void * ptr)
{
	free(ptr);
}


/**
 * _im_alive - The infamous spinning bar
 *
 * Each time it's called, this function returns a string
 * with a frame of an animated spinning bar. Just to
 * show the user the program is alive.
 */
char * _im_alive(void)
{
	static char str[5];
	static int nseq=0;
	char * seq="|/-\\";

	str[0]=seq[nseq++];
	nseq&=3;

	str[1]='\0';

	return(str);
}


/**
 * mp_notify_stub - Dummy notify function
 * @str: message (ignored)
 *
 * _mp_notify points here at startup. The upper level
 * must provide a function (with this prototype), that
 * will be called from within the engine when a message
 * must be notified to the user (str will be that message).
 */
static void mp_notify_stub(char * str)
{
}


/**
 * _mp_error - Bangs an error
 * @s: The error string
 * @line: the code file line number
 *
 * Dumps an error to stdout. Usually called only from
 * an assert-like macro on a probably fatal internal error.
 * It must not happen; this usually means
 * that it will be called when less expected.
 */
void _mp_error(char * s, int line)
{
	fprintf(stderr,"Oops! [%s] [%d] mp_error\n", s, line);
	fflush(stderr);

	mp_log("Oops! [%s] [%d] mp_error\n", s, line);
}


/**
 * mp_create_block - Creates a block
 *
 * Creates a new block of text. Returns the zero filled block
 * or NULL if out of memory or any other horrible event.
 */
static mp_blk * mp_create_block(void)
{
	mp_blk * b;

	if((b=(mp_blk *) mp_malloc(sizeof(mp_blk)))==NULL)
	{
		MP_ERROR("mp_create_block");
		return(NULL);
	}

	memset(b,'\0',sizeof(mp_blk));

	return(b);
}


/**
 * mp_name_txt - Names a text
 * @txt: The text to be named
 * @name: the name to set
 *
 * Assigns @name to a text. This usually is the file name.
 * If name is NULL, the empty string will be set.
 */
void mp_name_txt(mp_txt * txt, char * name)
{
	if(name==NULL)
		txt->name[0]='\0';
	else
		strncpy(txt->name,name,sizeof(txt->name));
}


/**
 * mp_find_text - Find a text by its name
 * @name: The name of the text to be found
 *
 * Returns a pointer to a text if found, or NULL
 * if there is no text with this name.
 */
mp_txt * mp_find_txt(char * name)
{
	mp_txt * txt;

	for(txt=_mp_txts;txt!=NULL;txt=txt->next)
	{
		if(strcmp(txt->name,name)==0)
			break;
	}

	return(txt);
}


/**
 * mp_create_sys_txt - Creates a new text
 * @name: the name of the new text
 *
 * Returns a new text with its first block, or NULL
 * if out of memory. The new text is NOT appended
 * to the active texts list; this function must only
 * be used to create special-purpose texts (as the
 * clipboard).
 */
mp_txt * mp_create_sys_txt(char * name)
{
	mp_txt * txt;

	if((txt=mp_malloc(sizeof(mp_txt)))!=NULL)
	{
		memset(txt,'\0',sizeof(mp_txt));

		txt->type=MP_TYPE_TEXT;
		txt->sys=1;

		txt->first=txt->cursor=mp_create_block();

		if(txt->first==NULL)
		{
			MP_ERROR("txt->first");
			mp_free(txt);
			txt=NULL;
		}
		else
		{
			mp_modified(txt);
			txt->mod=0;

			/* first block has one char: the EOF (\0) */
			txt->cursor->buf[0]='\0';
			txt->cursor->size=1;
		}

		mp_name_txt(txt,name);
	}
	else
		MP_ERROR("mp_create_txt");

	return(txt);
}


/**
 * mp_create_txt - Creates a new text
 * @name: the name of the new text
 *
 * Creates a new text and links it to the list of
 * loaded texts. Returns a pointer to it or NULL
 * if out of memory. The new text is set as the
 * active one.
 */
mp_txt * mp_create_txt(char * name)
{
	mp_txt * txt;

	if((txt=mp_create_sys_txt(name))==NULL)
		return(NULL);

	/* links to list */
	txt->next=_mp_txts;
	_mp_txts=txt;

	/* txt is not a system txt */
	txt->sys=0;

	/* now it's the active one */
	_mp_active=txt;

	return(txt);
}


/**
 * mp_get_tmp_txt - Gets a temporal text
 * @otxt: the original text
 *
 * Returns a text that is a copy of otxt.
 */
mp_txt * mp_get_tmp_txt(mp_txt * otxt)
{
	mp_txt * dtxt;

	dtxt=&_mp_tmp_txt;

	dtxt->first=otxt->first;
	dtxt->cursor=otxt->cursor;
	dtxt->offset=otxt->offset;
	dtxt->x=otxt->x;
	dtxt->y=otxt->y;
	dtxt->vx=otxt->vx;
	dtxt->vy=otxt->vy;
	dtxt->mbx=otxt->mbx;
	dtxt->mby=otxt->mby;
	dtxt->mex=otxt->mex;
	dtxt->mey=otxt->mey;

	return(dtxt);
}


/**
 * mp_end_tmp_txt - Stop using the temporal text
 *
 * Stop using the temporal copy returned by
 * mp_get_tmp_txt(). Now it's just a dummy.
 */
void mp_end_tmp_txt(void)
{
}


/**
 * mp_delete_sys_txt - Deletes a text
 * @txt: the text to be deleted
 *
 * Destroys a text and its blocks. Only to be
 * used in special-purpose texts (that returned
 * by mp_create_sys_txt()).
 */
void mp_delete_sys_txt(mp_txt * txt)
{
	mp_blk * blk;
	mp_blk * blk2;

	for(blk=txt->first;blk!=NULL;blk=blk2)
	{
		blk2=blk->next;
		mp_free(blk);
	}

	mp_free(txt);
}


/**
 * mp_delete_txt - Deletes a text
 * @txt: the text to be deleted
 *
 * Deletes a text and its blocks. The next one
 * in the list (if any) will become the
 * active one.
 */
void mp_delete_txt(mp_txt * txt)
{
	mp_txt * t2;

	/* unlinks it */
	if(_mp_txts==txt)
		_mp_txts=txt->next;
	else
	{
		/* find one whose next is txt */
		for(t2=_mp_txts;t2->next!=txt;
			t2=t2->next);

		t2->next=txt->next;
	}

	/* _mp_active will be the next one */
	if((_mp_active=txt->next)==NULL)
		_mp_active=_mp_txts;

	/* next is nothing */
	txt->next=NULL;

	/* finally destroy, unless it's a system text;
	   if txt->sys is set, it means that txt is
	   a system txt that was temporarily shown
	   so it must be only unchained */
	if(!txt->sys) mp_delete_sys_txt(txt);
}


/**
 * mp_show_sys_txt - Shows an internal (system) text
 * @txt: the system text
 *
 * Links to the 'visible' chain of normal (user) texts
 * a internal (system) text, as the clipboard or the
 * log. This make them be shown as read-only texts.
 */
void mp_show_sys_txt(mp_txt * txt)
{
	mp_txt * t2;

	/* test if already linked */
	for(t2=_mp_txts;t2 != NULL && t2 != txt;t2=t2->next);

	/* link only if not already linked */
	if(t2==NULL)
	{
		/* links to list */
		txt->next=_mp_txts;
		_mp_txts=txt;
	}

	/* now it's the active one */
	_mp_active=txt;

	/* txt is still marked as a system txt */
}


/**
 * mp_change_cursor - set the block where the cursor is
 * @txt: the text to be changed
 * @blk: the block where the cursor is
 *
 * Sets the block where the cursor is over.
 * This function is internal and must not be used.
 */
static int mp_change_cursor(mp_txt * txt, mp_blk * blk)
{
	txt->cursor=blk;

	return(1);
}


/**
 * mp_peek_char - Returns the character over the cursor
 * @txt: the text
 *
 * Returns the character over the cursor without moving it.
 * If the cursor is at EOF, '\0' is returned.
 */
int mp_peek_char(mp_txt * txt)
{
	return(txt->cursor->buf[txt->offset]);
}


/**
 * mp_modified - Marks a text as modified
 * @txt: the text
 *
 * Marks a text as modified.
 */
void mp_modified(mp_txt * txt)
{
	/* unmark block */
	mp_unmark(txt);

	/* forget last search match */
	txt->hbx=-1;

	/* now it's modified */
	txt->mod=1;
}


/**
 * mp_visual_column - Returns the visual column
 * @txt: the text
 *
 * Returns the visual column where the cursor is, taking into
 * account the possible tabs in the line. If there are no
 * tabs in the line, the return value should be equal to txt->x.
 */
int mp_visual_column(mp_txt * txt)
{
	int x,r;

	x=txt->x; r=0;
	mp_move_bol(txt);

	while(txt->x < x)
	{
		if(mp_peek_char(txt)=='\t')
			r+=MP_REAL_TAB_SIZE(r);
		else
			r++;

		mp_move_right(txt);
	}

	return(r);
}


/* movement functions */

/**
 * mp_is_sep - separator test
 * @c: the char to test
 *
 * Returns 1 if c is a word separator.
 */
int mp_is_sep(char c)
{
	if(strchr(_mp_separators, c)==NULL)
		return(0);
	else
		return(1);
}


/**
 * mp_recalc_x - Recalculates txt->x
 * @txt: the text
 *
 * Recalculates the value of the x cursor position.
 * Only called internally.
 */
void mp_recalc_x(mp_txt * txt)
{
	int offset;
	mp_blk * blk;
	int x;

	offset=txt->offset;
	blk=txt->cursor;
	x=0;

	for(;;)
	{
		offset--;

		/* if block underflow... */
		if(offset==-1)
		{
			/* it is the first block; stop */
			if(blk->last==NULL)
				break;

			/* travel back the block chain */
			blk=blk->last;

			offset=blk->size-1;
		}

		if(blk->buf[offset]=='\n')
			break;

		x++;
	}

	txt->x=x;
}


/**
 * mp_move_right - Moves cursor one char to the right
 * @txt: the text
 *
 * Moves the cursor one char to the right. Returns 0 if
 * the movement could not be done (i.e., at EOF). If
 * cursor moves over the end of line, it moves to
 * the beginning of the next line.
 */
int mp_move_right(mp_txt * txt)
{
	char c;

	c=mp_peek_char(txt);

	if(txt->offset==txt->cursor->size-1)
	{
		/* is it the last char of file? */
		if(txt->cursor->next==NULL)
		{
			txt->lasty=txt->y;
			return(0);
		}

		/* movement forces a block change */
		mp_change_cursor(txt, txt->cursor->next);

		txt->offset=0;
	}
	else
		txt->offset++;

	/* if char was a '\n', move forward to next line */
	if(c=='\n')
	{
		txt->x=0;
		txt->y++;
	}
	else
		txt->x++;

	return(1);
}


/**
 * mp_move_left - Moves cursor one char to the right
 * @txt: the text
 *
 * Moves the cursor one char to the left. Returns 0 if
 * the movement could not be done (i.e., at BOF). If it
 * moves to the left of the first column, moves to the
 * end of the previous line.
 */
int mp_move_left(mp_txt * txt)
{
	if(txt->offset==0)
	{
		/* is it the beginning of the text? */
		if(txt->cursor->last==NULL)
			return(0);

		/* block change */
		mp_change_cursor(txt, txt->cursor->last);

		txt->offset=txt->cursor->size;
	}

	txt->offset--;

	/* if over '\n', the line has changed */
	if(mp_peek_char(txt)=='\n')
	{
		/* integrity check: if a \n was reached and x is
		   not 0, something strange had happened */
		if(txt->x!=0)
			MP_ERROR("txt->x!=0");

		/* recalcs x to find previous \n */
		mp_recalc_x(txt);

		/* move up */
		txt->y--;
	}
	else
		txt->x--;

	return(1);
}


/**
 * mp_move_to_visual_column - Tries to recover a column position
 * @txt: the text
 * @r: the position
 *
 * Tries to move to the @r visual column position, taking
 * tabs into account.
 */
void mp_move_to_visual_column(mp_txt * txt, int r)
{
	int n,c;

	for(n=0;n < r;)
	{
		c=mp_peek_char(txt);

		if(c=='\n') break;
		if(! mp_move_right(txt)) break;

		if(c=='\t')
			n+=MP_REAL_TAB_SIZE(n);
		else
			n++;
	}
}


/**
 * mp_move_down - Moves cursor one line down
 * @txt: the text
 *
 * Moves the cursor one line down. Returns 0 if
 * the movement could not be done (i.e., at EOF). The
 * x position is preserved if possible.
 */
int mp_move_down(mp_txt * txt)
{
	int x,y,r;

	/* save current coords */
	x=txt->x;
	y=txt->y;
	r=mp_visual_column(txt);

	/* move to end of line */
	mp_move_eol(txt);

	/* try to move beyond it */
	if(! mp_move_right(txt))
	{
		/* no more lines; invalidate move */
		mp_move_xy(txt, x, y);

		return(0);
	}

	/* try to recover previous column */
	mp_move_to_visual_column(txt, r);

	return(1);
}


/**
 * mp_move_bol - Move to the beginning of the line.
 * @txt: the text
 *
 * Moves to the beginning of the current line.
 */
int mp_move_bol(mp_txt * txt)
{
	while(txt->x > 0)
	{
		if(! mp_move_left(txt))
		{
			MP_ERROR("txt->x!=0 at BOF");
			break;
		}
	}

	return(1);
}


/**
 * mp_move_eol - Move to the end of the line.
 * @txt: the text
 *
 * Moves to the end of the current line.
 */
int mp_move_eol(mp_txt * txt)
{
	while(mp_peek_char(txt)!='\n')
	{
		if(! mp_move_right(txt))
			break;
	}

	return(1);
}


/**
 * mp_move_up - Moves cursor one line up
 * @txt: the text
 *
 * Moves the cursor one line up. Returns 0 if
 * the movement could not be done (i.e., at BOF). The
 * x position is preserved if possible.
 */
int mp_move_up(mp_txt * txt)
{
	int ret=1;
	int x;

	x=mp_visual_column(txt);

	mp_move_bol(txt);

	if(! mp_move_left(txt))
		ret=0;

	mp_move_bol(txt);

	/* recover x position if possible */
	mp_move_to_visual_column(txt, x);

	return(ret);
}


/**
 * mp_move_bof - Moves to beginning of file
 * @txt: the text
 *
 * Moves to the beginning of the file.
 */
int mp_move_bof(mp_txt * txt)
{
	mp_change_cursor(txt, txt->first);
	txt->x=0;
	txt->y=0;
	txt->offset=0;

	return(1);
}


/**
 * mp_move_eof - Moves to end of file
 * @txt: the text
 *
 * Moves to the last line of the file. If the last line doesn't
 * end in a new line, the cursor may not actually be at the
 * end of file; use mp_move_eol() after it to be sure.
 */
int mp_move_eof(mp_txt * txt)
{
	while(mp_move_down(txt));

	return(1);
}


/**
 * mp_move_word_right - Moves a word to the right.
 * @txt: the text
 *
 * Moves the cursor one word to the right. Returns 0 if
 * the movement could not be done (i.e., at EOF).
 */
int mp_move_word_right(mp_txt * txt)
{
	while(! mp_is_sep(mp_peek_char(txt)))
	{
		if(! mp_move_right(txt))
			return(0);
	}

	while(mp_is_sep(mp_peek_char(txt)))
	{
		if(! mp_move_right(txt))
			return(0);
	}

	return(1);
}


/**
 * mp_move_word_left - Moves a word to the left.
 * @txt: the text
 *
 * Moves the cursor one word to the left. Returns 0 if
 * the movement could not be done (i.e., at BOF).
 */
int mp_move_word_left(mp_txt * txt)
{
	while(! mp_is_sep(mp_peek_char(txt)))
	{
		if(! mp_move_left(txt))
			return(0);
	}

	while(mp_is_sep(mp_peek_char(txt)))
	{
		if(! mp_move_left(txt))
			return(0);
	}

	return(1);
}


/**
 * mp_move_xy - Moves to x, y position
 * @txt: the text
 * @x: the new x position
 * @y: the new y position
 *
 * Sets the cursor position of text to x, y. Returns 0
 * if the movement cannot be done.
 */
int mp_move_xy(mp_txt * txt, int x, int y)
{
	if(y != txt->y)
	{
		mp_move_bof(txt);

		while(txt->y < y)
		{
			if(! mp_move_down(txt))
				return(0);
		}
	}
	else
		mp_move_bol(txt);

	while(txt->x < x && txt->y==y)
	{
		if(! mp_move_right(txt))
			return(0);
	}

	if(txt->y > y)
		mp_move_left(txt);

	return(1);
}


/**
 * mp_get_char - Gets the char at cursor and advances
 * @txt: the text
 *
 * Returns the char over the cursor and advances to the right.
 * Returns '\0' at EOF.
 */
int mp_get_char(mp_txt * txt)
{
	int c;

	c=mp_peek_char(txt);
	mp_move_right(txt);

	return(c);
}


/**
 * mp_get_str - Gets a string from a text until a delimiter
 * @txt: the text
 * @str: the buffer where the string is get
 * @maxsize: maximum size of buffer
 * @delim: the delimiter to be found
 *
 * Gets a string from a text until the delimiter is found
 * or the maximum size of buffer is reached.
 * Returns the number of chars written into @str.
 */
int mp_get_str(mp_txt * txt, char * str, int maxsize, int delim)
{
	int n,c;

	for(n=0;n < maxsize;n++)
	{
		c=mp_peek_char(txt);
		mp_move_right(txt);

		if(c==delim || c=='\0') break;

		str[n]=c;
	}

	str[n]='\0';

	return(n);
}


/* edit functions */

/**
 * mp_insert_char - Inserts a char in cursor position
 * @txt: the text
 * @c: the char
 *
 * Inserts a character, moving forward the cursor position.
 * Returns 0 if character could not be inserted
 * (readonly text, errors, etc).
 */
int mp_insert_char(mp_txt * txt, int c)
{
	char o;
	int offset;
	char * buf;
	int size;
	mp_blk * blk;
	mp_blk * nblk;
	int insertar;

	if(txt->type != MP_TYPE_TEXT)
		return(0);

	/* if c is '\n', increment total line count */
	if(c=='\n')
		txt->lasty++;

	buf=txt->cursor->buf;
	size=txt->cursor->size;
	offset=txt->offset;

	/* this should be optimized using memcpy */
	for(;offset<size;offset++)
	{
		o=buf[offset];
		buf[offset]=c;
		c=o;
	}

	/* if no room in block... */
	if(size==BLK_SIZE)
	{
		insertar=1;

		/* take next block */
		blk=txt->cursor->next;

		if(blk!=NULL)
		{
			/* if there is room in next block,
			   no need to insert a new one */
			if(blk->size!=BLK_SIZE)
				insertar=0;
		}

		if(insertar)
		{
			if((nblk=mp_create_block())==NULL)
			{
				MP_ERROR("mp_create_block");
				return(0);
			}

			/* next block points to the new one */
			txt->cursor->next=nblk;

			if(blk!=NULL)
				blk->last=nblk;

			nblk->last=txt->cursor;
			nblk->next=blk;

			blk=nblk;
		}

		/* go on inserting */
		buf=blk->buf;
		size=blk->size;
		offset=0;

		for(;offset < size;offset++)
		{
			o=buf[offset];
			buf[offset]=c;
			c=o;
		}

		buf[offset]=c;
		blk->size++;
	}
	else
	{
		txt->cursor->size++;
		buf[offset]=c;
	}

	/* move forward */
	mp_move_right(txt);

	/* mark as modified */
	mp_modified(txt);

	return(1);
}


/**
 * mp_insert_tab - Inserts a tab
 * @txt: the text
 *
 * Inserts a tab, moving forward the cursor position.
 * Returns 0 if character could not be inserted
 * (readonly text, errors, etc).
 */
int mp_insert_tab(mp_txt * txt)
{
	if(txt->type != MP_TYPE_TEXT)
		return(0);

	mp_insert_char(txt, '\t');

	return(1);
}


/**
 * mp_insert_line - Inserts a new line
 * @txt: the text
 *
 * Inserts a new line, taking account of indentations
 * and unmarking the (possible) block.
 */
int mp_insert_line(mp_txt * txt)
{
	int nspcs;
	int c;
	char tmp[1024];

	if(txt->type != MP_TYPE_TEXT)
		return(0);

	/* if not auto-indenting, insert and go */
	if(! _mp_auto_indent)
	{
		mp_insert_char(txt, '\n');

		for(c=0;c < _mp_indent;c++)
			mp_insert_char(txt, ' ');

		return(1);
	}

	c=mp_peek_char(txt);

	/* if inserting before a line with content, just insert it */
	if(c!='\n' && c!='\0')
		return(mp_insert_char(txt, '\n'));

	/* auto-indenting */

	/* count the blanks in current line */
	mp_move_bol(txt);

	for(nspcs=0;nspcs < sizeof(tmp)-1;nspcs++)
	{
		c=mp_get_char(txt);

		if(c == '\n' || ! mp_is_sep(c))
			break;

		tmp[nspcs]=c;
	}

	tmp[nspcs]='\0';

	/* if last char is '\n' or '\0', means that there's nothing
	   but spaces in current line; so insert the new line BEFORE them */
	if(c=='\n' || c=='\0')
	{
		if(c=='\n')
			mp_move_left(txt);

		mp_move_bol(txt);

		mp_insert_char(txt, '\n');

		mp_move_eol(txt);
	}
	else
	{
		/* if not only spaces, create the new line and add
		   the blanks counted before */
		mp_move_eol(txt);

		mp_insert_char(txt, '\n');

		mp_put_str(txt, tmp, 1);
	}

	return(1);
}


/**
 * mp_over_char - Overwrites a char.
 * @arg: the text
 * @c: the char
 *
 * Overwrites current char, moving forward the cursor position.
 * Returns 0 if character could not be written
 * (readonly text, errors, etc).
 */
int mp_over_char(mp_txt * txt, char c)
{
	if(txt->type != MP_TYPE_TEXT)
		return(0);

	/* if a '\n' or over a '\n', insertion is needed */
	if(mp_peek_char(txt)=='\n' || c=='\n')
		return(mp_insert_char(txt, c));
	else
	{
		/* otherwise just substitute */
		txt->cursor->buf[txt->offset]=c;

		mp_move_right(txt);
	}

	mp_modified(txt);

	return(1);
}


/**
 * mp_put_char - Writes a char.
 * @arg: the text
 * @c: the char
 * @insert: current insert status
 *
 * Inserts or overwrites a character, depending of the
 * insert status. Also manages word wrapping,
 * tabs and newlines. Main character writing function.
 * Returns 0 if character could not be put
 * (readonly text, errors, etc).
 */
int mp_put_char(mp_txt * txt, int c, int insert)
{
	int ret;

	if(txt->type != MP_TYPE_TEXT)
		return(0);

	/* do not write '\0's */
	if(c=='\0')
		c='_';

	/* if wordwrapping... */
	if(_mp_word_wrap)
	{
		/* ... and it's on the limit... */
		if(txt->x>=_mp_word_wrap)
		{
			/* moves back until the space to destroy it */
			do
			{
				if(! mp_move_left(txt))
					break;
			}
			while(mp_peek_char(txt)!=' ');

			mp_delete_char(txt);
			mp_insert_line(txt);
			mp_move_eol(txt);
		}
	}

	if(c=='\t')
		ret=mp_insert_tab(txt);
	else
	if(c=='\n')
		ret=mp_insert_line(txt);
	else
	if(insert)
		ret=mp_insert_char(txt, c);
	else
		ret=mp_over_char(txt, c);

	return(ret);
}


/**
 * mp_put_str - Writes a string.
 * @txt: the text
 * @str: the string
 * @insert: current insert status
 *
 * Writes a string, calling mp_put_char() for each character.
 * Returns 0 if any character could not be put
 * (readonly text, errors, etc).
 */
int mp_put_str(mp_txt * txt, char * str, int insert)
{
	int n,ret=1;

	for(n=0;str[n] && (ret=mp_put_char(txt,str[n],insert));n++);

	return(ret);
}


/**
 * mp_put_strf - Writes a string with formatting
 * @txt: the text
 * @fmt: the format string
 *
 * Writes a string into @txt, using @fmt as a
 * formatting printf()-like string.
 */
int mp_put_strf(mp_txt * txt, char * fmt, ...)
{
	char buf[4096];
	va_list argptr;

	va_start(argptr, fmt);
	vsprintf(buf, fmt, argptr);
	va_end(argptr);

	return(mp_put_str(txt, buf, 1));
}


/**
 * mp_delete_char - Deletes a char.
 * @txt: the text
 *
 * Deletes the character in cursor position.
 * Returns 0 if character could not be deleted
 * (readonly text, errors, etc).
 */
int mp_delete_char(mp_txt * txt)
{
	mp_blk * blk;
	mp_blk * dblk;
	int n;

	if(txt->type != MP_TYPE_TEXT)
		return(0);

	/* you cannot delete the EOF */
	if(mp_peek_char(txt)=='\0')
		return(0);

	/* if a '\n' is being deleted, decrement line count */
	if(mp_peek_char(txt)=='\n')
	{
		if(txt->lasty)
			txt->lasty--;
	}

	if(txt->cursor->size > 1)
	{
		txt->cursor->size--;

		/* compress block by moving back */
		/* this must be optimized with memcpy */
		for(n=txt->offset;n<txt->cursor->size;n++)
			txt->cursor->buf[n]=txt->cursor->buf[n+1];

		/* move to next block if deleted char was
		   the last of the block */
		if(txt->offset==txt->cursor->size)
		{
			mp_change_cursor(txt, txt->cursor->next);
			txt->offset=0;
		}
	}
	else
	{
		/* it was the last char of the block: destroy it */

		/* save previous block */
		blk=txt->cursor->last;
		dblk=txt->cursor;

		if(txt->cursor->next==NULL)
		{
			/* inconsistency check: last block cannot have
			   a char that is not EOF */
			MP_ERROR("last block char must be EOF");
			return(0);
		}

		/* move cursor to next block */
		mp_change_cursor(txt, txt->cursor->next);
		txt->offset=0;

		txt->cursor->last=blk;

		if(blk != NULL)
			blk->next=txt->cursor;

		if(txt->first==dblk)
			txt->first=txt->cursor;

		mp_free(dblk);
	}

	mp_modified(txt);

	return(1);
}


/**
 * mp_delete_line - Deletes current line.
 * @txt: the text
 *
 * Deletes current line.
 * Returns 0 if the line could not be deleted
 * (readonly text, errors, etc).
 */
int mp_delete_line(mp_txt * txt)
{
	char c;

	if(txt->type != MP_TYPE_TEXT)
		return(0);

	mp_move_bol(txt);

	/* deletes until '\n' inclusive */
	for(;;)
	{
		c=mp_peek_char(txt);

		if(! mp_delete_char(txt))
			break;

		if(c=='\n')
			break;
	}

	return(1);
}


/**
 * mp_load_open_file - Loads a file into the text.
 * @txt: the text
 * @f: the file
 *
 * Loads a file into the text.
 * Returns 0 if some character could not be put
 * (readonly text, errors, etc).
 */
int mp_load_open_file(mp_txt * txt, FILE * f)
{
	char tmp[30];
	int c;
	int ret;
	long bytes;
	int ww,ai;

	ret=0;
	bytes=0;

	ww=_mp_word_wrap;
	_mp_word_wrap=0;
	ai=_mp_auto_indent;
	_mp_auto_indent=0;

	while((c=fgetc(f))!=EOF)
	{
		if(c=='\r' || c=='\x1a')
			continue;

		/* special case: '\b' deletes previous char */
		if(c=='\b')
		{
			mp_move_left(txt);
			mp_delete_char(txt);
		}
		else
		if(! mp_put_char(txt, c, 1))
		{
			ret=-2;
			break;
		}

		bytes++;

		if(bytes % 50000==0)
		{
			sprintf(tmp,"%ld bytes",bytes);
			_mp_notify(tmp);
		}
	}

	/* try to move forward the end of file, to force
	   a line number recalculation */
	mp_move_right(txt);

	sprintf(tmp,"%ld bytes",bytes);
	_mp_notify(tmp);
	mp_log("mp_load_open_file: %d bytes\n",bytes);

	mp_move_bof(txt);

	_mp_word_wrap=ww;
	_mp_auto_indent=ai;

	return(ret);
}


/**
 * mp_load_file - Opens and loads a file into text.
 * @txt: the text
 * @filename: the file name
 *
 * Opens and loads the file named by filename into text.
 * Returns -1 if file cannot be open, or the output
 * from mp_load_open_file() otherwise.
 */
int mp_load_file(mp_txt * txt, char * filename)
{
	int ret;
	FILE * f;

	if((f=fopen(filename,"rb"))==NULL)
		return(-1);

	ret=mp_load_open_file(txt, f);

	fclose(f);

	return(ret);
}


/**
 * mp_write_open_file - Writes the text into file
 * @txt: the text
 * @f: the file
 *
 * Writes the text into the file, previously opened
 * for writing.
 * Returns always 1 (no error checking is done).
 */
int mp_write_open_file(mp_txt * txt, FILE * f)
{
	char tmp[30];
	int c;
	mp_txt * ctxt;
	long bytes;

	ctxt=mp_get_tmp_txt(txt);

	mp_move_bof(ctxt);

	bytes=0;

	for(;;)
	{
		if((c=mp_get_char(ctxt))=='\0')
			break;

		if(_mp_cr_lf)
		{
			if(c=='\n')
				fputc('\r',f);
		}

		/* FIXME: _mp_save_tabs should be treated here;
		   by now, disabling _mp_save_tabs has no effect */

		fputc(c,f);

		bytes++;

		if(bytes % 50000==0)
		{
			sprintf(tmp,"%ld bytes",bytes);
			_mp_notify(tmp);
		}
	}

	sprintf(tmp,"%ld bytes",bytes);
	_mp_notify(tmp);
	mp_log("mp_write_open_file: %d bytes\n",bytes);

	mp_end_tmp_txt();

	return(1);
}


/**
 * mp_write_file - Opens the file and writes the text into it.
 * @txt: the text
 * @filename: the filename
 *
 * Opens filename for writing and writes text into it.
 * Returns -1 if file cannot be open for writing or 1 otherwise.
 */
int mp_write_file(mp_txt * txt, char * filename)
{
	int ret;
	FILE * f;

	if((f=fopen(filename,"wb"))==NULL)
		return(-1);

	ret=mp_write_open_file(txt,f);

	fclose(f);

	return(ret);
}


/* block, cut and paste routines */

/**
 * mp_unmark - Unmarks the selection block.
 * @txt: the text
 *
 * Unmarks the selection block.
 */
void mp_unmark(mp_txt * txt)
{
	if(txt->mbx!=-1)
		txt->mbx=txt->mby=txt->mex=txt->mey=-1;
}


/**
 * mp_marked - Tests if selection block is marked.
 * @txt: the text
 *
 * Returns 1 if a selection block is marked.
 */
int mp_marked(mp_txt * txt)
{
	if(txt->mbx==-1 || txt->mex==-1)
		return(0);

	return(1);
}


/**
 * mp_mark - Marks the beginning or end of the selection block.
 * @txt: the text
 *
 * Marks the current cursor position as the beginning or the end
 * of the selection block. Both boundaries can be reselected.
 */
void mp_mark(mp_txt * txt)
{
	int before;

	if(!mp_marked(txt))
	{
		txt->mbx=txt->mex=txt->x;
		txt->mby=txt->mey=txt->y;

		return;
	}

	if(txt->y < txt->mby)
		before=1;
	else
	if(txt->y > txt->mey)
		before=0;
	else
	{
		if(txt->x < txt->mbx)
			before=1;
		else
			before=0;
	}

	if(before)
	{
		txt->mbx=txt->x;
		txt->mby=txt->y;
	}
	else
	{
		txt->mex=txt->x;
		txt->mey=txt->y;
	}
}


void mp_lock_clipboard(int lock)
{
	if(lock)
	{
		/* make clipboard writable */
		_mp_clipboard->type=MP_TYPE_TEXT;

		/* deletes previous clipboard */

		mp_move_bof(_mp_clipboard);
		mp_mark(_mp_clipboard);
		mp_move_eof(_mp_clipboard);
		mp_move_eol(_mp_clipboard);
		mp_mark(_mp_clipboard);
		mp_delete_mark(_mp_clipboard);
	}
	else
	{
		/* clipboard is back to read-only */
		_mp_clipboard->type=MP_TYPE_READ_ONLY;
		_mp_clipboard->mod=0;
	}
}


/**
 * mp_copy_mark - Copies the selection block into the clipboard
 * @txt: the text
 *
 * Copies the selection block into the internal clipboard.
 * Returns 0 if the selection block is not marked.
 */
int mp_copy_mark(mp_txt * txt)
{
	int x,y;
	int c;
	mp_txt * ctxt;

	if(! mp_marked(txt))
		return(0);

	mp_lock_clipboard(1);

	ctxt=mp_get_tmp_txt(txt);

	/* move to beginning of block */
	mp_move_xy(ctxt, txt->mbx, txt->mby);

	/* read until the end */
	for(x=txt->mex,y=txt->mey;ctxt->y<=y;)
	{
		c=mp_get_char(ctxt);

		if(c=='\0')
		{
			MP_ERROR("unexpected EOF");
			break;
		}

		mp_insert_char(_mp_clipboard, c);

		if(ctxt->y==y && ctxt->x==x)
			break;
	}

	mp_end_tmp_txt();

	mp_lock_clipboard(0);

	return(1);
}


/**
 * mp_paste_mark - Pastes the clipboard into cursor position.
 * @txt: the text
 *
 * Pastes the clipboard into cursor position.
 * Returns 0 if text is read-only or if there is no
 * clipboard to paste.
 */
int mp_paste_mark(mp_txt * txt)
{
	int c;

	if(txt->type != MP_TYPE_TEXT)
		return(0);

	if(_mp_clipboard==NULL)
		return(0);

	mp_move_bof(_mp_clipboard);

	while((c=mp_get_char(_mp_clipboard)))
		mp_put_char(txt, c, 1);

	return(1);
}


/**
 * mp_delete_mark - Deletes the marked selection block.
 * @txt: the text
 *
 * Deletes the marked selection block. It is done
 * by deleting char by char and this is slow; an
 * optimized version (releasing entire blocks) should
 * be written.
 * Returns 0 if text is read-only or there is no
 * marked selection block.
 */
int mp_delete_mark(mp_txt * txt)
{
	long l;

	if(txt->type != MP_TYPE_TEXT)
		return(0);

	if(! mp_marked(txt))
		return(0);

	mp_move_xy(txt, txt->mex, txt->mey);

	for(l=0;txt->y!=txt->mby ||
	    (txt->y==txt->mby && txt->x>txt->mbx);l++)
	{
		if(! mp_move_left(txt))
		{
			MP_ERROR("Unexpected EOF");
			return(0);
		}
	}

	for(;l;l--)
	{
		if(! mp_delete_char(txt))
		{
			MP_ERROR("Unexpected EOF");
			return(0);
		}
	}

	return(1);
}


/* other functions */

/**
 * mp_seek_plain - Seeks the string as plain text.
 * @txt: the text
 * @str: the string
 *
 * Seeks the string into text. If found, the cursor is moved there.
 * The string must be found as-is (i.e., it does not contain any
 * special characters). Returns 1 if the string is found.
 */
int mp_seek_plain(mp_txt * txt, char * str)
{
	int c,t,n,l;
	int found;
	int x,y,hbx,hby;

	x=txt->x;
	y=txt->y;
	n=found=0;
	hbx=hby=-1;

	l=strlen(str);

	c=mp_get_char(txt);

	while(c != '\0')
	{
		if(_mp_case_cmp)
			t=(c == str[n]);
		else
			t=(toupper(c) == toupper(str[n]));

		if(t)
		{
			if(n==0)
			{
				hbx=txt->x - 1;
				hby=txt->y;
			}

			if(++n == l)
			{
				found=1;
				break;
			}

			c=mp_get_char(txt);
		}
		else
		{
			if(n)
				n=0;
			else
				c=mp_get_char(txt);
		}
	}

	if(!found)
		mp_move_xy(txt,x,y);
	else
	{
		/* store search hit */
		txt->hbx=hbx; txt->hby=hby;
		txt->hex=txt->x; txt->hey=txt->y;
	}

	return(found);
}


/**
 * mp_seek_regex - Seeks the string as a regular expression.
 * @txt: the text
 * @str: the string
 *
 * Seeks the string into text. If found, the cursor is moved there.
 * The string must be a GNU regular expression.
 * Returns 1 if the string is found.
 */
int mp_seek_regex(mp_txt * txt, char * str)
{
	int ret=0;

#ifndef WITHOUT_REGEX

	int err;
	regex_t r;
	regmatch_t m;
	char line[4096];
	int x, y, t;

	/* compiles the expression */
	if((err=regcomp(&r, str, REG_EXTENDED|
		(_mp_case_cmp ? 0: REG_ICASE))))
	{
		regerror(err, &r, line, sizeof(line));

		mp_log("regex error: %s\n", line);
		return(0);
	}

	x=txt->x; y=txt->y;

	mp_move_right(txt);

	while(mp_peek_char(txt) != '\0')
	{
		t=txt->x;

		/* reads a line */
		mp_get_str(txt, line, sizeof(line), '\n');

		/* executes the regex */
		if(regexec(&r, line, 1, &m, (t > 0 ? REG_NOTBOL : 0))==0)
		{
			/* found! */
			ret=1;

			/* store search hit */
			x=txt->hbx=m.rm_so + t;
			txt->hex=m.rm_eo + t;
			y=txt->hby=txt->hey=txt->y - 1;

			break;
		}
	}

	regfree(&r);

	/* final (or initial if not found) position */
	mp_move_xy(txt, x, y);

#endif /* WITHOUT_REGEX */

	return(ret);
}


/**
 * mp_seek - Seeks the string.
 * @txt: the text
 * @str: the string
 *
 * Seeks the string into text. If found, the cursor is moved there.
 * Returns 1 if the string is found.
 */
int mp_seek(mp_txt * txt, char * str)
{
#ifndef WITHOUT_REGEX
	if(_mp_regex)
		return(mp_seek_regex(txt, str));
#endif

	return(mp_seek_plain(txt, str));
}


/**
 * mp_mark_match - Marks (selects) the last successful match
 * @txt: the text
 *
 * Marks (as a block) the last successful match.
 */
int mp_mark_match(mp_txt * txt)
{
	if(txt->hbx==-1) return(0);

	mp_unmark(txt);
	mp_move_xy(txt, txt->hbx, txt->hby);
	mp_mark(txt);
	mp_move_xy(txt, txt->hex, txt->hey);
	mp_mark(txt);

	return(1);
}


/**
 * mp_replace - Seeks and replaces a string.
 * @txt: the text
 * @src: the string to be searched.
 * @des: the string to replace src.
 *
 * Seeks a string and, if it is found, replaces it with another.
 * Returns 0 if the text is read-only or the string is not found.
 */
int mp_replace(mp_txt * txt, char * src, char * des)
{
	if(txt->type != MP_TYPE_TEXT)
		return(0);

	if(! mp_seek(txt, src))
		return(0);

	mp_mark_match(txt);
	mp_delete_mark(txt);
	mp_put_str(txt,des,1);

	return(1);
}


/**
 * mp_adjust - Adjusts the current window pointers.
 * @txt: the text
 * @tx: the current window width
 * @ty: the current window height
 *
 * Adjust the current window pointers (variables vx and vy
 * of the text) to a window of tx width and ty height so
 * that the cursor is visible.
 */
void mp_adjust(mp_txt * txt, int tx, int ty)
{
	int r;

	r=mp_visual_column(txt);

	if(r < txt->vx)
		txt->vx=r;

	if(r > (txt->vx + tx - 1))
		txt->vx=r-tx+1;

	if(txt->y < txt->vy)
		txt->vy=txt->y;

	if(txt->y > (txt->vy + ty - 1))
		txt->vy=txt->y-ty+1;
}


/**
 * mp_get_word - Gets the word over the cursor.
 * @txt: the text
 * @buf: the buffer to hold the word
 * @size: maximum size of the buffer
 *
 * Gets the word over the cursor and writes it over
 * the buffer, using the global word separators.
 */
void mp_get_word(mp_txt * txt, char * buf, int size)
{
	mp_txt * ctxt;
	char c;
	int n;

	ctxt=mp_get_tmp_txt(txt);

	/* moves back while not a separator */
	while(! mp_is_sep(mp_peek_char(ctxt)) &&
		mp_move_left(ctxt));

	if(mp_is_sep(mp_peek_char(ctxt)))
		mp_move_right(ctxt);

	/* writes in buffer while not a separator */
	for(n=0;n < size - 1;n++)
	{
		c=mp_get_char(ctxt);

		if(c=='\0' || mp_is_sep(c))
			break;

		buf[n]=c;
	}

	buf[n]='\0';

	mp_end_tmp_txt();
}


/**
 * mp_set_notify - Sets the notify function.
 * @func: the new notify function.
 *
 * The notify function is called from inside file loading
 * and searching routines to inform the user about what is
 * happening, sending a message in a character string.
 * This function changes the current (dummy) one.
 */
void mp_set_notify(void (* func)(char *))
{
	_mp_notify=func;
}


/**
 * mp_debug_hook - Dummy debug hook function.
 *
 * This function does nothing. It is just defined to
 * be inserted as a breakpoint for the debugger.
 */
void mp_debug_hook(void)
{
}


/**
 * mp_log - Logs to Minimum Profit's internal log
 * @fmt: printf() like format string
 * @...: variable list of arguments
 *
 * Writes a formatted string to the internal log txt.
 */
void mp_log(char * fmt, ...)
{
	char buf[4096];
	va_list argptr;

	/* create log txt if it doesn't exist */
	if(_mp_log == NULL)
		_mp_log=mp_create_sys_txt("<log>");

	_mp_log->type=MP_TYPE_TEXT;

	mp_move_eof(_mp_log);
	mp_move_eol(_mp_log);

	va_start(argptr, fmt);
	vsprintf(buf, fmt, argptr);
	va_end(argptr);

	mp_put_str(_mp_log, buf, 1);

	_mp_log->type=MP_TYPE_READ_ONLY;
	_mp_log->mod=0;
}


/**
 * mp_startup - Starts up the mp core.
 *
 * Starts up the mp core. By now, it does nothing.
 * Returns 1 is succesful.
 */
int mp_startup(void)
{
	mp_log("Minimum Profit version " VERSION " (" __DATE__ " " __TIME__ ")\n");
	mp_log("Block size: %d\n",BLK_SIZE);

	_mp_clipboard=mp_create_sys_txt("<clipboard>");

	return(1);
}


/**
 * mp_shutdown - Shuts down the mp core.
 *
 * Shuts down the mp core. By now, it does nothing.
 */
void mp_shutdown(void)
{
}
