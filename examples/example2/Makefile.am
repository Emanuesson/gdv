noinst_PROGRAMS += examples/example2/example
# bin_PROGRAMS += gdv/gdvtest

examples_example2_example_SOURCES =					                        \
                  examples/example2/example-main.c
 
#If possible write own exportable library with libtool
examples_example2_example_LDADD =			                \
	              $(GDV_LIBS)             \
                libgdv.la

CLEANFILES += $(BUILT_SOURCES)

