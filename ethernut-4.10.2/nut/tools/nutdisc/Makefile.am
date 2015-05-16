## Process this file with automake to produce Makefile.in
## Created by Anjuta

ACLOCAL_AMFLAGS = -I m4
MAINTAINERCLEANFILES = \
	configure \
	Makefile.in \
	depcomp \
	config.guess \
	config.sub \
	ltmain.sh \
	config.h.in \
	missing \
	aclocal.m4 \
	install-sh

SUBDIRS = src 

nutdiscdocdir = ${prefix}/doc/nutdisc

nutdiscdoc_DATA = \
	README\
	COPYING\
	AUTHORS\
	ChangeLog\
	INSTALL\
	NEWS


EXTRA_DIST = $(nutdiscdoc_DATA)


# Remove doc directory on uninstall
uninstall-local:
#	-rm -r $(nutdiscdocdir)

