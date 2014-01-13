#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([ketamarouter], [0.1], [jinoos@gmail.com])
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE
AC_PROG_RANLIB

# Checks for programs.
# AC_PROG_CC
AM_PROG_CC_C_O

# Checks for libraries.
AM_PATH_CHECK()
PKG_CHECK_MODULES([libglib], [glib-2.0 >= 2.28])
LIBS="$LIBS $libglib_LIBS"

AX_CHECK_LIBRARY([LIBEVENT], [event.h], [event], [], [AC_MSG_ERROR([Unable to find libevent])])

PKG_CHECK_MODULES([libevent], [libevent >= 2.0])
LIBS="$LIBS $libevent_LIBS"

AC_CHECK_LIB([check_pic], [main])
AC_CHECK_LIB([rt], [clock_gettime],, [AC_MSG_ERROR([[librt not found]])])
AC_CHECK_LIB([m], [exp],, [AC_MSG_ERROR([[libm not found]])])

# Checks for header files.
AC_CHECK_HEADERS([stddef.h stdint.h stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_FUNC_REALLOC

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
