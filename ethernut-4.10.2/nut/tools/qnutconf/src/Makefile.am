## **************************************************************************
## (c) 2009 Ole Reinhardt <ole.reinhardt@embedded-it.de>
## **************************************************************************
## Process this file with automake to produce Makefile.in

ACLOCAL_AMFLAGS = -I m4
MAINTAINERCLEANFILES = Makefile.in

DEFINES       = -DQT_NO_DEBUG -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED -DNUTCONF_VERSION_STR=\"3.0.0\"

## UI compiler

UIC=uic-qt4

ui_%.h : %.ui
	$(UIC) -o "$@" "$<"

clean-ui-extra:
	rm -vf ui_*.h

## MOC compiler    
    
MOC=moc

moc_%.cpp : %.h
	$(MOC) $(DEFINES) -o "$@" "$<"

clean-moc-extra:
	rm -vf moc_*.cpp
    
## Ressources and ressource compiler    

IMAGES =                        \
	images/paste.bmp        \
	images/buildlibary_large_dis.bmp \
	images/search_large.bmp \
	images/library.png      \
	images/save_large.png   \
	images/nutconf.bmp      \
	images/integer.png      \
	images/copy.bmp         \
	images/search_dis.bmp   \
	images/stopbuild.bmp    \
	images/text.png         \
	images/open_large.png   \
	images/search_large_dis.bmp \
	images/library.xpm      \
	images/help_large.bmp   \
	images/help.bmp         \
	images/module.xpm       \
	images/help_large_dis.bmp \
	images/new.bmp          \
	images/cut.bmp          \
	images/module.png       \
	images/buildlibrary_large.png \
	images/cshelp.bmp       \
	images/help_dis.bmp     \
	images/search.bmp       \
	images/save.bmp         \
	images/buildlibary_dis.bmp \
	images/buildlibary.bmp  \
	images/building.bmp     \
	images/enumerated.png   \
	images/building_large.bmp

RCC=rcc

qrc_%.cpp : %.qrc $(IMAGES)
	$(RCC) -o "$@" "$<"

clean-qrc-extra:
	rm -vf qrc_*.cpp
    
## Extra clean targets     
    
clean-am: clean-ui-extra clean-moc-extra clean-qrc-extra

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" $(QNUTCONF_CFLAGS) \
        $(DEFINES) -D_REENTRANT 

AM_CFLAGS =\
	 -Wall\
	 -g

MOC_SOURCES = \
	moc_builder.cpp                  \
	moc_finddialog.cpp               \
	moc_mainwindow.cpp               \
	moc_nutcomponentdelegate.cpp     \
	moc_nutcomponentdetailsmodel.cpp \
	moc_nutcomponentmodelfilterproxy.cpp \
	moc_nutcomponentmodel.cpp        \
	moc_settings.cpp                 \
	moc_settingsdialog.cpp

UI_HEADER = \
	ui_mainwindow.h                  \
	ui_settingsdialog.h              \
	ui_finddialog.h

QRC_SOURCES = \
    qrc_qnutconf.cpp

BUILT_SOURCES =                          \
	$(MOC_SOURCES)                   \
	$(UI_HEADER)			\
    $(QRC_SOURCES)

bin_PROGRAMS = qnutconf
    
nodist_qnutconf_SOURCES =   \
    $(BUILT_SOURCES)

qnutconf_SOURCES =          \
	builder.cpp             \
	builder.h               \
	dirtraverser.cpp        \
	dirtraverser.h          \
	finddialog.cpp          \
	finddialog.h            \
	main.cpp                \
	mainwindow.cpp          \
	mainwindow.h            \
	nutcomponent.c          \
	nutcomponent.h          \
	nutcomponentdelegate.cpp\
	nutcomponentdelegate.h  \
	nutcomponentmodel.cpp   \
	nutcomponentmodel.h     \
	nutcomponentmodelfilterproxy.cpp \
	nutcomponentmodelfilterproxy.h \
	nutcomponentdetailsmodel.cpp   \
	nutcomponentdetailsmodel.h     \
	nutcomponentmodel_p.cpp \
	nutcomponentmodel_p.h   \
	settings.cpp            \
	settings.h              \
	settingsdialog.cpp      \
	settingsdialog.h        \
	systeminfo.cpp          \
	systeminfo.h
    
qnutconf_LDFLAGS = 

qnutconf_LDADD = $(QNUTCONF_LIBS)

EXTRA_DIST = finddialog.ui  mainwindow.ui  settingsdialog.ui qnutconf.qrc

