AM_CPPFLAGS =								          \
  -I m4 ${ACLOCAL_FLAGS}               \
	-I$(top_srcdir)							        \
	-I$(srcdir)							            \
	$(GDV_CFLAGS)						            \
	-DDATADIR=\""$(datadir)"\"					\
	-DLIBDIR=\""$(libdir)"\"

#ACLOCAL_AMFLAGS = 

ACLOCAL_AMFLAGS = -I m4macros

EXTRA_DIST = 			\
	BUGS			\
	HACKING			\
	MAINTAINERS

MAINTAINERCLEANFILES = 		\
	$(GITIGNORE_MAINTAINERCLEANFILES_TOPLEVEL) \
	$(GITIGNORE_MAINTAINERCLEANFILES_MAKEFILE_IN) \
	$(GITIGNORE_MAINTAINERCLEANFILES_M4_LIBTOOL) \
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


noinst_LTLIBRARIES        =
noinst_PROGRAMS	          =
bin_PROGRAMS              = testbin
pkglib_LTLIBRARIES        =
CLEANFILES                =
DISTCLEANFILES            =
BUILT_SOURCES             =

testbin_CPPFLAGS = $(TESTBIN_CPPFLAGS)
testbin_CFLAGS = $(TESTBIN_CFLAGS)

INST_H_FILES =			               \
               test_bin.h

testbin_SOURCES =					   \
               test_bin.c              \
                	$(INST_H_FILES)

#If possible write own exportable library with libtool
testbin_LDADD =			                \
	            $(GDV_LIBS) 
