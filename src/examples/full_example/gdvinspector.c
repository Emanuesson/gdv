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
#include <math.h>
#include <gtk/gtk.h>

#include <gdv/gdv.h>
#include "gdvinspector-app.h"

/* TODO: parsing command-line arguments to create a small test from this exe */

int
main (int argc, char **argv)
{
/*  GdvLayerContent *layer_content;

  GtkWidget *test_indicator_label;

  GdkScreen *screen_window;
  GtkCssProvider *css_provider;

  gtk_init (&argc, &argv);

  main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  frame = gtk_frame_new (NULL);
  g_signal_connect (main_window, "destroy",
	      G_CALLBACK (destroy), NULL);
  gtk_container_set_border_width (GTK_CONTAINER (main_window), 8);
  gtk_container_add (GTK_CONTAINER (main_window), GTK_WIDGET (frame));

  layer = g_object_new (gdv_layer_get_type (), NULL);
  test_axis = g_object_new (gdv_axis_get_type (), NULL);
  gtk_container_add (GTK_CONTAINER (layer), GTK_WIDGET (test_axis));

  main_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_container_add (GTK_CONTAINER (main_box), GTK_WIDGET (layer));
  gtk_container_add (GTK_CONTAINER (frame), GTK_WIDGET (main_box));

  g_print ("Gdv::Testing the next layer with indicator\n");

  var_index = 0;
  g_timeout_add (10, ((GSourceFunc) timeout_cb), NULL);
*/

  GdvInspectorApp *primary_instance;
  int status = 0;

  primary_instance = gdv_inspector_app_new();

  status = g_application_run (G_APPLICATION (primary_instance), argc, argv);
  g_object_unref (primary_instance);

//  gtk_widget_show_all (GTK_WIDGET (main_window));
//  gtk_main ();

  return status;
}

