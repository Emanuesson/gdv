/*
 * example-app.c
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
//#include <portaudio.h>
#include <math.h>

#include "example-app.h"

typedef struct
{
  GdvLayerContent *content_ra_data;
  GdvLayerContent *content_la_data;
  GdvLayerContent *content_ra_spectrum;
  GdvLayerContent *content_la_spectrum;

  gint index;

  GdvLayerContent *layer_content;
  GdvLayer *layer;
}
gdvExampleContents;

static gboolean run;

static void 
stop_button_callback (GtkButton *button, gpointer user_data)
{
  run = FALSE;

  g_print ("stream stopped\n");
}

static gboolean
timeout_cb (gpointer user_data)
{
  gdvExampleContents *contents = user_data;
  GdvLayerContent *layer_content = contents->layer_content;

  if (run && contents->index < 1000)
  {
    gdv_layer_content_add_data_point (
      layer_content,
      00.1 * (gdouble) contents->index,
      sin (0.1 * (gdouble) contents->index),
      0.0);
    contents->index++;
  }

  return TRUE;
}

static void 
start_button_callback (GtkButton *button, gpointer user_data)
{
  gdvExampleContents *contents = user_data;
  GdvLayerContent *layer_content = contents->layer_content;
  GdvLayer *layer = contents->layer;
  GTask *task;
//  ReadData *op;

  gdv_layer_content_reset (contents->layer_content);

  contents->index = 0;
  run = TRUE;

  g_timeout_add (1000, timeout_cb, contents);

//  op = g_slice_new0 (ReadData);
//  task = g_task_new (stream, cancellable, callback, user_data);
//  g_task_set_task_data (task, op, (GDestroyNotify) free_read_data);
//  g_task_set_priority (task, io_priority);
//  op->buffer = buffer;
//  op->count = count;

//  if (!g_input_stream_async_read_is_via_threads (stream))
//    read_async_pollable (G_POLLABLE_INPUT_STREAM (stream), task);
//  else
//    g_task_run_in_thread (task, read_async_thread);
//  g_object_unref (task);

//  gdv_layer_content_add_data_point (
//    layer_content,
//    00.1 * (gdouble) contents->index,
//    sin (0.01 * (gdouble) contents->index),
//    0.0);

//  gtk_widget_queue_draw (GTK_WIDGET (layer));
//  contents->index++;

  g_print ("starting stream\n");
}

static void
gdv_example_regular_gui_app_activate (GApplication *application)
{
  GtkWidget *app_window;
//  Gdv2dView *simple_view;
  GdvTwodLayer *layer_left_audio, *layer_right_audio;
  GdvTwodLayer *layer_left_spectrum, *layer_right_spectrum;
  GtkWidget *example_label_1, *example_label_2;
  GtkWidget *vbox, *hbox, *start_button, *stop_button;
  GtkWidget *frame;
  GtkWidget *main_grid;
  GdkScreen *screen_window;
  GtkCssProvider *css_provider;
//  GdvLayerContent *layer_content_right_audio;
//  GdvLayerContentStream *layer_content_left_spectrum;
  GdvLayerContent *layer_content_left_audio;

  app_window = gtk_application_window_new(GTK_APPLICATION (application));
  frame = gtk_frame_new (NULL);

  gtk_container_set_border_width (GTK_CONTAINER (app_window), 8);
//  simple_view = gdv_2d_view_new();
  layer_left_audio = g_object_new (gdv_twod_layer_get_type (), NULL);
  layer_left_spectrum = g_object_new (gdv_twod_layer_get_type (), NULL);

  start_button = gtk_button_new_with_label ("start");
  stop_button = gtk_button_new_with_label ("stop");

  hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_box_pack_start (GTK_BOX (hbox), start_button, TRUE, TRUE, 5);
  gtk_box_pack_start (GTK_BOX (hbox), stop_button, TRUE, TRUE, 5);

  vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
//  gtk_box_pack_start (GTK_BOX (vbox), GTK_WIDGET(simple_view), TRUE, TRUE, 5);
  gtk_box_pack_start (GTK_BOX (vbox), GTK_WIDGET(hbox), FALSE, FALSE, 0);

  gtk_container_add (GTK_CONTAINER (app_window), GTK_WIDGET (vbox));

  gtk_widget_set_size_request (GTK_WIDGET (layer_left_audio), 200, 200);
  gtk_widget_set_size_request (GTK_WIDGET (layer_left_spectrum), 200, 200);

  main_grid = gtk_grid_new();
  gtk_grid_attach (GTK_GRID (main_grid), GTK_WIDGET (layer_left_spectrum), 0, 1, 1, 1);
  gtk_grid_attach (GTK_GRID (main_grid), GTK_WIDGET (layer_left_audio), 1, 1, 1, 1);

  g_object_set (G_OBJECT (layer_left_spectrum), "expand", TRUE, NULL);
  g_object_set (G_OBJECT (layer_left_audio), "expand", TRUE, NULL);

//  layer_right_audio = g_object_new (gdv_2d_static_layer_get_type (), NULL);
//  layer_right_spectrum = g_object_new (gdv_2d_static_layer_get_type (), NULL);
//  gtk_widget_set_size_request (GTK_WIDGET (layer_right_audio), 200, 200);
//  gtk_widget_set_size_request (GTK_WIDGET (layer_right_spectrum), 200, 200);
//  g_object_set (G_OBJECT (layer_right_spectrum), "expand", TRUE, NULL);
//  g_object_set (G_OBJECT (layer_right_audio), "expand", TRUE, NULL);
//  gtk_grid_attach (GTK_GRID (main_grid), GTK_WIDGET (layer_right_spectrum), 0, 0, 1, 1);
//  gtk_grid_attach (GTK_GRID (main_grid), GTK_WIDGET (layer_right_audio), 1, 0, 1, 1);

  gtk_container_add (GTK_CONTAINER (frame), GTK_WIDGET (main_grid));
//  gtk_container_add (GTK_CONTAINER (simple_view), GTK_WIDGET (frame));

//  screen_window = gtk_widget_get_screen (GTK_WIDGET (simple_view));
//  css_provider = gtk_css_provider_get_default ();
//  gtk_css_provider_load_from_path (css_provider, "../add_info/example_style_yellow.css", NULL);
//  gtk_style_context_add_provider_for_screen(
//    screen_window,
//    GTK_STYLE_PROVIDER(css_provider),
//    GTK_STYLE_PROVIDER_PRIORITY_USER);

  layer_content_left_audio = gdv_layer_content_new();
//  layer_content_left_spectrum = gdv_layer_content_stream_new();

  gtk_container_add(GTK_CONTAINER (layer_left_audio), GTK_WIDGET(layer_content_left_audio));
//  gtk_container_add(GTK_CONTAINER (layer_left_spectrum), GTK_WIDGET(layer_content_left_spectrum));

  gdvExampleContents *contents = g_new (gdvExampleContents, 1);
  contents->layer_content = layer_content_left_audio;
  contents->layer = GDV_LAYER (layer_left_audio);
  contents->index = 1;
//  contents->content_la_spectrum = GDV_LAYER_CONTENT (layer_content_left_spectrum);

//  g_object_freeze_notify (G_OBJECT (contents->content_la_spectrum));

//  layer_content_right_audio = gdv_layer_content_new();
//  gtk_container_add(GTK_CONTAINER (layer_right_audio), GTK_WIDGET(layer_content_right_audio));
//  contents->content_ra_data = layer_content_right_audio;

  g_signal_connect (start_button, "clicked", G_CALLBACK (start_button_callback), contents);
  g_signal_connect (stop_button, "clicked", G_CALLBACK (stop_button_callback), contents);

  gtk_widget_show_all (GTK_WIDGET (app_window));
}

struct _GdvExampleAppPrivate
{
  GObject           *settings;
  GtkCssProvider    *css_provider;
  GtkStyleContext   *style_context;

  GdvLayer *layer;
  GdvLayerContent *content;

};

G_DEFINE_TYPE_WITH_PRIVATE (GdvExampleApp, gdv_example_app, GTK_TYPE_APPLICATION);

static void
show_about_activated (GSimpleAction *action,
                      GVariant      *parameter,
                      gpointer       user_data)
{
  gtk_show_about_dialog (NULL,
                         "program-name", "Gdv-Example-Suite",
                         "title", "About Gdv",
                         "logo-icon-name", "gdv",
                         "comments", "Prerelease version gdv 0.0",
                         NULL);
}


static void
quit_app_activated (GSimpleAction *action,
                    GVariant      *parameter,
                    gpointer       user_data)
{
  GList *list, *next;
  GtkWindow *win;

  list = gtk_application_get_windows (GTK_APPLICATION (
                                        g_application_get_default ()));

  while (list)
    {
      win = list->data;
      next = list->next;

      gtk_widget_destroy (GTK_WIDGET (win));

      list = next;
    }
}

static void
new_activated (GSimpleAction *action,
               GVariant      *parameter,
               gpointer       user_data)
{
  GApplication *app = user_data;

  g_application_activate (app);
}

static GActionEntry app_entries[] =
{
  { "about", show_about_activated, NULL, NULL, NULL },
  { "quit", quit_app_activated, NULL, NULL, NULL },
  { "new", new_activated, NULL, NULL, NULL }
};

static gboolean
_gdv_example_app_has_app_menu (GdvExampleApp *app)
{
  GtkSettings *gtk_settings;
  gboolean show_app_menu;
  gboolean show_menubar;

  g_return_val_if_fail (GDV_EXAMPLE_IS_APP (app), FALSE);

  /* thanks to Gedit source-code
   * We have three cases:
   * - GNOME 3: show-app-menu true, show-menubar false -> use the app menu
   * - Unity, OSX: show-app-menu and show-menubar true -> use the normal menu
   * - Other WM, Windows: show-app-menu and show-menubar false -> use the normal menu
   */
  gtk_settings = gtk_settings_get_default ();
  g_object_get (G_OBJECT (gtk_settings),
                "gtk-shell-shows-app-menu", &show_app_menu,
                "gtk-shell-shows-menubar", &show_menubar,
                NULL);

  return show_app_menu && !show_menubar;
}

