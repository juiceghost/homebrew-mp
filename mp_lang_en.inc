/*

    mp_lang_en.inc

    English messages

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
/* -*- Mode: C */

static char * _en_ids[] = { "en", "english", "en_US", "en_UK", "EN", NULL };

static struct lang_str _en_strs[] = 
{
	{ "move-up",		 "Line up" },
	{ "move-down",		 "Line down" },
	{ "move-left",		 "Character left" },
	{ "move-right", 	 "Character right" },
	{ "move-word-left",	 "Word left" },
	{ "move-word-right",	 "Word right" },
	{ "move-eol",		 "End of line" },
	{ "move-bol",		 "Beginning of line" },
	{ "move-eof",		 "End of document" },
	{ "move-bof",		 "Beginning of document" },
	{ "move-page-up",	 "Page up" },
	{ "move-page-down",	 "Page down" },
	{ "goto",		 "Go to line..." },
	{ "insert-line",	 "Insert line" },
	{ "insert-tab", 	 "Insert tab" },
	{ "delete",		 "Delete char over cursor" },
	{ "delete-left",	 "Delete char to the left of cursor" },
	{ "delete-line",	 "Delete line" },
	{ "mark",		 "Mark beginning/end of block" },
	{ "unmark",		 "Unmark block" },
	{ "copy",		 "Copy block" },
	{ "paste",		 "Paste block" },
	{ "cut",		 "Cut block" },
	{ "seek",		 "Search text..." },
	{ "seek-next",		 "Search next" },
	{ "replace",		 "Replace..." },
	{ "replace-all",	 "Replace in all..." },
	{ "next",		 "Next" },
	{ "new",		 "New" },
	{ "open",		 "Open..." },
	{ "reopen",		 "Reopen..." },
	{ "save",		 "Save..." },
	{ "save-as",		 "Save as..." },
	{ "close",		 "Close" },
	{ "open-under-cursor",	 "Open file under cursor" },
	{ "zoom-in",		 "Increment font size" },
	{ "zoom-out",		 "Decrement font size" },
	{ "toggle-insert",	 "Insert/Overwrite" },
	{ "toggle-case",	 "Case sensitive" },
	{ "toggle-save-tabs",	 "Save tabs" },
	{ "toggle-cr-lf",	 "Save LF as CR/LF" },
	{ "toggle-auto-indent",  "Automatic indentation" },
	{ "toggle-column-80",	 "Mark column # 80" },
	{ "toggle-regex",	"Use regular expressions" },
	{ "help",		 "Help for word under cursor" },
	{ "exec-command",	 "Run system command..." },
	{ "document-list",	 "Document list" },
	{ "find-tag",		 "Search tag..." },
	{ "insert-template",	 "Insert template..." },
	{ "completion", 	 "Complete tag..." },
	{ "edit-templates-file", "Edit templates file" },
	{ "edit-config-file",	 "Edit configuration file" },
	{ "set-word-wrap",	 "Word wrap..." },
	{ "set-tab-size",	 "Tab size..." },
	{ "menu",		 "Menu" },
	{ "about",		 "About..." },
	{ "exit",		 "Exit" },
	{ "play-macro", 	 "Play macro" },
	{ "record-macro",	 "Record macro" },
	{ "key-help",		 "Help on keys" },
	{ "mouse-position",	 "Position cursor with mouse" },
	{ "exec-function",	 "Execute editor function..." },
	{ "sync",		 "Save modified texts" },
	{ "grep",		 "Grep (find inside) files..." },
	{ "mark-match", 	 "Select last successful search" },

	{ NULL, NULL }
};

