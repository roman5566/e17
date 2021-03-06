# get rid of that stupid cache mechanism
rm -f config.cache

AC_INIT(rage, 0.3.0.042, enlightenment-devel@lists.sourceforge.net)
AC_PREREQ(2.52)
AC_CONFIG_SRCDIR(configure.ac)

AM_INIT_AUTOMAKE(1.6 dist-bzip2)
AM_CONFIG_HEADER(config.h)
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_C_BIGENDIAN
AC_ISC_POSIX
AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC
AC_C_CONST
AM_ENABLE_SHARED
AM_PROG_LIBTOOL

if test "x${bindir}" = 'x${exec_prefix}/bin'; then
  if test "x${exec_prefix}" = "xNONE"; then
    if test "x${prefix}" = "xNONE"; then
      bindir="${ac_default_prefix}/bin";
    else
      bindir="${prefix}/bin";
    fi
  else
    if test "x${prefix}" = "xNONE"; then
      bindir="${ac_default_prefix}/bin";
    else
      bindir="${prefix}/bin";
    fi
  fi
fi

if test "x${libdir}" = 'x${exec_prefix}/lib'; then
  if test "x${exec_prefix}" = "xNONE"; then
    if test "x${prefix}" = "xNONE"; then
      libdir="${ac_default_prefix}/lib";
    else
      libdir="${prefix}/lib";
    fi
  else
    if test "x${prefix}" = "xNONE"; then
      libdir="${ac_default_prefix}/lib";
    else
      libdir="${prefix}/lib";
    fi
  fi
fi

dnl Set PACKAGE_DATA_DIR in config.h.
if test "x${prefix}" = "xNONE"; then
  AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR, "${ac_default_prefix}/share/${PACKAGE}", "Package installed data destination")
else
  AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR, "${prefix}/share/${PACKAGE}", "Package installed data destination")
fi

dnl Set PACKAGE_BIN_DIR in config.h.
if test "x${bindir}" = 'xNONE'; then
  if test "x${prefix}" = "xNONE"; then
    AC_DEFINE_UNQUOTED(PACKAGE_BIN_DIR, "${ac_default_prefix}/bin", [Install Location for Executables])
  else
    AC_DEFINE_UNQUOTED(PACKAGE_BIN_DIR, "${prefix}/bin", [Install Location for Executables])
  fi
else
  AC_DEFINE_UNQUOTED(PACKAGE_BIN_DIR, "${bindir}", [Install Location for Executables])
fi

dnl Set PACKAGE_LIB_DIR in config.h.
if test "x${libdir}" = 'xNONE'; then
  if test "x${prefix}" = "xNONE"; then
    AC_DEFINE_UNQUOTED(PACKAGE_LIB_DIR, "${ac_default_prefix}/lib", [Install Location for Libraries])
  else
    AC_DEFINE_UNQUOTED(PACKAGE_LIB_DIR, "${prefix}/lib", [Install Location for Libraries])
  fi
else
  AC_DEFINE_UNQUOTED(PACKAGE_LIB_DIR, "${libdir}", [Install Location for Libraries])
fi

dnl Set PACKAGE_SOURCE_DIR in config.h.
packagesrcdir=`cd $srcdir && pwd`
AC_DEFINE_UNQUOTED(PACKAGE_SOURCE_DIR, "${packagesrcdir}", [Source Directory])

PKG_CHECK_MODULES([EINA], eina >= 1.0.0)
PKG_CHECK_MODULES([EET], eet >= 1.4.0)
PKG_CHECK_MODULES([EVAS], evas >= 1.0.0)
PKG_CHECK_MODULES([EDJE], edje >= 1.0.0)
PKG_CHECK_MODULES([ECORE], [
   ecore >= 1.0.0
   ecore-con >= 1.0.0
   ecore-evas >= 1.0.0
   ecore-ipc >= 1.0.0
   ecore-file >= 1.0.0])
PKG_CHECK_MODULES([EMOTION], emotion)

my_libs="-lm "$dlopen_libs""
AC_SUBST(my_libs)

AC_OUTPUT([
Makefile
src/Makefile
src/bin/Makefile
data/Makefile
])
