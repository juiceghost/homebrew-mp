/*

    mp_lang_de.inc

    Deutsche Nachrichten

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

    German messages by Peter Ludikovsky

*/
/* -*- Mode: C */

static char * _de_ids[] = { "de", "german", "de_DE", "de_AT", "DE", NULL };

static struct lang_str _de_strs[] = 
{
	{ MSG_YES_STRING,	"Ja" },
	{ MSG_NO_STRING,	"Nicht" },

	{ MSG_UNNAMED,	    "<unbenannt>" },
	{ MSG_TEXTTOSEEK,   "Zu suchender Text: " },
	{ MSG_TAGTOSEEK,    "ZU suchender Tag: " },
	{ MSG_NOTFOUND,     "Text nicht gefunden." },
	{ MSG_TAGNOTFOUND,  "Tag(s) nicht gefunden." },
	{ MSG_FILENAME,     "Dateinamen eingeben: " },
	{ MSG_SAVECHANGES,  "Datei wurde ver�ndert. Sollen die �nderungen gespeichert werden?" },
	{ MSG_FILENOTFOUND, "Datei '%s' nicht gefunden." },
	{ MSG_CANTWRITE,    "Kann Datei '%s' nicht erzeugen." },
	{ MSG_YESNO,	    " [J/N]" },
	{ MSG_ENTER,	    " [EINGABE]" },
	{ MSG_HELP,	    "<Hilfe �ber '%s'>" },
	{ MSG_LOADING,	    "LADE" },
	{ MSG_LINETOGO,     "Springe zu Zeile: " },
	{ MSG_NOHELPFOR,    "Keine Hilfe �ber '%s'" },
	{ MSG_REPLACETHIS,  "Ersetze Text: " },
	{ MSG_REPLACEWITH,  "Ersetze durch: " },
	{ MSG_TOENDOFFILE,  "Zum Ende der Datei?" },
	{ MSG_YES,	    "J" },
	{ MSG_NO,	    "N" },
	{ MSG_KEYHELP,	    "<Tastaturk�rzel>" },
	{ MSG_ABOUT,	    "<�ber Morphed Profit>" },
	{ MSG_EXEC,	    "System Kommando: " },
	{ MSG_CANTEXEC,     "Fehler beim Ausf�hren des Kommandos." },
	{ MSG_WORDWRAP,     "Zeilenumbruch bei Spalte (0, kein Umbruch): " },
	{ MSG_TABSIZE,	    "Tabulatorgr��e: " },
	{ MSG_BADMODE,	    "Unbekannter Modus. Verf�gbare Modi:" },
	{ MSG_TMPLNOTFOUND, "Vorlage nicht gefunden (%s)" },
	{ MSG_SELECTTMPL,   "Vorlage ausw�hlen" },
	{ MSG_TAGLIST,	    "Tagliste" },
	{ MSG_OPENLIST,     "�ffne Dokumente" },
	{ MSG_CANCELHINT,   "ESC Abbrechen" },
	{ MSG_EXECFUNCTION, "Function to execute: " },
	{ MSG_BADFUNCTION,  "Function not found (%s)" },
	{ MSG_FILESTOGREP,  "Files to grep (empty, all): " },

