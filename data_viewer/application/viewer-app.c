/*
 * viewer-app.c
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cairo-gobject.h>

#include "application/viewer-app.h"
#include "gui/viewer-app-win.h"
#include "gui/viewer-welcome-win.h"

#define SLEEP_LGTH  5
#define NPOINTS     50

struct _GdvViewerAppPrivate
{
  GObject           *settings;
  GtkCssProvider    *css_provider;
  GtkStyleContext   *style_context;
};

G_DEFINE_TYPE_WITH_PRIVATE (GdvViewerApp, gdv_viewer_app, GTK_TYPE_APPLICATION);

static void
gdv_viewer_app_activate (GApplication *application)
{
  GdvViewerAppWindow *win;

  win = gdv_viewer_app_window_new (GDV_VIEWER_APP (application));


  gtk_window_present (GTK_WINDOW (win));

//  builder = gtk_builder_new_from_resource ("/org/gtk/exampleapp/app-menu.ui");
//  app_menu = G_MENU_MODEL (gtk_builder_get_object (builder, "appmenu"));

}

static void
gdv_viewer_app_startup (GApplication *application)
{
  G_APPLICATION_CLASS (gdv_viewer_app_parent_class)->startup (application);
}

static void
gdv_viewer_app_init (GdvViewerApp *app)
{
  app->priv = gdv_viewer_app_get_instance_private (app);

}

static void
gdv_viewer_app_dispose (GObject *object)
{
  GdvViewerApp *app = GDV_VIEWER_APP (object);

  g_clear_object (&app->priv->settings);

  G_OBJECT_CLASS (gdv_viewer_app_parent_class)->dispose (object);
}

static void
gdv_viewer_app_finalize (GObject *object)
{
  G_OBJECT_CLASS (gdv_viewer_app_parent_class)->finalize (object);
}

static void
gdv_viewer_app_class_init (GdvViewerAppClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GApplicationClass *application_class = G_APPLICATION_CLASS (klass);

  object_class->dispose = gdv_viewer_app_dispose;
  object_class->finalize = gdv_viewer_app_finalize;

  application_class->startup = gdv_viewer_app_startup;
  application_class->activate = gdv_viewer_app_activate;
}


/**
 * gdv_viewer_app_new:
 *
 * Creates a new #GdvViewerApp. A simple wrapper-function of
 * the #gtk_application_new function.
 *
 * Returns: (transfer none): a new #GdvViewerApp
 */
GdvViewerApp *
gdv_viewer_app_new (void)
{
  /*FIXME: change application-id in correct nomenclature; look for other appearence */
  return g_object_new (gdv_viewer_app_get_type (),
                       "application-id", "net.gdv.viewerapp",
                       NULL);
}
