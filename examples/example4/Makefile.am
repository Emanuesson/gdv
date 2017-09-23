noinst_PROGRAMS += examples/example4/example
# bin_PROGRAMS += gdv/gdvtest

NOINST_H_FILES =				                              \
               example-app.h

examples_example4_example_SOURCES =					                        \
                  examples/example4/example-main.c       \
                  examples/example4/example-app.c        \
                	$(NOINST_H_FILES)

#If possible write own exportable library with libtool
examples_example4_example_LDADD =			                \
	              $(GDV_LIBS)             \
                libgdv.la

CLEANFILES += $(BUILT_SOURCES)

