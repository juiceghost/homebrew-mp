/*

    mp - Programmer Text Editor

    i18n.

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
#include "mp_lang.h"


/*******************
	Data
********************/

/* language info */

int _mpi_language=0;


struct lang_str
{
	char * msgid;
	char * msgstr;
};


struct lang
{
	char ** ids;
	struct lang_str * strs;
};


#include "mp_lang_en.inc"

#ifndef NO_LANGUAGES

#include "mp_lang_es.inc"
#include "mp_lang_de.inc"
#include "mp_lang_it.inc"
#include "mp_lang_nl.inc"

#endif /* NO_LANGUAGES */

struct lang langs[] =
{
	{ _en_ids, _en_strs },

#ifndef NO_LANGUAGES

	{ _es_ids, _es_strs },
	{ _de_ids, _de_strs },
	{ _it_ids, _it_strs },
	{ _nl_ids, _nl_strs },

#endif /* NO_LANGUAGES */

	{ NULL, NULL }
};


/*******************
	Code
********************/

/**
 * L - Translate a string
 * @msgid: the string to be translated
 *
 * Translate @msgid using the current language info. If no
 * translation string is found, the same @msgid is returned.
 */
char * L(char * msgid)
{
	int n;

	/* default messages */
	if(_mpi_language == -1 || langs[_mpi_language].strs == NULL)
		return(msgid);

	/* find string in the current language */
	for(n=0;langs[_mpi_language].strs[n].msgid != NULL;n++)
	{
		if(strcmp(msgid,langs[_mpi_language].strs[n].msgid) == 0)
		{
			if(langs[_mpi_language].strs[n].msgstr != NULL)
				return(langs[_mpi_language].strs[n].msgstr);
			else
				return(msgid);
		}
	}

	return(msgid);
}


/**
 * mpl_set_language - Sets the current language
 * @langname: name of the language to be set
 *
 * Sets the current language for language-dependent strings.
 * The @langname can be a two letter standard or the english
 * name for the language. If the required language is not
 * found in the internal database, english is set.
 */
void mpl_set_language(char * langname)
{
	int n;

	for(_mpi_language=0;langs[_mpi_language].ids != NULL;_mpi_language++)
	{
		for(n=0;langs[_mpi_language].ids[n] != NULL;n++)
		{
			if(strcmp(langname,langs[_mpi_language].ids[n])==0)
				return;
		}
	}

	_mpi_language=0;
}


/**
 * mpl_enumerate_langs - Returns the available languages
 *
 * Returns a pointer to a static buffer containing the names,
 * concatenated by spaces, of the available languages.
 */
char * mpl_enumerate_langs(void)
{
	static char langstr[1024];
	int n;

	/* buffer overflow should be tested */

	langstr[0]='\0';
	for(n=0;langs[n].ids != NULL;n++)
	{
		if(n) strcat(langstr," ");
		strcat(langstr,* langs[n].ids);
	}

	return(langstr);
}
