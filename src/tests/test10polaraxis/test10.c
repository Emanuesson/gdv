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

//#include "tests/gemu_sub/gemu-glib-util.h"
//#include "tests/gemu_sub/gemu-gtk-util.h"

static void destroy( GtkWidget *widget,
                     gpointer   data )
{
  gtk_main_quit ();
}

int
main (int argc, char **argv)
{
  GtkWidget *main_window;
  GdvAxis *test_axis = NULL;
  GdvLayer *layer;

  gtk_init (&argc, &argv);

  main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (main_window), "gdv test-app 7");

  layer = g_object_new (gdv_oned_layer_get_type(), NULL);
  gtk_container_add (GTK_CONTAINER (main_window), GTK_WIDGET (layer));

  test_axis = g_object_new (GDV_SPECIAL_POLAR_AXIS_TYPE, NULL);
  gdv_oned_layer_replace_axis (GDV_ONED_LAYER (layer), test_axis);

  g_signal_connect (main_window, "destroy",
	      G_CALLBACK (destroy), NULL);

  gtk_widget_set_size_request (GTK_WIDGET (main_window),
                               200, 200);

  gtk_widget_show_all (GTK_WIDGET (main_window));

  gtk_main ();

  return 0;
}

