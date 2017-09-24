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

#include "tests/gemu_sub/gemu-glib-util.h"
#include "tests/gemu_sub/gemu-gtk-util.h"

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
    "scale-beg-val", &axis_min_val,
    "scale-end-val", &axis_max_val,
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
    "scale-auto-increment", &scale_auto_increment,
    "scale-limits-automatic", &scale_limits_auto,
    "tics-automatic", &tics_auto,
    "mtics-automatic", &mtics_auto,
    NULL);

  gtk_widget_style_get (GTK_WIDGET (axis),
    "scale-min-diff-pix", &scale_min_diff_pix,
    "scale-max-diff-pix", &scale_max_diff_pix,
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

static gboolean
timeout_cb_oned (GdvLayer *layer)
{
  if (layer && GDV_IS_LAYER (layer))
  {
    GList *axis_list;

    axis_list = gdv_layer_get_axis_list (layer);

    if (axis_list)
    {
      gdouble current_axis_end;

      g_object_get (axis_list->data,
        "scale-end-val", &current_axis_end,
        NULL);

      if (current_axis_end == 100.0)
        g_object_set (axis_list->data,
          "scale-end-val", 99.9,
          NULL);
      else
        g_object_set (axis_list->data,
          "scale-end-val", 100.0,
          NULL);
    }
  }

  return TRUE;
}

static void destroy( GtkWidget *widget,
                     gpointer   data )
{
  gtk_main_quit ();
}

static void activate_button (GtkSpinButton *spin_button,
                             gpointer data)
{
  GdvAxis *x_axis = GDV_AXIS (data);
  gdouble value;

  g_object_get (G_OBJECT (spin_button),
    "value", &value,
    NULL);

  g_object_set (G_OBJECT(x_axis),
    "axis-orientation", value * M_PI,
    "axis-direction-outside", value * M_PI + 0.5 * M_PI,
    NULL);

  gtk_widget_queue_resize (GTK_WIDGET (x_axis));
  gtk_widget_queue_allocate (GTK_WIDGET (x_axis));
  gtk_widget_queue_draw (GTK_WIDGET (x_axis));
}

int
main (int argc, char **argv)
{
  GdvLayerContent *layer_content;
  GtkWidget *left_box, *right_box, *main_box, *overlay;
  GList *tic_list;

  gint var_index;
  GtkWidget *main_window;
  GtkWidget *frame;
  GdvAxis *test_axis = NULL;
  GtkWidget *test_indicator_label;
  GdvLegend *test_legend;
  GdkScreen *screen_window;
  GtkWidget *spin_button;
  GdvLayer *layer;
  GdvLayerContent *content1, *content2;
  GList *axis_list;

  gtk_init (&argc, &argv);

  main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (main_window), "gdv test-app 7");

  layer = g_object_new (gdv_oned_layer_get_type(), NULL);
  gtk_container_add (GTK_CONTAINER (main_window), GTK_WIDGET (layer));

  g_signal_connect (main_window, "destroy",
	      G_CALLBACK (destroy), NULL);

  gtk_widget_set_size_request (GTK_WIDGET (main_window),
    200, 200);

  axis_list = gdv_layer_get_axis_list (layer);

  if (axis_list)
  {
    g_object_set (axis_list->data,
      "scale-limits-automatic", FALSE,
      NULL);
  }

  g_list_free (axis_list);

  g_timeout_add (100, ((GSourceFunc) timeout_cb_oned),
                 layer);

  gtk_widget_show_all (GTK_WIDGET (main_window));

  gtk_main ();

  return 0;
}

