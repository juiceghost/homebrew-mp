/*

    mp_lang_nl.inc

    Dutch  messages

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

    Dutch messages by Rick van der Zwet.

*/
/* -*- Mode: C */

static char * _nl_ids[] = { "nl", "dutch", "nl_NL", "nl_BE", "NL", NULL };

static struct lang_str _nl_strs[] = 
{
	{ MSG_YES_STRING,	"Ja" },
	{ MSG_NO_STRING,	"Nee" },
	{ MSG_UNNAMED,		"<nieuw bestand>" },
	{ MSG_TEXTTOSEEK,	"Zoektekst: " },
	{ MSG_TAGTOSEEK, 	"Zoek markering: " },
	{ MSG_NOTFOUND, 	"Tekst niet gevonden." },
	{ MSG_TAGNOTFOUND, 	"Markering(en) niet gevonden." },
	{ MSG_FILENAME, 	"Bestandsnaam" },
	{ MSG_SAVECHANGES, 	"Bestand veranderd. Wijzigingen Opslaan?" },
	{ MSG_FILENOTFOUND 	"Bestand '%s' niet vinden." },
	{ MSG_CANTWRITE,	"Kan bestand '%s' niet aanmaken." },
	{ MSG_YESNO, 		" [J/N]" },
	{ MSG_ENTER,	 	" [ENTER]" },
	{ MSG_HELP,	 	"<hulp over '%s'>" },
	{ MSG_LOADING,	 	"LADEN" },
	{ MSG_LINETOGO,	 	"Naar regelnummer: " },
	{ MSG_NOHELPFOR,	"Geen hulp mogelijk voor '%s'" },
	{ MSG_REPLACETHIS,	"Vervang tekst: " },
	{ MSG_REPLACEWITH,	"Vervang met: " },
	{ MSG_TOENDOFFILE,  	"Naar het einde van het bestand?" },
	{ MSG_YES, 	 	"J" },
	{ MSG_NO,	 	"N" },
	{ MSG_KEYHELP,	 	"<hulp voor toetsen>" },
	{ MSG_ABOUT,	 	"<over Morphed Profit>" },
	{ MSG_EXEC,	 	"Systeem commando: " },
	{ MSG_CANTEXEC,	 	"Fout bij uitvoeren commando." },
	{ MSG_WORDWRAP,	 	"Aantal tekens per regel (0, geen nieuwe regels): " },
	{ MSG_TABSIZE,	 	"Tab grootte: " },
	{ MSG_BADMODE,	 	"Verkeerde mode. Beschikbare modes:" },
	{ MSG_TMPLNOTFOUND, 	"Template bestand niet gevonden (%s)" },
	{ MSG_SELECTTMPL,	"Selecteer template" },
	{ MSG_TAGLIST,	 	"Markerings lijst" },
	{ MSG_OPENLIST,	 	"Open bestanden" },
	{ MSG_CANCELHINT,	"^N Annuleren" },
	{ MSG_EXECFUNCTION, 	"Functie om uit te voeren: " }, 
	{ MSG_BADFUNCTION,  	"Functie niet gevonden (%s)" },
	{ MSG_FILESTOGREP,  	"In welke bestanden te zoeken (leeg, alles): " },


