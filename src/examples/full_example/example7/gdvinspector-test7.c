/*
 * gdvinspector-test7.c
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

#include "gdvinspector-test7.h"
#include "math.h"

#include "gemu_sub/gemu-glib-util.h"
#include "gemu_sub/gemu-gtk-util.h"

struct _GdvInspectorTest7Private
{
  gint placeholder;
};

G_DEFINE_TYPE_WITH_PRIVATE (
  GdvInspectorTest7,
  gdv_inspector_test7,
  GTK_TYPE_FRAME);

static void
gdv_inspector_test7_class_init (GdvInspectorTest7Class *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
}

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
          gdouble current_axis_end;

          g_object_get (axis_list->data,
                        "scale-end-val", &current_axis_end,
                        NULL);
          if (current_axis_end == 100.0)
            {
              g_object_set (axis_list->data,
                            "scale-end-val", 99.9,
                            NULL);
//              g_print ("End_value to %.1f\n", 99.9);
            }
          else
            {
              g_object_set (axis_list->data,
                            "scale-end-val", 100.0,
                            NULL);
//              g_print ("End_value to %.1f\n", 100.0);
            }
        }
    }

  return TRUE;
}


void
gdv_inspector_test7_init (GdvInspectorTest7 *frame)
{
  GdvInspectorTest7Private *priv;
  GdvLayer *layer;
  GList *axis_list;
  guint callback_source_id;

  priv = gdv_inspector_test7_get_instance_private (frame);

  layer = g_object_new (gdv_oned_layer_get_type (), NULL);
  gtk_container_add (GTK_CONTAINER (frame), GTK_WIDGET (layer));

  axis_list = gdv_layer_get_axis_list (layer);

//  gemu_glib_util_connect_to_all_signals (axis_list->data,
//                                         gemu_glib_util_show_details,
//                                         NULL,
//                                         NULL);

//  gemu_gtk_util_signal_connect_to_widget_children (
//    axis_list->data,
//    gemu_glib_util_show_details,
//    NULL, NULL);

  gemu_glib_util_connect_to_all_signals (
   G_OBJECT (axis_list->data),
   gemu_glib_util_show_details,
   NULL,
   NULL);


  if (axis_list)
    g_object_set (axis_list->data,
                  "scale-limits-automatic", FALSE,
                  NULL);

  g_list_free (axis_list);

  /* Do it with 100 and everything seems to break */
  callback_source_id = 0;
  callback_source_id = g_timeout_add (400, ((GSourceFunc)timeout_cb_oned),
                                      frame);

}

GdvInspectorTest7 *
gdv_inspector_test7_new (void)
{
  return g_object_new (GDV_INSPECTOR_TYPE_TEST7, NULL);
}

