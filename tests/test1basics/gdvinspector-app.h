/*
 * gdvinspector-app.h
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

#ifndef GDV_INSPECTOR_APP_H_INCLUDED
#define GDV_INSPECTOR_APP_H_INCLUDED

#include <stdlib.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GDV_INSPECTOR_APP_TYPE                 (gdv_inspector_app_get_type ())
#define GDV_INSPECTOR_APP(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), GDV_INSPECTOR_APP_TYPE, GdvInspectorApp))
#define GDV_INSPECTOR_IS_APP(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_INSPECTOR_APP_TYPE))
#define GDV_INSPECTOR_APP_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), GDV_INSPECTOR_APP_TYPE, GdvInspectorAppClass))
#define GDV_INSPECTOR_APP_IS_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), GDV_INSPECTOR_APP_TYPE))
#define GDV_INSPECTOR_APP_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), GDV_INSPECTOR_APP_TYPE, GdvInspectorAppClass))

typedef struct _GdvInspectorApp GdvInspectorApp;
typedef struct _GdvInspectorAppClass GdvInspectorAppClass;
typedef struct _GdvInspectorAppPrivate GdvInspectorAppPrivate;

struct _GdvInspectorApp
{
  GtkApplication parent;

  /*< private > */
  GdvInspectorAppPrivate *priv;
};

struct _GdvInspectorAppClass
{
  GtkApplicationClass parent_class;
};

/* Public Method definitions. */
GType gdv_inspector_app_get_type (void);

GdvInspectorApp *gdv_inspector_app_new (void);

G_END_DECLS

#endif // GDV_INSPECTOR_APP_H_INCLUDED

