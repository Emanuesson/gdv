# Flatpak build

The following command can be used to test the build in a
Flatpak environment with minimal effort:
```
$ flatpak-builder app/ ../build-aux/flatpak/org.gnome.Dataviewer.json --disable-cache --force-clean --disable-tests
```

If rebuilds are necessary, the `--disable-download`-option is 
handy to safe time.

To run the app, use the following command:
```
flatpak-builder --run app ../build-aux/flatpak/org.gnome.Dataviewer.json app/files/bin/dataviewer
```

To run a shell in the flatp-builder environment, run:
```
flatpak-builder --run ../build-aux/flatpak/org.gnome.Dataviewer.json sh

flatpak-builder --run /home/emanuel_schmidt/.var/app/org.gnome.Builder/cache/gnome-builder/projects/libgdv/flatpak/staging/x86_64-eschmidt_wip/ ../build-aux/flatpak/org.gnome.dataviewer.json sh
```

