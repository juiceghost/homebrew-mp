/*

    mp_synhi.c

    Syntax higlighters.

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
#include <string.h>
#include <stdlib.h>
#include "mp_core.h"
#include "mp_video.h"
#include "mp_conf.h"
#include "mp_synhi.h"


/******************
	Data
*******************/


/* plain text, no decorations */

static char * _plain_exts[] = { ".txt", NULL };
static char * _plain_magics_1[] = { "-*- Mode: plain", NULL };

#ifndef NO_SYNHI

/* html, xml, sgml */

static char * _html_exts[] = { ".html", ".htm", ".artemus", NULL };
static char * _html_magics_1[] = { "-*- Mode: HTML", NULL };
static char * _html_magics_2[] = { "<html>", "<body>", "<i>", "<b>", NULL };
static char * _html_tokens[] = { "a", "abbr", "acronym", "address",
				"area", "b", "base", "bdo", "big",
				"blockquote", "body", "br", "button",
				"caption", "center", "cite", "code", "col",
				"colgroup", "dd", "del", "dfn", "div",
				"dl", "dt", "em", "fieldset", "form",
				"h1", "h2", "h3", "h4", "h5", "h6",
				"head", "hr", "html", "i", "img",
				"input", "ins", "kbd", "label", "legend",
				"li", "link", "map", "meta", "noscript",
				"object", "ol", "optgroup", "option",
				"p", "param", "pre", "q", "samp",
				"script", "select", "small", "span",
				"strong", "style", "sub", "sup", "table",
				"tbody", "td", "textarea", "tfoot", "th",
				"thead", "title", "tr", "tt", "ul",
				"var", "!DOCTYPE", "class", "type",
				"cellspacing", "cellpadding",
				"href", "align", "valign", "name", "lang",
				"value", "action", "width", "height",
				"content", "http-equiv", "src", "alt",
				"bgcolor", "text", "link", "vlink", "alink",
				NULL };
static char * _html_c_start[] = { "!--", NULL };
static char * _html_c_end[] = { "--", NULL };

static char * _xml_exts[] = { ".xml", ".sgml", NULL };
static char * _xml_magics_1[] = { "-*- Mode: XML", NULL };
static char * _xml_magics_2[] = { "<?xml", NULL };

/* C, C++ */

static char * _c_exts[] = { ".c", ".h", ".cpp", ".hpp", ".c++", NULL };
static char * _c_magics_1[] = { "-*- Mode: C", NULL };
static char * _c_magics_2[] = { "#include", "int main", "/*" , NULL };
static char * _c_tokens[] = { "for", "while", "if", "switch", "case", "do",
			      "else", "break", "continue", "return",
			      "default", "goto", "main", "fopen", "fclose",
			      "fgets", "fgetc", "fputs", "fputc", "fprintf",
			      "putc", "printf", "sprintf", "strcpy", "strcat",
			      "strcmp", "strncmp", "strtok", "stricmp", "strchr",
			      "strrchr", "strlen", "memcmp", "memcpy", "malloc",
			      "free", "strncpy", "strncat", "snprintf", "strstr",
			      "memset", "memcpy", "#include", "#define", "#ifdef",
			      "va_start", "va_end", "vsprintf", "vsnprintf", "atoi",
			      "qsort", "bsearch", "getenv", "fscanf", "popen",
			      "realloc", "fread", "fwrite", "fseek", "sscanf",
			      "#ifndef", "#if", "#else", "#elif", "#endif", "#pragma",
			      "#undef", "{", "}", "putchar", "fflush", NULL };
static char * _c_vars[]= { "char", "int", "long", "struct", "union", "const",
			   "void", "unsigned", "signed", "auto", "volatile",
			   "enum", "typedef", "float", "double", "extern",
			   "register", "short", "sizeof", "static", "far",
			   "near", "defined", "va_list", NULL }; 
static char * _c_helpers[] = { "man 2 %s", "man 3 %s", "./localhelp.sh %s",
				"man ./%s.3", "man ./man/%s.3", NULL };
static char * _c_c_start[] = { "/*", "//", NULL };
static char * _c_c_end[] = { "*/", "\n", NULL };

/* Perl */

