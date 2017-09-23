#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])

m4_define(gdv_major_version, 0)
m4_define(gdv_minor_version, 7)
m4_define(gdv_micro_version, 2)
m4_define(gdv_version, gdv_major_version.gdv_minor_version.gdv_micro_version)

# libtool version related macros
m4_define([gdv_lt_relea'se], [major_version.minor_version])
m4_define([gdv_lt_current],
          [m4_eval(100 * minor_version + micro_version)])

AC_INIT([gdv], [gdv_version], [eschmidt216@gmail.com],[gdv],[gnome/libs/gdv])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR(data_viewer/viewer-main.c)
AC_CONFIG_MACRO_DIR([m4])

# define auxiliary directory
AC_CONFIG_AUX_DIR([aux-build])

# Save this value here, since automake will set cflags later
cflags_set=${CFLAGS:+set}

# foreign is included, to substitute the README with a README.md
AM_INIT_AUTOMAKE([1.11 -Wno-portability no-define no-dist-gzip dist-xz tar-ustar subdir-objects foreign])
AM_MAINTAINER_MODE([enable])

# check for programs
AC_PROG_CC
AM_PROG_CC_STDC
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_SYS_LARGEFILE
PKG_PROG_PKG_CONFIG

# needed on osx
AC_PROG_OBJC

# start libtool
LT_PREREQ([2.2.6])
LT_INIT(disable-static)

GDV_API_VERSION=0.0
AC_SUBST(GDV_API_VERSION)

AC_CHECK_FUNC(sigaction)
AC_CHECK_LIB(m, floor)

AC_HEADER_STDC

# start libtool
LT_PREREQ([2.2.6])
LT_INIT([disable-static])

GDV_MAJOR_VERSION=major_version
GDV_MINOR_VERSION=minor_version
GDV_MICRO_VERSION=micro_version
GDV_VERSION=gdv_version
GDV_API_VERSION=gdv_version
AC_SUBST(GDV_MAJOR_VERSION)
AC_SUBST(GDV_MINOR_VERSION)
AC_SUBST(GDV_MICRO_VERSION)
AC_SUBST(GDV_VERSION)
AC_SUBST(GDV_API_VERSION)

AC_DEFINE(GDV_MAJOR_VERSION, [major_version],
	  [Define to the GDV major version])
AC_DEFINE(GDV_MINOR_VERSION, [minor_version],
	  [Define to the GDV minor version])
AC_DEFINE(GDV_MICRO_VERSION, [micro_version],
	  [Define to the GDV micro version])

AC_CHECK_FUNC(sigaction)
AC_CHECK_LIB(m, floor)

# make sure we keep ACLOCAL_FLAGS around for maintainer builds to work
AC_SUBST(ACLOCAL_AMFLAGS, "$ACLOCAL_FLAGS -I m4")

# location to install runtime libraries, e.g. ../../lib to install
# to /lib if libdir is /usr/lib
AC_ARG_WITH(runtime-libdir,
           [AS_HELP_STRING([--with-runtime-libdir=RELPATH],
                           [install runtime libraries relative to libdir])],
           [],
	   [with_runtime_libdir=""])
GDV_RUNTIME_LIBDIR="$with_runtime_libdir"
ABS_GDV_RUNTIME_LIBDIR="`readlink -f $libdir/$with_runtime_libdir`"
AC_SUBST(GDV_RUNTIME_LIBDIR)
AC_SUBST(ABS_GDV_RUNTIME_LIBDIR)
AM_CONDITIONAL(HAVE_GDV_RUNTIME_LIBDIR, [test "x$with_runtime_libdir" != "x"])

# use GObject Introspection
GOBJECT_INTROSPECTION_CHECK([1.30.0])

if test "$found_introspection" = "yes"; then
  AM_CONDITIONAL([HAVE_INTROSPECTION], true)
  enable_introspection=yes
  AC_DEFINE([ENABLE_INTROSPECTION], [1], [Define to enable GObject Introspection])
else
  enable_introspection=no
fi

# Start of pkg-config checks
PKG_PROG_PKG_CONFIG(0.16)

if test "x$enable_debug" = "xyes"; then
  test "$cflags_set" = set || CFLAGS="$CFLAGS -g"
  GDV_DEBUG_FLAGS="-DG_ENABLE_DEBUG -DG_ENABLE_CONSISTENCY_CHECKS"
else
  if test "x$enable_debug" = "xno"; then
    GDV_DEBUG_FLAGS="-DG_DISABLE_ASSERT -DG_DISABLE_CHECKS -DG_DISABLE_CAST_CHECKS"
  else
    GDV_DEBUG_FLAGS="-DG_ENABLE_DEBUG -DG_DISABLE_CAST_CHECKS"
  fi
fi
GDV_DEBUG_FLAGS="$GDV_DEBUG_FLAGS $GLIB_VERSION_CFLAGS"


# Dependencies
PKG_CHECK_MODULES(GDV, [
	glib-2.0 >= 2.37.5
	gio-2.0 >= 2.37.0
	gtk+-3.0 >= 3.20.0
	gsl >= 1.16
])

GDV_CFLAGS="$GDV_CFLAGS $X11_CFLAGS $UNIX_CFLAGS"
GDV_LIBS="$GDV_LIBS $X11_LIBS $UNIX_LIBS"

AC_SUBST(GDV_CFLAGS)
AC_SUBST(GDV_LIBS)
AC_SUBST(BIN_GDV_LIBS)
AC_SUBST(BIN_GDV_LDFLAGS)
AC_SUBST(BIN_GDV_CFLAGS)
AC_SUBST(GDV_DEBUG_FLAGS)

# check for gtk-doc
m4_ifdef([GTK_DOC_CHECK], [
GTK_DOC_CHECK([1.14],[--flavour no-tmpl])
],[
AM_CONDITIONAL([ENABLE_GTK_DOC], false)
])

# GSettings
GLIB_GSETTINGS

# misc
AC_PATH_PROG(GLIB_COMPILE_RESOURCES, glib-compile-resources)

AC_ARG_ENABLE(deprecations,
              [AS_HELP_STRING([--enable-deprecations],
              [warn about deprecated usages [default=yes]])],
              [enable_deprecations=$enableval], [])

m4_define([debug_default],
          m4_if(m4_eval(gdv_minor_version % 2), [1], [yes], [minimum]))

AC_ARG_ENABLE(debug,
              [AS_HELP_STRING([--enable-debug=@<:@no/minimum/yes@:>@],
                              [turn on debugging @<:@default=debug_default@:>@])],,
              [enable_debug=debug_default])


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
data/gdv.pc
data_viewer/viewer.gresource.xml
gdv/libgdv.gresource.xml
doc_in/Makefile
README.md
doc_in/reference/Makefile
doc_in/reference/gdv/Makefile
doc_in/reference/gdv/version.xml
tests/test1basics/gdvinspec.gresource.xml
])

if test "$platform_win32" = yes; then
	GDV_IMPLIB='-L${libdir}/gdv -lgdv'
	PLUGIN_LIBTOOL_FLAGS="-module -avoid-version -no-undefined"
	if test "$os_win32" = yes; then
		PLUGIN_LIBTOOL_FLAGS="${PLUGIN_LIBTOOL_FLAGS} -mwindows"
	fi
else
	GDV_IMPLIB=""
	PLUGIN_LIBTOOL_FLAGS="-module -avoid-version"
fi
AC_SUBST(GDV_IMPLIB)
AC_SUBST(PLUGIN_LIBTOOL_FLAGS)

AC_OUTPUT

