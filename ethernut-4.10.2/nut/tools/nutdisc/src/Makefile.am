## Process this file with automake to produce Makefile.in

## Created by Anjuta

ACLOCAL_AMFLAGS = -I m4
PROPGRIDLIBNAME = libwx_gtk2_propgrid-2.9.a

MAINTAINERCLEANFILES = Makefile.in

if BUILD_NUTDISC

AM_CPPFLAGS = \
	$(WX_CXXFLAGS) \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" $(NUTDISC_CFLAGS)

AM_CFLAGS =\
	 -Wall \
	 -g \
	 $(WX_CPPFLAGS)

bin_PROGRAMS = nutdisc

nutdisc_SOURCES = \
	discovery.h       \
	mainframe.cpp     \
	mainframe.h       \
	nutdisc.cpp       \
	nutdisc.h         \
	options.cpp	  \
	options.h	  \
	propdialog.cpp    \
	propdialog.h      \
	resource.h        \
	setup.cpp         \
	setup.h           \
	taskbaricon.cpp   \
	taskbaricon.h     \
	udpthread.cpp     \
	udpthread.h       \
	version.h         \
	bitmaps/nutdisc.xpm

nutdisc_CXXFLAGS = @WX_CXXFLAGS@ 

if PROPGRID_LOCAL
nutdisc_LDADD = /usr/local/lib/$(PROPGRIDLIBNAME)
else
nutdisc_LDADD = /usr/lib/$(PROPGRIDLIBNAME)
endif

nutdisc_LDADD += \
	$(WX_LIBS) \
	$(NUTDISC_LIBS)

endif

