lib_LTLIBRARIES += libgdv.la

#AM_CPPFLAGS =								          \
#	-I$(top_srcdir)							        \
#	-I$(srcdir)							            \
#	$(GDV_CFLAGS)						            \
#	-DDATADIR=\""$(datadir)"\"					\
#	-DLIBDIR=\""$(libdir)"\"


#noinst_LIBRARIES = libgdv.a

UNBUILD_CORE_GDV_INST_H_FILES =                       \
              gdv/gdv.h                               \
              gdv/gdvrender.h                         \
              gdv/gdvcentral.h                        \
              gdv/gdvtwodlayer.h                      \
              gdv/gdvonedlayer.h                      \
              gdv/gdvaxis.h                           \
              gdv/gdvlinearaxis.h                     \
              gdv/gdvlogaxis.h                        \
              gdv/gdvtic.h                            \
              gdv/gdvmtic.h                           \
              gdv/gdvindicator.h                      \
              gdv/gdvlayer.h                          \
              gdv/gdvlegend.h                         \
              gdv/gdvlegendelement.h                  \
              gdv/gdvlayercontent.h                   \
              gdv/gdv-enums.h                         \
              gdv/gdv-data-vector.h                   \
              gdv/gdv-data-boxed.h                    \
              gdv/gdv-data-matrix.h

#              gdv/gdvdebug.h                          
#              gdv/gdvelement.h                        

#              gdv/gdvarrow.h                          
#              gdv/gdvgrid.h                           
#              gdv/gdvlabel.h                          
#              gdv/gdvlayercontentstream.h             
#              gdv/gdvlayertitle.h                     

#              gdv/gdvtwodview.h                       
#              gnuplot_i/gnuplot_i.h                   
#              gdv/gdvinteractionable.h                
#              gdv/gdvaxistitle.h                      
#              gdv/basic_numeric/gdvdatavector.h       

UNBUILD_GDV_C_FILES =                                 \
              gdv/gdvrender.c                         \
              gdv/gdvtwodlayer.c                      \
              gdv/gdvonedlayer.c                      \
              gdv/gdvaxis.c                           \
              gdv/gdvlinearaxis.c                     \
              gdv/gdvlogaxis.c                        \
              gdv/gdvtic.c                            \
              gdv/gdvmtic.c                           \
              gdv/gdvindicator.c                      \
              gdv/gdvlayer.c                          \
              gdv/gdvlegend.c                         \
              gdv/gdvlegendelement.c                  \
              gdv/gdvlayercontent.c                   \
              gdv/gdv-data-vector.c                   \
              gdv/gdv-data-boxed.c                    \
              gdv/gdv-data-matrix.c

include gdv/specialized_widgets/Makefile.am

CORE_GDV_INST_H_FILES =				                        \
              $(UNBUILD_CORE_GDV_INST_H_FILES)        \
              gdv/libgdv-resources.h

GDV_C_FILES =                                         \
							$(UNBUILD_GDV_C_FILES)                  \
              gdv/libgdv-resources.c

#              gdv/gdvdebug.c                          
#              gdv/gdvelement.c                        

#              gdv/gdvarrow.c                          
#              gdv/gdvgrid.c                           
#              gdv/gdvlabel.c                          
#              gdv/gdvlayercontentstream.c             
#              gdv/gdvlayertitle.c                     


#              gdv/gdvtwodview.c                       
#              gnuplot_i/gnuplot_i.c                   
#              gdv/gdvinteractionable.c                
#              gdv/gdvaxistitle.c                      
#              gdv/basic_numeric/gdvdatavector.c      

GDV_INST_H_FILES = $(CORE_GDV_INST_H_FILES)

gdv_headerdir = $(prefix)/include/gdv-$(GDV_API_VERSION)/gdv
gdv_header_HEADERS = $(CORE_GDV_INST_H_FILES)

libgdv_la_SOURCES =                  \
                   $(GDV_INST_H_FILES)   \
                   $(GDV_C_FILES)

libgdv_la_LIBADD =			\
	$(GDV_LIBS)

lib_resource_deps = $(shell $(GLIB_COMPILE_RESOURCES) --generate-dependencies --sourcedir $(srcdir)/gdv --sourcedir $(builddir)/gdv $(builddir)/gdv/libgdv.gresource.xml)

