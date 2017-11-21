#if get_option('enable_tests')

viewer_public_headers = []

viewer_public_sources = [
  'viewer-main.c',
]

viewer_private_sources = []

viewer_resources = gnome.compile_resources(
  'viewer-resources',
  'viewer.gresources.xml',
  'gui/viewer-app-win.ui',
  'gui/viewer-welcome-win.ui',
  'gui/gdv-viewer-settings-panel.ui',

  c_name: 'gdv',
)

subdir('application')
subdir('file')
subdir('gui')
subdir('sourceview')

viewer_sources = [
  viewer_public_sources,
  viewer_private_sources,
  viewer_resources,
]

viewer_deps = [
  dependency('gtksourceview-3.0'),
]

executable('dataviewer', viewer_sources,
       dependencies: libgdv_deps + viewer_deps + [libgdv_dep],
         link_with: libgdv,
       link_depends: libgdv_map,
include_directories: [ root_inc, src_inc ],
            install: true,
)

