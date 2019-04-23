/*
 * gdvinspector-test9.c
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

#include "gdvinspector-test9.h"
#include "math.h"

struct _GdvInspectorTest9Private
{
  gint placeholder;
};

G_DEFINE_TYPE_WITH_PRIVATE (
  GdvInspectorTest9,
  gdv_inspector_test9,
  GTK_TYPE_FRAME);

static void
gdv_inspector_test9_class_init (GdvInspectorTest9Class *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
}

void
gdv_inspector_test9_init (GdvInspectorTest9 *frame)
{
  GdvInspectorTest9Private *priv;
  GdvAxis *test_axis = NULL;
  GdvLayer *layer;

  priv = gdv_inspector_test9_get_instance_private (frame);

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
                NULL);
}

GdvInspectorTest9 *
gdv_inspector_test9_new (void)
{
  return g_object_new (GDV_INSPECTOR_TYPE_TEST9, NULL);
}

