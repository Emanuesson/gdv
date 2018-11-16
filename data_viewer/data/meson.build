
desktop_file = i18n.merge_file(
  input: 'org.gnome.dataviewer.desktop.in.in',
  output: 'org.gnome.dataviewer.desktop',
  install_dir: join_paths(get_option('datadir'), 'applications'),
  type: 'desktop',
  po_dir: '../po',
  install: true,
)

desktop_utils = find_program('desktop-file-validate', required: false)
if desktop_utils.found()
  test('Validate desktop file', desktop_utils,
    args: [desktop_file]
  )
endif

subdir('icons')