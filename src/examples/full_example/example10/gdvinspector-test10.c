/*
 * gdvinspector-test10.c
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

#include "gdvinspector-test10.h"
#include "math.h"

struct _GdvInspectorTest10Private
{
  gint placeholder;
};

G_DEFINE_TYPE_WITH_PRIVATE (
  GdvInspectorTest10,
  gdv_inspector_test10,
  GTK_TYPE_FRAME)

static void
gdv_inspector_test10_class_init (GdvInspectorTest10Class *klass)
{
}

static gdouble current_orientation = 0;

static gboolean
timeout_cb_oned (GtkFrame *main_window)
{
  GdvLayer *layer;
  GList *child_list;

  if (!main_window || !GTK_IS_CONTAINER (main_window))
    return FALSE;

  child_list = gtk_container_get_children (GTK_CONTAINER (main_window));
  layer = child_list->data;

//  g_print ("\nTimeout Callback!\n");
  if (layer && GDV_IS_LAYER (layer))
    {
      GList *axis_list;

      axis_list = gdv_layer_get_axis_list (layer);

      if (axis_list)
        {
              g_object_set (axis_list->data,
                            "axis-orientation", current_orientation,
                            "axis-direction-outside", current_orientation + 0.5 * M_PI,
                            NULL);
          current_orientation += 0.02;


        }
    }

  return TRUE;
}


void
gdv_inspector_test10_init (GdvInspectorTest10 *frame)
{
  GdvAxis *test_axis = NULL;
  GdvLayer *layer;

  layer = g_object_new (gdv_oned_layer_get_type(), NULL);
  gtk_container_add (GTK_CONTAINER (frame), GTK_WIDGET (layer));

  test_axis = g_object_new (GDV_SPECIAL_TIME_AXIS_TYPE, NULL);
  gdv_oned_layer_replace_axis (GDV_ONED_LAYER (layer), test_axis);

  g_object_set (G_OBJECT(test_axis),
                "axis-beg-at-screen-y", 0.5,
                "axis-end-at-screen-y", 0.5,
                "axis-beg-at-screen-x", 0.0,
                "axis-end-at-screen-x", 1.0,
                "scale-increment-val", 20.0,
                "scale-auto-increment", FALSE,
                "scale-limits-automatic", FALSE,
//                  "scale-beg-val",
//                    (display->priv->center_value - display->priv->symm_lr_range),
//                  "scale-end-val",
//                    (display->priv->center_value + display->priv->symm_lr_range),
//                            "axis-orientation", 4.1,
//                            "axis-direction-outside", 4.1 + 0.5 * M_PI,
                NULL);

  /* Do it with 100 and everything seems to break */
//  callback_source_id = g_timeout_add (400, ((GSourceFunc)timeout_cb_oned),
//                                      frame);
  g_timeout_add (20, ((GSourceFunc)timeout_cb_oned), frame);

}

GdvInspectorTest10 *
gdv_inspector_test10_new (void)
{
  return g_object_new (GDV_INSPECTOR_TYPE_TEST10, NULL);
}

