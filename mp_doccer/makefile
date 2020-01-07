# -*- Mode: sh

PREFIX=/usr/local
INSTALL=install
VERSION=$(shell ./mp_doccer --version)

install:
	$(INSTALL) -o root -m 755 mp_doccer $(PREFIX)/bin
	$(INSTALL) -o root -d $(PREFIX)/share/doc/mp_doccer
	-$(INSTALL) -o root -m 0644 AUTHORS README COPYING Changelog \
		$(PREFIX)/share/doc/mp_doccer

dist:
	cd ..; ln -s mp_doccer mp_doccer-$(VERSION) ; \
		tar czvf mp_doccer/mp_doccer-$(VERSION).tar.gz --exclude=CVS mp_doccer-$(VERSION)/* ; \
		rm mp_doccer-$(VERSION)
