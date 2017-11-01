application_headers = [
  'viewer-app.h',
]

application_sources = [
  'viewer-app.c',
]

application_private_sources = [
#  'ide-application-actions.c',
#  'ide-application-actions.h',
]

viewer_public_headers += files(application_headers)
viewer_public_sources += files(application_sources)
viewer_private_sources += files(application_private_sources)

install_headers(application_headers, subdir: join_paths(libgdv_header_subdir, 'application'))
