dnl **************************************************************************
dnl (c) 2009 Ole Reinhardt <ole.reinhardt@embedded-it.de>
dnl **************************************************************************
dnl Process this file with autoconf to produce a configure script.

AC_INIT(qnutconf, 3.0)
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)
AC_CONFIG_HEADERS([config.h])
AM_MAINTAINER_MODE

AC_PROG_CXX






AM_PROG_LIBTOOL



PKG_CHECK_MODULES(QNUTCONF, [QtCore lua5.1 QtGui ])




AC_OUTPUT([
Makefile
src/Makefile

])
