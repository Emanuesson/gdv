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

  gtk_widget_set_size_request (main_window, 800, 800);
  g_signal_connect (main_window, "destroy",
	      G_CALLBACK (destroy), NULL);
  gtk_container_set_border_width (GTK_CONTAINER (main_window), 80);

//  layer = g_object_new (gdv_twod_layer_get_type (),
  layer = g_object_new (gdv_oned_layer_get_type (),
//                        "base-orientation", GTK_ORIENTATION_HORIZONTAL, //GTK_ORIENTATION_VERTICAL,
                        NULL);

  gtk_container_add (GTK_CONTAINER (main_window), GTK_WIDGET (layer));

  g_object_get (layer, "axis", &tmp, NULL);
  g_object_set (tmp,
//                "scale-end-val", 0.250,
                "scale-increment-base", 10.0,
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

  gtk_widget_show_all (GTK_WIDGET (main_window));

  gtk_main ();

  return 0;
}