static char * _perl_exts[] = { ".pl", ".pm", ".PL", NULL };
static char * _perl_magics_1[] = { "#!/usr/bin/perl", "-*- Mode: Perl", NULL };
static char * _perl_tokens[] = { "for", "if", "next", "last", "else", "elsif",
				 "unless", "while", "shift", "unshift", "push",
				 "pop", "delete", "new", "bless", "return",
				 "foreach", "keys", "values", "sort", "grep",
				 "tr", "length", "system", "exec", "fork", "map",
				 "print", "write", "open", "close", "chop",
				 "chomp", "exit", "sleep", "split", "join",
				 "sub", "printf", "sprintf", "s", "tr", "glob",
				 "{", "}", NULL };
static char * _perl_vars[] = { "scalar", "my", "local", "undef", "defined",
			       "use", "package", "require", "pod", "head1",
			       "head2", "item", "cut", "over", "back",
			       "ref", "qw", "qq", "eq", "ne", "or", "exists",
			       "and", "not", "import", "our", "::", "=", "=~",
			       ".=", "==", ">", "<", ">=", "<=", NULL };
static char * _perl_helpers[] = { "perldoc -f %s", NULL };
static char * _perl_c_start[] = { "#", "\n=", NULL };
static char * _perl_c_end[] = { "\n", "=cut", NULL };

/* Man Pages */

static char * _man_exts[] = { ".man", NULL };
static char * _man_tokens[] = { "NAME", "SYNOPSIS", "DESCRIPTION", 
				"OPTIONS", "BUGS", "AUTHOR", "FILES",
				"SEE", "ALSO", "RETURN", "VALUE",
				"NOTES", "PORTABILITY", NULL };

static char * _man_helpers[] = { "man %s", "./localhelp.sh %s",
				"man ./%s.3", "man ./man/%s.3", NULL };

/* RFC822 mail message */

static char * _rfc822_tokens[] = { "From:", "To:", "Cc:", "Bcc:", "Subject:",
				   "Reply-To:", "In-Reply-To:", "Received:",
				   "Date:", "Message-Id:", "Return-Path:",
				   "Apparently-To:", "Delivered-To:",
				   "Organization:", NULL };
static char * _rfc822_magics_2[] = { "From:", "Subject:", "Reply-To:",
				   "Message-Id:", "Return-Path:", "Apparently-To:",
				   "Delivered-To:", NULL };
static char * _rfc822_c_start[] = { ">", NULL };
static char * _rfc822_c_end[] = { "\n", NULL };

/* Shell */
/* Shell syntax hilighter by Sergey P. Vazulia */

static char * _sh_exts[] = { ".sh", NULL };
static char * _sh_magics_1[] = { "!/bin/sh", "!/bin/bash", "-*- Mode: sh", NULL };
static char * _sh_tokens[] = { "if", "then", "else", "elif", "fi", "case", "do",
				"done", "esac", "for", "until", "while", "break",
				"in", "source", "alias", "cd", "continue",
				"echo", "eval", "exec", "exit", "export", "kill",
				"logout", "printf", "pwd", "read", "return",
				"shift", "test", "trap", "ulimit", "umask",
				"unset", "wait", NULL };
static char * _sh_vars[] = { "$", "local", "let", "set", NULL };
static char * _sh_helpers[] = { "man %s", NULL };
static char * _sh_c_start[] = { "#", NULL };
static char * _sh_c_end[] = { "\n", NULL };

/* Ruby */
/* Ruby syntax hilighter by Gabriel Emerson */
 
static char * _ruby_exts[] = { ".rb", NULL };
static char * _ruby_magics_1[] = { "#!/usr/bin/ruby", "-*- Mode: Ruby", NULL };
static char * _ruby_tokens[] = { "BEGIN", "END", "alias", "and", "begin",
				 "break", "case", "class", "def", "defined",
				 "do", "else", "elsif", "end", "ensure", 
				 "false", "for", "if", "in", "module", "next",
				 "nil", "not", "or", "redo", "rescue", "retry",
				 "return", "self", "super", "then", "true",
				 "undef", "unless", "until", "when", "while",
				 "yield", NULL };
static char * _ruby_vars[] = { "load", "require","%q", "%!", "%Q", "%r", 
			       "%x", "=begin", "=end", NULL };
static char * _ruby_helpers[] = { "ri %s", NULL };
static char * _ruby_c_start[] = { "#", NULL };
static char * _ruby_c_end[] = { "\n", NULL };

/* PHP */
/* PHP syntax hilighter by Geoff Youngs */

