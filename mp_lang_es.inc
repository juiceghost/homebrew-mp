/*

    mp_lang_es.inc

    Mensajes en espa�ol

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

static char * _es_ids[] = { "es", "spanish", "es_ES", "ES", NULL };

static struct lang_str _es_strs[] = 
{
	{ MSG_YES_STRING,   "S�" },
	{ MSG_NO_STRING,    "No" },
	{ MSG_UNNAMED,	    "<sin nombre>" },
	{ MSG_TEXTTOSEEK,   "Texto a buscar: " },
	{ MSG_TAGTOSEEK,    "Tag a buscar: " },
	{ MSG_NOTFOUND,     "Texto no encontrado." },
	{ MSG_TAGNOTFOUND,  "Tag(s) no encontrado(s)." },
	{ MSG_FILENAME,     "Nombre del fichero: " },
	{ MSG_SAVECHANGES,  "El fichero ha cambiado. �Salvar cambios?" },
	{ MSG_FILENOTFOUND, "Fichero '%s' no encontrado." },
	{ MSG_CANTWRITE,    "Imposible crear fichero '%s'." },
	{ MSG_YESNO,	    " [S/N]" },
	{ MSG_ENTER,	    " [ENTER]" },
	{ MSG_HELP,	    "<ayuda sobre '%s'>" },
	{ MSG_LOADING,	    "CARGANDO" },
	{ MSG_LINETOGO,     "N�mero de l�nea: " },
	{ MSG_NOHELPFOR,    "No hay ayuda sobre '%s'" },
	{ MSG_REPLACETHIS,  "Reemplazar texto: " },
	{ MSG_REPLACEWITH,  "Reemplazar con: " },
	{ MSG_TOENDOFFILE,  "�Hasta el final del fichero?" },
	{ MSG_YES,	    "S" },
	{ MSG_NO,	    "N" },
	{ MSG_KEYHELP,	    "<ayuda sobre teclas>" },
	{ MSG_ABOUT,	    "<acerca de Morphed Profit>" },
	{ MSG_EXEC,	    "Comando del sistema: " },
	{ MSG_CANTEXEC,     "Error ejecutando comando." },
	{ MSG_WORDWRAP,     "Word wrap en columna (0, ninguno): " },
	{ MSG_TABSIZE,	    "Tama�o del tabulador: " },
	{ MSG_BADMODE,	    "Modo incorrecto. Los modos disponibles son:" },
	{ MSG_TMPLNOTFOUND, "Imposible abrir fichero de moldes (%s)" },
	{ MSG_SELECTTMPL,   "Seleccionar molde" },
	{ MSG_TAGLIST,	    "Lista de tags" },
	{ MSG_OPENLIST,     "Documentos abiertos" },
	{ MSG_CANCELHINT,   "ESC Cancelar" },
	{ MSG_EXECFUNCTION, "Funci�n a ejecutar: " },
	{ MSG_BADFUNCTION,  "Funci�n no encontrada (%s)" },
	{ MSG_FILESTOGREP,  "Ficheros en los que buscar (vac�o, todos): " },

	{ "move-up",		 "L�nea arriba" },
	{ "move-down",		 "L�nea abajo" },
	{ "move-left",		 "Car�cter izquierda" },
	{ "move-right", 	 "Car�cter Derecha" },
	{ "move-word-left",	 "Palabra izquierda" },
	{ "move-word-right",	 "Palabra Derecha" },
	{ "move-eol",		 "Fin L�nea" },
	{ "move-bol",		 "Principio L�nea" },
	{ "move-eof",		 "Fin Documento" },
	{ "move-bof",		 "Principio Documento" },
	{ "move-page-up",	 "P�gina arriba" },
	{ "move-page-down",	 "P�gina abajo" },
	{ "goto",		 "Ir a l�nea..." },
	{ "insert-line",	 "Insertar l�nea" },
	{ "insert-tab", 	 "Insertar tabulador" },
	{ "delete",		 "Borrar car�cter en cursor" },
	{ "delete-left",	 "Borrar car�cter a la izquierda del cursor" },
	{ "delete-line",	 "Borrar l�nea" },
	{ "mark",		 "Marcar principio/fin de bloque" },
	{ "unmark",		 "Desmarcar bloque" },
	{ "copy",		 "Copiar bloque" },
	{ "paste",		 "Pegar bloque" },
	{ "cut",		 "Cortar bloque" },
	{ "seek",		 "Buscar texto..." },
	{ "seek-next",		 "Buscar siguiente" },
	{ "replace",		 "Reemplazar..." },
	{ "replace-all",	 "Reemplazar en todos..." },
	{ "next",		 "Siguiente" },
	{ "new",		 "Nuevo" },
	{ "open",		 "Abrir..." },
	{ "reopen",		 "Reabrir..." },
	{ "save",		 "Guardar..." },
	{ "save-as",		 "Guardar c�mo..." },
	{ "close",		 "Cerrar" },
	{ "open-under-cursor",	 "Abrir fichero bajo el cursor" },
	{ "zoom-in",		 "Aumentar tama�o del font" },
	{ "zoom-out",		 "Disminuir tama�o del font" },
	{ "toggle-insert",	 "Insertar/Sobreescribir" },
	{ "toggle-case",	 "Distinguir may�sculas" },
	{ "toggle-save-tabs",	 "Salvar tabuladores" },
	{ "toggle-cr-lf",	 "Salvar LF como CR/LF" },
	{ "toggle-auto-indent",  "Indentaci�n autom�tica" },
	{ "toggle-column-80",	 "Marcar columna n� 80" },
	{ "toggle-regex",	"Usar expresiones regulares" },
	{ "help",		 "Ayuda para la palabra en el cursor" },
	{ "exec-command",	 "Ejecutar comando del sistema..." },
	{ "document-list",	 "Lista de documentos" },
	{ "find-tag",		 "Buscar tag..." },
	{ "insert-template",	 "Insertar molde..." },
	{ "completion", 	 "Completar tag..." },
	{ "edit-templates-file", "Editar fichero de moldes" },
	{ "edit-config-file",	 "Editar fichero de configuraci�n" },
	{ "set-word-wrap",	 "Word wrap..." },
	{ "set-tab-size",	 "Tama�o del tabulador..." },
	{ "menu",		 "Men�" },
	{ "about",		 "Acerca de..." },
	{ "exit",		 "Salir" },
	{ "play-macro", 	 "Ejecutar macro" },
	{ "record-macro",	 "Grabar macro" },
	{ "key-help",		 "Ayuda sobre teclas" },
	{ "mouse-position",	 "Posicionar el cursor con el rat�n" },
	{ "exec-function",	 "Ejecutar funci�n del editor..." },
	{ "sync",		 "Guardar textos modificados" },
	{ "grep",		 "Buscar en ficheros (grep)..." },
	{ "mark-match", 	 "Selecciona la �ltima b�squeda con �xito" },

	{ MSG_FILE_MENU,	 "&Archivo" },
	{ MSG_EDIT_MENU,	 "&Editar" },
	{ MSG_SEARCH_MENU,	 "&Buscar" },
	{ MSG_GOTO_MENU,	 "&Ir A" },
	{ MSG_OPTIONS_MENU,	 "&Opciones" },

	{ "Unlinked functions",  "Funciones sin enlazar" },

	{ MSG_USAGE_TEXT, "\
Uso: mp [opciones] [fichero [fichero ...]]\n\
\n\
Opciones:\n\
\n\
 -t|--tag [tag] 	Edita el fichero donde est� definido tag\n\
 -w|--word-wrap [col]	Activa el wordwrapping en lacolumna col\n\
 -ts|--tab-size [size]	Define el tama�o del tabulador a size\n\
 --mouse		Activa el uso del rat�n para posicionamiento\n\
 --col80		Marca la columna 80\n\
 -hw|--hardware-cursor	Activa el uso del cursor hardware\n\
 --version		Muestra la versi�n\n\
 -h|--help		Esta pantalla de ayuda\n\
 -bw|--monochrome	Monocromo\n\
 -ai|--autoindent	Activa el modo de indentaci�n autom�tica\n\
 -nt|--no-transparent	Desactiva el modo transparente (eterm, aterm, etc.)\n\
 -l|--lang [lang]	Selecci�n de idioma\n\
 -m|--mode [modo]	Modo de syntax-hilight\n\
			" },

	{ MSG_EMPTY_TEMPLATE, "\
%%Fichero de moldes vac�o\n\
\n\
Este fichero de moldes est� vac�o. Para crear moldes, s�lo hay que\n\
escribir un nombre (marcado mediante dos caracteres % seguidos al\n\
principio de la l�nea) y un bloque de texto, delimitado por el\n\
siguiente molde o el final del fichero. Al seleccionar uno de los\n\
moldes desde la lista (abierta mediante Ctrl-U), �ste se insertar�\n\
en el texto activo.\n" },

	{ NULL, NULL }
};

