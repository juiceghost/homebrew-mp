/*

    mp_lang_it.inc

    Italian messages

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

    Italian messages by lesion

*/
/* -*- Mode: C */

static char * _it_ids[] = { "it", "italian", "it_IT", "it_IT", "IT", NULL };

static struct lang_str _it_strs[] = 
{
	{ MSG_YES_STRING,   "Si" },
	{ MSG_NO_STRING,    "No" },
	{ MSG_UNNAMED,	    "<Senza nome>" },
	{ MSG_TEXTTOSEEK,   "Testo da cercare: " },
	{ MSG_TAGTOSEEK,    "Tag da cercare: " },
	{ MSG_NOTFOUND,     "Testo non trovato." },
	{ MSG_TAGNOTFOUND,  "Tag(s) non trovato(i)." },
	{ MSG_FILENAME,     "Nome del file: " },
	{ MSG_SAVECHANGES,  "Il file e' cambiato. Salvare i cambiamenti?" },
	{ MSG_FILENOTFOUND, "File '%s' non trovato." },
	{ MSG_CANTWRITE,    "Impossibile scrivere '%s'." },
	{ MSG_YESNO,	    " [S/N]" },
	{ MSG_ENTER,	    " [INVIO]" },
	{ MSG_HELP,	    "<aiutami su '%s'>" },
	{ MSG_LOADING,	    "CARICAMENTO" },
	{ MSG_LINETOGO,     "Numero riga: " },
	{ MSG_NOHELPFOR,    "Non posso aiutarti per '%s'" },
	{ MSG_REPLACETHIS,  "Cambiare testo: " },
	{ MSG_REPLACEWITH,  "Con: " },
	{ MSG_TOENDOFFILE,  "Fine del file" },
	{ MSG_YES,	    "S" },
	{ MSG_NO,	    "N" },
	{ MSG_KEYHELP,	    "<Aiuto tasti>" },
	{ MSG_ABOUT,	    "<Circa Minimum Profit>" },
	{ MSG_EXEC,	    "Esegui: " },
	{ MSG_CANTEXEC,     "Errore eseguendo il comando." },
	{ MSG_WORDWRAP,     "Word wrap alla colonna:  " },
	{ MSG_TABSIZE,	    "Lunghezza del tab: " },
	{ MSG_BADMODE,	    "Modo errato. modi possibili:" },
	{ MSG_TMPLNOTFOUND, "Impossibile aprire il file dei modelli (%s)" },
	{ MSG_SELECTTMPL,   "Seleziona modello" },
	{ MSG_TAGLIST,	    "Lista dei tags" },
	{ MSG_OPENLIST,     "Documenti aperti" },
	{ MSG_CANCELHINT,   "ESC Cancellare" },
	{ MSG_EXECFUNCTION, "Funzione da eseguire: " },
	{ MSG_BADFUNCTION,  "Funzione non trovata (%s)" },
	{ MSG_FILESTOGREP,  "File dove cercare: " },