static char * _php_exts[] = { ".php", ".inc", ".php4", ".php3", NULL };
static char * _php_magics_1[] = { "-*- Mode: PHP", NULL };
static char * _php_magics_2[] = { "<?php", "<?",  NULL };
static char * _php_tokens[] = { "^=", "<<=", "<<", "<=", "===", "==", "=>", "+",
				">=", ">>=", ">>", "|=", "||", "-=", "--", "-",
				"::", "!==", "!=", "<>", "?>", "/=", "/*", "*",
				".=", "{", "}", "=", "*=", "&=", "&&",
				"%=", "%>" "+=", "++", "and", "array", "as",
				"bool", "boolean", "break", "case", "class",
				"const", "continue", "declare", "default",
				"die", "do", "double", "echo", "else", "elseif",
				"empty", "enddeclare", "endfor", "endforeach",
				"endif", "endswitch", "endwhile", "eval",
				"exit", "extends", "__FILE__", "float", "for",
				"foreach", "function", "cfunction", "global",
				"if", "include", "include_once", "int",
				"integer", "isset", "__LINE__", "list", "new",
				"object", "old_function", "or", "print", "real",
				"require", "require_once", "return",
				"static", "string", "switch", "unset", "use",
				"var", "while", "xor", NULL };
static char * _php_vars[]= { "$", "?php", "?", NULL }; 
static char * _php_helpers[] = { "man %s", NULL };
static char * _php_c_start[] = { "/*", "//", "!--", "#", NULL };
static char * _php_c_end[] = { "*/", "\n", "--", "\n", NULL };

/* .po (gettext) files */

static char * _po_exts[] = { ".po", ".pot", ".pox", NULL };
static char * _po_magics_1[] = { "-*- Mode: po", NULL };
static char * _po_magics_2[] = { "msgid", "msgstr", NULL };
static char * _po_tokens[] = { "msgid", "msgstr", NULL };
static char * _po_c_start[] = { "#", NULL };
static char * _po_c_end[] = { "\n", NULL };

/* SQL */
/* SQL syntax hilighter by Gabriel Emerson */

static char * _sql_exts[] = { ".sql", NULL };
static char * _sql_magics_1[] = { NULL };
static char * _sql_tokens[] = { "alter", "analyze", "audit", "comment",
		"commit", "create", "delete", "drop", "execute", "explain", "grant",
		"insert", "lock", "noaudit", "rename", "revoke", "rollback",
		"savepoint", "select", "set", "truncate", "update", 
		"access", "add", "as", "asc", "begin", "by", "check", "cluster",
		"column", "compress", "connect", "current", "cursor", 
		"decimal", "default", "desc", "else", "elsif", "end",
		"exception", "exclusive", "file", "for", "from", "function",
		"group", "having", "identified", "if", "immediate", "increment",
		"index", "initial", "into", "is", "level", "loop", "maxextents",
		"mode", "modify", "nocompress", "nowait", "of", "offline",
		"on", "online", "start", "successful", "synonym", "table", 
		"then", "to", "trigger", "uid", "unique", "user", "validate",
		"values", "view", "whenever", "where", "with", "option", 
		"order", "pctfree", "privileges", "procedure", "public", "resource",
		"return", "row", "rowlabel", "rownum", "rows", "session", 
		"share", "size", "smallint", "type", "using", NULL };
static char * _sql_vars[] = { "boolean", "char", "character", "date", "float",
		"integer", "long", "mlslabel", "number", "raw", "rowid", "varchar",
		"varchar2", "varray", NULL };
static char * _sql_helpers[] = { NULL };
static char * _sql_c_start[] = { "--", NULL };
static char * _sql_c_end[] = { "\n", NULL };

/* PostScript */

static char * _ps_exts[] = { ".ps", ".eps", NULL };
static char * _ps_magics_1[] = { "-*- Mode: ps", "%!PS-Adobe-", NULL };
static char * _ps_magics_2[] = { "findfont", "showpage", NULL };
static char * _ps_tokens[] = { "def", "if", "ifelse", "forall", "for",
	"repeat", "bind", "loop", "and", "or", "null", "exit", NULL };
static char * _ps_vars[] = { "{", "}", "[", "]", NULL };

