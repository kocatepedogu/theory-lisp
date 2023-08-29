AC_INIT([theory-lisp], [0.1.0])
AM_INIT_AUTOMAKE([foreign -Wall -Werror subdir-objects])
AC_PROG_CC
AC_PROG_CC_C99
AM_PROG_AR
LT_INIT
AC_CONFIG_MACRO_DIRS([m4])
PKG_CHECK_MODULES([CHECK], [check >= 0.9.6])
AC_SUBST([CHECK_CFLAGS])
AC_SUBST([CHECK_LIBS])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile tests/Makefile])
AC_OUTPUT