	{ "move-up",			"Regel omhoog" },
	{ "move-down",			"Regel omlaag" },
	{ "move-left",			"Teken links" },
	{ "move-right", 		"Teken rechts" },
	{ "move-word-left",		"Woord links" },
	{ "move-word-right",		"Woord rechts" },
	{ "move-eol",			"Einde van de regel" },
	{ "move-bol",			"Begin van de regel" },
	{ "move-eof",			"Einde van het bestand" },
	{ "move-bof",			"Begin van het bestand" },
	{ "move-page-up",		"Pagina omhoog" },
	{ "move-page-down",		"Pagina omlaag" },
	{ "goto",		 	"Ga naar regel..." },
	{ "insert-line",		"Voeg regel toe" },
	{ "insert-tab", 		"Voeg tab toe" },
	{ "delete",		 	"Wis het teken onder de aanwijzer" },
	{ "delete-left",		"Wis het teken links van de aanwijzer" },
	{ "delete-line",		"Wis regel" },
	{ "mark",		 	"Markeer begin/eind van een tekstblok" },
	{ "unmark",		 	"Demarkeer tekstblok" },
	{ "copy",		 	"Kopier tekstblok" },
	{ "paste",		 	"Plak tekstblok" },
	{ "cut",			"Knip tekstblok" },
	{ "seek",			"Zoek tekst..." },
	{ "seek-next",			"Zoek volgende..." },
	{ "replace",			"Vervang..." },
	{ "replace-all",		"Vervang in hele tekst..." },
	{ "next",		 	"Volgend bestand" },
	{ "new",			"Nieuw" },
	{ "open",			"Open..." },
	{ "reopen",			"Heropen..." },
	{ "save",		 	"Opslaan..." },
	{ "save-as",			"Opslaan als..." },
	{ "close",		 	"Sluiten" },
	{ "open-under-cursor",		"Open bestand onder de aanwijzer" },
	{ "zoom-in",			"Vergroot lettertype" },
	{ "zoom-out",			"Verklein lettertype" },
	{ "toggle-insert",		"Invoegen/Overschrijven" },
	{ "toggle-case",	 	"Hoofdletter gevoelig" },
	{ "toggle-save-tabs",		"Bewaar tabs" },
	{ "toggle-cr-lf",		"Bewaar LF als CR/LF" },
	{ "toggle-auto-indent", 	"Automatisch inspringen" },
	{ "toggle-column-80",		"Markeer column # 80" },
	{ "toggle-regex",		"Geavanceerder zoeken" },
	{ "help",			"Hulp voor woord onder de aanwijzer" },
	{ "exec-command",		"Voor systeem commando uit..." },
	{ "document-list",		"Bestand lijst" },
	{ "find-tag",			"Zoek markering..." },
	{ "insert-template",		"Voeg template toe..." },
	{ "completion", 		"Maak markering af..." },
	{ "edit-templates-file",	"Bewerk templates bestand" },
	{ "edit-config-file",		"Bewerk configuratie bestand" },
	{ "set-word-wrap",	 	"Woorden afkappen..." },
	{ "set-tab-size",		"Tab grootte..." },
	{ "menu",			"Menu" },
	{ "about",			"Over..." },
	{ "exit",			"Afsluiten" },
	{ "play-macro", 		"Uitvoeren macro" },
	{ "record-macro",		"Opnemen macro" },
	{ "key-help",			"Hulp voor toetsen" },
	{ "mouse-position",		"Gebruik muis om aan te wijzen" },
	{ "exec-function",		"Voor editor functies uit..." },
	{ "sync",			"Bewaar gewijzigde teksten" },
	{ "grep",			"Zoek in alle bestanden..." },
	{ "mark-match",			"Markeer overeenkomst" },

	{ MSG_FILE_MENU,		"&Bestand" },
	{ MSG_EDIT_MENU,		"&Bewerk" },
	{ MSG_SEARCH_MENU,		"&Zoeken" },
	{ MSG_GOTO_MENU,		"&Ga naar" },
	{ MSG_OPTIONS_MENU,		"&Opties" },

	{ "Unlinked functions", 	 "Unlinked functions" },

	{ MSG_USAGE_TEXT, "\
Gebruik: mp [opties] [bestand [bestand ...]]\n\
\n\
Opties:\n\
\n\
 -t|--tag [markering] 	Bewerk het bestand waar markering is gedefineerd\n\
 -w|--word-wrap [col]	Zet de woordafkapping op col\n\
 -ts|--tab-size [size]	Zet de tab grootte op size\n\
 --alt-color		Alternatief kleurenpatroon (meer contrast)\n\
 --mouse		Activeer muis\n\
 --col80		Markeer column # 80\n\
 -hw|--hardware-cursor	Activeer het gebruik van een hardware aanwijzer\n\
 --version		Laat versie zien\n\
 -h|--help		Dit help scherm\n\
 -bw|--monochrome	Monochroom\n\
 -ai|--autoindent	Automatische indenticatie mode\n\
 -nt|--no-transparent	Zet transparent mode uit (eterm, aterm, etc.)\n\
 -m|--mode [mode]	Code-hilight mode. Beschikbare modes:\n\
			" },

	{ MSG_EMPTY_TEMPLATE, "\
%%Leeg template bestand\n\
\n\
Dit template bestand is leeg.  Om templates te maken, type een naam\n\
voor elke (aangegeven met twee % tekens samen in het begin van de regel)\n\
en een tekstveld (veld tussen twee opvolgende markeringen/einde van \n\
bestand). Bij selecteren een template van de list\n\
zal het toegevoegd worden aan de huidige tekst.\n" },
	{ NULL, NULL }
};
