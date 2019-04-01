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

void
report_tic_props (GdvTic *tic)
{
  gdouble value;
  gdouble pos_x, pos_y, axis_dir_x, axis_dir_y, axis_inner_dir_x, axis_inner_dir_y;
  gdouble tics_in_length, tics_out_length;
  gchar *tic_format;
  gboolean tic_label;

  g_object_get (tic,
    "value", &value,
    "pos-x", &pos_x,
    "pos-y", &pos_y,
    "axis-dir-x", &axis_dir_x,
    "axis-dir-y", &axis_dir_y,
    "axis-inner-dir-x", &axis_inner_dir_x,
    "axis-inner-dir-y", &axis_inner_dir_y,
    NULL);

  gtk_widget_style_get (GTK_WIDGET (tic),
    "tics-in-length", &tics_in_length,
    "tics-out-length", &tics_out_length,
    "show-label", &tic_label,
    "tic-format", &tic_format,
    NULL);

  g_printerr ("\n---------------------------------------<\n");
  g_printerr ("Tic-Object: %p of type: %s\n",
    tic, g_type_name (G_TYPE_FROM_INSTANCE(tic)));
  g_printerr ("---------------------------------------|\n");
  g_printerr ("value: %e\n", value);
  g_printerr ("Position: %e;%e\n", pos_x, pos_y);
  g_printerr ("Axis-dir: %e;%e\n", axis_dir_x, axis_dir_y);
  g_printerr ("Inner-dir: %e;%e\n", axis_inner_dir_x, axis_inner_dir_y);
  g_printerr ("In-length: %e - Out-length: %e\n", tics_in_length, tics_out_length);
  if (!tic_label)
    g_printerr ("don't ");
  g_printerr ("plot a label with format: \"%s\"\n", tic_format);
  g_printerr ("--------------------------------------->\n\n");
  return;
}

void
report_axis_props (GdvAxis *axis)
{
  gdouble axis_min_val = 0.0, axis_max_val = 0.0, scale_incr = 0.0;
  guint axis_beg_pix_x = 0, axis_end_pix_x = 0, axis_beg_pix_y = 0, axis_end_pix_y = 0;
  gdouble axis_beg_screen_x = 0.0, axis_end_screen_x = 0.0,
          axis_beg_screen_y = 0.0, axis_end_screen_y = 0.0;
  gdouble tic_beg_val = 0.0, tic_end_val = 0.0, mtics_beg_val = 0.0, mtics_end_val = 0.0;
  gint scale_min_diff_pix = 0, scale_max_diff_pix = 0;
  gboolean tic_labels, scale_auto_increment, scale_limits_auto, tics_auto, mtics_auto;
  guint no_of_mtics;

  g_object_get (axis,
    "scale-min-val", &axis_min_val,
    "scale-max-val", &axis_max_val,
    "scale-increment-val", &scale_incr,
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
    "mtics-beg-val", &mtics_beg_val,
    "mtics-end-val", &mtics_end_val,
    "tic-labels", &tic_labels,
//    "scale-auto-increment", &scale_auto_increment,
//    "scale-min-diff-pix", &scale_min_diff_pix,
//    "scale-max-diff-pix", &scale_max_diff_pix,
//    "scale-limits-automatic", &scale_limits_auto,
//    "tics-automatic", &tics_auto,
//    "mtics-automatic", &mtics_auto,
//    "mtics-number",  &no_of_mtics,
    NULL);

  gtk_widget_style_get (GTK_WIDGET (axis),
    "scale-auto-increment", &scale_auto_increment,
    "scale-min-diff-pix", &scale_min_diff_pix,
    "scale-max-diff-pix", &scale_max_diff_pix,
    "scale-limits-automatic", &scale_limits_auto,
    "tics-automatic", &tics_auto,
    "mtics-automatic", &mtics_auto,
    "mtics-number",  &no_of_mtics,
//                  "axis-line-width", &axis_line_width,
//                "axis-line-source", &axis_line_source,
//                "tics-color", &tics_lc,
//                "tics-line-width", &tics_line_width,
//                "tics-in-length", &tics_inner_length,
//                "tics-out-length", &tics_outer_length,
//                "mtics-color", &mtics_lc,
//                "mtics-line-width", &mtics_line_width,
//                "mtics-in-length", &mtics_inner_length,
//                "mtics-out-length", &mtics_outer_length,
    NULL);

  g_printerr ("\n---------------------------------------<\n");
  g_printerr ("Axis-Object: %p of type: %s\n",
    axis, g_type_name (G_TYPE_FROM_INSTANCE(axis)));
  g_printerr ("---------------------------------------|\n");
  g_printerr ("scale: %e:%e:%e\n",
    axis_min_val, scale_incr, axis_max_val);
  g_printerr ("plot from (%u:%u) to (%u:%u)\n",
    axis_beg_pix_x, axis_beg_pix_y, axis_end_pix_x, axis_end_pix_y);
  g_printerr ("plot @screen from (%.2e:%.2e) to (%.2e:%.2e)\n",
    axis_beg_screen_x, axis_beg_screen_y, axis_end_screen_x, axis_end_screen_y);
  g_printerr ("tics from %e to %e\n", tic_beg_val, tic_end_val);
  g_printerr ("mtics from %e to %e\n", mtics_beg_val, mtics_end_val);
  if (!tic_labels)
    g_printerr ("don't ");
  g_printerr ("print show-label\n");
  g_printerr ("Scale diff Minimum Pixel: %d, Maximum Pixel: %d\n",
    scale_min_diff_pix, scale_max_diff_pix);
  if (!scale_auto_increment)
    g_printerr ("don't ");
  g_printerr ("recalculate the scale\n");
  if (!scale_limits_auto)
    g_printerr ("don't ");
  g_printerr ("recalculate the scale-limits\n");
  if (!tics_auto)
    g_printerr ("don't ");
  g_printerr ("calculate the tics automatically\n");
  if (!mtics_auto)
    g_printerr ("don't ");
  g_printerr ("calculate the mtics automatically\n");
  g_printerr ("number of mtics: %d\n", no_of_mtics);
  g_printerr ("--------------------------------------->\n\n");

  return;
}

