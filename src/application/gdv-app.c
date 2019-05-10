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

#include "application/gdv-app.h"
#include "gui/gdv-app-win.h"
//#include "gui/viewer-welcome-win.h"

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
gdv_viewer_app_open (GApplication  *app,
                     GFile        **files,
                     gint           n_files,
                     const gchar   *hint)
{
  int i;
  GdvViewerAppWindow *win;
  GtkWindow *window;

  window = gtk_application_get_active_window (GTK_APPLICATION (app));
  if (window && GDV_VIEWER_APP_IS_WINDOW (window))
    win = GDV_VIEWER_APP_WINDOW (window);
  else if (window == NULL)
    win = gdv_viewer_app_window_new (GDV_VIEWER_APP (app));
  else
    return;

  for (i = 0; i < n_files; i++)
  {
    gdv_viewer_app_window_open (win, files[i]);
  }

  gtk_window_present (GTK_WINDOW (win));
}

static void
open_activated (GSimpleAction *action,
                GVariant      *parameter,
                gpointer       app)
{
/* Open dialog here! */
  GtkWidget *open_dialog;
  GtkWindow *window;
  GdvViewerAppWindow *win;
  gint res;
  GtkFileChooser *chooser;

  window = gtk_application_get_active_window (GTK_APPLICATION (app));
  if (window && GDV_VIEWER_APP_IS_WINDOW (window))
    win = GDV_VIEWER_APP_WINDOW (window);
  else if (window == NULL)
    win = gdv_viewer_app_window_new (GDV_VIEWER_APP (app));
  else
    return;

  open_dialog = gtk_file_chooser_dialog_new ("open file",
                                             GTK_WINDOW (win),
                                             GTK_FILE_CHOOSER_ACTION_OPEN,
                                             _("_Cancel"),
                                             GTK_RESPONSE_CANCEL,
                                             _("_Open"),
                                             GTK_RESPONSE_ACCEPT,
                                             NULL);
  chooser = GTK_FILE_CHOOSER (open_dialog);
  gtk_file_chooser_set_select_multiple (chooser, TRUE);

  res = gtk_dialog_run (GTK_DIALOG (open_dialog));
  if (res == GTK_RESPONSE_ACCEPT)
  {
    GSList *files, *files_begin;
    GFile **file_array;
    gint no_of_files, i;

    files = gtk_file_chooser_get_filenames (chooser);

    i = 0;
    no_of_files = (gint) g_slist_length (files);
    file_array = (GFile **) g_new (gpointer, no_of_files);
    for (files_begin = files; files; files = files->next)
    {
      GFile *new_file = g_file_new_for_path (files->data);
//      ViewerFile *view_file = viewer_file_new_for_path (files->data);

      g_free (files->data);
      files->data = NULL;
      file_array[i] = new_file;
      i++;
    }
    g_slist_free (files_begin);

    gdv_viewer_app_open (app, file_array, no_of_files, NULL);
    for (i = 0; i < no_of_files; i++)
      g_object_unref (file_array[i]);
    g_free (file_array);
  }

  gtk_widget_destroy (open_dialog);
}

static void
new_window_activated (GSimpleAction *action,
                       GVariant      *parameter,
                       gpointer       app)
{
  gdv_viewer_app_activate (app);
}

static void
close_window_activated (GSimpleAction *action,
                       GVariant      *parameter,
                       gpointer       app)
{
  GtkWindow *current_window;

  g_object_get (app, "active-window", &current_window, NULL);
  gtk_widget_destroy (GTK_WIDGET (current_window));
}

static void
configuration_activated (GSimpleAction *action,
                         GVariant      *parameter,
                         gpointer       app)
{

}

static void
quit_activated (GSimpleAction *action,
                GVariant      *parameter,
                gpointer       app)
{
  g_application_quit (G_APPLICATION (app));
}

static GActionEntry app_entries[] =
{
  { "open", open_activated, NULL, NULL, NULL },
  { "new-window", new_window_activated, NULL, NULL, NULL },
  { "close-window", close_window_activated, NULL, NULL, NULL },
  //{ "configuration", configuration_activated, NULL, NULL, NULL },
  { "quit", quit_activated, NULL, NULL, NULL }
};

static void
gdv_viewer_app_startup (GApplication *application)
{
  GtkBuilder *builder;
  GMenuModel *app_menu;
  const gchar *quit_accels[2] = { "<Ctrl>Q", NULL };
  const gchar *open_accels[2] = { "<Ctrl>O", NULL };
  const gchar *neww_accels[2] = { "<Ctrl>N", NULL };
  const gchar *closew_accels[2] = { "<Ctrl>W", NULL };

  G_APPLICATION_CLASS (gdv_viewer_app_parent_class)->startup (application);

  g_action_map_add_action_entries (G_ACTION_MAP (application),
                                   app_entries, G_N_ELEMENTS (app_entries),
                                   application);
  gtk_application_set_accels_for_action (GTK_APPLICATION (application),
                                         "app.quit",
                                         quit_accels);
  gtk_application_set_accels_for_action (GTK_APPLICATION (application),
                                         "app.open",
                                         open_accels);
  gtk_application_set_accels_for_action (GTK_APPLICATION (application),
                                         "app.new-window",
                                         neww_accels);
  gtk_application_set_accels_for_action (GTK_APPLICATION (application),
                                         "app.close-window",
                                         closew_accels);

  builder = gtk_builder_new_from_resource ("/net/gdv/viewerapp/ui/application/gdv-menu.ui");
  app_menu = G_MENU_MODEL (gtk_builder_get_object (builder, "appmenu"));
  gtk_application_set_app_menu (GTK_APPLICATION (application), app_menu);
  g_object_unref (builder);
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
  application_class->open = gdv_viewer_app_open;

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
                       "application-id", "org.gnome.dataviewer",
                       "flags", G_APPLICATION_HANDLES_OPEN,
                       NULL);
}
