#!/usr/bin/env python3

import os
import subprocess

#prefix = os.environ.get('MESON_INSTALL_PREFIX', '/usr/local')
prefix = os.path.join(os.path.abspath(os.sep), 'app')
datadir = os.path.join(prefix, 'share')
builder_dir = os.path.join(datadir, 'gnome-builder')


# Packaging tools define DESTDIR and this isn't needed for them
if 'DESTDIR' not in os.environ:
    print('Compiling gsettings schemas...')
    subprocess.call(['glib-compile-schemas',
                     os.path.join(datadir, 'glib-2.0', 'schemas')])

    print('Updating icon cache...')
    subprocess.call(['gtk-update-icon-cache', '-qtf',
                     os.path.join(datadir, 'icons', 'hicolor')])

    print('Updating icon cache...')
    subprocess.call(['gtk-update-icon-cache', '-qtf',
                     os.path.join(builder_dir, 'icons', 'hicolor')])

    print('Updating desktop database...')
    subprocess.call(['update-desktop-database', '-q',
                     os.path.join(datadir, 'applications')])

