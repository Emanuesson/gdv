sourceview_headers = [
  'viewer-source-view.h',
]

sourceview_sources = [
  'viewer-source-view.c',
]

sourceview_private_sources = [
#  'ide-application-actions.c',
#  'ide-application-actions.h',
]

viewer_public_headers += files(sourceview_headers)
viewer_public_sources += files(sourceview_sources)
viewer_private_sources += files(sourceview_private_sources)

install_headers(sourceview_headers, subdir: join_paths(libgdv_header_subdir, 'application'))
