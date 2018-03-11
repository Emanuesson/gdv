libgdv_api_version = '1.0'

viewer_public_headers = []

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

subdir('application')
subdir('file')
subdir('gui')
subdir('sourceview')

data_viewer_bin_sources = [
  viewer_public_sources,
  viewer_resources,
]

libgtksource_dep = [
  dependency('gtksourceview-3.0'),
]

executable('dataviewer', data_viewer_bin_sources,
       dependencies: [ libgtksource_dep,
                       libgdv_dep,
                       libgdv_deps ],
         link_with: libgdv,
       link_depends: libgdv_map,
include_directories: [ root_inc, src_inc ],
            install: true,
)
