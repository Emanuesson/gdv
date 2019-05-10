libgdv_api_version = '1.0'

#src_inc = include_directories('gdv')
root_inc = include_directories('.')
src_inc = include_directories('gdv')

subdir('gdv')

if get_option('enable_tests')
  subdir('tests')
endif

subdir('examples')

viewer_public_sources = [
  'main.c',
]

viewer_resources = gnome.compile_resources(
  'viewer-resources',
  'viewer.gresources.xml',
  'gui/gdv-app-win.ui',

  c_name: 'gdv',
)

subdir('application')
subdir('gui')

viewer_sources = []
#viewer_sources += viewer_private_sources
viewer_sources += viewer_public_sources

data_viewer_bin_sources = [
  viewer_sources,
  viewer_resources,
]

builderlibdir_abs = join_paths(get_option('prefix'), get_option('libdir'), 'gnome-builder')

executable('dataviewer', data_viewer_bin_sources,
       dependencies: [ libgdv_dep ],
          link_with: libgdv,
       link_depends: libgdv_map,
      install_rpath: builderlibdir_abs,
include_directories: [ root_inc, src_inc ],
            install: true,
)

