noinst_PROGRAMS += examples/example1/example
# bin_PROGRAMS += gdv/gdvtest

examples_example1_example_SOURCES =					                        \
                  examples/example1/example-main.c
 
#If possible write own exportable library with libtool
examples_example1_example_LDADD =			                \
	              $(GDV_LIBS)             \
                libgdv.la

CLEANFILES += $(BUILT_SOURCES)

