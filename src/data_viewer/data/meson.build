
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

# D-Bus service file.
dbusconf = configuration_data()
dbusconf.set('bindir', join_paths(get_option('prefix'), get_option('bindir')))
configure_file(
  input: 'org.gnome.dataviewer.service.in',
  output: 'org.gnome.dataviewer.service',
  configuration: dbusconf,
  install: true,
  install_dir: join_paths(get_option('datadir'), 'dbus-1', 'services'),
)

subdir('icons')