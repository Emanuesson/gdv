application_headers = [
  'gdv-app.h',
]

application_sources = [
  'gdv-app.c',
]

viewer_public_sources += files(application_headers)
viewer_public_sources += files(application_sources)

install_headers(application_headers, subdir: join_paths(libgdv_header_subdir, 'application'))
