libgdv_api_version = '1.0'

viewer_private_sources = [
  'gdve-central.h',
]

viewer_public_sources = [
  'viewer-main.c',
]

viewer_resources = gnome.compile_resources(
  'viewer-resources',
  'viewer.gresources.xml',
  'gui/viewer-app-win.ui',
  'gui/viewer-welcome-win.ui',
  'gui/gdv-viewer-settings-panel.ui',

  c_name: 'gdv',
)

# External dependencies
#libdazzle_dep = dependency('libdazzle', required : false)

#if not libdazzle_dep.found()
#  libdazzle_proj = subproject('libdazzle')
#  libdazzle_dep = libdazzle_proj.get_variable('libdazzle_dep')
#endif

#libide_dep = dependency('libide', required : false)

#if not libide_dep.found()
#  libide_proj = subproject('gnome-builder')
#  libide_dep = libide_proj.get_variable('libide_dep')
#endif

subdir('application')
subdir('data')
subdir('file')
subdir('gui')
subdir('sourceview')

viewer_sources = []
viewer_sources += viewer_private_sources
viewer_sources += viewer_public_sources

data_viewer_bin_sources = [
  viewer_sources,
  viewer_resources,
]

libgtksource_dep = [
  dependency('gtksourceview-4'),
]

libide_dep = [
#  dependency('libdazzle-1.0'),
#  dependency('libide-1.0'),
]

builderlibdir_abs = join_paths(get_option('prefix'), get_option('libdir'), 'gnome-builder')

executable('dataviewer', data_viewer_bin_sources,
       dependencies: [ libgtksource_dep,
                       #libide_dep,
                       libgdv_dep ],
          link_with: libgdv,
       link_depends: libgdv_map,
      install_rpath: builderlibdir_abs,
include_directories: [ root_inc, src_inc ],
            install: true,
)