/*
static char * _ps_tokens[] = { "def", "pop", "exch", "false", "true",
	"roll", "add", "sub", "mul", "div", "idiv", "ne", "eq", "gt", "lt",
	"dup", "if", "ifelse", "forall", "for", "repeat", "neg", "bind",
	"array", "copy", "get", "put", "load", "aload", "where", "begin", "end",
	"and", "or", "null", "loop", "exit", NULL };
static char * _ps_vars[] = { "gsave", "grestore", "newpath", "closepath",
	"clippath", "moveto", "rmoveto", "charpath", "flattenpath","findfont",
	"scalefont", "definefont", "setfont", "dict", "put", "length",
	"setgray", "show", "stroke", "setlinewidth", "clip", "showpage",
	"lineto", "rlineto", "stringwidth", "arc", "curveto", "fill", "translate",
	"maxlength", "scale", "index", "transform", "exec",
	"currentpoint", "rotate", "arcto", NULL };
*/

static char * _ps_c_start[] = { "%", NULL };
static char * _ps_c_end[] = { "\n", NULL };

/* Python (Richard Harris) */

static char * _python_exts[] = { ".py", NULL };
static char * _python_magics_1[] = { "#!/usr/bin/python", "-*- Mode: Python", NULL };
static char * _python_tokens[] = { "and", "assert", "break", "class", "continue",
		"def", "del", "elif", "else", "except", "exec", "finally",
		"for", "from", "if", "import", "in", "is", "lambda", "not",
		"or", "pass", "print", "raise", "return", "try", "while",
		"yield", NULL };
static char * _python_vars[] = { "global","%", "[", "]", "{", "}", NULL };
static char * _python_c_start[] = { "#", NULL };
static char * _python_c_end[] = { "\n", NULL };

/* Gentoo ebuilds by Lars Strojny */

static char * _ebuild_exts[] = { ".ebuild", NULL };
static char * _ebuild_magics_1[] = { "-*- Mode: ebuild", NULL };
static char * _ebuild_magics_2[] = { "src_compile", "src_install", "emake",
		"src_unpack", "pkg_postinst", "pkg_setup", "pkg_postrm", 
		"kill_gconf", "pkg_preinst", "get_xft_setup", "check_licence", NULL };
static char * _ebuild_tokens[] = { "src_compile", "src_install", "emake",
		"src_unpack", "pkg_postinst", "pkg_setup", "pkg_postrm", 
		"kill_gconf", "pkg_preinst", "get_xft_setup", "check_licence", 
		"insinto", "dodoc", "make", "install", "dodir", "doins", 
		"dohtml", NULL };
static char * _ebuild_vars[] = { "$", "{", "}", NULL };
static char * _ebuild_c_start[] = { "#", NULL };
static char * _ebuild_c_end[] = { "\n", NULL };
static char * _ebuild_helpers[] = { "man emerge", "man ebuild", "man make.conf",
		"man make.conf",  NULL };

/* grub.conf Grub-config-file (Lars Strojny) */

static char * _grub_exts[] = { ".conf", ".lst", NULL };
static char * _grub_magics_1[] = { "-*- Mode: Grub", NULL };
static char * _grub_tokens[] = { "timeout", "default", "title", "root",
		"vga", "password --md5", "password", "kernel", "module", 
		"fallback", "makeactive", "chainloader", "color", "setup", NULL };
static char * _grub_c_start[] = { "#", NULL };
static char * _grub_c_end[] = { "\n", NULL };


#endif /* NO_SYNHI */


struct mps_synhi _mps_synhi[]=
{
	/* None */
	{ "Plain", "plain", NULL, NULL, NULL, NULL,
		_plain_magics_1, NULL, _plain_exts, NULL, NULL, NULL,
		NULL, 0, 0 },


#ifndef NO_SYNHI

	/* C/C++: usual quotes */
	{ "C/C++", "c", "\"'", "\"'", _c_c_start, _c_c_end,
		_c_magics_1, _c_magics_2, _c_exts, _c_tokens,
		_c_vars, _c_helpers, NULL, 1, 1  },

	/* Perl: usual quotes + backticks */
	{ "Perl", "perl", "\"'`", "\"'`", _perl_c_start, _perl_c_end,
		_perl_magics_1, NULL, _perl_exts, _perl_tokens,
		_perl_vars, _perl_helpers, " \r\n:)!-;,([]|+&\"\t$@%<>:", 1, 1 },

	/* Shell: usual quotes */
	/* Shell syntax hilighter by Sergey P. Vazulia */
	{ "Shell", "sh", "\"'`{([", "\"'`})]", _sh_c_start, _sh_c_end,
		_sh_magics_1, NULL, _sh_exts, _sh_tokens, _sh_vars,
		_sh_helpers, NULL, 1, 0 },

