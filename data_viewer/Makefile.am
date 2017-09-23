bin_PROGRAMS += gdv/gdvdataviewer

gdvdataviewer_common_cppflags =         \
  -I$(top_srcdir)                       \
  -I$(top_srcdir)/dataviewer            \
  -I$(top_builddir)/dataviewer          \
  -DDATADIR=\""$(datadir)"\"            \
  -DLIBDIR=\""$(libdir)"\"

gdvdataviewer_common_cflags =           \
  $(GDV_CFLAGS)                         \
  $(WARN_CFLAGS)                        \
  $(DISABLE_DEPRECATED_CFLAGS)

gdv_gdvdataviewer_CPPFLAGS = $(gdvdataviewer_common_cppflags)
gdv_gdvdataviewer_CFLAGS = $(gdvdataviewer_common_cflags)

INST_H_FILES =                                    \
               data_viewer/gui/viewer-app.h           \
               data_viewer/gui/viewer-app-win.h       \
               data_viewer/gui/viewer-welcome-win.h

gdv_gdvdataviewer_SOURCES =					              \
                  data_viewer/viewer-resources.c\
                  data_viewer/viewer-main.c       \
                  data_viewer/gui/viewer-app.c        \
                  data_viewer/gui/viewer-app-win.c    \
                  data_viewer/gui/viewer-welcome-win.c    \
               	$(INST_H_FILES)

gdv_gdvdataviewer_LDFLAGS = -export-dynamic -no-undefined -export-symbols-regex "^[[^_]].*"

gdv_gdvdataviewer_LDADD =			                \
	              $(GDV_LIBS)                   \
                libgdv.la

viewer_resource_deps = $(shell $(GLIB_COMPILE_RESOURCES) --generate-dependencies --sourcedir $(srcdir)/data_viewer --sourcedir $(builddir)/data_viewer --sourcedir $(srcdir)/gdv --sourcedir $(builddir)/gdv $(builddir)/data_viewer/viewer.gresource.xml)

viewer_resources_c_deps = $(builddir)/data_viewer/viewer.gresource.xml $(viewer_resource_deps)

data_viewer/viewer-resources.c: $(viewer_resources_c_deps)
	$(AM_V_GEN) $(GLIB_COMPILE_RESOURCES)\
		--target="$@"\
		--sourcedir="$(srcdir)/data_viewer"\
		--sourcedir="$(builddir)/data_viewer"\
		--sourcedir="$(srcdir)/gdv"\
		--sourcedir="$(builddir)/gdv"\
		--generate-source "$(builddir)/data_viewer/viewer.gresource.xml"

EXTRA_DIST +=                             \
  data_viewer/viewer.gresource.xml.in     \
  $(viewer_resource_deps)

BUILT_SOURCES +=                         \
	$(data_viewer_built_sources) data_viewer/viewer-resources.c

#data_viewer/viewer-resources.c: data_viewer/viewer.gresource.xml data_viewer/viewer-app-win.ui
#	$(AM_V_GEN) $(GLIB_COMPILE_RESOURCES) $(builddir)/data_viewer/viewer.gresource.xml \
#		--target=$@ --sourcedir=$(srcdir)/data_viewer --generate-source

#EXTRA_DIST +=				\
#	data_viewer/viewer.gresource.xml	\
#	data_viewer/viewer-app-win.ui

CLEANFILES += $(BUILT_SOURCES)

