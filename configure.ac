dnl --------------------------------
dnl Package name and version number
dnl --------------------------------

dnl --------------------------------
dnl Initialization macros.
dnl --------------------------------

AC_INIT([wifip2p], 0.1, morgenro@ibr.cs.tu-bs.de)
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR(wifip2p/wifip2p.h)
AC_CONFIG_HEADER(wifip2p/config.h)
LT_INIT
AM_PATH_CPPUNIT(1.9.6)
AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE

# Test for new silent rules and enable only if they are available
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

dnl --------------------------------
dnl library version number
dnl --------------------------------

#API version
GENERIC_API_VERSION=0.1

#shared library versioning
GENERIC_LIBRARY_VERSION=1:0:0

# define the library name
GENERIC_LIBRARY_NAME=wifip2p
GENERIC_VERSION=0.1
GENERIC_RELEASE=0.1

AC_SUBST(GENERIC_API_VERSION)
AC_SUBST(GENERIC_LIBRARY_VERSION)
AC_SUBST(GENERIC_LIBRARY_NAME)
AC_SUBST(GENERIC_RELEASE)
AC_SUBST(GENERIC_VERSION)

dnl -----------------------------------------------
dnl Checks for programs.
dnl -----------------------------------------------

AC_PROG_CXX
AC_PROG_CC

AC_PROG_INSTALL
AC_LIBTOOL_DLOPEN
AC_PROG_LIBTOOL
AM_SANITY_CHECK

AC_LANG_CPLUSPLUS

AM_CONDITIONAL(WIN32, false)

dnl -----------------------------------------------
dnl Generates Makefiles, configuration files and scripts
dnl -----------------------------------------------

AC_CONFIG_FILES([Makefile \
	wifip2p.pc \
	wifip2p/Makefile \
	tests/Makefile \
	tests/basetest/Makefile])
	
AC_OUTPUT
