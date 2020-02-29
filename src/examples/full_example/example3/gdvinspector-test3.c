/*
 * gdvinspector-test3.c
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

#include "gdvinspector-test3.h"
#include "math.h"

struct _GdvInspectorTest3Private
{
  gint placeholder;
};

G_DEFINE_TYPE_WITH_PRIVATE (
  GdvInspectorTest3,
  gdv_inspector_test3,
  GTK_TYPE_FRAME)

static gboolean added;


static void
gdv_inspector_test3_class_init (GdvInspectorTest3Class *klass)
{
}

void
gdv_inspector_test3_init (GdvInspectorTest3 *frame)
{
  GdvTwodLayer *layer;
  GdvAxis *tmp_axis;

//  GdvAxis *test_axis = NULL;
  layer = g_object_new (gdv_twod_layer_get_type (), NULL);
//  g_object_get (layer, "axis", &test_axis, NULL);
//  g_object_set (test_axis, "scale-min-val", -100.0, NULL);

//  report_axis_props (test_axis);

  gtk_widget_set_name (GTK_WIDGET (layer), "test-layer");

  gtk_container_add (GTK_CONTAINER (frame), GTK_WIDGET (layer));

  gtk_widget_show_all (GTK_WIDGET (frame));

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

  g_debug ("ADDING\n");
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

  g_debug ("ADDING\n");
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

  g_debug ("ADDING\n");
  gdv_twod_layer_set_axis (GDV_TWOD_LAYER (layer),
    GDV_AXIS (tmp_axis),
    GDV_X1_AXIS);




  added = FALSE;
//  g_timeout_add (1000, ((GSourceFunc) timeout_cb), layer);

}

GdvInspectorTest3 *
gdv_inspector_test3_new (void)
{
  return g_object_new (GDV_INSPECTOR_TYPE_TEST3, NULL);
}

