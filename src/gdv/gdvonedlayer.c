/*
 * gdvonedlayer.c
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

#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include <cairo-gobject.h>

#include <math.h>
#include <float.h>

/**
 * SECTION:gdvonedlayer
 * @title: GdvOnedLayer
 *
 * #GdvOnedLayer defines a simple layer showing just a single axis. This can be
 *  combined with #GdvIndicator to make an simple scale-widget
 *
 * # CSS nodes
 *
 * GdvOnedLayer uses a single CSS node with name layeroned.
 *
 */

#include "gdvonedlayer.h"
#include "gdvindicator.h"
#include "gdv-data-boxed.h"
#include "gdv-enums.h"
#include "gdvlinearaxis.h"

/* Define Properties */
enum
{
  PROP_0,

  PROP_AXIS,
  PROP_BASE_ORIENTATION,
  PROP_ORIENTATION,

  N_PROPERTIES
};

struct _GdvOnedLayerPrivate
{
  /* display elements */
  GdvAxis *axis;
  GdvIndicator *indicator;

  /* natural border-space */
  guint top_space;
  guint bot_space;
  guint left_space;
  guint right_space;

  gint axis_using;
  gboolean stay_in_range;

  /* ankle-points */
  guint beg_x;
  guint beg_y;
  guint end_x;
  guint end_y;

  /* orientation */
  GtkOrientation base_orientation;
  gdouble  orientation;
  gboolean left_top_scale;
  gboolean right_bottom_scale;

  GtkPackType pack_type;
};

enum
{
  x = 0,
  y,
  z
};

static GParamSpec *layer_properties[N_PROPERTIES] = { NULL, };

/* Prototypes */
static gboolean
evaluate_data_point (GdvLayer *layer,
                     gdouble data_point_x_value,
                     gdouble data_point_y_value,
                     gdouble data_point_z_value,
                     gdouble *pixel_x_value,
                     gdouble *pixel_y_value);
static void _replace_axis (GdvOnedLayer *layer, GdvAxis *axis);

static void
gdv_oned_layer_add (GtkContainer *container, GtkWidget *child);
static void
gdv_oned_layer_remove (GtkContainer *container, GtkWidget *child);

G_DEFINE_TYPE_WITH_PRIVATE (GdvOnedLayer,
                            gdv_oned_layer,
                            GDV_TYPE_LAYER);

static void
gdv_oned_layer_set_property (GObject      *object,
                             guint         property_id,
                             const GValue *value,
                             GParamSpec   *pspec)
{
  GdvOnedLayer *self;

  self = GDV_ONED_LAYER (object);

  switch (property_id)
  {
  case PROP_AXIS:
    _replace_axis (self, g_value_get_object (value));
    break;

  case PROP_BASE_ORIENTATION:
    self->priv->base_orientation = g_value_get_enum (value);
    break;

  case PROP_ORIENTATION:
    self->priv->orientation = g_value_get_double (value);
    break;

  default:
    /* unknown property */
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }
}


static void
gdv_oned_layer_get_property (GObject    *object,
                             guint       property_id,
                             GValue     *value,
                             GParamSpec *pspec)
{
  GdvOnedLayer *self;

  self = GDV_ONED_LAYER (object);

  switch (property_id)
  {
  case PROP_AXIS:
    g_value_set_object (value, self->priv->axis);
    break;

  case PROP_BASE_ORIENTATION:
    g_value_set_enum (value, self->priv->base_orientation);
    break;

  case PROP_ORIENTATION:
    g_value_set_double (value, self->priv->orientation);
    break;

  default:
    /* unknown property */
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }
}

static void
gdv_oned_layer_class_init (GdvOnedLayerClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
  GdvLayerClass *layer_class = GDV_LAYER_CLASS (klass);
  GtkContainerClass *container_class = GTK_CONTAINER_CLASS (klass);

  object_class->set_property = gdv_oned_layer_set_property;
  object_class->get_property = gdv_oned_layer_get_property;

  container_class->add = gdv_oned_layer_add;
  container_class->remove = gdv_oned_layer_remove;

  layer_class->evaluate_point = evaluate_data_point;

  gtk_widget_class_set_css_name (widget_class, "layeroned");

  /* FIXME: Implement this property correctly! */
  /**
   * GdvOnedLayer:axis:
   *
   * Gets the assigned axis.
   *
   * Since: 0.1
   */
  layer_properties[PROP_AXIS] =
    g_param_spec_object ("axis",
                         "axis of the graph",
                         "Get or assign a axis to the layer",
                         GDV_TYPE_AXIS,
                         G_PARAM_READWRITE);

  /**
   * GdvOnedLayer:base-orientation:
   *
   * Sets the basic orientation of the axis.
   *
   * Since: 0.1
   */
  layer_properties[PROP_BASE_ORIENTATION] =
    g_param_spec_enum ("base-orientation",
                       "sets the basic orientation of the main-axis",
                       "Displays the basic orientation of the main-axis",
                       GTK_TYPE_ORIENTATION,
                       GTK_ORIENTATION_VERTICAL,
                       G_PARAM_READWRITE);

  /**
   * GdvOnedLayer:orientation:
   *
   * Sets the orientation as an angular value relative to the base-orientation
   * of the axis.
   *
   * Since: 0.1
   */
  layer_properties[PROP_ORIENTATION] =
    g_param_spec_double ("orientation",
                         "sets the orientation of the main-axis",
                         "Displays the orientation of the main-axis",
                         -G_MAXDOUBLE,
                         G_MAXDOUBLE,
                         0.0,
                         G_PARAM_READWRITE);

  g_object_class_install_properties (object_class,
                                     N_PROPERTIES,
                                     layer_properties);
}

