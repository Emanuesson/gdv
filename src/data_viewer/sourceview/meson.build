sourceview_headers = [
  'gdve-column-delimiter.h',
  'gdve-source-view.h',
]

sourceview_sources = [
  'gdve-column-delimiter.c',
  'gdve-source-view.c',
]

viewer_public_sources += files(sourceview_headers)
viewer_public_sources += files(sourceview_sources)

install_headers(sourceview_headers, subdir: join_paths(libgdv_header_subdir, 'application'))