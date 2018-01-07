specialized_headers = [
  'gdvspecialcheckedindicator.h',
  'gdvspecialdrumdisplay.h',
  'gdvspecialtimeaxis.h',
  'gdvspecialcontentaxis.h',
]

specialized_sources = [
  'gdvspecialcheckedindicator.c',
  'gdvspecialdrumdisplay.c',
  'gdvspecialtimeaxis.c',
  'gdvspecialcontentaxis.c',
]

libgdv_public_headers += files(specialized_headers)
libgdv_public_sources += files(specialized_sources)

install_headers(libgdv_public_headers, subdir: join_paths(libgdv_header_subdir, 'specialized_widgets'))
