
libgdv_header_subdir = join_paths(package_subdir, 'libgdv-' + apiversion, 'gdv')
libgdv_header_dir = join_paths(get_option('includedir'), libgdv_header_subdir)

libgdv_resources = gnome.compile_resources(
  'libgdv-resources',
  'libgdv.gresources.xml',
  'themes/default_general.css',

  c_name: 'gdv',
)

version_data = configuration_data()
version_data.set('GDV_MAJOR_VERSION', gdv_version_major)
version_data.set('GDV_MINOR_VERSION', gdv_version_minor)
version_data.set('GDV_MICRO_VERSION', gdv_version_micro)
version_data.set('GDV_VERSION', meson.project_version())

gdv_version_h = configure_file(
          input: 'gdv-version.h.in',
         output: 'gdv-version.h',
    install_dir: libgdv_header_dir,
        install: true,
  configuration: version_data)

debug_data = configuration_data()
debug_data.set10('ENABLE_TRACING', get_option('enable_tracing'))

gdv_debug_h = configure_file(
          input: 'gdv-debug.h.in',
         output: 'gdv-debug.h',
  configuration: debug_data)

libgdv_generated_headers = [
  gdv_debug_h,
  gdv_version_h,
]

gdvcore_headers = [
  'gdv.h',
  'gdv-data-boxed.h',
#  'gdv-data-matrix.h',
#  'gdv-data-vector.h',
  'gdv-enums.h',
  'gdvaxis.h',
  'gdvindicator.h',
  'gdvlayer.h',
  'gdvlayercontent.h',
  'gdvlegend.h',
  'gdvlegendelement.h',
  'gdvlinearaxis.h',
  'gdvlogaxis.h',
  'gdvmtic.h',
  'gdvonedlayer.h',
  'gdvrender.h',
  'gdvtic.h',
  'gdvtwodlayer.h',
  'gdvcentral.h',
]

gdvcore_sources = [
  'gdv-data-boxed.c',
#  'gdv-data-matrix.c',
#  'gdv-data-vector.c',
  'gdvaxis.c',
  'gdvindicator.c',
  'gdvlayer.c',
  'gdvlayercontent.c',
  'gdvlegend.c',
  'gdvlegendelement.c',
  'gdvlinearaxis.c',
  'gdvlogaxis.c',
  'gdvmtic.c',
  'gdvonedlayer.c',
  'gdvrender.c',
  'gdvtic.c',
  'gdvtwodlayer.c',
  'gdvcentral.c',
]

gdvcore_private = [
#  'gdvcentral.h',
]

# Filled out in the subdirs
libgdv_public_headers = []
libgdv_public_sources = []
libgdv_private_sources = []

libgdv_public_headers += files(gdvcore_headers)
libgdv_public_sources += files(gdvcore_sources)
libgdv_private_sources += files(gdvcore_private)

install_headers(gdvcore_headers, subdir: libgdv_header_subdir)
message('OBSERVE:')
message(libgdv_header_subdir)

subdir('specialized_widgets')

libgdv_sources = [
  libgdv_generated_headers,
  libgdv_public_sources,
  libgdv_private_sources,
  libgdv_resources,
]

libgdv_deps = [
  dependency('gio-2.0'),
  dependency('gmodule-2.0'),
  dependency('gtk+-3.0'),
#  dependency('gtksourceview-3.0'),
#  dependency('gsl'),
  cc.find_library('m', required: false),
#  cc.find_library('rt', required: false),
#  cc.find_library('hdf5', required: false),
]

libgdv_args = []
if get_option('enable_rdtscp')
  libgdv_args += '-DGDV_HAVE_RDTSCP'
endif

libgdv_map = join_paths(meson.current_source_dir(), 'gdv.map')

libgdv_link_args = []
if ld_supports_version_script
libgdv_link_args += [ '-Wl,--version-script,' + libgdv_map ]
endif

libgdv = shared_library(
  'gdv-' + apiversion,
  libgdv_sources,

            soversion: 0,
         link_depends: libgdv_map,
               c_args: libgdv_args,
            link_args: libgdv_link_args,
         dependencies: libgdv_deps,
  include_directories: [ root_inc, src_inc ],
              install: true,
)

libgdv_dep = declare_dependency(
              sources: libgdv_generated_headers,
         dependencies: libgdv_deps,
            link_with: libgdv,
  include_directories: include_directories('.'),
)

if get_option('with_introspection')

  libgdv_gir = gnome.generate_gir(libgdv,
                sources: libgdv_generated_headers + libgdv_public_headers + libgdv_public_sources,
              nsversion: apiversion,
              namespace: 'Gdv',
          symbol_prefix: 'gdv',
      identifier_prefix: 'Gdv',
#              link_with: libgdv,
               includes: ['Gio-2.0', 'Gtk-3.0'],
                install: true,
        install_dir_gir: girdir,
    install_dir_typelib: typelibdir,
             extra_args: [ '--c-include=gdv.h', '--pkg-export=libgdv-1.0' ],
  )

  if get_option('with_vapi')

    libgdv_vapi = gnome.generate_vapi('gdv-' + apiversion,
      sources: libgdv_gir[0],
      packages: [ 'gio-2.0', 'gtk+-3.0' ],
      install: true,
      install_dir: vapidir,
      metadata_dirs: [ meson.current_source_dir() ],
    )

  endif
endif

pkgg = import('pkgconfig')

pkgg.generate(
    libraries: [libgdv],
      subdirs: libgdv_header_subdir,
      version: meson.project_version(),
         name: 'Gdv',
     filebase: 'gdv-@0@'.format(apiversion),
  description: 'Gdv for Gtk+ applications',
     requires: 'gtk+-3.0',                      #TODO: add all dependencies correctly
  install_dir: join_paths(libdir, 'pkgconfig'),
)
