#if get_option('enable_tests')

libtestgemu_sources = [
  'gemu-glib-util.c',
  'gemu-glib-util.h',
  'gemu-gtk-util.c',
  'gemu-gtk-util.h',
]

libtestgemu_deps = [
#  cc.find_library('asan', required: true),
  dependency('gio-2.0'),
  dependency('gmodule-2.0'),
  dependency('gtk+-3.0'),
]

libtestgemu_args = []
if get_option('enable_rdtscp')
  libtestgemu_args += '-DGDV_HAVE_RDTSCP'
endif

#libgdv_map = join_paths(meson.current_source_dir(), 'gdv.map')

#libgdv_link_args = []
#if ld_supports_version_script
#libgdv_link_args += [ '-Wl,--version-script,' + libgdv_map ]
#endif


libtestgemu = shared_library(
  'testgemu',
  files (libtestgemu_sources),
            soversion: 0,
#         link_depends: libgdv_map,
               c_args: libtestgemu_args,
#            link_args: libgdv_link_args,
          link_with: libgdv,
       link_depends: libgdv_map,
#      install_rpath: builderlibdir_abs,
#include_directories: [ root_inc, src_inc ],
         dependencies: libtestgemu_deps,
  include_directories: [ root_inc, src_inc ],
              install: false,
)
