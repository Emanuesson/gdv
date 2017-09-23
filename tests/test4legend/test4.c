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

static void destroy( GtkWidget *widget,
                     gpointer   data )
{
  gtk_main_quit ();
}

int
main (int argc, char **argv)
{
  GdvLayerContent *layer_content;
  GtkWidget *main_box;
  GList *tic_list;

  gint var_index;
  GtkWidget *main_window;
  GtkWidget *frame;
  GdvAxis *test_axis = NULL;
  GtkWidget *test_indicator_label;
  GdvLegend *test_legend;
  GdkScreen *screen_window;
  GdvOnedLayer *layer;
  GdvLayerContent *content1, *content2;

  gtk_init (&argc, &argv);

  main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  main_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);

  test_legend = gdv_legend_new ();

  frame = gtk_frame_new (NULL);
  g_signal_connect (main_window, "destroy",
	      G_CALLBACK (destroy), NULL);
  gtk_container_set_border_width (GTK_CONTAINER (main_window), 8);
  gtk_container_add (GTK_CONTAINER (frame), GTK_WIDGET (main_box));
  gtk_container_add (GTK_CONTAINER (main_window), GTK_WIDGET (frame));

  layer = g_object_new (gdv_twod_layer_get_type (), NULL);
//  g_object_get (layer, "axis", &test_axis, NULL);
//  g_object_set (test_axis, "scale-min-val", -100.0, NULL);

//  report_axis_props (test_axis);

/*
  gint64 test_int64 = G_MAXDOUBLE;
  gdouble test_double = G_MAXINT64;
  g_print ("compare size: double=%d; int64=%e\n",
    test_double, test_int64);

  GTimeZone *new_timezone = g_time_zone_new_local();
  GDateTime *curr_time = g_date_time_new_now (new_timezone);
  g_print ("current time: %d:%d:%d %d:%d:%d\n",
    g_date_time_get_day_of_month (curr_time),
    g_date_time_get_month (curr_time),
    g_date_time_get_year (curr_time),
    g_date_time_get_hour (curr_time),
    g_date_time_get_minute (curr_time),
    g_date_time_get_second (curr_time));
  test_int64 = g_date_time_to_unix (curr_time);
  test_double = g_date_time_to_unix (curr_time);
  g_print ("int64 Unix-time: %d\n", test_int64);
  g_print ("double Unix-time: %e\n", test_double);
  g_print ("TC Unix-time: %d\n", test_double);
  GDateTime *back_time = g_date_time_new_from_unix_local (test_double);
  g_print ("current time: %d:%d:%d %d:%d:%d\n",
    g_date_time_get_day_of_month (back_time),
    g_date_time_get_month (back_time),
    g_date_time_get_year (back_time),
    g_date_time_get_hour (back_time),
    g_date_time_get_minute (back_time),
    g_date_time_get_second (back_time));
*/

  gtk_widget_set_name (GTK_WIDGET (layer), "test-layer");

  gtk_box_pack_start (
    GTK_BOX (main_box), GTK_WIDGET (layer),
    TRUE, TRUE, 0);
  gtk_box_pack_start (
    GTK_BOX (main_box), GTK_WIDGET (test_legend),
    FALSE, TRUE, 0);

  content1 = gdv_layer_content_new();
  g_object_set (content1, "title", "sin(x)", NULL);
  gtk_container_add(GTK_CONTAINER (layer), GTK_WIDGET(content1));
  content2 = gdv_layer_content_new();
  g_object_set (content2, "title", "cos(x)", NULL);
  gtk_container_add(GTK_CONTAINER (layer), GTK_WIDGET(content2));

  gint i;
  for (i = 0; i < 400; i++)
  {
    gdv_layer_content_add_data_point (
      content1,
      M_PI * (gdouble) i  / 200.0,
      sin (M_PI * ((gdouble) i) / 200.0), 0.0);
    gdv_layer_content_add_data_point (
      content2,
      M_PI * (gdouble) i / 200.0,
      cos (M_PI * ((gdouble) i) / 200.0), 0.0);
  }

  g_object_set (test_legend, "layer", layer, NULL);

  gtk_widget_show_all (GTK_WIDGET (main_window));

  gtk_main ();

  return 0;
}

