/*
 * gdvspecialcontentaxis.c
 * This file is part of gdv
 *
 * Copyright (C) 2013 - Emanuel Schmidt
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <gtk/gtk.h>

#include "gdv/specialized_widgets/gdvspecialcontentaxis.h"

/* Define Properties */
/*enum
{
  PROP_0,

  PROP_GDV_DRUMD_CENTER_VAL,
  PROP_GDV_DRUMD_LR_RANGE,

  N_PROPERTIES
};
*/

struct _GdvSpecialContentAxisPrivate
{
  gdouble center_value;
  gdouble symm_lr_range;

  gulong notify_center_value_id;
  gulong notify_symm_lr_range_id;
};

G_DEFINE_TYPE_WITH_PRIVATE (
  GdvSpecialContentAxis,
  gdv_special_content_axis,
  GDV_TYPE_AXIS)

//static GParamSpec *content_axis_properties[N_PROPERTIES] = { NULL, };


/*static void
gdv_special_content_axis_set_property (
  GObject      *object,
  guint         property_id,
  const GValue *value,
  GParamSpec   *pspec)
{
  GdvSpecialContentAxis *self = GDV_SPECIAL_CONTENT_AXIS (object);

  switch (property_id)
  {
  case PROP_GDV_DRUMD_CENTER_VAL:
    self->priv->center_value = g_value_get_double (value);
    break;

  case PROP_GDV_DRUMD_LR_RANGE:
    self->priv->symm_lr_range = g_value_get_double (value);
    break;

  default:
*/    /* unknown property */
/*    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }
}

static void
gdv_special_content_axis_get_property (
  GObject    *object,
  guint       property_id,
  GValue     *value,
  GParamSpec *pspec)
{
  GdvSpecialContentAxis *self = GDV_SPECIAL_CONTENT_AXIS (object);

  switch (property_id)
  {
  case PROP_GDV_DRUMD_CENTER_VAL:
    g_value_set_double (value, self->priv->center_value);
    break;

  case PROP_GDV_DRUMD_LR_RANGE:
    g_value_set_double (value, self->priv->symm_lr_range);
    break;

  default:
*/    /* unknown property */
/*    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }
}
*/

static void
gdv_special_content_axis_class_init (GdvSpecialContentAxisClass *klass)
{
//  GObjectClass *object_class = G_OBJECT_CLASS (klass);

//  object_class->set_property = gdv_special_content_axis_set_property;
//  object_class->get_property = gdv_special_content_axis_get_property;

  /**
   * GdvSpecialContentAxis:center-value:
   *
   * Sets the center value that the axis will display.
   *
   * Since: 0.7.3
   */
/*  content_axis_properties[PROP_GDV_DRUMD_CENTER_VAL] =
    g_param_spec_double ("center-value",
                         "Sets the currect center value",
                         "Determines the value at the center of this axis",
                         -G_MAXDOUBLE,
                         G_MAXDOUBLE,
                         50.0,
                         G_PARAM_READWRITE);
*/

  /**
   * GdvSpecialContentAxis:left-right-range:
   *
   * Range of values to display on left and right of the center.
   *
   * Since: 0.7.3
   */
/*  content_axis_properties[PROP_GDV_DRUMD_LR_RANGE] =
    g_param_spec_double ("left-right-range",
                         "Range of values to display on left and right of the center",
                         "Determines the displayed range from the center of this axis",
                         -G_MAXDOUBLE,
                         G_MAXDOUBLE,
                         50.0,
                         G_PARAM_READWRITE);

  g_object_class_install_properties (object_class,
                                     N_PROPERTIES,
                                     content_axis_properties);
*/
}

static void
gdv_special_content_axis_init (GdvSpecialContentAxis *display)
{
  GdvAxis *axis;

  g_object_get (display, "axis", &axis, NULL);

  /* FIXME: The options should be implemented and working! They are essential
   *        for the look and functionality of this widget!
   */
/*  if (GDV_IS_AXIS (axis))
    g_object_set (axis,
      "scale-limits-automatic", FALSE,
//      "force-beg-end", TRUE,
//      "axis-beg-at-screen-x", 0.5,
//      "axis-end-at-screen-x", 0.5,
//      "axis-beg-at-screen-y", 0.0,
//      "axis-end-at-screen-y", 1.0,
      NULL);

  display->priv = gdv_special_content_axis_get_instance_private (display);

  display->priv->center_value = 50.0;
  display->priv->symm_lr_range = 50.0;


  display->priv->notify_center_value_id =
    g_signal_connect (display, "notify::center-value", (GCallback) refresh_values, NULL);
  display->priv->notify_symm_lr_range_id =
    g_signal_connect (display, "notify::left-right-range", (GCallback) refresh_values, NULL);
  display->priv->notify_center_value_id =
    g_signal_connect (display, "notify::axis", (GCallback) refresh_values, NULL);
*/
}

