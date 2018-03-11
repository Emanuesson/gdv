General Information
===================

Gdv is a graphical library, that adds some functionality to the GTK+-library to present and visualize scientific and generally all kind of data.

Status
======

The project is in a very early development state. Some things are already working, some things need a lot of rework and on many necessary things, work has not even started yet.

The GTK+-based-Data-Visualization-library
=========================================

The GDV-library tries to provide a simple data-visualization-library in the GTK+-framework. It is the attempt to use powerful programming-features of the glib- and GTK+-libraries such as object-oriented programming, referencing, introspection, widget-based geometry handling and theming by CSS-files in a library that is able to provide professional data-plots.

I wrote this library because I found the GTK+-framework and the glib-features quite handy in everyday programming. For the advantages of these libraries I should refer to the [gnome developer documentation](https://developer.gnome.org/). LibGdv should always orient on the native GTK+-theming engine and geometry-management and of course it should be possible to use it within the GTK+-builder and with gobject-introspection. The focus in the development should lie on a good and active integration in GTK+ to achieve a high portability and offer the features of user customization that GTK+ is able to offer. Of course it would be nice to see that these advantages are useful under non-scientific and non-engineering applications too.

Differences to other plotting libraries
=======================================

A powerful alternative is [libgoffice](https://github.com/GNOME/goffice), but despite it's strengths and functionality it is far more specialized onto office-applications than on the merely technical side.

Further alternatives, that should be considered are [GtkDataBox](https://sourceforge.net/projects/gtkdatabox/), [GtkExtra](http://gtkextra.sourceforge.net/cms/), the [GtkInstrumentationWidgets](https://sourceforge.net/projects/giw/), [slope](https://github.com/elvismt/slope) and [glinegraph](https://sourceforge.net/projects/gapcmon/files/). These libraries are to some part outdated and/or have a lack of functionality in terms of the current GTK+-interfaces. They are in many cases limited to GTK+2.0 only. An further integration of these libraries into the recent interface-builder such as GtkBuilder or Glade is rather difficult.

However strongly simplified libraries such as [slope](https://github.com/elvismt/slope) are by far less processing intensive than the LibGdv and for applications with limited processing resources these libraries are good and reasonable alternatives that should be taken into consideration.

Another good solution is to write an own graph-widget for the specific application. As a base for such a widget the GtkDrawingArea can be used. This solution seems to be somehow the best praxis in many Gnome-Apps for the moment. As some good examples look at the [load-graph widget in the gnome-sysmtem-monitor](https://github.com/GNOME/gnome-system-monitor/blob/master/src/load-graph.cpp) or the [realtime-graph of gnome-builder](https://git.gnome.org/browse/libdazzle/tree/src/graphing).

More indirect alternatives such as an [implementation of gnuplot in GTK+](https://github.com/joastbg/gtk-socket-gnuplot) or an implementation of a JavaScript based plotting library such as [lotr2 via webkit2 in GTK+](https://github.com/Emanuesson/ExampleFlotr2GTK) is, beside its advantages, in my personal opinion no long-term solution, as an interaction between the plotting and the user will be over complicated. A very nice approach in terms of usability and visual impression is using [Matplotlib in combination with GObject-introspection](http://gtk3-matplotlib-cookbook.readthedocs.io/en/latest/hello-plot.html#embedding-matplotlib). However the interaction with Gtk+ is similarly unilaterally limited as in the other indirect examples.

This open gap of scientific and instrument-like data plotting widgets should be closed to make GTK+ a convenient option for scientific and technical applications.

A few words to the current development status
=============================================

So far the master-branch is in an very early state. It is just possible to draw simple linear 2d-plots and graphs without more specialized things. The [examples](https://emanuesson.github.io/gdv/getting-started.html#id-1.2.3.8) given in the documentation show some basic functionality of the Gdv-library.

The good things
---------------

So far it is possible to plot data in different line and point styles with different colors in one or two-dimensions with either linear, semi-logarithmic or double-logarithmic graphs. Some other features and objects that are included are different kind of legends, tics, axes-labels and indicators, that can be used to mark a value on one particular axis.

These objects and their style-properties fully integrate in the theme-handling of GTK+ and therefor it is possible to use CSS-files for theme styling.

For the moment a rather minimalist but working geometry-handling on the basis of GTK+ is implemented:

[automatic scaling](https://github.com/Emanuesson/gdv/blob/master/add_info/promo_images/optim.gif)

The bad things
--------------

More specialized elements of data visualization such as three dimensional visualisation, error-bars, graph-meshes, histogram charts, radar-plots, pie charts, labels, arrows, fillings, time-formatted tic-labels and color effects are missing so far. Therfor it can't really challenge existing scientific plotting libraries or programms.

It is not clear if values of angles should be assigned in deg. or in grad.. Both implementations are used in GTK+ and GLib. However for the moment most angles are given in gradients to be consistent with the standard C math-libraries and most of the practical knowledge of the practical programmer.

The ugly
--------

Sometimes the coordinate-systems are not really obvious. Sometimes things are aligned relative of the drawing area, sometimes according to Pixel-units and sometimes in data-units. This is mainly caused by the mixture of influences from gnuplot and the gdk-rendering. These problems should be consistently solved.

The geometry-handling and height-for-width management is currently not perfectly designed. This is a problem arising from the different needs of the height-for-width management of GTK+ and LibGdv. It may induce also problems of aligning correct gdk-windows and arrangements for plotting-elements. An integration of these features should be considered for now as "experimental".

Due to the lack of stability in the development, Gdv does not provide a stable API so far. Changes in the concept and functionality require changes in the functions at this stage of the development. Therefor changes of function definitions, methods and properties might occur even with the release of new micro-versions.

Annotation about the problems in geometry-handling:

Some problems could be eased if the baseline-alignment of gtk+-widgets that is currently (since GTK+ version 3.10) just available for vertical alignment would be available for horizontal alignment too. However, this is hypothetical additional functionality of Gtk+ and a different story.

Documentation
=============

There is a compiled html-version of the [gdv reference manual](http://emanuesson.github.io/gdv/index.html) included in the repository.

Please feel free to contact me via github or elsewhere, if you have any further suggestions or questions.

Installation
============

In order to build gdv you will need:

  - a C99 compatible compiler
  - Python 3
  - [Meson](http://mesonbuild.com)
  - [Ninja](https://ninja-build.org)
  - [Gtk+](https://www.gtk.org/)
  - [GLib](https://download.gnome.org/sources/glib)

You will also need various dependencies, based on the platform you are
building for:

  - [GObject-Introspection](https://download.gnome.org/sources/gobject-introspection)

Once you have all the necessary dependencies, you can build gdv by using
Meson:

```sh
$ meson build
$ cd build
$ ninja
```

If you need to build the documentation enter the following commands:

```sh
$ meson build
$ cd build
$ meson configure -Denable_gtk_doc=true
$ ninja
$ ninja gdv-doc
```

You can run the test suite using:

```sh
$ ninja test
```

And, finally, you can install gdv using:

```
$ sudo ninja install
```

Complete information about installing gdv and related libraries
can be found in the file:

```
docs/reference/gdv/html/index.html
```

Or [online](https://emanuesson.github.io/#gdv)