	/* Ruby: usual quotes + backticks */
	/* Ruby syntax hilighter by Gabriel Emerson */
	{ "Ruby", "ruby", "\"'`", "\"'`", _ruby_c_start, _ruby_c_end,
		_ruby_magics_1, NULL, _ruby_exts, _ruby_tokens,
		_ruby_vars, _ruby_helpers, NULL, 1, 0 },

	/* PHP: usual quotes + backticks */
	/* PHP syntax hilighter by Geoff Youngs */
	{ "PHP", "php", "\"'`", "\"'`", _php_c_start, _php_c_end,
		_php_magics_1, _php_magics_2, _php_exts, _php_tokens,
		_php_vars, _php_helpers, " \r\n<>:)=;,([]|+&\"\t$", 1, 0  },

	/* SQL92: single quotes */
	/* SQL92 syntax hilighter by Gabriel Emerson */
	{ "SQL", "sql", "\"'", "\"'", _sql_c_start, _sql_c_end,
		_sql_magics_1, NULL, _sql_exts, _sql_tokens,
		_sql_vars, _sql_helpers, NULL, 0, 0 },

	/* PostScript */
	{ "PostScript", "ps", "(/", ") ", _ps_c_start, _ps_c_end,
		_ps_magics_1, _ps_magics_2, _ps_exts, _ps_tokens,
		_ps_vars, NULL, " \r\n\t{}()[]\"", 1, 1  },

        /* Python (Richard Harris) */
        { "Python", "python", "\"`_", "\"`_", _python_c_start, _python_c_end,
                _python_magics_1, NULL, _python_exts, _python_tokens,
                _python_vars, NULL, NULL, 1, 1 },

	/* HTML */
	{ "HTML", "html", "{&\"'", "};\"'", _html_c_start, _html_c_end,
		_html_magics_1, _html_magics_2, _html_exts,
		_html_tokens, NULL, NULL, " \r\n/<>:)=;,([]|+&\"\t", 0, 1 },

	/* XML/SGML */
	{ "XML/SGML", "xml", "<&\"'", ">;\"'", _html_c_start, _html_c_end,
		_xml_magics_1, _xml_magics_2, _xml_exts, NULL, NULL, NULL, NULL, 1, 1 },

	/* po files */
	{ "po", "po", "\"`", "\"`", _po_c_start, _po_c_end,
		_po_magics_1, _po_magics_2, _po_exts, _po_tokens,
		NULL, NULL, NULL, 0, 0 },

	/* RFC822 mail message */
	{ "RFC822 Mail", "mail", "<\"", ">\"", _rfc822_c_start, _rfc822_c_end,
		NULL, _rfc822_magics_2, NULL, _rfc822_tokens, NULL,
		NULL, " \r\n\t", 0, 0 },

	/* Man pages: use tokens as magic */
	{ "Man Page", "man", NULL, NULL, NULL, NULL,
		NULL, _man_tokens, _man_exts, _man_tokens,
		NULL, _man_helpers, NULL, 1, 0 },

	/* Gentoo eBuild-files */
	{ "eBuild", "ebuild", "\"'`", "\"'`", _ebuild_c_start, _ebuild_c_end,
		_ebuild_magics_1, _ebuild_magics_2, _ebuild_exts, _ebuild_tokens,
		_ebuild_vars, _ebuild_helpers, "\"'$()[]", 0, 1 },

	/* Grub config-file */
       { "Grub", "grub", "\"'", "\"'", _grub_c_start, _grub_c_end, 
               _grub_magics_1, NULL, _grub_exts, _grub_tokens, 
               NULL, NULL, 0, 0 },
 
	/* ... yours here ... */

#endif /* NO_SYNHI */

	/* End of syntax hilighters */
	{ NULL }
};


/* quoting flag */
int _draw_quoting=0;

/* in comment flag */
int _in_comment=0;

/* line where the comment is started */
int _comment_line;

/* override */
int _override_synhi=0;

/* local tokens */
char * _local_tokens[MAX_LOCAL_TOKENS];
int _num_local_tokens=0;
int _token_reset=1;


/******************
	Code
*******************/

/**
 * _wrd_cmp - qsort compare function
 * @s1: first string
 * @s2: second string
 *
 * Compare function (qsort and bsearch) to search a word.
 * Returns -1, 0, or 1. Internal (do not use).
 */
