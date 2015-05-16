## Process this file with automake to produce Makefile.in
## Created by Anjuta

ACLOCAL_AMFLAGS = -I m4
MAINTAINERCLEANFILES = \
	configure \
	Makefile.in \
	depcomp \
	compile \
	config.guess \
	config.sub \
	ltmain.sh \
	config.h.in \
	missing \
	aclocal.m4 \
	install-sh

SUBDIRS = src 

nutconfdocdir = ${prefix}/doc/nutconf
nutconfdoc_DATA = \
	README\
	COPYING\
	AUTHORS\
	ChangeLog\
	INSTALL\
	NEWS


EXTRA_DIST = $(nutconfdoc_DATA)

dist-hook:
	mkdir -p $(distdir)/src/bitmaps
	cp -R $(srcdir)/src/bitmaps/* $(distdir)/src/bitmaps/

# Remove doc directory on uninstall
uninstall-local:
#	-rm -r $(nutconfdocdir)
