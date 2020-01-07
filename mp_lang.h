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

#define MSG_UNNAMED	 "<unnamed>"
#define MSG_TEXTTOSEEK	 "Text to seek: "
#define MSG_TAGTOSEEK	 "Tag to seek: "
#define MSG_NOTFOUND	 "Text not found."
#define MSG_TAGNOTFOUND  "Tag(s) not found."
#define MSG_FILENAME	 "Enter file name: "
#define MSG_SAVECHANGES  "File has changed. Save changes?"
#define MSG_FILENOTFOUND "File '%s' not found."
#define MSG_CANTWRITE	 "Can't create file '%s'."
#define MSG_YESNO	 " [Y/N]"
#define MSG_ENTER	 " [ENTER]"
#define MSG_HELP	 "<help about '%s'>"
#define MSG_LOADING	 "LOADING"
#define MSG_LINETOGO	 "Line to go to: "
#define MSG_NOHELPFOR	 "No help for '%s'"
#define MSG_REPLACETHIS  "Replace text: "
#define MSG_REPLACEWITH  "Replace with: "
#define MSG_TOENDOFFILE  "To end of file?"
#define MSG_YES 	 "Y"
#define MSG_NO		 "N"
#define MSG_YES_STRING	 "Yes"
#define MSG_NO_STRING	 "No"
#define MSG_OK_STRING	 "OK"
#define MSG_CANCEL_STRING "Cancel"
#define MSG_KEYHELP	 "<help on keys>"
#define MSG_ABOUT	 "<about Minimum Profit>"
#define MSG_EXEC	 "System command: "
#define MSG_CANTEXEC	 "Error executing command."
#define MSG_WORDWRAP	 "Word wrap on column (0, no word wrap): "
#define MSG_TABSIZE	 "Tab size: "
#define MSG_BADMODE	 "Bad mode. Available modes:"
#define MSG_TMPLNOTFOUND "Template file not found (%s)"
#define MSG_SELECTTMPL	 "Select template"
#define MSG_TAGLIST	 "Tag list"
#define MSG_OPENLIST	 "Open documents"
#define MSG_CANCELHINT	 "ESC Cancel"
#define MSG_EXECFUNCTION "Function to execute: "
#define MSG_BADFUNCTION  "Function not found (%s)"
#define MSG_FILESTOGREP  "Files to grep (empty, all): "

#define MSG_FILE_MENU	 "&File"
#define MSG_EDIT_MENU	 "&Edit"
#define MSG_SEARCH_MENU  "&Search"
#define MSG_GOTO_MENU	 "&Go to"
#define MSG_OPTIONS_MENU "&Options"

#define MSG_USAGE_TEXT "\
Usage: mp [options] [file [file ...]]\n\
\n\
Options:\n\
\n\
 -t|--tag [tag] 	Edits the file where tag is defined\n\
 -w|--word-wrap [col]	Sets wordwrapping in column col\n\
 -ts|--tab-size [size]	Sets tab size\n\
 --mouse		Activate mouse usage for cursor positioning\n\
 --col80		Marks column # 80\n\
 -hw|--hardware-cursor	Activates the use of hardware cursor\n\
 --version		Shows version\n\
 -h|--help		This help screen\n\
 -bw|--monochrome	Monochrome\n\
 -ai|--autoindent	Sets automatic indentation mode\n\
 -nt|--no-transparent	Disable transparent mode (eterm, aterm, etc.)\n\
 -l|--lang [lang]	Language selection\n\
 -m|--mode [mode]	Syntax-hilight mode\n\
			"

#define MSG_EMPTY_TEMPLATE "\
%%Empty template file\n\
\n\
This template file is empty. To create templates, write a name for\n\
each one (marked by two % characteres together in the beginning of\n\
the line) and a text body, delimited by the next template name\n\
or the end of file. By selecting a template from the list (popped up\n\
by Ctrl-U), it will be inserted into the current text.\n"

#define MSG_EMPTY_CONFIG_FILE "\
#\n\
# Minimum Profit Config File\n\
#\n\
\n"

extern int _mpi_language;

char * L(char * msgid);
void mpl_set_language(char * langname);
char * mpl_enumerate_langs(void);