static int _wrd_cmp(const void * s1, const void * s2)
{
	struct mps_wsynhi * w1, * w2;

	w1=(struct mps_wsynhi *) s1;
	w2=(struct mps_wsynhi *) s2;

	if(w1->word==NULL || w2->word==NULL) return(0);

	return(strcmp(w1->word, w2->word));
}


/**
 * _wrd_icmp - qsort compare function
 * @s1: first string
 * @s2: second string
 *
 * Compare function (qsort and bsearch) to search a word.
 * Case insensitive version.
 * Returns -1, 0, or 1. Internal (do not use).
 */
static int _wrd_icmp(const void * s1, const void * s2)
{
	struct mps_wsynhi * w1, * w2;

	w1=(struct mps_wsynhi *) s1;
	w2=(struct mps_wsynhi *) s2;

	if(w1->word==NULL || w2->word==NULL) return(0);

	return(mpv_strcasecmp(w1->word, w2->word));
}


/**
 * _local_cmp - qsort compare function
 * @s1: first string
 * @s2: second string
 *
 * Compare function (qsort and bsearch) to search a local token.
 * Returns -1, 0, or 1. Internal (do not use).
 */
static int _local_cmp(const void * s1, const void * s2)
{
	char ** w1, ** w2;

	w1=(char **)s1;
	w2=(char **)s2;

	return(strcmp(*w1,*w2));
}


/**
 * mps_is_sep - separator test
 * @c: character to test
 * @synhi: syntax highlighter index
 *
 * Tests if c is a character separator.
 * If no synhi is defined or the synhi separators are
 * set to NULL, mp_is_sep() is used.
 * Returns 1 if it is.
 */
int mps_is_sep(char c, int synhi)
{
	if(c==' ' || c=='\t') return(2);

	if(synhi==0)
		return(mp_is_sep(c));

	--synhi;
	if(_mps_synhi[synhi].seps==NULL)
		return(mp_is_sep(c));

	if(strchr(_mps_synhi[synhi].seps, c)!=NULL)
		return(1);

	return(0);
}


/**
 * mps_auto_synhi - Autodetects syntax hilight mode
 * @txt: text to inspect
 *
 * Tries to detect the type of the document in txt by
 * the file extension and the content in the first
 * lines. If a type is matched, internal synhi index
 * txt->synhi is set.
 */
void mps_auto_synhi(mp_txt * txt)
{
	int n,c;
	mp_txt * ttxt;
	mp_txt * wtxt;
	char * ext;
	char ** ptr;

	if(txt->synhi) return;

	/* overriding mode? */
	if(_override_synhi)
	{
		txt->synhi=_override_synhi;
		return;
	}

	/* nothing by now */
	txt->synhi=0;

	/* transfer some bytes */
	ttxt=mp_get_tmp_txt(txt);
	mp_move_bof(ttxt);
	wtxt=mp_create_sys_txt(NULL);

	for(n=0;n < 1000;n++)
	{
		c=mp_get_char(ttxt);
		mp_put_char(wtxt,c,1);
	}

	mp_end_tmp_txt();

	/* test magic_1 first */
	for(n=0;_mps_synhi[n].type!=NULL && txt->synhi==0;n++)
	{
		for(ptr=_mps_synhi[n].magic_1;ptr!=NULL && *ptr!=NULL;ptr++)
		{
			mp_move_bof(wtxt);

			if(mp_seek_plain(wtxt,*ptr))
			{
				txt->synhi=n+1;
				mp_delete_sys_txt(wtxt);

				mp_log("magic_1: '%s' found; setting synhi to '%s'\n",
					*ptr,_mps_synhi[n].type);

				return;
			}
		}
	}

	/* test extensions next */
	if((ext=strrchr(txt->name,'.'))!=NULL)
	{
		for(n=0;_mps_synhi[n].type!=NULL;n++)
		{
			for(ptr=_mps_synhi[n].exts;
				ptr!=NULL && *ptr!=NULL;ptr++)
			{
				if(strcmp(*ptr,ext)==0)
				{
					txt->synhi=n+1;
					mp_delete_sys_txt(wtxt);

					mp_log("ext: '%s' found; setting synhi to '%s'\n",
					*ptr,_mps_synhi[n].type);

					return;
				}
			}
		}
	}

	/* try desperately magic_2 */
	for(n=0;_mps_synhi[n].type!=NULL && txt->synhi==0;n++)
	{
		for(ptr=_mps_synhi[n].magic_2;ptr!=NULL && *ptr!=NULL;ptr++)
		{
			mp_move_bof(wtxt);

			if(mp_seek_plain(wtxt,*ptr))
			{
				txt->synhi=n+1;

				mp_log("magic_2: '%s' found; setting synhi to '%s'\n",
					*ptr,_mps_synhi[n].type);

				break;
			}
		}
	}

	mp_delete_sys_txt(wtxt);
}