static void
gdv_example_app_startup (GApplication *application)
{
  GdvExampleApp *app = GDV_EXAMPLE_APP (application);

  G_APPLICATION_CLASS (gdv_example_app_parent_class)->startup (application);

  /* initialize app menu on application startup */
  if (_gdv_example_app_has_app_menu (app))
    {
      GtkBuilder *builder;
      GError *error = NULL;

      g_action_map_add_action_entries (G_ACTION_MAP (app),
                                       app_entries,
                                       G_N_ELEMENTS (app_entries),
                                       app);
    }
}

static void
gdv_example_app_init (GdvExampleApp *app)
{

  app->priv = gdv_example_app_get_instance_private (app);


}

static void app_start_stream (GdvExampleApp  *app)
{
}

void app_stop_stream (GdvExampleApp  *app)
{
}

static void
gdv_example_app_dispose (GObject *object)
{
  GdvExampleApp *app = GDV_EXAMPLE_APP (object);

  g_clear_object (&app->priv->settings);

  G_OBJECT_CLASS (gdv_example_app_parent_class)->dispose (object);
}

static void
gdv_example_app_finalize (GObject *object)
{
  GdvExampleApp *app = GDV_EXAMPLE_APP (object);

  G_OBJECT_CLASS (gdv_example_app_parent_class)->finalize (object);
}

static void
gdv_example_app_class_init (GdvExampleAppClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GApplicationClass *application_class = G_APPLICATION_CLASS (klass);

  object_class->dispose = gdv_example_app_dispose;
  object_class->finalize = gdv_example_app_finalize;

//  klass->start_stream = app_start_stream;
//  klass->stop_stream = app_stop_stream;
  klass->start_stream = NULL;
  klass->stop_stream = NULL;

  application_class->startup = gdv_example_app_startup;

  application_class->activate = gdv_example_regular_gui_app_activate;
}


/**
 * gdv_example_app_new:
 *
 * Creates a new #GdvExampleApp. A simple wrapper-function of
 * the #gtk_application_new function.
 *
 * Returns: (transfer none): a new #GdvExampleApp
 */
GdvExampleApp *
gdv_example_app_new (void)
{
  /*FIXME: change application-id in correct nomenclature; look for other appearence */
  return g_object_new (gdv_example_app_get_type (),
                       "application-id", "org.gtk.Gdv.GdvExampleApp",
                       NULL);
}
