dnl Process this file with autoconf to produce a configure script.
dnl Created by Anjuta application wizard.

AC_INIT(ethernut, 4.10.2)
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])
AC_PREREQ(2.59)
AC_COPYRIGHT([Copyright (C) 2005-2012 Ethernut Project])
AC_REVISION($Revision: 3877 $)


AC_CONFIG_SRCDIR(README)
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)
AC_CONFIG_HEADERS([config.h])
#AM_MAINTAINER_MODE

AC_ARG_ENABLE(html-docs,
  AS_HELP_STRING([--disable-html-docs],[disable generation of API documents]),
  [],
  [enable_html_docs=yes]
)

AC_ARG_ENABLE(crurom,
  AS_HELP_STRING([--disable-crurom],[disable crurom build]),
  [],
  [enable_crurom=yes]
)

AC_ARG_ENABLE(nutconf,
  AS_HELP_STRING([--disable-nutconf],[disable configuration tool build]),
  [],
  [enable_nutconf=yes]
)

AC_ARG_ENABLE(qnutconf,
  AS_HELP_STRING([--disable-qnutconf],[disable qt configuration tool build]),
  [],
  [enable_qnutconf=yes]
)

AC_ARG_ENABLE(nutdisc,
  AS_HELP_STRING([--disable-nutdisc],[disable NutOs Discoverer]),
  [],
  [enable_nutdisc=yes]
)

if test "x$enable_html_docs" = "xno"; then
  doxygen_ok=no
else
  AC_CHECK_PROG(HAVE_DOXYGEN,doxygen,true,false)
  if test $HAVE_DOXYGEN = "false"; then
    AC_MSG_WARN([Doxygen not found, will not create API reference.])
    doxygen_ok=no
  else
    doxygen_ver=`doxygen --version`
    doxygen_ver_major=`expr "$doxygen_ver" : '\(@<:@0-9@:>@\)\..*'`
    doxygen_ver_minor=`expr "$doxygen_ver" : '@<:@0-9@:>@\.\(@<:@0-9@:>@\).*'`
    if test $doxygen_ver_major -lt "1" -o $doxygen_ver_minor -lt "4" ; then
      AC_MSG_WARN([Doxygen $doxygen_ver is too old.])
      doxygen_ok=no
    else
      doxygen_ok=yes
    fi
  fi
  AC_CHECK_PROG(HAVE_GRAPHVIZ,[dot],true,false)
  if test $HAVE_GRAPHVIZ = "false"; then
    AC_MSG_WARN([Graphviz not found, will create incomplete API reference.])
  fi
fi

AM_CONDITIONAL(USE_DOXYGEN, test x$doxygen_ok = xyes)
AM_CONDITIONAL(HAVE_GRAPHVIZ, $HAVE_GRAPHVIZ)

AM_CONDITIONAL(ENABLE_CRUROM, test x$enable_crurom = xyes)
AM_CONDITIONAL(ENABLE_NUTCONF, test x$enable_nutconf = xyes)
AM_CONDITIONAL(ENABLE_QNUTCONF, test x$enable_qnutconf = xyes)
AM_CONDITIONAL(ENABLE_NUTDISC, test x$enable_nutdisc = xyes)

if test "x$enable_crurom" = "xyes"; then
  if test -d $srcdir/nut/tools/crurom; then
    AC_CONFIG_SUBDIRS(nut/tools/crurom)
  fi
fi

if test "x$enable_nutconf" = "xyes"; then
  if test -d $srcdir/nut/tools/nutconf; then
    AC_CONFIG_SUBDIRS(nut/tools/nutconf)
  fi
fi

if test "x$enable_qnutconf" = "xyes"; then
  if test -d $srcdir/nut/tools/qnutconf; then
    AC_CONFIG_SUBDIRS(nut/tools/qnutconf)
  fi
fi

if test "x$enable_nutdisc" = "xyes"; then
  if test -d $srcdir/nut/tools/nutdisc; then
    AC_CONFIG_SUBDIRS(nut/tools/nutdisc)
  fi
fi

AC_CONFIG_FILES([
	Makefile
	nut/tools/linux/Makefile
	nut/tools/packaging/ethernut.spec
	nut/doc/Makefile
	nut/doc/gen/Makefile
	nut/doc/gen/nut_en.cfg
	])
AC_OUTPUT

AC_OUTPUT
