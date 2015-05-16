dnl Process this file with autoconf to produce a configure script.
dnl Created by Anjuta application wizard.

AC_INIT(nutconf, 3.0)
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)
AC_CONFIG_HEADERS([config.h])
AM_MAINTAINER_MODE

#AC_PROG_CC
AM_PROG_CC_C_O

# Commandline arguments ###################################################################################


AC_ARG_ENABLE(nutconf-gui,
  AS_HELP_STRING([--disable-nutconf-gui],[disable GUI configuration tool build]),
  [],
  [enable_nutconf_gui=yes]
)


nutconf_gui_ok=no
if test x$enable_nutconf_gui = xyes; then
	WXCONFIG=wx-config
	AC_ARG_WITH(wx-config,
	[[  --with-wx-config=FILE
	    Use the given path to wx-config when determining
	    wxWidgets configuration; defaults to "wx-config"]],
	[
	    if test "$withval" != "yes" -a "$withval" != ""; then
		WXCONFIG=$withval
	    fi
	])
	AC_MSG_CHECKING([wxWidgets version])
	if wxversion=`$WXCONFIG --version`; then
	    AC_MSG_RESULT([$wxversion])
	    nutconf_gui_ok=yes
	else
	    AC_MSG_RESULT([not found])
	    AC_MSG_WARN([wxWidgets is required. Try --with-wx-config.])
	    nutconf_gui_ok=no
	fi
	 
	WX_CPPFLAGS="`$WXCONFIG --cppflags`"
	WX_CXXFLAGS="`$WXCONFIG --cxxflags`"
	WX_LIBS="`$WXCONFIG --libs`"
	 
	AC_SUBST(WX_CPPFLAGS)
	AC_SUBST(WX_CXXFLAGS)
	AC_SUBST(WX_LIBS)
fi

AC_PROG_CPP
AC_PROG_CXX

AM_PROG_LIBTOOL

PKG_CHECK_MODULES(NUTCONF, [lua5.1])

if test x$nutconf_gui_ok = xyes; then
	AC_MSG_NOTICE([nutconfgui is enabled])
else
	AC_MSG_WARN([nutconfgui won't be build])
fi

AM_CONDITIONAL(BUILD_NUTCONF_GUI, test x$nutconf_gui_ok = xyes)

AC_OUTPUT([
Makefile
src/Makefile

])
