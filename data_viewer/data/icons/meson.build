icondir = join_paths(get_option('datadir'), 'icons/hicolor')
pkgicondir = join_paths(get_option('datadir'), 'icons/hicolor')

#install_subdir('hicolor/48x48',
#    install_dir: get_option('datadir'), 'icons', 'hicolor', '48x48', 'apps')

install_subdir('hicolor/16x16', install_dir: pkgicondir)
install_subdir('hicolor/24x24', install_dir: pkgicondir)
install_subdir('hicolor/32x32', install_dir: pkgicondir)
install_subdir('hicolor/48x48', install_dir: pkgicondir)
install_subdir('hicolor/64x64', install_dir: pkgicondir)
install_subdir('hicolor/128x128', install_dir: pkgicondir)
install_subdir('hicolor/256x256', install_dir: pkgicondir)
install_subdir('hicolor/512x512', install_dir: pkgicondir)
#install_subdir('hicolor/symbolic', install_dir: pkgicondir)

install_data(['hicolor/48x48/apps/org.gnome.dataviewer.png',
              'hicolor/48x48/apps/org.gnome.DataViewer-symbolic.symbolic.png',],
  install_dir: join_paths(icondir, '48x48/apps'))

#install_data(['hicolor/48x48/org.gnome.dataviewer.png'],)

#icons_dir = join_paths(get_option('datadir'), 'icons', 'hicolor', '48x48', 'apps')

#install_data (icons,
#  install_dir: icons_dir,
#)

#mkdir -p ~/.local/share/applications
#sed -e "s#@bindir@#$PWD#" exampleapp.desktop \
#  > ~/.local/share/applications/lt-exampleapp.desktop

#mkdir -p ~/.local/share/icons/hicolor/48x48/apps
#cp exampleapp.png ~/.local/share/icons/hicolor/48x48/apps