libgdv_resources_c_deps = $(builddir)/gdv/libgdv.gresource.xml $(lib_resource_deps)

gdv/libgdv-resources.h: $(libgdv_resources_deps)
	$(AM_V_GEN) $(GLIB_COMPILE_RESOURCES)\
		--target="$@"\
		--sourcedir="$(srcdir)/gdv"\
		--sourcedir="$(builddir)/gdv"\
		--generate-header "$(builddir)/gdv/libgdv.gresource.xml"

gdv/libgdv-resources.c: $(libgdv_resources_c_deps)
	$(AM_V_GEN) $(GLIB_COMPILE_RESOURCES)\
		--target="$@"\
		--sourcedir="$(srcdir)/gdv"\
		--sourcedir="$(builddir)/gdv"\
		--generate-source "$(builddir)/gdv/libgdv.gresource.xml"

#		--manual-register

EXTRA_DIST +=                             \
	gdv/libgdv.gresource.xml.in             \
	$(lib_resource_deps)                    
#	$(theme_sources)                        

BUILT_SOURCES +=                         \
	$(gdv_libgdv_la_sources)               \
	gdv/libgdv-resources.c                 \
	gdv/libgdv-resources.h                 \
	gdv/libgdv.gresource.xml

CLEANFILES += $(BUILT_SOURCES)

-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_GIRS =
INTROSPECTION_SCANNER_ARGS = --add-include-path=$(srcdir) --warn-all
INTROSPECTION_COMPILER_ARGS = --includedir=$(srcdir)

if HAVE_INTROSPECTION
introspection_sources = $(libgdv_la_SOURCES)

Gdv-1.0.gir: libgdv.la Makefile

Gdv_1_0_gir_NAMESPACE = Gdv
Gdv_1_0_gir_INCLUDES = GObject-2.0 Gio-2.0
Gdv_1_0_gir_CFLAGS =                  \
	$(GDV_CFLAGS)						            \
  $(INCLUDES)                         \
	-DLIBDIR=\""$(libdir)"\"            \
	-DDATADIR=\""$(datadir)"\"					\
  -I$(top_srcdir)							        
#	-I$(srcdir)							            
Gdv_1_0_gir_LIBS = libgdv.la
Gdv_1_0_gir_FILES = $(introspection_sources)
Gdv_1_0_gir_SCANNERFLAGS = --warn-all --pkg-export gdv-1.0 --c-include "gdv/gdv.h"

INTROSPECTION_GIRS += Gdv-1.0.gir

girdir = $(datadir)/gir-1.0
gir_DATA = $(INTROSPECTION_GIRS)

typelibdir = $(libdir)/girepository-1.0
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

CLEANFILES += $(gir_DATA) $(typelib_DATA)
endif

# make a script to use lint on the project
#   Annotations: '-warnposixheaders' is a bad workaround for a nasty splint-bug
noinst_PROGRAMS += linting.sh$(EXEEXT)
linting.sh$(EXEEXT): Makefile
	echo "#!/usr/bin/env sh" >$@
	chmod +x $@
	echo "splint -warnposixheaders -badflag $(GDV_CFLAGS) \\" >>$@
	echo "  -I $(srcdir) -I $(builddir) \\" >>$@
	echo " $(UNBUILD_CORE_GDV_INST_H_FILES)\\" | sed 's/ / $(srcdir)\//g' >> $@
	echo " $(UNBUILD_GDV_C_FILES)" | sed 's/ / $(srcdir)\//g' >> $@

# make a script to use astyle to correct any syntax-problems
noinst_PROGRAMS += aformat.sh$(EXEEXT)
aformat.sh$(EXEEXT): Makefile
	echo "#!/usr/bin/env sh" >$@
	chmod +x $@
	echo "astyle --mode=c --style=allman --indent=spaces=2 -z2 -n -xW -f -p -H \\" >>$@
	echo " $(UNBUILD_CORE_GDV_INST_H_FILES)\\" | sed 's/ / $(srcdir)\//g' >> $@
	echo " $(UNBUILD_GDV_C_FILES)" | sed 's/ / $(srcdir)\//g' >> $@