static gboolean added;


int
main (int argc, char **argv)
{
  GtkWidget *main_window;
  GtkWidget *frame;
//  GdvAxis *test_axis = NULL;

  GdvTwodLayer *layer;

  GdvAxis *tmp_axis;

  gtk_init (&argc, &argv);

  main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  frame = gtk_frame_new (NULL);
  g_signal_connect (main_window, "destroy",
	      G_CALLBACK (destroy), NULL);
  gtk_container_set_border_width (GTK_CONTAINER (main_window), 8);
  gtk_container_add (GTK_CONTAINER (main_window), GTK_WIDGET (frame));

  layer = g_object_new (gdv_twod_layer_get_type (), NULL);
//  g_object_get (layer, "axis", &test_axis, NULL);
//  g_object_set (test_axis, "scale-min-val", -100.0, NULL);

//  report_axis_props (test_axis);

  gtk_widget_set_name (GTK_WIDGET (layer), "test-layer");

  gtk_container_add (GTK_CONTAINER (frame), GTK_WIDGET (layer));

  gtk_widget_show_all (GTK_WIDGET (main_window));

  tmp_axis =
    gdv_twod_layer_get_axis (GDV_TWOD_LAYER (layer),
      GDV_Y2_AXIS);
//  gtk_widget_hide (GTK_WIDGET (tmp_axis));
//  g_object_set (tmp_axis,
//    "visible", FALSE,
//    NULL);

  tmp_axis =
    gdv_twod_layer_get_axis (GDV_TWOD_LAYER (layer),
      GDV_X2_AXIS);
//  gtk_widget_hide (GTK_WIDGET (tmp_axis));
  g_object_set (tmp_axis,
    "visible", TRUE,
    NULL);

  tmp_axis =
    gdv_twod_layer_get_axis (GDV_TWOD_LAYER (layer),
      GDV_Y1_AXIS);
//  gtk_widget_hide (GTK_WIDGET (tmp_axis));
  g_object_set (tmp_axis,
    "visible", TRUE,
    NULL);
//  gtk_container_remove (GTK_CONTAINER (layer), GTK_WIDGET (tmp_axis));

  tmp_axis =
    gdv_twod_layer_get_axis (GDV_TWOD_LAYER (layer),
      GDV_X1_AXIS);
//  g_object_set (tmp_axis,
//    "tics-automatic", FALSE,
//    "mtics-automatic", FALSE,
//    "scale-limits-automatic", FALSE,
//    "scale-auto-increment", FALSE,
//    "scale-increment-val", 14400.0,
//    "scale-beg-val", 0.0,
//    "scale-end-val", 86400.0,
//    "scale-end-val", 220.0,
//    "scale-increment-base", 60.0,
//    "tics-beg-val", 0.0,
//    "tics-end-val", 120.0,
//    NULL);
/*
  tmp_axis =
    GDV_AXIS (g_object_new (gdv_linear_axis_get_type (),
          "halign", GTK_ALIGN_START,
          "valign", GTK_ALIGN_FILL,
          "axis-orientation", 0.0,
          "axis-direction-outside", -0.5*M_PI,
          "visible", FALSE,
          "scale-increment-val", 3600.0,
          "tics-automatic", TRUE,
//          "mtics-automatic", FALSE,
          "scale-limits-automatic", FALSE,
          "scale-beg-val", 0.0,
          "scale-end-val", 86400.0,
//          "scale-auto-increment", FALSE,
          NULL));
  gtk_widget_set_name (GTK_WIDGET (tmp_axis), "y1-axis");
  gtk_widget_show(GTK_WIDGET (tmp_axis));

  gdv_twod_layer_set_axis (GDV_TWOD_LAYER (layer),
    GDV_AXIS (tmp_axis),
    GDV_Y1_AXIS);
*/

  tmp_axis = 
    GDV_AXIS (g_object_new (gdv_linear_axis_get_type (),
//    GDV_AXIS (g_object_new (gdv_special_time_axis_get_type (),
          "halign", GTK_ALIGN_START,
          "valign", GTK_ALIGN_FILL,
          "axis-orientation", 0.0,
          "axis-direction-outside", -0.5*M_PI,
/*          "scale-increment-val", 3600.0,
          "visible", FALSE,
          "tics-automatic", TRUE,
//          "mtics-automatic", FALSE,
          "scale-limits-automatic", FALSE,
          "scale-beg-val", 0.0,
          "scale-end-val", 86400.0,
//          "scale-auto-increment", FALSE,
*/
          NULL));
  gtk_widget_set_name (GTK_WIDGET (tmp_axis), "y1-axis");
  gtk_widget_show(GTK_WIDGET (tmp_axis));

  gdv_twod_layer_unset_axis (GDV_TWOD_LAYER (layer), GDV_Y1_AXIS);

  g_print ("ADDING\n");
  gdv_twod_layer_set_axis (GDV_TWOD_LAYER (layer),
    GDV_AXIS (tmp_axis),
    GDV_Y1_AXIS);




  tmp_axis =
//    GDV_AXIS (g_object_new (gdv_linear_axis_get_type (),
    GDV_AXIS (g_object_new (gdv_special_time_axis_get_type (),
          "valign", GTK_ALIGN_START,
          "halign", GTK_ALIGN_FILL,
          "axis-orientation", -0.5*M_PI,
          "axis-direction-outside", 0.0 * M_PI,
          "scale-limits-automatic", FALSE,
          "is-unix-time", TRUE,
/*          "scale-increment-val", 3600.0,
          "visible", FALSE,
          "tics-automatic", TRUE,
//          "mtics-automatic", FALSE,
          "scale-limits-automatic", FALSE,
          "scale-beg-val", 0.0,
          "scale-end-val", 86400.0,
//          "scale-auto-increment", FALSE,
*/
          NULL));
  gtk_widget_set_name (GTK_WIDGET (tmp_axis), "x2-axis");
  gtk_widget_show(GTK_WIDGET (tmp_axis));

  gdv_twod_layer_unset_axis (GDV_TWOD_LAYER (layer), GDV_X2_AXIS);

  g_print ("ADDING\n");
  gdv_twod_layer_set_axis (GDV_TWOD_LAYER (layer),
    GDV_AXIS (tmp_axis),
    GDV_X2_AXIS);


  tmp_axis =
//    GDV_AXIS (g_object_new (gdv_linear_axis_get_type (),
    GDV_AXIS (g_object_new (gdv_special_time_axis_get_type (),
          "valign", GTK_ALIGN_END,
          "halign", GTK_ALIGN_FILL,
          "axis-orientation", -0.5*M_PI,
          "axis-direction-outside", 1.0 * M_PI,
          "scale-limits-automatic", FALSE,
          "is-unix-time", TRUE,
/*          "scale-increment-val", 3600.0,
          "visible", FALSE,
          "tics-automatic", TRUE,
//          "mtics-automatic", FALSE,
          "scale-limits-automatic", FALSE,
          "scale-beg-val", 0.0,
          "scale-end-val", 86400.0,
//          "scale-auto-increment", FALSE,
*/
          NULL));
  gtk_widget_set_name (GTK_WIDGET (tmp_axis), "x1-axis");
  gtk_widget_show(GTK_WIDGET (tmp_axis));

  gdv_twod_layer_unset_axis (GDV_TWOD_LAYER (layer), GDV_X1_AXIS);

  g_print ("ADDING\n");
  gdv_twod_layer_set_axis (GDV_TWOD_LAYER (layer),
    GDV_AXIS (tmp_axis),
    GDV_X1_AXIS);




  added = FALSE;
//  g_timeout_add (1000, ((GSourceFunc) timeout_cb), layer);

  gtk_main ();

  return 0;
}

