## Process this file with automake to produce Makefile.in

## Created by Anjuta

ACLOCAL_AMFLAGS = -I m4
MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
	$(WX_CXXFLAGS) \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" $(NUTCONF_CFLAGS)

AM_CFLAGS =\
	 -Wall \
	 -g \
	 $(WX_CPPFLAGS) \
	 -I$(srcdir)


bin_PROGRAMS = nutconfigure

if BUILD_NUTCONF_GUI

bin_PROGRAMS += nutconf

nutconf_SOURCES = \
	nutconf.cpp       \
	nutconf.h         \
	nutcomponent.c    \
	nutcomponent.h    \
	appoptdlg.cpp     \
	appoptdlg.h       \
	bldoptdlg.cpp     \
	bldoptdlg.h       \
	confitemrender.cpp \
	confitemrender.h  \
	conftreectrl.cpp  \
	conftreectrl.h    \
	conftreemodel.cpp \
	conftreemodel.h   \
	conftreenode.cpp  \
	conftreenode.h    \
	confvalrender.cpp \
	confvalrender.h   \
	ids.h             \
	infowindow.cpp    \
	infowindow.h      \
	mainframe.cpp     \
	mainframe.h       \
	nutconfdoc.cpp    \
	nutconfdoc.h      \
	nutconfhint.cpp   \
	nutconfhint.h     \
	nutconfview.cpp   \
	nutconfview.h     \
	propertylist.cpp  \
	propertylist.h    \
	repoptdlg.cpp     \
	repoptdlg.h       \
	settings.cpp      \
	settings.h        \
	settingsdlg.cpp   \
	settingsdlg.h     \
	stateicontext.cpp \
	stateicontext.h   \
	tooloptdlg.cpp    \
	tooloptdlg.h      \
	utils.cpp         \
	utils.h           \
	finddlg.cpp       \
	finddlg.h         \
	pathvalidator.cpp \
	pathvalidator.h

nutconf_CXXFLAGS = @WX_CXXFLAGS@ 

nutconf_LDFLAGS = 

nutconf_LDADD = \
	$(WX_LIBS) \
	$(NUTCONF_LIBS)

endif

nutconfigure_SOURCES = \
	nutcomponent.c \
	nutcomponent.h

nutconfigure_CPPFLAGS = \
	-DNUT_CONFIGURE_EXEC \
	$(NUTCONF_CFLAGS)

nutconfigure_LDADD = $(NUTCONF_LIBS)