	{ "move-up",		 "Zeile hoch" },
	{ "move-down",		 "Zeile runter" },
	{ "move-left",		 "Zeichen links" },
	{ "move-right", 	 "Zeichen rechts" },
	{ "move-word-left",	 "Wort nach links" },
	{ "move-word-right",	 "Wort nach rechts" },
	{ "move-eol",		 "Ende der Zeile" },
	{ "move-bol",		 "Anfang der Zeile" },
	{ "move-eof",		 "Ende des Dokuments" },
	{ "move-bof",		 "Anfang des Dokuments" },
	{ "move-page-up",	 "Seite hoch" },
	{ "move-page-down",	 "Seite runter" },
	{ "goto",		 "Gehe zu Zeile..." },
	{ "insert-line",	 NULL },
	{ "insert-tab", 	 "Tabulator" },
	{ "delete",		 "L�sche Zeichen �ber dem Cursor" },
	{ "delete-left",	 "L�sche Zeichen links vom Cursor" },
	{ "delete-line",	 "Zeile l�schen" },
	{ "mark",		 "Markiere Anfang / Ende eines Blocks" },
	{ "unmark",		 "Blockmarkierung l�schen" },
	{ "copy",		 "Block kopieren" },
	{ "paste",		 "Block einf�gen" },
	{ "cut",		 "Block ausschneiden" },
	{ "seek",		 "Suche Text..." },
	{ "seek-next",		 "Weitersuchen" },
	{ "replace",		 "Suchen und Ersetzen..." },
	{ "replace-all",	 "In allen Ersetzen..." },
	{ "next",		 "N�chste Datei" },
	{ "new",		 "Neu" },
	{ "open",		 "�ffnen..." },
	{ "reopen",		 "Erneut �ffnen..." },
	{ "save",		 "Speichern..." },
	{ "save-as",		 "Speichern Als..." },
	{ "close",		 "Schlie�en" },
	{ "zoom-in",		 "Erh�he Schriftgr��e" },
	{ "zoom-out",		 "Verringere Schriftgr��e" },
	{ "toggle-insert",	 "Umschalten Einf�gen / �berschreiben" },
	{ "toggle-case",	 "Gro�/Kleinschreibung beachten" },
	{ "toggle-save-tabs",	 "Speichere Tabulatoren" },
	{ "toggle-cr-lf",	 "Speicher LF als CR/LF" },
	{ "toggle-auto-indent",  "Auto Einr�cken" },
	{ "toggle-column-80",	 "Markiere Spalte 80" },
	{ "help",		 "Hilfe zu Wort unter dem Cursor" },
	{ "exec-command",	 "System Kommando..." },
	{ "document-list",	 "Dokumente Liste" },
	{ "find-tag",		 "Suche Tag..." },
	{ "insert-template",	 "Vorlage einf�gen..." },
	{ "completion", 	 NULL },
	{ "edit-templates-file", "Editiere Vorlage" },
	{ "edit-config-file",	 NULL },
	{ "set-word-wrap",	 "Zeilenumbruch..." },
	{ "set-tab-size",	 "Tabulatorgr��e..." },
	{ "menu",		 "Men�" },
	{ "about",		 "�ber..." },
	{ "exit",		 "Beenden" },
	{ "play-macro", 	 "Makro abspielen" },
	{ "record-macro",	 "Makro aufzeichen" },
	{ "key-help",		 "Tastaturbelegung" },
	{ "mouse-position",	 "Position cursor with mouse" },
	{ "exec-function",	 "Execute editor function..." },
	{ "sync",		 "Save modified texts" },
	{ "grep",		 "Grep (find inside) files..." },
	{ "mark-match", 	 "Select last successful search" },
	{ "toggle-regex",	"Use regular expressions" },

	{ MSG_FILE_MENU,	 "&Datei" },
	{ MSG_EDIT_MENU,	 "&Bearbeiten" },
	{ MSG_SEARCH_MENU,	 "&Suche" },
	{ MSG_GOTO_MENU,	 "&Gehe zu" },
	{ MSG_OPTIONS_MENU,	 "&Optionen" },

	{ MSG_USAGE_TEXT, "\
Aufruf: mp [Optionen] [Datei [Datei ...]]\n\
\n\
Optionen:\n\
\n\
 -t|--tag [Tag] 	�ffnet die Datei in der Tag definiert ist\n\
 -w|--word-wrap [col]	Setzt den Zeilenumbruch auf Spalte col\n\
 -ts|--tab-size [size]	Setzt Tabulatorgr��e auf size\n\
 --mouse		Aktiviere Mausunterst�tzung zur Cursorplatzierung\n\
 --col80		Markiert Spalte # 80\n\
 -hw|--hardware-cursor	Aktiviert den Hardwarecursor\n\
 --version		Version anzeigen\n\
 -h|--help		Diese Hilfeseite\n\
 -bw|--monochrome	Monochrom\n\
 -ai|--autoindent	Aktiviert das Automatische Einr�cken\n\
 -nt|--no-transparent	Deaktiviert transparenten Modus (eterm, aterm, etc.)\n\
 -l|--lang [lang]	Language selection\n\
 -m|--mode [mode]	Syntax-Hervorhebung\n\
			" },

	{ MSG_EMPTY_TEMPLATE, "\
%%Leere Vorlage\n\
\n\
Diese Vorlage ist leer. Um Vorlagen zu erstellen, schreiben Sie f�r\n\
jedes Name (markiert durch zwei % zusammen am Anfang der Zeile) und den\n\
Textk�rper, abgegrenzt durch den n�chsten Vorlagennamen oder das Ende\n\
der Datei. Durch das Ausw�hlen einer Vorlage aus der Liste (angezeigt\n\
durch Strg+U) wird sie in den momentanen Text eingef�gt.\n" },
	{ NULL, NULL }
};