/**
 * mps_word_color - returns the color associated to the word
 * @synhi: syntax hilighter index
 * @word: the word to search
 * @col: first column of word
 * @line: the line being drawn
 *
 * Returns the color associated to the word, or MP_COLOR_NORMAL
 * if the word has nothing special.
 */
int mps_word_color(int synhi, char * word, int col, int line)
{
	int n;
	struct mps_wsynhi wd;
	struct mps_wsynhi * w;

	/* if text hasn't (yet?) a syntax highlighter,
	   just return normal color */
	if(synhi==0 || word[0]=='\0') return(MP_COLOR_NORMAL);

	synhi--;

	/* test if inside comments */
	if(_in_comment != -1)
	{
		if(strcmp(_mps_synhi[synhi].c_end[_in_comment],"\n") != 0)
		{
			char * ptr;
			int r=MP_COLOR_COMMENT;

			/* test if this word ends with the comment end */
			if((ptr=strstr(word,_mps_synhi[synhi].c_end[_in_comment]))!=NULL)
				if(strcmp(ptr,_mps_synhi[synhi].c_end[_in_comment])==0)
					_in_comment=-1;

			return(r);
		}
		else
		{
			/* c_end is \n; test if line is different */
			if(line+1 != _comment_line)
				_in_comment=-1;
			else
				return(MP_COLOR_COMMENT);
		}
	}

	/* is this a new start of comment? */
	if(! _draw_quoting && _mps_synhi[synhi].c_start != NULL)
	{
		char * ptr;

		for(n=0;(ptr=_mps_synhi[synhi].c_start[n])!=NULL;n++)
		{
			/* if delimiter starts with \n, match only
			   the beginning of a line */
			if(*ptr=='\n')
			{
				if(col==0)
					ptr++;
				else
					ptr=NULL;
			}

			if(ptr!=NULL && strncmp(word,ptr,strlen(ptr))==0 &&
				strcmp(word,_mps_synhi[synhi].c_end[n]))
			{
				_in_comment=n;
				_comment_line=line+1;
				return(MP_COLOR_COMMENT);
			}
		}
	}

	/* are numbers treated as strings (literals)? */
	if(_mps_synhi[synhi].numbers)
	{
		if(word[0]>='0' && word[0]<='9')
			return(MP_COLOR_STRING);

		if(word[0]=='-' && word[1]>='0' && word[1]<='9')
			return(MP_COLOR_STRING);
	}

	/* test if word is a local token */
	if(bsearch(&word,_local_tokens,_num_local_tokens,
		sizeof(char *),_local_cmp)!=NULL)
			return(MP_COLOR_LOCAL);

	/* if case insensitive and word is all caps... */
	if(_mps_synhi[synhi].casesig)
	{
		for(n=0;word[n];n++)
		{
			if(word[n]=='_')
				continue;
			if(word[n]>='0' && word[n]<='9')
				continue;

			if(word[n]<'A' || word[n]>'Z')
				break;
		}

		if(!word[n]) return(MP_COLOR_CAPS);
	}

	if(_mps_synhi[synhi].wi==0)
		return(MP_COLOR_NORMAL);

	wd.word=word;

	/* test if special word */
	w=bsearch(&wd,_mps_synhi[synhi].w,
		_mps_synhi[synhi].wi,
		sizeof(struct mps_wsynhi),
		_mps_synhi[synhi].casesig ? _wrd_cmp : _wrd_icmp);

	if(w!=NULL)
		return(w->color);

	return(MP_COLOR_NORMAL);
}


/**
 * mps_quoting - Test if we are inside quotes
 * @c: character to test
 * @color: color previously calculated
 * @synhi: syntax hilighter index
 *
 * If current text is between quotes, returns the
 * quoting color.
 */
