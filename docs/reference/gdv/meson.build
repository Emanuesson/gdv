
private_headers = [
  'config.h',
  'gdv.h',
  'gdv-version.h',
  'gdvrender.h',
]


glib_prefix = dependency('glib-2.0').get_pkgconfig_variable('prefix')
glib_docpath = join_paths(glib_prefix, 'share', 'gtk-doc', 'html')
docpath = join_paths(get_option('datadir'), 'gdv-doc', 'html')


images = [
#  'images',
  join_paths('images', 'example1.png'),
  join_paths('images', 'example2.png'),
  join_paths('images', 'realloc_prob.png'),
  ]

configure_file(input: 'version.xml.in', output: 'version.xml', configuration: version_data)

gnome.gtkdoc('gdv',
           main_xml: 'gdv-docs.xml',
            src_dir: [
                       join_paths(meson.source_root(), 'gdv'),
                       join_paths(meson.build_root(), 'gdv'),
                     ],
       dependencies: libgdv_dep,
  gobject_typesfile: 'gdv.types',
          scan_args: [
                       '--rebuild-types',
                       '--ignore-headers=' + ' '.join(private_headers) +
                       ' gdv-data-matrix.h gdv-data-vector.h',
                     ],
       fixxref_args: [
                       '--html-dir=@0@'.format(docpath),
                       '--extra-dir=@0@'.format(join_paths(glib_docpath, 'glib')),
                       '--extra-dir=@0@'.format(join_paths(glib_docpath, 'gobject')),
                       '--extra-dir=@0@'.format(join_paths(glib_docpath, 'gio')),
                       '--extra-dir=@0@'.format(join_paths(glib_docpath, 'gi')),
                       '--extra-dir=@0@'.format(join_paths(glib_docpath, 'gtk3')),
                     ],
        install_dir: 'gdv',
        html_assets: images,
            install: true)
