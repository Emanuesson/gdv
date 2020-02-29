/*
 * gdvinspector-test2.c
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

#include <gdv/gdv.h>
#include <gio/gio.h>
#include <gtk/gtk.h>

#include "gdvinspector-test2.h"
#include "math.h"

struct _GdvInspectorTest2Private
{
  gint placeholder;
};

G_DEFINE_TYPE_WITH_PRIVATE (
  GdvInspectorTest2,
  gdv_inspector_test2,
  GTK_TYPE_FRAME)

static void
gdv_inspector_test2_class_init (GdvInspectorTest2Class *klass)
{
}

void
gdv_inspector_test2_init (GdvInspectorTest2 *frame)
{
  GdvLayer *layer;
  GdvAxis *new_axis;

//  gtk_widget_set_size_request (main_window, 1800, 1800);

  layer = g_object_new (gdv_oned_layer_get_type (),
//                        "base-orientation", GTK_ORIENTATION_HORIZONTAL, //GTK_ORIENTATION_VERTICAL,
                        NULL);

  gtk_container_add (GTK_CONTAINER (frame), GTK_WIDGET (layer));

  new_axis = (GdvAxis *) gdv_special_polar_axis_new ();
  gdv_oned_layer_replace_axis(GDV_ONED_LAYER(layer), new_axis);
  gtk_widget_show_all (GTK_WIDGET (new_axis));

}

GdvInspectorTest2 *
gdv_inspector_test2_new (void)
{
  return g_object_new (GDV_INSPECTOR_TYPE_TEST2, NULL);
}


