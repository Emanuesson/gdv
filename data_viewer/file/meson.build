file_headers = [
  'viewer-file.h',
]

file_sources = [
  'viewer-file.c',
]

file_private_sources = [
#  'ide-file-actions.c',
#  'ide-file-actions.h',
]

viewer_public_headers += files(file_headers)
viewer_public_sources += files(file_sources)
viewer_private_sources += files(file_private_sources)

install_headers(file_headers, subdir: join_paths(libgdv_header_subdir, 'file'))
