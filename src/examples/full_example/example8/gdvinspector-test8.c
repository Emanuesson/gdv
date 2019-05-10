/*
 * gdvinspector-test8.c
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

#include <gio/gio.h>
#include <gtk/gtk.h>

#include <gdv/gdv.h>

#include "gdvinspector-test8.h"
#include "math.h"

struct _GdvInspectorTest8Private
{
  gint placeholder;
};

G_DEFINE_TYPE_WITH_PRIVATE (
  GdvInspectorTest8,
  gdv_inspector_test8,
  GTK_TYPE_FRAME);

static void
gdv_inspector_test8_class_init (GdvInspectorTest8Class *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
}

void
gdv_inspector_test8_init (GdvInspectorTest8 *frame)
{
  GdvInspectorTest8Private *priv;
  GdvAxis *test_axis = NULL;
  GdvLayer *layer;

  priv = gdv_inspector_test8_get_instance_private (frame);

  layer = g_object_new (gdv_oned_layer_get_type(), NULL);
  gtk_container_add (GTK_CONTAINER (frame), GTK_WIDGET (layer));

  test_axis = g_object_new (GDV_SPECIAL_TIME_AXIS_TYPE, NULL);
  gdv_oned_layer_replace_axis (GDV_ONED_LAYER (layer), test_axis);

}

GdvInspectorTest8 *
gdv_inspector_test8_new (void)
{
  return g_object_new (GDV_INSPECTOR_TYPE_TEST8, NULL);
}