int mps_quoting(int c, int color, int synhi)
{
	char * ptr1;
	char * ptr2;
	static char _prev_char=' ';
	
	if(synhi==0) return(color);
	if(_in_comment != -1) return(color);

	synhi--;
	if((ptr1=_mps_synhi[synhi].q_start)==NULL) return(color);
	if((ptr2=_mps_synhi[synhi].q_end)==NULL) return(color);

	if(_draw_quoting)
	{
		if(c==_draw_quoting && _prev_char!='\\')
			_draw_quoting='\0';

		if(_prev_char=='\\' && c=='\\')
			_prev_char=' ';
		else
			_prev_char=c;

		color=MP_COLOR_STRING;
	}
	else
	{
		while(*ptr1)
		{
			if(c==*ptr1)
			{
				_draw_quoting=*ptr2;
				color=MP_COLOR_STRING;
				_prev_char=' ';
				break;
			}
			ptr1++;
			ptr2++;
		}
	}

	return(color);
}


/**
 * mps_set_override_mode - Forces the syntax hilight mode
 * @mode: mode name to set
 *
 * Forces the syntax hilight to be the one
 * named as mode. If mode is not found, 0 is returned.
 */
int mps_set_override_mode(char * mode)
{
	int n;

	for(n=0;_mps_synhi[n].type!=NULL;n++)
	{
		if(strcmp(_mps_synhi[n].mode,mode)==0)
		{
			_override_synhi=n+1;
			return(1);
		}
	}

	return(0);
}


/**
 * mps_enumerate_modes - Returns the available synhi modes
 *
 * Returns a pointer to a static buffer containing the names,
 * concatenated by spaces, of the available syntax hilighters.
 */
char * mps_enumerate_modes(void)
{
	static char modes[1024];
	int n;

	/* buffer overflow should be tested */

	modes[0]='\0';
	for(n=0;_mps_synhi[n].type!=NULL;n++)
	{
		if(n) strcat(modes," ");
		strcat(modes,_mps_synhi[n].mode);
	}

	return(modes);
}


/**
 * mps_add_local_token - Adds a local token
 * @token: the token
 *
 * Adds a @token to the local database. This database
 * is sorted and searched to be highlighted with the
 * MP_COLOR_LOCAL color. Called when reading the
 * 'tags' file. If @token is NULL, it's taken as the
 * end of local tokens (and the next calling to this
 * function resets the database to start again).
 */
void mps_add_local_token(char * token)
{
	if(token==NULL)
	{
		qsort(_local_tokens, _num_local_tokens,
			sizeof(char *), _local_cmp);

		_token_reset=1;

		return;
	}

	if(_token_reset)
	{
		_num_local_tokens=0;
		_token_reset=0;
	}

	if(_num_local_tokens==MAX_LOCAL_TOKENS)
		return;

	if(_local_tokens[_num_local_tokens]!=NULL)
		free(_local_tokens[_num_local_tokens]);

	_local_tokens[_num_local_tokens]=(char *)malloc(strlen(token)+1);
	strcpy(_local_tokens[_num_local_tokens],token);

	_num_local_tokens++;
}


/**
 * mps_startup - Syntax hilight engine startup
 *
 * Initializes the syntax highlighting engine.
 */
void mps_startup(void)
{
	int n,m;
	char ** ptr;
	struct mps_synhi * s;
	int overflow=0;

	for(n=0;_mps_synhi[n].type!=NULL;n++)
	{
		m=0;

		s=&_mps_synhi[n];

		for(ptr=s->tokens;ptr!=NULL &&
			*ptr!=NULL;ptr++)
		{
			s->w[m].word=*ptr;
			s->w[m].color=MP_COLOR_TOKEN;

			if(++m == MAX_WORDS_PER_SYNHI)
			{
				overflow++;
				break;
			}
		}
		for(ptr=s->vars;
			ptr!=NULL && *ptr!=NULL;ptr++)
		{
			s->w[m].word=*ptr;
			s->w[m].color=MP_COLOR_VAR;

			if(++m == MAX_WORDS_PER_SYNHI)
			{
				overflow++;
				break;
			}
		}

		s->wi=m;

		qsort(s->w, s->wi, sizeof(struct mps_wsynhi), _wrd_cmp);
	}

	memset(_local_tokens,'\0',sizeof(_local_tokens));

	if(overflow)
	{
		mp_log("WARNING: syntax highlight overflow - Please increment ");
		mp_log("MAX_WORDS_PER_SYNHI in mp_synhi.h and recompile.\n");
	}

	mp_log("Syntax highlight modes: %s\n", mps_enumerate_modes());
}
