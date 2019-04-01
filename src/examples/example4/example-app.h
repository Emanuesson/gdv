/*
 * example-app.h
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

#ifndef GDV_EXAMPLE_APP_H_INCLUDED
#define GDV_EXAMPLE_APP_H_INCLUDED

#include <stdlib.h>
#include <gtk/gtk.h>
#include <gtk/gtkx.h>
#include <gdk/gdkx.h>

#include <gdv/gdv.h>

#include <cairo-gobject.h>

G_BEGIN_DECLS

#define GDV_EXAMPLE_APP_TYPE                 (gdv_example_app_get_type ())
#define GDV_EXAMPLE_APP(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), GDV_EXAMPLE_APP_TYPE, GdvExampleApp))
#define GDV_EXAMPLE_IS_APP(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_EXAMPLE_APP_TYPE))
#define GDV_EXAMPLE_APP_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), GDV_EXAMPLE_APP_TYPE, GdvExampleAppClass))
#define GDV_EXAMPLE_APP_IS_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), GDV_EXAMPLE_APP_TYPE))
#define GDV_EXAMPLE_APP_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), GDV_EXAMPLE_APP_TYPE, GdvExampleAppClass))

typedef struct _GdvExampleApp GdvExampleApp;
typedef struct _GdvExampleAppClass GdvExampleAppClass;
typedef struct _GdvExampleAppPrivate GdvExampleAppPrivate;

struct _GdvExampleApp
{
  GtkApplication parent;

  /*< private > */
  GdvExampleAppPrivate *priv;
};

struct _GdvExampleAppClass
{
  GtkApplicationClass parent_class;

  void (*start_stream) (GdvExampleApp  *app);
  void (*stop_stream) (GdvExampleApp  *app);
};

/* Public Method definitions. */
GType gdv_example_app_get_type (void);

GdvExampleApp *gdv_example_app_new (void);

G_END_DECLS

#endif // GDV_EXAMPLE_APP_H_INCLUDED

