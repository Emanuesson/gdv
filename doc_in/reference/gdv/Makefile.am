## Process this file with automake to produce Makefile.in

# see /gtk-doc/examples/Makefile.am

# We require automake 1.6 at least.
AUTOMAKE_OPTIONS = 1.6

# The name of the module
DOC_MODULE=gdv

# The top-level XML file (SGML in the past).
DOC_MAIN_SGML_FILE=gdv-docs.xml

# The directory containing the source code. Relative to $(srcdir).
#DOC_SOURCE_DIR=$(top_srcdir)/epsara
DOC_SOURCE_DIR  =           \
  $(top_srcdir)/gdv

#  $(top_srcdir)/docs/reference/gdv

# Extra options to supply to gtkdoc-scan.
# e.g. SCAN_OPTIONS = --deprecated-guards="GTK_DISABLE_DEPRECATED"
SCAN_OPTIONS=--deprecated-guards="G_DISABLE_DEPRECATED" \
             --ignore-decorators="G_GNUC_INTERNAL|G_GNUC_WARN_UNUSED_RESULT"

#SCAN_OPTIONS = --rebuild-types

# Extra options to supply to gtkdoc-mkdb.
#MKDB_OPTIONS = --xml-mode --output-format=xml

MKDB_OPTIONS=--output-format=xml

# Used for dependencies. The docs will be rebuilt if any of these change.
HFILE_GLOB = $(top_srcdir)/gdv/*.h
CFILE_GLOB = $(top_srcdir)/gdv/*.c

# Header files to ignore when scanning (These are internal to gedit).
IGNORE_HFILES =                     \
  $(top_srcdir)/gdv/themes/*        \
  $(top_srcdir)/gdv/gdvrender.h
#  $(top_srcdir)/gdv/gdvlabel.h

#  $(top_srcdir)/gdv/gdvaxisticlabels.c  
#  $(top_srcdir)/gdv/gdvaxisticlabels.h  

# CFLAGS and LDFLAGS for compiling scan program. Only needed
# if $(DOC_MODULE).types is non-empty.
AM_CPPFLAGS = 				\
	-I$(srcdir) 			\
	$(gdv_INCLUDES) 		\
	$(GDV_FLAGS)

# Images to copy into HTML directory.
HTML_IMAGES =                             \
  $(srcdir)/images/example1.png           \
  $(srcdir)/images/example2.png           \
  $(srcdir)/images/realloc_prob.png

# Extra SGML files that are included by $(DOC_MAIN_SGML_FILE).
content_files =             \
  version.xml               \
  dev_todo.xml              \
  dev_probs.xml              \
  tut_intro.xml

include $(top_srcdir)/gtk-doc.make

EXTRA_DIST +=         \
  version.xml.in

# CFLAGS and LDFLAGS for compiling gtkdoc-scangobj with your library.
# Only needed if you are using gtkdoc-scangobj to dynamically query widget
# signals and properties.
GTKDOC_CFLAGS =           \
	-I$(top_srcdir)/gdv    \
	-I$(top_builddir)/gdv  \
	-I$(top_srcdir)         \
	-I$(top_builddir)       \
	$(GDV_CFLAGS)

GTKDOC_LIBS = \
	$(top_builddir)/libgdv.la		\
	$(GDV_LIBS)

# MAINTAINERCLEANFILES = gedit.types

dist-hook-local: all-local

gdv-docs-clean: clean
	cd $(srcdir) && rm -rf xml html

