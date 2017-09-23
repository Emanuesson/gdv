noinst_PROGRAMS += tests/test1

GEMU_SUB_FILES =                           \
	tests/gemu_sub/gemu-glib-util.c          \
	tests/gemu_sub/gemu-glib-util.h          \
	tests/gemu_sub/gemu-gtk-util.c           \
	tests/gemu_sub/gemu-gtk-util.h


tests_test1_SOURCES =					                \
  $(GEMU_SUB_FILES)                           \
	tests/test1basics/gdvinspector.c            \
	tests/test1basics/gdvinspector-app.c        \
	tests/test1basics/gdvinspector-app.h        \
	tests/test1basics/gdvinspector-app-win.c    \
	tests/test1basics/gdvinspector-app-win.h    \
  tests/test1basics/gdvinspector-resources.c  \
	$(GDV_INST_H_FILES)                         \
	$(GDV_c_FILES)

tests_test1_LDADD =			            \
	$(GDV_LIBS)                       \
	libgdv.la

gdvinspector_resource_deps = $(shell $(GLIB_COMPILE_RESOURCES) --generate-dependencies --sourcedir $(srcdir)/tests/test1basics --sourcedir $(builddir)/tests/test1basics $(builddir)/tests/test1basics/gdvinspec.gresource.xml)

gdvinspector_resources_c_deps = $(builddir)/tests/test1basics/gdvinspec.gresource.xml $(gdvinspector_resource_deps)

tests/test1basics/gdvinspector-resources.c: $(gdvinspector_resources_c_deps)
	$(AM_V_GEN) $(GLIB_COMPILE_RESOURCES)\
		--target="$@"\
		--sourcedir="$(srcdir)/tests/test1basics"\
		--sourcedir="$(builddir)/tests/test1basics"\
		--sourcedir="$(srcdir)/gdv"\
		--sourcedir="$(builddir)/gdv"\
		--generate-source "$(builddir)/tests/test1basics/gdvinspec.gresource.xml"

EXTRA_DIST +=                             \
  tests/test1basics/gdvinspec.gresource.xml.in     \
  $(gdvinspector_resource_deps)

BUILT_SOURCES +=                         \
	$(gdvinspector_built_sources) tests/test1basics/gdvinspector-resources.c


noinst_PROGRAMS += tests/test3

tests_test3_SOURCES =					      \
	tests/test3axes/test3.c         \
	$(GDV_INST_H_FILES)               \
	$(GDV_c_FILES)

tests_test3_LDADD =			            \
	$(GDV_LIBS)                       \
	libgdv.la

noinst_PROGRAMS += tests/test4

tests_test4_SOURCES =					      \
	tests/test4legend/test4.c         \
	$(GDV_INST_H_FILES)               \
	$(GDV_c_FILES)

tests_test4_LDADD =			            \
	$(GDV_LIBS)                       \
	libgdv.la

noinst_PROGRAMS += tests/test5

tests_test5_SOURCES =					      \
	tests/test5overlay/test5.c        \
	$(GDV_INST_H_FILES)               \
	$(GDV_c_FILES)

tests_test5_LDADD =			            \
	$(GDV_LIBS)                       \
	libgdv.la

noinst_PROGRAMS += tests/test6

tests_test6_SOURCES =					      \
  $(GEMU_SUB_FILES)                 \
	tests/test6gengui/test6.c         \
	$(GDV_INST_H_FILES)               \
	$(GDV_c_FILES)

tests_test6_LDADD =			            \
	$(GDV_LIBS)                       \
	libgdv.la

noinst_PROGRAMS += tests/test7

tests_test7_SOURCES =					      \
  $(GEMU_SUB_FILES)                 \
	tests/test7retic/test7.c         \
	$(GDV_INST_H_FILES)               \
	$(GDV_c_FILES)

tests_test7_LDADD =			            \
	$(GDV_LIBS)                       \
	libgdv.la

noinst_PROGRAMS += tests/test8

tests_test8_SOURCES =					      \
  $(GEMU_SUB_FILES)                 \
	tests/test8timeaxis/test8.c         \
	$(GDV_INST_H_FILES)               \
	$(GDV_c_FILES)

tests_test8_LDADD =			            \
	$(GDV_LIBS)                       \
	libgdv.la


