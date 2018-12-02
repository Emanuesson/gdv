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
```

