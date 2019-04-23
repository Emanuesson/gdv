/*
 * gdvinspector-test6.c
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

#include "gdvinspector-test6.h"
#include "math.h"

struct _GdvInspectorTest6Private
{
  gint placeholder;
};

G_DEFINE_TYPE_WITH_PRIVATE (
  GdvInspectorTest6,
  gdv_inspector_test6,
  GTK_TYPE_FRAME);

static void
gdv_inspector_test6_class_init (GdvInspectorTest6Class *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
}

void
gdv_inspector_test6_init (GdvInspectorTest6 *frame)
{
  GdvInspectorTest6Private *priv;

  priv = gdv_inspector_test6_get_instance_private (frame);

}

GdvInspectorTest6 *
gdv_inspector_test6_new (void)
{
  return g_object_new (GDV_INSPECTOR_TYPE_TEST6, NULL);
}