	{ "move-up",		 "Freccia su'" },
	{ "move-down",		 "Freccia giu'" },
	{ "move-left",		 "Freccia sinistra" },
	{ "move-right", 	 "Freccia destra" },
	{ "move-word-left",	 "Parola a sinistra" },
	{ "move-word-right",	 "Parola a destra" },
	{ "move-eol",		 "Fine riga" },
	{ "move-bol",		 "Inizio riga" },
	{ "move-eof",		 "Fine documento" },
	{ "move-bof",		 "Inizio documento" },
	{ "move-page-up",	 "Pagina su'" },
	{ "move-page-down",	 "Pagina giu'" },
	{ "goto",		 "Vai a riga..." },
	{ "insert-line",	 "Inserisci riga" },
	{ "insert-tab", 	 "Inserisci tabulazione" },
	{ "delete",		 "Elimina il carattere sopra il cursore" },
	{ "delete-left",	 "Elimina il carattere alla sinistra del cursore" },
	{ "delete-line",	 "Elimina la riga" },
	{ "mark",		 "Marca la fine/inizio del blocco" },
	{ "unmark",		 "Smarca il blocco" },
	{ "copy",		 "Copia il blocco" },
	{ "paste",		 "Incolla il blocco" },
	{ "cut",		 "Taglia il blocco" },
	{ "seek",		 "Cerca testo..." },
	{ "seek-next",		 "Cerca successivo" },
	{ "replace",		 "Sostituisci..." },
	{ "replace-all",	 "Sostituisci tutti..." },
	{ "next",		 "Prossimo" },
	{ "new",		 "Nuovo" },
	{ "open",		 "Apri..." },
	{ "reopen",		 "Riapri..." },
	{ "save",		 "Salva..." },
	{ "save-as",		 "Salva come..." },
	{ "close",		 "Chiudere" },
	{ "open-under-cursor",	 "Apri il file sotto il cursore" },
	{ "zoom-in",		 "Aumenta la grandezza del font" },
	{ "zoom-out",		 "Diminuisci la grandezza del font" },
	{ "toggle-insert",	 "Inserisci/sovrascrivi" },
	{ "toggle-case",	 "Case sensitive" },
	{ "toggle-save-tabs",	 "Salva tabulazioni" },
	{ "toggle-cr-lf",	 "Salva LF come CR/LF" },
	{ "toggle-auto-indent",  "Indentazione automatica" },
	{ "toggle-column-80",	 "Segna colonna 80" },
	{ "help",		 "Aiuto x la parola sotto il cursore" },
	{ "exec-command",	 "Esegui comando di sistema..." },
	{ "document-list",	 "Lista dei documenti" },
	{ "find-tag",		 "Cerca tag..." },
	{ "insert-template",	 "Inserisci modulo..." },
	{ "completion", 	 "Completa tag..." },
	{ "edit-templates-file", "Modifica file dei moduli" },
	{ "edit-config-file",	 "Modifica il file di configurazione" },
	{ "set-word-wrap",	 "Word wrap..." },
	{ "set-tab-size",	 "Lunghezza del tab..." },
	{ "menu",		 "Menu" },
	{ "about",		 "Circa..." },
	{ "exit",		 "Esci" },
	{ "play-macro", 	 "Esegui macro" },
	{ "record-macro",	 "Registra macro" },
	{ "key-help",		 "Aiuto per tasti" },
	{ "mouse-position",	 "Posiziona il cursore col mouse" },
	{ "exec-function",	 "Esegui funzione dell'editor..." },
	{ "sync",		 "Salva il testo modificato" },
	{ "grep",		 "Cerca dentro i file..." },
	{ "mark-match", 	 "Select last successful search" },
	{ "toggle-regex",	"Use regular expressions" },

	{ MSG_FILE_MENU,	 "&Archivo" },
	{ MSG_EDIT_MENU,	 "&Edita" },
	{ MSG_SEARCH_MENU,	 "&Cerca" },
	{ MSG_GOTO_MENU,	 "&Vai A" },
	{ MSG_OPTIONS_MENU,	 "&Opzioni" },

	{ "Unlinked functions",  "Funzione morta" },

	{ MSG_USAGE_TEXT, "\
Uso: mp [opzioni] [file [file ...]]\n\
\n\
Opzioni:\n\
\n\
 -t|--tag [tag] 	Edita il file dove e' definito tag\n\
 -w|--word-wrap [col]	Attiva il wordwrapping alla colonna col\n\
 -ts|--tab-size [size]	Definisce la lunghezza delle tabulazioni\n\
 --mouse		Attiva l'uso del mouse per il posizionamento del cursore\n\
 --col80		Marca la colonna 80\n\
 -hw|--hardware-cursor	Attiva l'uso del cursore hardware\n\
 --version		Mostra la versione\n\
 -h|--help		Questa piccola schermata\n\
 -bw|--monochrome	Bianco e nero\n\
 -ai|--autoindent	Attiva l'indentazione automatica\n\
 -nt|--no-transparent	Disattiva la trasparenza (eterm, aterm, etc.)\n\
 -l|--lang [lang]	Seleziona il linguaggio\n\
 -m|--mode [modo]	Modi del syntax-hilight\n\
			" },

	{ MSG_EMPTY_TEMPLATE, "\
%%File dei moduli vuoto\n\
\n\
Questo file dei moduli e' vuoto. Per creare dei moduli scrivi un nome\n\
per ognuno (marcato con 2 caratteri % all'inizio della riga) e un blocco\n\
di testo. Per richiamare i moduli premi ctrl+U.\n" },

	{ NULL, NULL }
};

