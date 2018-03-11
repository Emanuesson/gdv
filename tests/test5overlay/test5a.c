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

  GtkWidget *main_window;
  GdvLayer *layer;
  GdvAxis *tmp;

  gtk_init (&argc, &argv);

  main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

//  gtk_widget_set_size_request (main_window, 1800, 1800);

  g_signal_connect (main_window, "destroy",
	      G_CALLBACK (destroy), NULL);
//  gtk_container_set_border_width (GTK_CONTAINER (main_window), 80);

//  layer = g_object_new (gdv_twod_layer_get_type (),
  layer = g_object_new (gdv_oned_layer_get_type (),
//                        "base-orientation", GTK_ORIENTATION_HORIZONTAL, //GTK_ORIENTATION_VERTICAL,
                        NULL);

  gtk_container_add (GTK_CONTAINER (main_window), GTK_WIDGET (layer));

  g_object_get (layer, "axis", &tmp, NULL);
  g_object_set (tmp,
//                "scale-beg-val", -10.0,
//                "scale-end-val", 10.0,
                "scale-increment-base", 30.0,
/*                "axis-orientation", &orientation,
                "axis-direction-outside", &direction_outside,
                "scale-beg-val", &scale_min_val,
                "scale-end-val", &scale_max_val,
                "scale-increment-val", &scale_incr,
                "scale-auto-increment", &scale_auto_increment,
                "scale-limits-automatic", &scale_limits_auto,
                "axis-beg-pix-x", &axis_beg_pix_x,
                "axis-beg-pix-y", &axis_beg_pix_y,
                "axis-end-pix-x", &axis_end_pix_x,
                "axis-end-pix-y", &axis_end_pix_y,
                "axis-beg-at-screen-x", &axis_beg_screen_x,
                "axis-end-at-screen-x", &axis_end_screen_x,
                "axis-beg-at-screen-y", &axis_beg_screen_y,
                "axis-end-at-screen-y", &axis_end_screen_y,
                "tics-beg-val", &tic_beg_val,
                "tics-end-val", &tic_end_val,
                "tics-automatic", &tics_auto,
                "mtics-beg-val", &mtics_beg_val,
                "mtics-end-val", &mtics_end_val,
                "mtics-automatic", &mtics_auto,
                "mtics", &no_of_mtics,
                "force-beg-end", &force_beg_end,*/
//    "axis-orientation", 0.5 * M_PI,
//    "axis-direction-outside", 1.0 * M_PI,
                NULL);


//  gdv_twod_layer_unset_axis (GDV_TWOD_LAYER (layer), GDV_X2_AXIS);
//  gdv_twod_layer_unset_axis (GDV_TWOD_LAYER (layer), GDV_Y2_AXIS);
//  gdv_twod_layer_unset_axis (GDV_TWOD_LAYER (layer), GDV_Y1_AXIS);


/*
  GtkWidget *frame;
  GtkWidget *overlay;

  frame = gtk_frame_new (NULL);
  gtk_container_add (GTK_CONTAINER (main_window), GTK_WIDGET (frame));

  overlay = gtk_overlay_new ();
  gtk_container_add (GTK_CONTAINER (overlay), GTK_WIDGET (layer));
  gtk_container_add (GTK_CONTAINER (frame), GTK_WIDGET (overlay));
*/

/*
  GdvLayerContent *content1, *content2;
  gint i;
  GdvLegend *test_legend;

  test_legend = gdv_legend_new ();

  gtk_overlay_add_overlay (GTK_OVERLAY (overlay), GTK_WIDGET(test_legend));
  //gtk_widget_set_size (GTK_WIDGET(test_legend), 40, 40);
  g_object_set (G_OBJECT(test_legend),
    "margin-start", 180, "margin-end", 180,
    "margin-top", 180, "margin-bottom", 70, NULL);

  content1 = gdv_layer_content_new();
  g_object_set (content1, "title", "sin(x)", NULL);
  gtk_container_add(GTK_CONTAINER (layer), GTK_WIDGET(content1));
  content2 = gdv_layer_content_new();
  g_object_set (content2, "title", "cos(x)", NULL);
  gtk_container_add(GTK_CONTAINER (layer), GTK_WIDGET(content2));

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
*/

  gtk_widget_show_all (GTK_WIDGET (main_window));

  gtk_main ();

  return 0;
}

int
main_BAK (int argc, char **argv)
{
  GtkWidget *window;
  GdvLinearAxis *lin_axis;
  GdvLayer *layer;
//  GList *tics, *mtics;
//  GList *tics_cpy, *mtics_cpy;

//  gdouble orientation, direction_outside;
//  gdouble scale_min_val = 0.0, scale_max_val = 0.0, scale_incr = 0.0;
//  gdouble axis_beg_pix_x = 0.0, axis_end_pix_x = 0.0, axis_beg_pix_y = 0.0, axis_end_pix_y = 0.0;
//  gdouble axis_beg_screen_x = 0.0, axis_end_screen_x = 0.0,
//          axis_beg_screen_y = 0.0, axis_end_screen_y = 0.0;
//  gdouble tic_beg_val = 0.0, tic_end_val = 0.0, mtics_beg_val = 0.0, mtics_end_val = 0.0;
//  gboolean scale_auto_increment, scale_limits_auto, tics_auto, mtics_auto;
//  guint no_of_mtics;
//  gboolean force_beg_end;
//  gint scale_min_diff_pix = 0, scale_max_diff_pix = 0;

  gtk_init (&argc, &argv);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  lin_axis = g_object_new (GDV_LINEAR_TYPE_AXIS, NULL);
  layer = g_object_new (GDV_TYPE_LAYER, NULL);

  g_object_set (lin_axis,
    "axis-orientation", 0.5 * M_PI,
    "axis-direction-outside", 1.0 * M_PI,
    NULL);

  gtk_container_add (GTK_CONTAINER (window), GTK_WIDGET (layer));
  gtk_container_add (GTK_CONTAINER (layer), GTK_WIDGET (lin_axis));
  gtk_container_set_border_width (GTK_CONTAINER (window), 80);

  /* Realizing and Allocating the Layer */
  gtk_widget_show_all (window);

//  gtk_main ();

  return 0;
}

