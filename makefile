############################################
#
#	Morphed Profit makefile
#
############################################
# -*- Mode: sh

VERSION=3.2.14
PREFIX=/usr/local
SYSTEM=AUTODETECT
CURSES=ncurses
DOCPATH=doc/mp_api
DOCFLAGS=
DOCFORMAT=html1
INSTALL=install

ifeq ($(SYSTEM),AUTODETECT)
	ifdef DJGPP
		SYSTEM=WIN32
	else
	ifeq ($(OSTYPE),win32)
		SYSTEM=WIN32
	else
	ifeq ($(OSTYPE),beos)
		SYSTEM=BEOS
	else
		SYSTEM=GNU
	endif
	endif
	endif
endif

ifeq ($(SYSTEM),GNU)
	DEFS=-Os -Wall -D_LARGEFILE_SOURCE
	# Used to be -g instead of -OS on the line above
	OBJFLAGS=-c
	VIDEO=mpv_curses.o
	LIBS=-l$(CURSES)
	RM=-rm
	BIN=mp gmp
	O=o
endif

ifeq ($(SYSTEM),ARM)                                                                           
        CC=/opt/Embedix/tools/bin/arm-linux-gcc
	DEFS=-g -Wall -D_LARGEFILE_SOURCE                                                      
        OBJFLAGS=-c                                                                            
        VIDEO=mpv_curses.o                                                                     
        LIBS=-l$(CURSES)                                                                       
        RM=-rm                                                                                 
        BIN=mp                                                                             
        O=o                                                                                    
endif

ifeq ($(SYSTEM),WIN32)
	CC=lcc
	DEFS=-O -DREGEX -DHAVE_STRING_H
	OBJFLAGS=
	VIDEO=mpv_win32.obj
	LIBS=
	BIN=wmp.exe
	RM=-del
	O=obj
endif

ifeq ($(SYSTEM),BEOS)
	CC=gcc -g -Wall -DWITHOUT_GLOB -DPOOR_MAN_BOXES -I/boot/home/config/include
	OBJFLAGS=-c
	VIDEO=mpv_curses.o
	LIBS=-L/boot/home/config/lib -l$(CURSES)
	RM=-rm
	BIN=mp
	O=o
endif

OBJS=mp_core.$(O) mp_synhi.$(O) mp_iface.$(O) gnu_regex.$(O) \
	mp_lang.$(O) mp_conf.$(O) mp_func.$(O)

DEFS+=-DVERSION="\"$(VERSION)\""


###############################################################

all: $(BIN)

# general rules
%.$(O): %.c
	$(CC) $(DEFS) $(CFLAGS) $(OBJFLAGS) $<

# dependencies
-include makefile.depend

Changelog:
	rcs2log > Changelog

dep:
	gcc $(DEFS) -MM *.c | sed -e 's/.o:/.$$(O):/' > makefile.depend

###############################################################

# Linux/Unix binaries (console)
mp: $(OBJS) mpv_curses.$(O)
	$(CC) $(DEFS) $(CFLAGS) $(LDFLAGS) \
		$(OBJS) $(VIDEO) $(LIBS) -o mp

# Win32 binaries
wmp.exe: $(OBJS) mpv_win32.$(O) mp_res.res
	lcclnk -s *.obj mp_res.res -subsystem windows -version $(VERSION) -o wmp.exe

# Win32 resource file
mp_res.res: mp_res.rc mp_res.h
	lrc mp_res.rc

mpv_gtk.o: mpv_gtk.c
	$(CC) $(DEFS) `gtk-config --cflags` $(CFLAGS) $(OBJFLAGS) $<

# Gtk binary
gmp: $(OBJS) mpv_gtk.$(O)
	$(CC) $(DEFS) `gtk-config --cflags` $(CFLAGS) \
		`gtk-config --libs` $(LDFLAGS) \
		$(OBJS) mpv_gtk.$(O) -o gmp


###############################################################

install:
	-$(INSTALL) -o root -m 0755 mp $(PREFIX)/bin
	-$(INSTALL) -o root -m 0755 gmp $(PREFIX)/bin
	-$(INSTALL) -o root -d $(PREFIX)/share/doc/mp
	-$(INSTALL) -o root -m 0644 AUTHORS COPYING README README.solaris \
		README.IRIX README.mingw32 Changelog mprc.sample doc/mp_api.html \
		$(PREFIX)/share/doc/mp
	$(MAKE) -C mp_doccer install

clean:
	$(RM) -f *.o *.obj mp gmp *.exe *.gz *.res tags localhelp.sh

docclean:
	rm -f doc/*.html man/*

dist: clean doc Changelog
	cd ..; ln -s mp mp-$(VERSION); \
	tar czvf mp-$(VERSION)/mp-$(VERSION).tar.gz --exclude=CVS mp-$(VERSION)/* ; \
	rm mp-$(VERSION)

win32dist:
	zip mp32x-win32.zip README README.html COPYING Changelog mp.reg wmp.exe mprc.sample mprc-win32.sample


###############################################################

.PHONY: doc

doc: README.html
	-mp_doccer *.c -o $(DOCPATH) -f $(DOCFORMAT) \
		-t "The Morphed Profit API ($(VERSION))" \
		-a 'Angel Ortega - <em>angel@triptico.com</em>' $(DOCFLAGS)

.PHONY: man
man:
	mp_doccer *.c -o man -f man \
	-t "The Morphed Profit API ($(VERSION))" \
	-a 'Angel Ortega <angel@triptico.com>' $(DOCFLAGS)

help:
	mp_doccer *.c -f localhelp \
	-t "The Morphed Profit API ($(VERSION))" \
	-a 'Angel Ortega <angel@triptico.com>'

README.html: README
	-grutatxt < README > README.html

# gtk 2.0 support:
# cc -DVERSION="\"\"" `pkg-config --cflags gtk+-2.0` -c mpv_gtk.c
# cc -DVERSION="\"\"" `pkg-config --libs gtk+-2.0` mp_core.o mp_synhi.o mp_iface.o gnu_regex.o mp_lang.o mp_conf.o mp_func.o mpv_gtk.o -o gmp

gtk2: $(OBJS)
	unset IFS ; $(CC) $(DEFS) `pkg-config --cflags gtk+-2.0` $(CFLAGS) $(OBJFLAGS) -c mpv_gtk.c
	unset IFS ; $(CC) $(DEFS) `pkg-config --libs gtk+-2.0` $(LDFLAGS) \
		$(OBJS) mpv_gtk.$(O) -o gmp
