/*
 * gdvinspector-app.c
 * This file is part of gevse
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cairo-gobject.h>

#include "gdvinspector-app.h"
#include "gdvinspector-app-win.h"

struct _GdvInspectorAppPrivate
{
  gint placeholder;
};

G_DEFINE_TYPE_WITH_PRIVATE (GdvInspectorApp, gdv_inspector_app, GTK_TYPE_APPLICATION);

static void
gdv_inspector_app_activate (GApplication *application)
{
  GdvInspectorAppWindow *win;

  win = gdv_inspector_app_window_new (GDV_INSPECTOR_APP (application));
//  gtk_window_present (GTK_WINDOW (win));
//  gtk_window_maximize (GTK_WINDOW (win));

  gtk_window_present (GTK_WINDOW (win));
//  gtk_widget_show_all (GTK_WIDGET (win));

//  builder = gtk_builder_new_from_resource ("/org/gtk/exampleapp/app-menu.ui");
//  app_menu = G_MENU_MODEL (gtk_builder_get_object (builder, "appmenu"));

}

static void
gdv_inspector_app_startup (GApplication *application)
{
  GdvInspectorApp *app = GDV_INSPECTOR_APP (application);

  G_APPLICATION_CLASS (gdv_inspector_app_parent_class)->startup (application);
}

static void
gdv_inspector_app_init (GdvInspectorApp *app)
{

  app->priv = gdv_inspector_app_get_instance_private (app);    

}

static void
gdv_inspector_app_dispose (GObject *object)
{
  GdvInspectorApp *app = GDV_INSPECTOR_APP (object);

  G_OBJECT_CLASS (gdv_inspector_app_parent_class)->dispose (object);
}

static void
gdv_inspector_app_finalize (GObject *object)
{
  GdvInspectorApp *app = GDV_INSPECTOR_APP (object);

  G_OBJECT_CLASS (gdv_inspector_app_parent_class)->finalize (object);
}

static void
gdv_inspector_app_class_init (GdvInspectorAppClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GApplicationClass *application_class = G_APPLICATION_CLASS (klass);

  object_class->dispose = gdv_inspector_app_dispose;
  object_class->finalize = gdv_inspector_app_finalize;

  application_class->startup = gdv_inspector_app_startup;
  application_class->activate = gdv_inspector_app_activate;
}


/**
 * gdv_inspector_app_new:
 *
 * Creates a new #GdvInspectorApp. A simple wrapper-function of
 * the #gtk_application_new function.
 *
 * Returns: (transfer none): a new #GdvInspectorApp
 */
GdvInspectorApp *
gdv_inspector_app_new (void)
{
  /*FIXME: change application-id in correct nomenclature; look for other appearence */
  return g_object_new (gdv_inspector_app_get_type (),
                       "application-id", "net.libgdv.gdvinspector",
                       NULL);
}
