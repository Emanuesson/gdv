#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])

m4_define(gdv_version, 0.0.1)

AC_INIT([gdv], [gdv_version], [schmidt.emanuel@uni-jena.de],[gdv],[ifk.uni.jena.de/libs/gdv])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR(test-main.c)
AC_CONFIG_MACRO_DIR([m4macros])

# define auxiliary directory
AC_CONFIG_AUX_DIR([aux-build])

# Save this value here, since automake will set cflags later
cflags_set=${CFLAGS:+set}

#AM_INIT_AUTOMAKE([foreign -Wall -Werror subdir-objects])
AM_INIT_AUTOMAKE([1.11 -Wno-extra-portability subdir-objects foreign])
#AM_INIT_AUTOMAKE([1.11 -Wall -Werror subdir-objects])
AM_MAINTAINER_MODE([enable])

# check for programs
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_SYS_LARGEFILE
PKG_PROG_PKG_CONFIG
# needed on osx
AC_PROG_OBJC

# start libtool
LT_PREREQ([2.2.6])
LT_INIT(disable-static)
dnl LT_INIT()

TESTBIN_API_VERSION=0.0
AC_SUBST(TESTBIN_API_VERSION)

AC_CHECK_FUNC(sigaction)
AC_CHECK_LIB(m, floor)

dnl make sure we keep ACLOCAL_FLAGS around for maintainer builds to work
AC_SUBST(ACLOCAL_AMFLAGS, "$ACLOCAL_FLAGS -I m4")

dnl location to install runtime libraries, e.g. ../../lib to install
dnl to /lib if libdir is /usr/lib
AC_ARG_WITH(runtime-libdir,
           [AS_HELP_STRING([--with-runtime-libdir=RELPATH],
                           [install runtime libraries relative to libdir])],
           [],
	   [with_runtime_libdir=""])
TESTBIN_RUNTIME_LIBDIR="$with_runtime_libdir"
ABS_TESTBIN_RUNTIME_LIBDIR="`readlink -f $libdir/$with_runtime_libdir`"
AC_SUBST(TESTBIN_RUNTIME_LIBDIR)
AC_SUBST(ABS_TESTBIN_RUNTIME_LIBDIR)
AM_CONDITIONAL(HAVE_TESTBIN_RUNTIME_LIBDIR, [test "x$with_runtime_libdir" != "x"])

dnl ================================================================
dnl GObject Introspection
dnl ================================================================

dnl GOBJECT_INTROSPECTION_CHECK([0.9.3])

dnl if test "$found_introspection" = "yes"; then
dnl 	enable_introspection=yes
dnl 	AC_DEFINE([ENABLE_INTROSPECTION], [1], [Define to enable GObject Introspection])
dnl else
dnl 	enable_introspection=no
dnl fi

dnl ================================================================
dnl Gettext stuff.
dnl ================================================================

dnl IT_PROG_INTLTOOL([0.50.1])

dnl AM_GNU_GETTEXT_VERSION([0.18])
dnl AM_GNU_GETTEXT([external])

dnl AC_SUBST([GETTEXT_PACKAGE], [gedit])
dnl AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE], "$GETTEXT_PACKAGE", [Define to the gettext package name.])

dnl check for gtk-doc
dnl m4_ifdef([GTK_DOC_CHECK], [
dnl GTK_DOC_CHECK([1.14],[--flavour no-tmpl])
dnl ],[
dnl AM_CONDITIONAL([ENABLE_GTK_DOC], false)
dnl ])

dnl ================================================================
dnl Start of pkg-config checks
dnl ================================================================

PKG_PROG_PKG_CONFIG(0.16)

# Dependencies
PKG_CHECK_MODULES(TESTBIN, [
	glib-2.0 >= 2.37.5
	gio-2.0 >= 2.37.0
	gtk+-3.0 >= 3.9.9
])

# These libraries are needed for the comparison
#  x11 >= 1.0.0
#  libgoffice-0.10 >= 0.10.0


#  gsl >= 0.28
#  libxml-2.0 >= 0.28


#	gsettings-desktop-schemas


TESTBIN_CFLAGS="$TESTBIN_CFLAGS $X11_CFLAGS $UNIX_CFLAGS"
TESTBIN_LIBS="$TESTBIN_LIBS $X11_LIBS $UNIX_LIBS"

AC_SUBST(TESTBIN_CFLAGS)
AC_SUBST(TESTBIN_LIBS)

dnl ================================================================
dnl libgd
dnl ================================================================

dnl LIBGD_INIT([tagged-entry static])

dnl ================================================================
dnl GSettings stuff
dnl ================================================================

GLIB_GSETTINGS

dnl ================================================================
dnl misc
dnl ================================================================

dnl LIBGD_INIT([tagged-entry static])

AC_PATH_PROG(GLIB_COMPILE_RESOURCES, glib-compile-resources)
dnl AC_PATH_PROG(GLIB_GENMARSHAL, glib-genmarshal)
dnl AC_PATH_PROG(GLIB_MKENUMS, glib-mkenums)

dnl GNOME_COMPILE_WARNINGS([maximum])

AC_ARG_ENABLE(deprecations,
              [AS_HELP_STRING([--enable-deprecations],
              [warn about deprecated usages [default=yes]])],
              [enable_deprecations=$enableval], [])

AC_MSG_CHECKING([whether to forbid deprecated symbols])
if test "$enable_deprecations" = "yes"; then
	AC_MSG_RESULT(yes)
	DISABLE_DEPRECATED_CFLAGS="\
  -DGSEAL_ENABLE \
  -DG_DISABLE_DEPRECATED \
  -DGDK_DISABLE_DEPRECATED \
  -DGTK_DISABLE_DEPRECATED \
  -DGDK_PIXBUF_DISABLE_DEPRECATED \
  -DGNOME_DISABLE_DEPRECATED \
  -DPEAS_DISABLE_DEPRECATED"

	AC_SUBST(DISABLE_DEPRECATED_CFLAGS)
else
	AC_MSG_RESULT(no)
fi

AC_CONFIG_FILES([
Makefile
])

if test "$platform_win32" = yes; then
	TESTBIN_IMPLIB='-L${libdir}/gdv -lgdv'
	PLUGIN_LIBTOOL_FLAGS="-module -avoid-version -no-undefined"
	if test "$os_win32" = yes; then
		PLUGIN_LIBTOOL_FLAGS="${PLUGIN_LIBTOOL_FLAGS} -mwindows"
	fi
else
	TESTBIN_IMPLIB=""
	PLUGIN_LIBTOOL_FLAGS="-module -avoid-version"
fi
AC_SUBST(TESTBIN_IMPLIB)
AC_SUBST(PLUGIN_LIBTOOL_FLAGS)


dnl gdv/Makefile
dnl test/Makefile

dnl physcore/Makefile
dnl data/org.epsara.gschema.xml])

#libgd/Makefile])

AC_OUTPUT




