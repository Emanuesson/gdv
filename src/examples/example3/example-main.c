/*
 * example-main.c
 * This file is part of gdv
 *
 * Copyright (C) 2013 - Emanuel Schmidt
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <gtk/gtk.h>

#include <gdv/gdv.h>

static void destroy( GtkWidget *widget,
                     gpointer   data )
{
    gtk_main_quit ();
}

int
main (int argc, char **argv)
{
  GtkWidget *main_window;
//  Gdv2dView *simple_view;
  GdvTwodLayer *static_layer;
  GtkWidget *frame;
  GtkWidget *main_grid;
  GdkScreen *screen_window;
  GtkCssProvider *css_provider;
  GdvLayerContent *layer_content;

  gtk_init (&argc, &argv);

  main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  frame = gtk_frame_new (NULL);

  g_signal_connect (main_window, "destroy",
	      G_CALLBACK (destroy), NULL);

  gtk_container_set_border_width (GTK_CONTAINER (main_window), 8);
  gtk_container_add (GTK_CONTAINER (main_window), GTK_WIDGET (frame));

//  simple_view = gdv_2d_view_new();
//  gtk_container_add (GTK_CONTAINER (frame), GTK_WIDGET (simple_view));

  static_layer = g_object_new (gdv_twod_layer_get_type (), NULL);
//  gtk_container_add (GTK_CONTAINER (simple_view), GTK_WIDGET (static_layer));

  gtk_widget_set_size_request (GTK_WIDGET (static_layer), 200, 200);

  g_object_set (G_OBJECT (static_layer), "expand", TRUE, NULL);

//  screen_window = gtk_widget_get_screen (GTK_WIDGET (simple_view));
//  css_provider = gtk_css_provider_get_default ();
//  gtk_css_provider_load_from_path (css_provider, "../add_info/test_style_blue.css", NULL);
//  gtk_style_context_add_provider_for_screen(
//    screen_window,
//    GTK_STYLE_PROVIDER(css_provider),
//    GTK_STYLE_PROVIDER_PRIORITY_USER);

  layer_content = gdv_layer_content_new();

  gtk_container_add(GTK_CONTAINER (static_layer), GTK_WIDGET(layer_content));
//  gdv_layer_content_add_data_point (layer_content, 0.0, 0.0, 0.0);
//  gdv_layer_content_add_data_point (layer_content, -0.5, -0.5, 0.0);
  gdv_layer_content_add_data_point (layer_content, 1.0, 0.0, 0.0);
  gdv_layer_content_add_data_point (layer_content, 1.2, 0.0, 0.0);
  gdv_layer_content_add_data_point (layer_content, 2.0, 0.0, 0.0);
  gdv_layer_content_add_data_point (layer_content, 4.9, 0.0, 0.0);
  //gdv_layer_content_add_data_point (layer_content, -0.9, -0.9, 0.0);

  gtk_widget_show_all (GTK_WIDGET (main_window));
  gtk_main ();

//  while( )
//  {
//    Pa_Sleep(1000);
//  }
//  if( err < 0 ) return;

  return 0;
}

