/*
 * viewer-app.h
 * This file is part of gdv
 *
 * Copyright (C) 2013 - Emanuel Schmidt
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GDV_VIEWER_APP_H_INCLUDED
#define GDV_VIEWER_APP_H_INCLUDED

#include <stdlib.h>
#include <gtk/gtk.h>
#include <gtk/gtkx.h>
#include <gdk/gdkx.h>

#include <gdv.h>


#include <cairo-gobject.h>

#include <gdv/gdv.h>

#include<X11/Xlib.h>

G_BEGIN_DECLS

#define GDV_VIEWER_APP_TYPE                 (gdv_viewer_app_get_type ())
#define GDV_VIEWER_APP(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), GDV_VIEWER_APP_TYPE, GdvViewerApp))
#define GDV_VIEWER_IS_APP(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_VIEWER_APP_TYPE))
#define GDV_VIEWER_APP_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), GDV_VIEWER_APP_TYPE, GdvViewerAppClass))
#define GDV_VIEWER_APP_IS_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), GDV_VIEWER_APP_TYPE))
#define GDV_VIEWER_APP_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), GDV_VIEWER_APP_TYPE, GdvViewerAppClass))

typedef struct _GdvViewerApp GdvViewerApp;
typedef struct _GdvViewerAppClass GdvViewerAppClass;
typedef struct _GdvViewerAppPrivate GdvViewerAppPrivate;

struct _GdvViewerApp
{
  GtkApplication parent;

  /*< private > */
  GdvViewerAppPrivate *priv;
};

struct _GdvViewerAppClass
{
  GtkApplicationClass parent_class;
};

/* Public Method definitions. */
GType gdv_viewer_app_get_type (void);

GdvViewerApp *gdv_viewer_app_new (void);

G_END_DECLS

#endif // GDV_VIEWER_APP_H_INCLUDED

