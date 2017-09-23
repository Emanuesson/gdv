AM_CPPFLAGS =								          \
  -I m4 ${ACLOCAL_FLAGS}              \
	-I$(top_srcdir)							        \
	-I$(srcdir)							            \
	$(GDV_CFLAGS)						            \
	$(GDV_DEBUG_FLAGS)						      \
	-DDATADIR=\""$(datadir)"\"					\
	-DLIBDIR=\""$(libdir)"\"

SUBDIRS = doc_in

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = 			\
	BUGS			\
	HACKING			\
	MAINTAINERS \
  gtk-doc.make\
  $(NULL)


MAINTAINERCLEANFILES = 		                          \
	$(GITIGNORE_MAINTAINERCLEANFILES_TOPLEVEL)        \
	$(GITIGNORE_MAINTAINERCLEANFILES_MAKEFILE_IN)     \
	$(GITIGNORE_MAINTAINERCLEANFILES_M4_LIBTOOL)      \
	$(srcdir)/REAMDE.md                               \
	$(srcdir)/linting.sh                              \
	ABOUT-NLS		\
	INSTALL			\
	config.rpath		\
	gtk-doc.make		\
	py-compile		\
	test-driver		\
	m4/gnome-doc-utils.m4	\
	m4/gtk-doc.m4		\
	m4/intltool.m4		\
	m4/codeset.m4		\
	m4/fcntl-o.m4		\
	m4/gettext.m4		\
	m4/glibc2.m4		\
	m4/glibc21.m4		\
	m4/iconv.m4		\
	m4/intdiv0.m4		\
	m4/intl.m4		\
	m4/intldir.m4		\
	m4/intlmacosx.m4	\
	m4/intmax.m4		\
	m4/inttypes-pri.m4	\
	m4/inttypes_h.m4	\
	m4/lcmessage.m4		\
	m4/lib-ld.m4		\
	m4/lib-link.m4		\
	m4/lib-prefix.m4	\
	m4/lock.m4		\
	m4/longlong.m4		\
	m4/nls.m4		\
	m4/po.m4		\
	m4/printf-posix.m4	\
	m4/progtest.m4		\
	m4/size_max.m4		\
	m4/stdint_h.m4		\
	m4/threadlib.m4		\
	m4/uintmax_t.m4		\
	m4/visibility.m4	\
	m4/wchar_t.m4		\
	m4/wint_t.m4		\
	m4/xsize.m4		\
	po/Makevars.template	\
	po/Rules-quot		\
	po/*.sed		\
	po/*.header		\
	po/*.sin

DISTCHECK_CONFIGURE_FLAGS =       \
  --enable-gtk-doc                \
  --disable-doc-cross-references  \
   --enable-man                   \
  --enable-introspection

GRESDEPS = $(srcdir)/$1 $(shell $(GLIB_COMPILE_RESOURCES) --generate-dependencies --sourcedir $(srcdir)/$(dir $1) $(srcdir)/$1)

GRESGEN = $(AM_V_GEN) $(GLIB_COMPILE_RESOURCES) --target="$@" --sourcedir="$(dir $<)" --generate-source "$<"

noinst_LTLIBRARIES        =
noinst_PROGRAMS	          =
bin_PROGRAMS              =
lib_LTLIBRARIES           =
gsettings_SCHEMAS         =
gsettings_ENUMS           =
dist_gsettingsschema_DATA =
CLEANFILES                =
DISTCLEANFILES            =
BUILT_SOURCES             =

EXTRA_DIST +=         \
  README.md.in

include data/Makefile.am
include gdv/Makefile.am
include data_viewer/Makefile.am
include examples/example1/Makefile.am
include examples/example2/Makefile.am
#include examples/example3/Makefile.am
#include examples/example4/Makefile.am
include tests/Makefile.am

