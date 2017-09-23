noinst_PROGRAMS += examples/example3/example
# bin_PROGRAMS += gdv/gdvtest

examples_example3_example_SOURCES =					                        \
                  examples/example3/example-main.c
 
#If possible write own exportable library with libtool
examples_example3_example_LDADD =			                \
	              $(GDV_LIBS)             \
                libgdv.la

CLEANFILES += $(BUILT_SOURCES)