static void
gdv_oned_layer_init (GdvOnedLayer *layer)
{
  layer->priv = gdv_oned_layer_get_instance_private (layer);

  layer->priv->base_orientation = GTK_ORIENTATION_VERTICAL;

  layer->priv->axis_using = 0;
  layer->priv->stay_in_range = TRUE;
  layer->priv->axis = NULL;

  layer->priv->axis = g_object_new (gdv_linear_axis_get_type (),
                                    "halign", GTK_ALIGN_FILL,
                                    "valign", GTK_ALIGN_FILL,
                                    NULL);
  gtk_overlay_add_overlay (GTK_OVERLAY (layer), GTK_WIDGET (layer->priv->axis));

  layer->priv->base_orientation = GTK_ORIENTATION_VERTICAL;
  layer->priv->orientation = 0.0;
  layer->priv->left_top_scale = TRUE;
}

static void
gdv_oned_layer_add (GtkContainer *container, GtkWidget *child)
{
  GdvOnedLayer *layer = GDV_ONED_LAYER (container);
  GdvOnedLayerPrivate *priv = layer->priv;

  GTK_CONTAINER_CLASS (gdv_oned_layer_parent_class)->add (container, child);

  if (GDV_IS_AXIS (child))
  {
    priv->axis = GDV_AXIS (child);
    g_object_notify_by_pspec (G_OBJECT (layer), layer_properties[PROP_AXIS]);
  }
}

static void
gdv_oned_layer_remove (GtkContainer *container, GtkWidget *child)
{
  GdvOnedLayer *layer = GDV_ONED_LAYER (container);
  GdvOnedLayerPrivate *priv = layer->priv;

  GTK_CONTAINER_CLASS (gdv_oned_layer_parent_class)->remove (container, child);

  if (GTK_WIDGET (priv->axis) == child)
  {
    priv->axis = NULL;
    g_object_notify_by_pspec (G_OBJECT (layer), layer_properties[PROP_AXIS]);
  }
}

static gboolean
evaluate_data_point (GdvLayer *layer,
                     gdouble data_point_x_value,
                     gdouble data_point_y_value,
                     gdouble data_point_z_value,
                     gdouble *pixel_x_value,
                     gdouble *pixel_y_value)
{
  GdvOnedLayer *layeroned;
  gboolean return_value;

  g_return_val_if_fail (GDV_ONED_IS_LAYER (layer), FALSE);

  layeroned = GDV_ONED_LAYER (layer);

  return_value = FALSE;

  layeroned = GDV_ONED_LAYER (layer);

  if (layeroned->priv->axis_using == 0)
  {
    GdvAxis *local_axis = layeroned->priv->axis;
    gboolean on_axis = FALSE;

    if (local_axis)
      g_signal_emit_by_name (
        local_axis,
        "get-point",
        data_point_x_value,
        pixel_x_value,
        pixel_y_value,
        &on_axis);

    return_value = layeroned->priv->stay_in_range || on_axis;
  }

  return return_value;
}

GdvOnedLayer *gdv_oned_layer_new (void)
{
  return g_object_new (gdv_oned_layer_get_type (),
                       NULL);
}

static void _replace_axis (GdvOnedLayer *layer, GdvAxis *axis)
{
  if (layer->priv->axis)
    gtk_container_remove (GTK_CONTAINER (layer), GTK_WIDGET (layer->priv->axis));

  layer->priv->axis = axis;
  gtk_overlay_add_overlay (GTK_OVERLAY (layer), GTK_WIDGET (layer->priv->axis));
}

/**
 * gdv_oned_layer_replace_axis:
 * @layer: a #GdvOnedLayer
 * @axis: a #GdvAxis
 *
 * Replaces the main axis attached to the layer.
 *
 **/
void gdv_oned_layer_replace_axis (GdvOnedLayer *layer, GdvAxis *axis)
{
  g_return_if_fail (GDV_ONED_IS_LAYER (layer));
  g_return_if_fail (GDV_IS_AXIS (axis));

  _replace_axis (layer, axis);
}

