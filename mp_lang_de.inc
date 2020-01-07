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
	{ MSG_SAVECHANGES,  "Datei wurde verändert. Sollen die Änderungen gespeichert werden?" },
	{ MSG_FILENOTFOUND, "Datei '%s' nicht gefunden." },
	{ MSG_CANTWRITE,    "Kann Datei '%s' nicht erzeugen." },
	{ MSG_YESNO,	    " [J/N]" },
	{ MSG_ENTER,	    " [EINGABE]" },
	{ MSG_HELP,	    "<Hilfe über '%s'>" },
	{ MSG_LOADING,	    "LADE" },
	{ MSG_LINETOGO,     "Springe zu Zeile: " },
	{ MSG_NOHELPFOR,    "Keine Hilfe über '%s'" },
	{ MSG_REPLACETHIS,  "Ersetze Text: " },
	{ MSG_REPLACEWITH,  "Ersetze durch: " },
	{ MSG_TOENDOFFILE,  "Zum Ende der Datei?" },
	{ MSG_YES,	    "J" },
	{ MSG_NO,	    "N" },
	{ MSG_KEYHELP,	    "<Tastaturkürzel>" },
	{ MSG_ABOUT,	    "<über Minimum Profit>" },
	{ MSG_EXEC,	    "System Kommando: " },
	{ MSG_CANTEXEC,     "Fehler beim Ausführen des Kommandos." },
	{ MSG_WORDWRAP,     "Zeilenumbruch bei Spalte (0, kein Umbruch): " },
	{ MSG_TABSIZE,	    "Tabulatorgröße: " },
	{ MSG_BADMODE,	    "Unbekannter Modus. Verfügbare Modi:" },
	{ MSG_TMPLNOTFOUND, "Vorlage nicht gefunden (%s)" },
	{ MSG_SELECTTMPL,   "Vorlage auswählen" },
	{ MSG_TAGLIST,	    "Tagliste" },
	{ MSG_OPENLIST,     "Öffne Dokumente" },
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
	{ "delete",		 "Lösche Zeichen über dem Cursor" },
	{ "delete-left",	 "Lösche Zeichen links vom Cursor" },
	{ "delete-line",	 "Zeile löschen" },
	{ "mark",		 "Markiere Anfang / Ende eines Blocks" },
	{ "unmark",		 "Blockmarkierung löschen" },
	{ "copy",		 "Block kopieren" },
	{ "paste",		 "Block einfügen" },
	{ "cut",		 "Block ausschneiden" },
	{ "seek",		 "Suche Text..." },
	{ "seek-next",		 "Weitersuchen" },
	{ "replace",		 "Suchen und Ersetzen..." },
	{ "replace-all",	 "In allen Ersetzen..." },
	{ "next",		 "Nächste Datei" },
	{ "new",		 "Neu" },
	{ "open",		 "Öffnen..." },
	{ "reopen",		 "Erneut öffnen..." },
	{ "save",		 "Speichern..." },
	{ "save-as",		 "Speichern Als..." },
	{ "close",		 "Schließen" },
	{ "zoom-in",		 "Erhöhe Schriftgröße" },
	{ "zoom-out",		 "Verringere Schriftgröße" },
	{ "toggle-insert",	 "Umschalten Einfügen / Überschreiben" },
	{ "toggle-case",	 "Groß/Kleinschreibung beachten" },
	{ "toggle-save-tabs",	 "Speichere Tabulatoren" },
	{ "toggle-cr-lf",	 "Speicher LF als CR/LF" },
	{ "toggle-auto-indent",  "Auto Einrücken" },
	{ "toggle-column-80",	 "Markiere Spalte 80" },
	{ "help",		 "Hilfe zu Wort unter dem Cursor" },
	{ "exec-command",	 "System Kommando..." },
	{ "document-list",	 "Dokumente Liste" },
	{ "find-tag",		 "Suche Tag..." },
	{ "insert-template",	 "Vorlage einfügen..." },
	{ "completion", 	 NULL },
	{ "edit-templates-file", "Editiere Vorlage" },
	{ "edit-config-file",	 NULL },
	{ "set-word-wrap",	 "Zeilenumbruch..." },
	{ "set-tab-size",	 "Tabulatorgröße..." },
	{ "menu",		 "Menü" },
	{ "about",		 "Über..." },
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
 -t|--tag [Tag] 	Öffnet die Datei in der Tag definiert ist\n\
 -w|--word-wrap [col]	Setzt den Zeilenumbruch auf Spalte col\n\
 -ts|--tab-size [size]	Setzt Tabulatorgröße auf size\n\
 --mouse		Aktiviere Mausunterstützung zur Cursorplatzierung\n\
 --col80		Markiert Spalte # 80\n\
 -hw|--hardware-cursor	Aktiviert den Hardwarecursor\n\
 --version		Version anzeigen\n\
 -h|--help		Diese Hilfeseite\n\
 -bw|--monochrome	Monochrom\n\
 -ai|--autoindent	Aktiviert das Automatische Einrücken\n\
 -nt|--no-transparent	Deaktiviert transparenten Modus (eterm, aterm, etc.)\n\
 -l|--lang [lang]	Language selection\n\
 -m|--mode [mode]	Syntax-Hervorhebung\n\
			" },

	{ MSG_EMPTY_TEMPLATE, "\
%%Leere Vorlage\n\
\n\
Diese Vorlage ist leer. Um Vorlagen zu erstellen, schreiben Sie für\n\
jedes Name (markiert durch zwei % zusammen am Anfang der Zeile) und den\n\
Textkörper, abgegrenzt durch den nächsten Vorlagennamen oder das Ende\n\
der Datei. Durch das Auswählen einer Vorlage aus der Liste (angezeigt\n\
durch Strg+U) wird sie in den momentanen Text eingefügt.\n" },
	{ NULL, NULL }
};

