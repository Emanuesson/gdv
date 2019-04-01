/*
 * test-app.h
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

#ifndef GDV_TEST_APP_H_INCLUDED
#define GDV_TEST_APP_H_INCLUDED

#include <stdlib.h>
#include <gtk/gtk.h>
#include <gtk/gtkx.h>
#include <gdk/gdkx.h>

#include <gdv/gdv.h>

#include <cairo-gobject.h>

G_BEGIN_DECLS

#define GDV_TEST_APP_TYPE                 (gdv_test_app_get_type ())
#define GDV_TEST_APP(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), GDV_TEST_APP_TYPE, GdvTestApp))
#define GDV_TEST_IS_APP(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_TEST_APP_TYPE))
#define GDV_TEST_APP_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), GDV_TEST_APP_TYPE, GdvTestAppClass))
#define GDV_TEST_APP_IS_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), GDV_TEST_APP_TYPE))
#define GDV_TEST_APP_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), GDV_TEST_APP_TYPE, GdvTestAppClass))

typedef struct _GdvTestApp GdvTestApp;
typedef struct _GdvTestAppClass GdvTestAppClass;
typedef struct _GdvTestAppPrivate GdvTestAppPrivate;

struct _GdvTestApp
{
  GtkApplication parent;

  /*< private > */
  GdvTestAppPrivate *priv;
};

struct _GdvTestAppClass
{
  GtkApplicationClass parent_class;
};

/* Public Method definitions. */
GType gdv_test_app_get_type (void);

GdvTestApp *gdv_test_app_new (void);

G_END_DECLS

#endif // GDV_TEST_APP_H_INCLUDED

