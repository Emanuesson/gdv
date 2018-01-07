/*
 * gdvtwodlayer.c
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

/*
 * TODO:
 *  - overwrite widget-draw function
 *  - set halign and valign automatically, as well as the widget-name, when replacing an axis
 *  - or at least detect anything, that is set incorrectly
 *  - same problem with the orientations
 *  - write a replacing-function to unref the existingn axis
 *
 */

#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include <math.h>
#include <stdio.h>
#include <gsl/gsl_math.h>

#include "gdvtwodlayer.h"
#include "gdvlinearaxis.h"
#include "gdvaxis.h"
#include "gdvlayercontent.h"
#include "gdv-data-boxed.h"

/* local minimum-function */
static gint min (gint a,
                  gint b)
{
  return a < b ? a : b;
}

/* local maximum-function */
static gint max (gint a,
                  gint b)
{
  return a > b ? a : b;
}

/**
 * SECTION:gdvtwodlayer
 * @Short_description: A plain two-dimensional layer
 * @Title: GdvTowdLayer
 *
 * GdvTowdLayer plots an usual cartesian XY-style graph. It handles GdvAxis- and
 * GdvLayerContent-Objects.
 *
 */

/* Define Properties */
enum
{
  PROP_0,

  PROP_GDV_X1_AXIS,
  PROP_GDV_X2_AXIS,
  PROP_GDV_Y1_AXIS,
  PROP_GDV_Y2_AXIS,

  N_PROPERTIES
};

struct _GdvTwodLayerPrivate
{
  /* axes */
  GdvAxis *x1_axis;
  GdvAxis *x2_axis;
  GdvAxis *y1_axis;
  GdvAxis *y2_axis;

  gboolean min_max_refresh_x;
  gboolean min_max_refresh_y;

  /* natural border-space */
  guint top_space;
  guint bot_space;
  guint left_space;
  guint right_space;

  /* ankle-points */
  guint bot_left_x;
  guint bot_left_y;
  guint bot_right_x;
  guint bot_right_y;
  guint upp_left_x;
  guint upp_left_y;
  guint upp_right_x;
  guint upp_right_y;
};

enum
{
  x = 0,
  y,
  z
};

/* Prototypes */
static gboolean
evaluate_data_point (GdvLayer *layer,
                     gdouble data_point_x_value,
                     gdouble data_point_y_value,
                     gdouble data_point_z_value,
                     gdouble *pixel_x_value,
                     gdouble *pixel_y_value);
static void
gdv_twod_layer_size_allocate (GtkWidget           *widget,
                              GtkAllocation       *allocation);
static void
gdv_twod_layer_get_preferred_width (GtkWidget           *widget,
                                    gint                *minimum_size,
                                    gint                *natural_size);
static void
gdv_twod_layer_get_preferred_height (GtkWidget           *widget,
                                     gint                *minimum_size,
                                     gint                *natural_size);
static void
gdv_twod_layer_get_preferred_height_for_width (GtkWidget           *widget,
    gint                 width,
    gint                *minimum_height,
    gint                *natural_height);
static void
gdv_twod_layer_get_preferred_width_for_height (GtkWidget           *widget,
    gint                 height,
    gint                *minimum_width,
    gint                *natural_width);
static void
gdv_twod_layer_measure (
  GdvTwodLayer             *twod_layer,
  GtkOrientation       orientation,
  int                  for_size,
  int                 *minimum,
  int                 *natural,
  int                 *minimum_baseline,
  int                 *natural_baseline,
  gpointer             data);
static void gdv_twod_layer_remove (
  GtkContainer   *container,
  GtkWidget      *child);

G_DEFINE_TYPE_WITH_PRIVATE (GdvTwodLayer,
                            gdv_twod_layer,
                            GDV_TYPE_LAYER);

static void
gdv_twod_layer_class_init (GdvTwodLayerClass *klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
  GdvLayerClass *layer_class = GDV_LAYER_CLASS (klass);
  GtkContainerClass *container_class = GTK_CONTAINER_CLASS (klass);

  container_class->remove = gdv_twod_layer_remove;

  widget_class->size_allocate = gdv_twod_layer_size_allocate;

  widget_class->get_preferred_width =
    gdv_twod_layer_get_preferred_width;
  widget_class->get_preferred_height =
    gdv_twod_layer_get_preferred_height;
  widget_class->get_preferred_height_for_width =
    gdv_twod_layer_get_preferred_height_for_width;
  widget_class->get_preferred_width_for_height =
    gdv_twod_layer_get_preferred_width_for_height;

  layer_class->evaluate_point = evaluate_data_point;

  gtk_widget_class_set_css_name (widget_class, "layertwod");

}

static void
gdv_twod_layer_init (GdvTwodLayer *layer)
{
  layer->priv = gdv_twod_layer_get_instance_private (layer);

  layer->priv->x1_axis =
    GDV_AXIS (g_object_new (gdv_linear_axis_get_type (),
                            "halign", GTK_ALIGN_FILL,
                            "valign", GTK_ALIGN_END,
                            "axis-orientation", -0.5 * M_PI,
                            "axis-direction-outside", M_PI,
                            "visible", TRUE,
                            NULL));
  gtk_widget_set_name (GTK_WIDGET (layer->priv->x1_axis), "x1-axis");
  gtk_overlay_add_overlay (GTK_OVERLAY (layer),
                           GTK_WIDGET (layer->priv->x1_axis));

  layer->priv->y1_axis =
    GDV_AXIS (g_object_new (gdv_linear_axis_get_type (),
                            "halign", GTK_ALIGN_START,
                            "valign", GTK_ALIGN_FILL,
                            "axis-orientation", M_PI,
                            "axis-direction-outside", -0.5 * M_PI,
                            "visible", TRUE,
                            NULL));
  gtk_widget_set_name (GTK_WIDGET (layer->priv->y1_axis), "y1-axis");
  gtk_overlay_add_overlay (GTK_OVERLAY (layer),
                           GTK_WIDGET (layer->priv->y1_axis));

  layer->priv->x2_axis =
    GDV_AXIS (g_object_new (gdv_linear_axis_get_type (),
                            "halign", GTK_ALIGN_FILL,
                            "valign", GTK_ALIGN_START,
                            "axis-orientation", -0.5 * M_PI,
                            "axis-direction-outside", 0.0,
                            "visible", TRUE,
                            NULL));
  gtk_widget_set_name (GTK_WIDGET (layer->priv->x2_axis), "x2-axis");
  gtk_overlay_add_overlay (GTK_OVERLAY (layer),
                           GTK_WIDGET (layer->priv->x2_axis));

  layer->priv->y2_axis =
    GDV_AXIS (g_object_new (gdv_linear_axis_get_type (),
                            "halign", GTK_ALIGN_END,
                            "valign", GTK_ALIGN_FILL,
                            "axis-orientation", M_PI,
                            "axis-direction-outside", 0.5 * M_PI,
                            "visible", TRUE,
                            NULL));
  gtk_widget_set_name (GTK_WIDGET (layer->priv->y2_axis), "y2-axis");
  gtk_overlay_add_overlay (GTK_OVERLAY (layer),
                           GTK_WIDGET (layer->priv->y2_axis));

  layer->priv->min_max_refresh_x = TRUE;
  layer->priv->min_max_refresh_y = TRUE;
}

static void gdv_twod_layer_remove (
  GtkContainer   *container,
  GtkWidget      *child)
{
  GdvTwodLayer *layer = GDV_TWOD_LAYER (container);

  if (!GDV_IS_AXIS (child))
    GTK_CONTAINER_CLASS (gdv_twod_layer_parent_class)->remove (
      container,
      child);
  else
  {
    if (layer->priv->x1_axis && layer->priv->x1_axis == GDV_AXIS (child))
      layer->priv->x1_axis = NULL;
    else if (layer->priv->x2_axis && layer->priv->x2_axis == GDV_AXIS (child))
      layer->priv->x2_axis = NULL;
    else if (layer->priv->y1_axis && layer->priv->y1_axis == GDV_AXIS (child))
      layer->priv->y1_axis = NULL;
    else if (layer->priv->y2_axis && layer->priv->y2_axis == GDV_AXIS (child))
      layer->priv->y2_axis = NULL;

    GTK_CONTAINER_CLASS (gdv_twod_layer_parent_class)->remove (
      container,
      child);
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
  GdvTwodLayer *layer_2d;
  gdouble x_pos_x1 = 0.0,
          y_pos_x1 = 0.0,
          x_pos_y1 = 0.0,
          y_pos_y1 = 0.0;
  gdouble x_pos_x2 = 0.0,
          y_pos_x2 = 0.0,
          x_pos_y2 = 0.0,
          y_pos_y2 = 0.0;
  gboolean on_x1_axis, on_x2_axis, on_y1_axis, on_y2_axis;
  GtkAllocation local_allocation;

  g_return_val_if_fail (GDV_IS_LAYER (layer), FALSE);

  layer_2d = GDV_TWOD_LAYER (layer);

  /* FIXME: make this a better interpolation of all axes */

  if (layer_2d->priv->x1_axis &&
      gtk_widget_get_visible (GTK_WIDGET (layer_2d->priv->x1_axis)))
  {
    g_signal_emit_by_name (layer_2d->priv->x1_axis,
                           "get-point", data_point_x_value,
                           &x_pos_x1, &y_pos_x1, &on_x1_axis);
    gtk_widget_get_allocation (
      GTK_WIDGET (layer_2d->priv->x1_axis),
      &local_allocation);
    x_pos_x1 += (gdouble) local_allocation.x;
    y_pos_x1 += (gdouble) local_allocation.y;
  }
  else on_x1_axis = FALSE;

  if (layer_2d->priv->x2_axis &&
      gtk_widget_get_visible (GTK_WIDGET (layer_2d->priv->x2_axis)))
  {
    g_signal_emit_by_name (layer_2d->priv->x2_axis,
                           "get-point", data_point_x_value,
                           &x_pos_x2, &y_pos_x2, &on_x2_axis);
    gtk_widget_get_allocation (
      GTK_WIDGET (layer_2d->priv->x2_axis),
      &local_allocation);
    x_pos_x2 += (gdouble) local_allocation.x;
    y_pos_x2 += (gdouble) local_allocation.y;
  }
  else on_x2_axis = FALSE;

  if (layer_2d->priv->y1_axis &&
      gtk_widget_get_visible (GTK_WIDGET (layer_2d->priv->y1_axis)))
  {
    g_signal_emit_by_name (layer_2d->priv->y1_axis,
                           "get-point", data_point_y_value,
                           &x_pos_y1, &y_pos_y1, &on_y1_axis);
    gtk_widget_get_allocation (
      GTK_WIDGET (layer_2d->priv->y1_axis),
      &local_allocation);
    x_pos_y1 += (gdouble) local_allocation.x;
    y_pos_y1 += (gdouble) local_allocation.y;
  }
  else on_y1_axis = FALSE;

  if (layer_2d->priv->y2_axis &&
      gtk_widget_get_visible (GTK_WIDGET (layer_2d->priv->y2_axis)))
  {
    g_signal_emit_by_name (layer_2d->priv->y2_axis,
                           "get-point", data_point_y_value,
                           &x_pos_y2, &y_pos_y2, &on_y2_axis);
    gtk_widget_get_allocation (
      GTK_WIDGET (layer_2d->priv->y2_axis),
      &local_allocation);
    x_pos_y2 += (gdouble) local_allocation.x;
    y_pos_y2 += (gdouble) local_allocation.y;
  }
  else on_y2_axis = FALSE;

  if (!layer_2d->priv->y1_axis && layer_2d->priv->y2_axis)
    on_y1_axis = FALSE;

  *pixel_x_value = x_pos_x1;
  *pixel_y_value = y_pos_y1;

  return on_x1_axis && on_y1_axis && on_x2_axis && on_y2_axis;
}

static void _hide_show_all_contents (GList *child_list, gboolean show)
{
  GList *child_list_cpy;

  for (child_list_cpy = child_list;
       child_list_cpy;
       child_list_cpy = child_list_cpy->next)
  {
    if (GDV_LAYER_IS_CONTENT (child_list_cpy->data))
      gtk_widget_set_visible (child_list_cpy->data, show);
  }
}

/**
 * gdv_twod_layer_new:
 *
 * Just a common constructor.
 *
 * Returns: a new #GdvTwodLayer
 **/
GdvTwodLayer *gdv_twod_layer_new (void)
{
  return g_object_new (gdv_twod_layer_get_type (),
                       NULL);
}

static void
gdv_twod_layer_size_allocate (
  GtkWidget           *widget,
  GtkAllocation       *allocation)
{
  /* TODO: Fix this by memcpy zero to all structs */
  gint x1_beg_space[4] = {0};
  gint x1_beg_space_nat[4] = {0};
  gint x1_end_space[4] = {0};
  gint x1_end_space_nat[4] = {0};

  gint x2_beg_space[4] = {0};
  gint x2_beg_space_nat[4] = {0};
  gint x2_end_space[4] = {0};
  gint x2_end_space_nat[4] = {0};

  gint y1_beg_space[4] = {0};
  gint y1_beg_space_nat[4] = {0};
  gint y1_end_space[4] = {0};
  gint y1_end_space_nat[4] = {0};

  gint y2_beg_space[4] = {0};
  gint y2_beg_space_nat[4] = {0};
  gint y2_end_space[4] = {0};
  gint y2_end_space_nat[4] = {0};

  GList *list, *children, *children_copy;
  GtkAllocation content_allocation = {0};

  GtkAllocation x1_allocation = {0},
                x2_allocation = {0},
                y1_allocation = {0},
                y2_allocation = {0};
  GdvTwodLayer *layer = GDV_TWOD_LAYER (widget);
  GdvTwodLayerPrivate *priv = layer->priv;

  g_return_if_fail (GDV_TWOD_IS_LAYER (widget));
  g_return_if_fail (allocation != NULL);

  gtk_widget_set_allocation (GTK_WIDGET (layer), allocation);

  /*
  g_print ("RUN ALLOC FROM START: X(%d, %d) - Y(%d, %d)\n",
           allocation->x, allocation->width,
           allocation->y, allocation->height);
   */

  /* Measuring the x1-axis */
  if (priv->x1_axis &&
      gtk_widget_get_visible (GTK_WIDGET(priv->x1_axis)))
  {
    GTK_OVERLAY_CLASS (gdv_twod_layer_parent_class)->get_child_position (
      GTK_OVERLAY (layer), GTK_WIDGET (layer->priv->x1_axis),
      &x1_allocation);

    GDV_AXIS_CLASS (GDV_AXIS_GET_CLASS (
                      layer->priv->x1_axis))->get_space_to_beg_position (
                        GDV_AXIS (layer->priv->x1_axis), GTK_POS_TOP, -1,
                        &x1_beg_space[0], &x1_beg_space_nat[0], NULL);
    GDV_AXIS_CLASS (GDV_AXIS_GET_CLASS (
                      layer->priv->x1_axis))->get_space_to_beg_position (
                        GDV_AXIS (layer->priv->x1_axis), GTK_POS_BOTTOM, -1,
                        &x1_beg_space[1], &x1_beg_space_nat[1], NULL);
    GDV_AXIS_CLASS (GDV_AXIS_GET_CLASS (
                      layer->priv->x1_axis))->get_space_to_beg_position (
                        GDV_AXIS (layer->priv->x1_axis), GTK_POS_LEFT, -1,
                        &x1_beg_space[2], &x1_beg_space_nat[2], NULL);
    GDV_AXIS_CLASS (GDV_AXIS_GET_CLASS (
                      layer->priv->x1_axis))->get_space_to_beg_position (
                        GDV_AXIS (layer->priv->x1_axis), GTK_POS_RIGHT, -1,
                        &x1_beg_space[3], &x1_beg_space_nat[3], NULL);

    GDV_AXIS_CLASS (GDV_AXIS_GET_CLASS (
                      layer->priv->x1_axis))->get_space_to_end_position (
                        GDV_AXIS (layer->priv->x1_axis), GTK_POS_TOP, -1,
                        &x1_end_space[0], &x1_end_space_nat[0], NULL);
    GDV_AXIS_CLASS (GDV_AXIS_GET_CLASS (
                      layer->priv->x1_axis))->get_space_to_end_position (
                        GDV_AXIS (layer->priv->x1_axis), GTK_POS_BOTTOM, -1,
                        &x1_end_space[1], &x1_end_space_nat[1], NULL);
    GDV_AXIS_CLASS (GDV_AXIS_GET_CLASS (
                      layer->priv->x1_axis))->get_space_to_end_position (
                        GDV_AXIS (layer->priv->x1_axis), GTK_POS_LEFT, -1,
                        &x1_end_space[2], &x1_end_space_nat[2], NULL);
    GDV_AXIS_CLASS (GDV_AXIS_GET_CLASS (
                      layer->priv->x1_axis))->get_space_to_end_position (
                        GDV_AXIS (layer->priv->x1_axis), GTK_POS_RIGHT, -1,
                        &x1_end_space[3], &x1_end_space_nat[3], NULL);
  }

  /* Measuring the x2-axis */
  if (priv->x2_axis &&
      gtk_widget_get_visible (GTK_WIDGET(priv->x2_axis)))
  {
    GTK_OVERLAY_CLASS (gdv_twod_layer_parent_class)->get_child_position (
      GTK_OVERLAY (layer), GTK_WIDGET (layer->priv->x2_axis),
      &x2_allocation);

    GDV_AXIS_CLASS (GDV_AXIS_GET_CLASS (
                      layer->priv->x2_axis))->get_space_to_beg_position (
                        GDV_AXIS (layer->priv->x2_axis), GTK_POS_TOP, -1,
                        &x2_beg_space[0], &x2_beg_space_nat[0], NULL);
    GDV_AXIS_CLASS (GDV_AXIS_GET_CLASS (
                      layer->priv->x2_axis))->get_space_to_beg_position (
                        GDV_AXIS (layer->priv->x2_axis), GTK_POS_BOTTOM, -1,
                        &x2_beg_space[1], &x2_beg_space_nat[1], NULL);
    GDV_AXIS_CLASS (GDV_AXIS_GET_CLASS (
                      layer->priv->x2_axis))->get_space_to_beg_position (
                        GDV_AXIS (layer->priv->x2_axis), GTK_POS_LEFT, -1,
                        &x2_beg_space[2], &x2_beg_space_nat[2], NULL);
    GDV_AXIS_CLASS (GDV_AXIS_GET_CLASS (
                      layer->priv->x2_axis))->get_space_to_beg_position (
                        GDV_AXIS (layer->priv->x2_axis), GTK_POS_RIGHT, -1,
                        &x2_beg_space[3], &x2_beg_space_nat[3], NULL);

    GDV_AXIS_CLASS (GDV_AXIS_GET_CLASS (
                      layer->priv->x2_axis))->get_space_to_end_position (
                        GDV_AXIS (layer->priv->x2_axis), GTK_POS_TOP, -1,
                        &x2_end_space[0], &x2_end_space_nat[0], NULL);
    GDV_AXIS_CLASS (GDV_AXIS_GET_CLASS (
                      layer->priv->x2_axis))->get_space_to_end_position (
                        GDV_AXIS (layer->priv->x2_axis), GTK_POS_BOTTOM, -1,
                        &x2_end_space[1], &x2_end_space_nat[1], NULL);
    GDV_AXIS_CLASS (GDV_AXIS_GET_CLASS (
                      layer->priv->x2_axis))->get_space_to_end_position (
                        GDV_AXIS (layer->priv->x2_axis), GTK_POS_LEFT, -1,
                        &x2_end_space[2], &x2_end_space_nat[2], NULL);
    GDV_AXIS_CLASS (GDV_AXIS_GET_CLASS (
                      layer->priv->x2_axis))->get_space_to_end_position (
                        GDV_AXIS (layer->priv->x2_axis), GTK_POS_RIGHT, -1,
                        &x2_end_space[3], &x2_end_space_nat[3], NULL);
  }

  /* Measuring the y1-axis */
  if (priv->y1_axis &&
      gtk_widget_get_visible (GTK_WIDGET(priv->y1_axis)))
  {
    GTK_OVERLAY_CLASS (gdv_twod_layer_parent_class)->get_child_position (
      GTK_OVERLAY (layer), GTK_WIDGET (layer->priv->y1_axis),
      &y1_allocation);

    GDV_AXIS_CLASS (GDV_AXIS_GET_CLASS (
                      layer->priv->y1_axis))->get_space_to_beg_position (
                        GDV_AXIS (layer->priv->y1_axis), GTK_POS_TOP, -1,
                        &y1_beg_space[0], &y1_beg_space_nat[0], NULL);
    GDV_AXIS_CLASS (GDV_AXIS_GET_CLASS (
                      layer->priv->y1_axis))->get_space_to_beg_position (
                        GDV_AXIS (layer->priv->y1_axis), GTK_POS_BOTTOM, -1,
                        &y1_beg_space[1], &y1_beg_space_nat[1], NULL);
    GDV_AXIS_CLASS (GDV_AXIS_GET_CLASS (
                      layer->priv->y1_axis))->get_space_to_beg_position (
                        GDV_AXIS (layer->priv->y1_axis), GTK_POS_LEFT, -1,
                        &y1_beg_space[2], &y1_beg_space_nat[2], NULL);
    GDV_AXIS_CLASS (GDV_AXIS_GET_CLASS (
                      layer->priv->y1_axis))->get_space_to_beg_position (
                        GDV_AXIS (layer->priv->y1_axis), GTK_POS_RIGHT, -1,
                        &y1_beg_space[3], &y1_beg_space_nat[3], NULL);

    GDV_AXIS_CLASS (GDV_AXIS_GET_CLASS (
                      layer->priv->y1_axis))->get_space_to_end_position (
                        GDV_AXIS (layer->priv->y1_axis), GTK_POS_TOP, -1,
                        &y1_end_space[0], &y1_end_space_nat[0], NULL);
    GDV_AXIS_CLASS (GDV_AXIS_GET_CLASS (
                      layer->priv->y1_axis))->get_space_to_end_position (
                        GDV_AXIS (layer->priv->y1_axis), GTK_POS_BOTTOM, -1,
                        &y1_end_space[1], &y1_end_space_nat[1], NULL);
    GDV_AXIS_CLASS (GDV_AXIS_GET_CLASS (
                      layer->priv->y1_axis))->get_space_to_end_position (
                        GDV_AXIS (layer->priv->y1_axis), GTK_POS_LEFT, -1,
                        &y1_end_space[2], &y1_end_space_nat[2], NULL);
    GDV_AXIS_CLASS (GDV_AXIS_GET_CLASS (
                      layer->priv->y1_axis))->get_space_to_end_position (
                        GDV_AXIS (layer->priv->y1_axis), GTK_POS_RIGHT, -1,
                        &y1_end_space[3], &y1_end_space_nat[3], NULL);
  }

  /* Measuring the y2-axis */
  if (priv->y2_axis &&
      gtk_widget_get_visible (GTK_WIDGET(priv->y2_axis)))
  {
    GTK_OVERLAY_CLASS (gdv_twod_layer_parent_class)->get_child_position (
      GTK_OVERLAY (layer), GTK_WIDGET (layer->priv->y2_axis),
      &y2_allocation);

    GDV_AXIS_CLASS (GDV_AXIS_GET_CLASS (
                      layer->priv->y2_axis))->get_space_to_beg_position (
                        GDV_AXIS (layer->priv->y2_axis), GTK_POS_TOP, -1,
                        &y2_beg_space[0], &y2_beg_space_nat[0], NULL);
    GDV_AXIS_CLASS (GDV_AXIS_GET_CLASS (
                      layer->priv->y2_axis))->get_space_to_beg_position (
                        GDV_AXIS (layer->priv->y2_axis), GTK_POS_BOTTOM, -1,
                        &y2_beg_space[1], &y2_beg_space_nat[1], NULL);
    GDV_AXIS_CLASS (GDV_AXIS_GET_CLASS (
                      layer->priv->y2_axis))->get_space_to_beg_position (
                        GDV_AXIS (layer->priv->y2_axis), GTK_POS_LEFT, -1,
                        &y2_beg_space[2], &y2_beg_space_nat[2], NULL);
    GDV_AXIS_CLASS (GDV_AXIS_GET_CLASS (
                      layer->priv->y2_axis))->get_space_to_beg_position (
                        GDV_AXIS (layer->priv->y2_axis), GTK_POS_RIGHT, -1,
                        &y2_beg_space[3], &y2_beg_space_nat[3], NULL);

    GDV_AXIS_CLASS (GDV_AXIS_GET_CLASS (
                      layer->priv->y2_axis))->get_space_to_end_position (
                        GDV_AXIS (layer->priv->y2_axis), GTK_POS_TOP, -1,
                        &y2_end_space[0], &y2_end_space_nat[0], NULL);
    GDV_AXIS_CLASS (GDV_AXIS_GET_CLASS (
                      layer->priv->y2_axis))->get_space_to_end_position (
                        GDV_AXIS (layer->priv->y2_axis), GTK_POS_BOTTOM, -1,
                        &y2_end_space[1], &y2_end_space_nat[1], NULL);
    GDV_AXIS_CLASS (GDV_AXIS_GET_CLASS (
                      layer->priv->y2_axis))->get_space_to_end_position (
                        GDV_AXIS (layer->priv->y2_axis), GTK_POS_LEFT, -1,
                        &y2_end_space[2], &y2_end_space_nat[2], NULL);
    GDV_AXIS_CLASS (GDV_AXIS_GET_CLASS (
                      layer->priv->y2_axis))->get_space_to_end_position (
                        GDV_AXIS (layer->priv->y2_axis), GTK_POS_RIGHT, -1,
                        &y2_end_space[3], &y2_end_space_nat[3], NULL);
  }

  /*
  g_print ("\tX1 SPACE: (%d, %d, %d, %d) - (%d, %d, %d, %d)\n",
           x1_beg_space[0], x1_beg_space[1], x1_beg_space[2], x1_beg_space[3],
           x1_end_space[0], x1_end_space[1], x1_end_space[2], x1_end_space[3]);
  g_print ("\tX2 SPACE: (%d, %d, %d, %d) - (%d, %d, %d, %d)\n",
           x2_beg_space[0], x2_beg_space[1], x2_beg_space[2], x2_beg_space[3],
           x2_end_space[0], x2_end_space[1], x2_end_space[2], x2_end_space[3]);
  g_print ("\tY1 SPACE: (%d, %d, %d, %d) - (%d, %d, %d, %d)\n",
           y1_beg_space[0], y1_beg_space[1], y1_beg_space[2], y1_beg_space[3],
           y1_end_space[0], y1_end_space[1], y1_end_space[2], y1_end_space[3]);
  g_print ("\tY2 SPACE: (%d, %d, %d, %d) - (%d, %d, %d, %d)\n",
           y2_beg_space[0], y2_beg_space[1], y2_beg_space[2], y2_beg_space[3],
           y2_end_space[0], y2_end_space[1], y2_end_space[2], y2_end_space[3]);
  */

  /* aligning crossing-points between x- and y-axes */
  /* FIXME: Use different functions than self-defined min-max-functions! */
  /* FIXME: Use extensive tests (at best even unittests) to test these functionalities */
  /* TODO: Write more documentation here */
  y1_allocation.x +=
    max (
      max(x2_beg_space[2] - y1_end_space[2], 0),
      max(x1_beg_space[2] - y1_beg_space[2], 0));
  y1_allocation.y -=
    min (
      min(y1_end_space[0] - x2_beg_space[0], 0),
      min(x2_end_space[0] - y2_end_space[0], 0));
  y1_allocation.height -=
    max (max(x2_beg_space[0] - y1_end_space[0], 0),
         max(y2_end_space[0] - x1_end_space[0], 0)) +
    max (max(x1_beg_space[1] - y1_beg_space[1], 0),
         max(y2_beg_space[1] - x1_end_space[1], 0));

  y2_allocation.x -=
    max (
      max(x2_end_space[3] - y2_end_space[3], 0),
      max(x1_end_space[3] - y2_beg_space[3], 0));
  y2_allocation.y -=
    min (
      min(x2_beg_space[0] - y1_end_space[0], 0),// + min(x2_end_space[0] - y2_end_space[0], 0),
      min(y2_end_space[0] - x2_end_space[0], 0));
  y2_allocation.height -=
    max (max(x2_end_space[0] - y2_end_space[0], 0),
         max(y1_end_space[0] - x2_end_space[0], 0)) +
    max (max(x1_end_space[1] - y2_beg_space[1], 0),
         max(y1_beg_space[1] - x1_beg_space[1], 0));

  x2_allocation.x +=
    max (
      max(x1_beg_space[2] - y1_beg_space[2], 0) + max(y1_end_space[2] - x2_beg_space[2], 0),
      max(y1_end_space[2] - x2_beg_space[2], 0));
  x2_allocation.y -=
    min (
      min(x2_beg_space[0] - y1_end_space[0], 0),
      min(x2_end_space[0] - y2_end_space[0], 0));
  x2_allocation.width -=
    max (
      max(y2_end_space[3] - x2_end_space[3], 0),
      max(x1_end_space[3] - y2_beg_space[3], 0)) +
    max (
      max(y1_end_space[2] - x2_beg_space[2], 0),
      max(x1_beg_space[2] - y1_beg_space[2], 0) + max(y1_end_space[2] - x2_beg_space[2], 0));

  x1_allocation.x +=
    max (
      max (y1_beg_space[2] - x1_beg_space[2], 0),
      max (x2_beg_space[2] - y1_end_space[2], 0) + max(y1_beg_space[2] - x1_beg_space[2], 0));
  x1_allocation.y -=
    max (
      max (y1_beg_space[1] - x1_beg_space[1], 0),
      max (y2_beg_space[1] - x1_end_space[1], 0));
  x1_allocation.width -=
    max (
      max(y2_beg_space[3] - x1_end_space[3], 0),
      max(x2_beg_space[3] - y2_end_space[3], 0)) +
    max (
      max(y1_beg_space[2] - x1_beg_space[2], 0),
      max(x2_beg_space[2] - y1_end_space[2], 0) + max(y1_beg_space[2] - x1_beg_space[2], 0));

  /*
  g_print ("\tX1 ALLOC: X(%d, %d) - Y(%d, %d)\n",
           x1_allocation.x, x1_allocation.width,
           x1_allocation.y, x1_allocation.height);
  g_print ("\tX2 ALLOC: X(%d, %d) - Y(%d, %d)\n",
           x2_allocation.x, x2_allocation.width,
           x2_allocation.y, x2_allocation.height);
  g_print ("\tY1 ALLOC: X(%d, %d) - Y(%d, %d)\n",
           y1_allocation.x, y1_allocation.width,
           y1_allocation.y, y1_allocation.height);
  g_print ("\tY2 ALLOC: X(%d, %d) - Y(%d, %d)\n",
           y2_allocation.x, y2_allocation.width,
           y2_allocation.y, y2_allocation.height);
  */

  if (priv->x1_axis &&
      gtk_widget_get_visible (GTK_WIDGET(priv->x1_axis)))
  {
    if (gtk_widget_get_window (GTK_WIDGET (layer->priv->x1_axis)))
      gdk_window_move_resize (
        gtk_widget_get_window (GTK_WIDGET (layer->priv->x1_axis)),
        allocation->x, allocation->y,
        allocation->width, allocation->height);

    gtk_widget_size_allocate (
      GTK_WIDGET (layer->priv->x1_axis), &x1_allocation);
  }

  if (priv->x2_axis &&
      gtk_widget_get_visible (GTK_WIDGET(priv->x2_axis)))
  {
    if (gtk_widget_get_window (GTK_WIDGET (layer->priv->x2_axis)))
      gdk_window_move_resize (
        gtk_widget_get_window (GTK_WIDGET (layer->priv->x2_axis)),
        allocation->x, allocation->y,
        allocation->width, allocation->height);

    gtk_widget_size_allocate (
      GTK_WIDGET (layer->priv->x2_axis), &x2_allocation);
  }

  if (priv->y1_axis &&
      gtk_widget_get_visible (GTK_WIDGET(priv->y1_axis)))
  {
    if (gtk_widget_get_window (GTK_WIDGET (layer->priv->y1_axis)))
      gdk_window_move_resize (
        gtk_widget_get_window (GTK_WIDGET (layer->priv->y1_axis)),
        allocation->x, allocation->y,
        allocation->width, allocation->height);

    gtk_widget_size_allocate (
      GTK_WIDGET (layer->priv->y1_axis), &y1_allocation);
  }

  if (priv->y2_axis &&
      gtk_widget_get_visible (GTK_WIDGET(priv->y2_axis)))
  {
    if (gtk_widget_get_window (GTK_WIDGET (layer->priv->y2_axis)))
      gdk_window_move_resize (
        gtk_widget_get_window (GTK_WIDGET (layer->priv->y2_axis)),
        allocation->x, allocation->y,
        allocation->width, allocation->height);

    gtk_widget_size_allocate (
      GTK_WIDGET (layer->priv->y2_axis), &y2_allocation);
  }


  /* This is not the very best solution but it avoids ugly relicts */
  {
    gboolean resize_axis;

    children = gtk_container_get_children (GTK_CONTAINER (layer));

    for (children_copy = children;
         children_copy;
         children_copy = children_copy->next)
    {

      if (GDV_IS_AXIS (children_copy->data))
      {
        g_object_get (children_copy->data,
                      "resize-during-redraw", &resize_axis, NULL);

        if (resize_axis)// && !gtk_widget_get_visible (GTK_WIDGET (children_copy->next)))
        {
          _hide_show_all_contents (children, FALSE);
          g_list_free (children);
          return;
        }
        else
          _hide_show_all_contents (children, TRUE);
      }

    }

    g_list_free (children);
  }

  /* Determine the content-allocation */
  content_allocation.x =
    min (
      min(
        min(y1_allocation.x + y1_beg_space[2],
            x1_allocation.x + x1_beg_space[2]),
        min(y2_allocation.x + y2_beg_space[2],
            x2_allocation.x + x2_beg_space[2])),
      min(
        min(y1_allocation.x + y1_end_space[2],
            x1_allocation.x + x1_end_space[2]),
        min(y2_allocation.x + y2_end_space[2],
            x2_allocation.x + x2_end_space[2])));
  content_allocation.y =
    min (
      min(
        min(y1_allocation.y + y1_beg_space[0],
            x1_allocation.y + x1_beg_space[0]),
        min(y2_allocation.y + y2_beg_space[0],
            x2_allocation.y + x2_beg_space[0])),
      min(
        min(y1_allocation.y + y1_end_space[0],
            x1_allocation.y + x1_end_space[0]),
        min(y2_allocation.y + y2_end_space[0],
            x2_allocation.y + x2_end_space[0])));

  content_allocation.width =
    max (
      max(
        max(y1_allocation.x + y1_allocation.width - y1_beg_space[3],
            x1_allocation.x + x1_allocation.width - x1_beg_space[3]),
        max(y2_allocation.x + y2_allocation.width - y2_beg_space[3],
            x2_allocation.x + x2_allocation.width - x2_beg_space[3])),
      max(
        max(y1_allocation.x + y1_allocation.width - y1_end_space[3],
            x1_allocation.x + x1_allocation.width - x1_end_space[3]),
        max(y2_allocation.x + y2_allocation.width - y2_end_space[3],
            x2_allocation.x + x2_allocation.width - x2_end_space[3])));
  content_allocation.width  -= content_allocation.x;

  content_allocation.height =
    max (
      max(
        max(y1_allocation.y + y1_allocation.height - y1_beg_space[1],
            x1_allocation.y + x1_allocation.height - x1_beg_space[1]),
        max(y2_allocation.y + y2_allocation.height - y2_beg_space[1],
            x2_allocation.y + x2_allocation.height - x2_beg_space[1])),
      max(
        max(y1_allocation.y + y1_allocation.height - y1_end_space[1],
            x1_allocation.y + x1_allocation.height - x1_end_space[1]),
        max(y2_allocation.y + y2_allocation.height - y2_end_space[1],
            x2_allocation.y + x2_allocation.height - x2_end_space[1])));
  content_allocation.height  -= content_allocation.y;

  children = gtk_container_get_children (GTK_CONTAINER (layer));

  for (list = children; list; list = list->next)
  {
    if (GDV_LAYER_IS_CONTENT (list->data) &&
        gtk_widget_get_realized (list->data))
    {
      GdkRectangle local_allocation;

      GTK_OVERLAY_CLASS (gdv_twod_layer_parent_class)->get_child_position (
        GTK_OVERLAY (layer), GTK_WIDGET (list->data),
        &local_allocation);

      if (gtk_widget_get_window (GTK_WIDGET (list->data)))
        gdk_window_move_resize (
          gtk_widget_get_window (GTK_WIDGET (list->data)),
          allocation->x, allocation->y,
          allocation->width, allocation->height);

      gtk_widget_size_allocate (
        GTK_WIDGET (list->data), &content_allocation);
    }
  }

  g_list_free (children);

}

static void
gdv_twod_layer_get_preferred_width (GtkWidget           *widget,
                                    gint                *minimum_size,
                                    gint                *natural_size)
{
  gdv_twod_layer_measure (GDV_TWOD_LAYER (widget),
                          GTK_ORIENTATION_HORIZONTAL,
                          -1,
                          minimum_size, natural_size,
                          NULL, NULL, NULL);
}

static void
gdv_twod_layer_get_preferred_height (GtkWidget           *widget,
                                     gint                *minimum_size,
                                     gint                *natural_size)
{
  gdv_twod_layer_measure (GDV_TWOD_LAYER (widget),
                          GTK_ORIENTATION_VERTICAL,
                          -1,
                          minimum_size, natural_size,
                          NULL, NULL, NULL);
}

static void
gdv_twod_layer_get_preferred_height_for_width (GtkWidget           *widget,
    gint                 width,
    gint                *minimum_height,
    gint                *natural_height)
{
  gdv_twod_layer_measure (GDV_TWOD_LAYER (widget),
                          GTK_ORIENTATION_VERTICAL,
                          width,
                          minimum_height, natural_height,
                          NULL, NULL, NULL);
}

static void
gdv_twod_layer_get_preferred_width_for_height (GtkWidget           *widget,
    gint                 height,
    gint                *minimum_width,
    gint                *natural_width)
{
  gdv_twod_layer_measure (GDV_TWOD_LAYER (widget),
                          GTK_ORIENTATION_HORIZONTAL,
                          height,
                          minimum_width, natural_width,
                          NULL, NULL, NULL);
}


/**
 * gdv_twod_layer_set_xrange:
 * @layer: a #GdvTwodLayer
 * @x_beg: the new scale begin in x-direction
 * @x_end: the new scale end in x-direction
 *
 * Can be used to fastly change the layer-range.
 *
 **/
void gdv_twod_layer_set_xrange (GdvTwodLayer *layer,
                                gdouble x_beg, gdouble x_end)
{
  g_return_if_fail (GDV_TWOD_IS_LAYER (layer));

//  if (layer->priv->x1_axis)
//    g_print ("X1 CHECK\n");
//  if (layer->priv->x2_axis)
//    g_print ("X2 CHECK\n");

  if (layer->priv->x1_axis)
    g_object_set (layer->priv->x1_axis,
                  "scale-beg-val", x_beg,
                  "scale-end-val", x_end,
                  NULL);

  if (layer->priv->x2_axis)
    g_object_set (layer->priv->x2_axis,
                  "scale-beg-val", x_beg,
                  "scale-end-val", x_end,
                  NULL);
}

/**
 * gdv_twod_layer_set_yrange:
 * @layer: a #GdvTwodLayer
 * @y_beg: the new scale begin in y-direction
 * @y_end: the new scale end in y-direction
 *
 * Can be used to fastly change the layer-range.
 *
 **/
void gdv_twod_layer_set_yrange (GdvTwodLayer *layer,
                                gdouble y_beg, gdouble y_end)
{
  g_return_if_fail (GDV_TWOD_IS_LAYER (layer));

  if (layer->priv->y1_axis)
    g_object_set (layer->priv->y1_axis,
                  "scale-beg-val", y_beg,
                  "scale-end-val", y_end,
                  NULL);

  if (layer->priv->y2_axis)
    g_object_set (layer->priv->y2_axis,
                  "scale-beg-val", y_beg,
                  "scale-end-val", y_end,
                  NULL);
}

/**
 * gdv_twod_layer_get_xrange:
 * @layer: a #GdvTwodLayer
 * @x_beg: the position to store the begin of this layer in x-direction
 * @x_end: the position to store the end of this layer in x-direction
 *
 * Returns the maximum and minimum of both x-axes.
 *
 **/
void gdv_twod_layer_get_xrange (GdvTwodLayer *layer, gdouble *x_beg, gdouble *x_end)
{
  gboolean x1_axis_avl = FALSE;
  gboolean x2_axis_avl = FALSE;
  gdouble x1_beg, x1_end;
  gdouble x2_beg, x2_end;

  g_return_if_fail(GDV_TWOD_IS_LAYER (layer));

  if (layer->priv->x1_axis &&
      gtk_widget_is_visible(GTK_WIDGET(layer->priv->x1_axis)))
  {
    x1_axis_avl = TRUE;
    g_object_get (G_OBJECT(layer->priv->x1_axis),
                  "scale-beg-val", &x1_beg,
                  "scale-end-val", &x1_end,
                  NULL);
  }

  if (layer->priv->x2_axis &&
      gtk_widget_is_visible(GTK_WIDGET(layer->priv->x2_axis)))
  {
    x2_axis_avl = TRUE;
    g_object_get (G_OBJECT(layer->priv->x2_axis),
                  "scale-beg-val", &x2_beg,
                  "scale-end-val", &x2_end,
                  NULL);
  }

  if (x1_axis_avl && x2_axis_avl)
  {
    if (x1_beg >= x1_end)
    {
      *x_beg = GSL_MAX(x1_beg, x2_beg);
      *x_end = GSL_MIN(x1_end, x2_end);
      return;
    }
    else
    {
      *x_beg = GSL_MIN(x1_beg, x2_beg);
      *x_end = GSL_MAX(x1_end, x2_end);
      return;
    }
  }
  else if (x1_axis_avl)
  {
      *x_beg = x1_beg;
      *x_end = x1_end;
      return;
  }
  else if (x2_axis_avl)
  {
      *x_beg = x2_beg;
      *x_end = x2_end;
      return;
  }

  return;
}

/**
 * gdv_twod_layer_get_yrange:
 * @layer: a #GdvTwodLayer
 * @y_beg: the position to store the begin of this layer in y-direction
 * @y_end: the position to store the end of this layer in y-direction
 *
 * Returns the maximum and minimum of both y-axes.
 *
 **/
void gdv_twod_layer_get_yrange (GdvTwodLayer *layer, gdouble *y_beg, gdouble *y_end)
{
  gboolean y1_axis_avl = FALSE;
  gboolean y2_axis_avl = FALSE;
  gdouble y1_beg, y1_end;
  gdouble y2_beg, y2_end;

  g_return_if_fail(GDV_TWOD_IS_LAYER (layer));

  if (layer->priv->y1_axis &&
      gtk_widget_is_visible(GTK_WIDGET(layer->priv->y1_axis)))
  {
    y1_axis_avl = TRUE;
    g_object_get (G_OBJECT(layer->priv->y1_axis),
                  "scale-beg-val", &y1_beg,
                  "scale-end-val", &y1_end,
                  NULL);
  }

  if (layer->priv->y2_axis &&
      gtk_widget_is_visible(GTK_WIDGET(layer->priv->y2_axis)))
  {
    y2_axis_avl = TRUE;
    g_object_get (G_OBJECT(layer->priv->y2_axis),
                  "scale-beg-val", &y2_beg,
                  "scale-end-val", &y2_end,
                  NULL);
  }

  if (y1_axis_avl && y2_axis_avl)
  {
    if (y1_beg >= y1_end)
    {
      *y_beg = GSL_MAX(y1_beg, y2_beg);
      *y_end = GSL_MIN(y1_end, y2_end);
      return;
    }
    else
    {
      *y_beg = GSL_MIN(y1_beg, y2_beg);
      *y_end = GSL_MAX(y1_end, y2_end);
      return;
    }
  }
  else if (y1_axis_avl)
  {
      *y_beg = y1_beg;
      *y_end = y1_end;
      return;
  }
  else if (y2_axis_avl)
  {
      *y_beg = y2_beg;
      *y_end = y2_end;
      return;
  }

  return;
}

/**
 * gdv_twod_layer_get_axis:
 * @layer: a #GdvTwodLayer
 * @axis_type: the axis-type
 *
 * This function can be used to receive a particular axis of an #GdvTwodLayer.
 *
 * Returns: (transfer none): A pointer to the axis
 **/
GdvAxis *gdv_twod_layer_get_axis (GdvTwodLayer *layer,
                                  GdvTwodAxisType axis_type)
{
  g_return_val_if_fail (GDV_TWOD_IS_LAYER (layer), NULL);

  switch (axis_type)
  {
  case GDV_X1_AXIS:
    return layer->priv->x1_axis;
    break;

  case GDV_X2_AXIS:
    return layer->priv->x2_axis;
    break;

  case GDV_Y1_AXIS:
    return layer->priv->y1_axis;
    break;

  case GDV_Y2_AXIS:
    return layer->priv->y2_axis;
    break;

  default:
    /* unknown axis type */
    g_warning ("gdv_twod_layer_get_axis: axis_type %d unknown\n", axis_type);
    break;
  }

  return NULL;
}

/**
 * gdv_twod_layer_set_axis:
 * @layer: a #GdvTwodLayer
 * @axis: a #GdvAxis
 * @axis_type: the axis-type
 *
 * This function replaces an single axis of the @layer.
 **/
void gdv_twod_layer_set_axis (
  GdvTwodLayer *layer,
  GdvAxis *axis,
  GdvTwodAxisType axis_type)
{
  g_return_if_fail (GDV_TWOD_IS_LAYER (layer));
  g_return_if_fail (GDV_AXIS (axis));

  /* FIXME:
   *   instead of requesting the allocation, it would be better to make a
   *   good implementation of size-management for axes/layer */
  /* FIXME:
   *   there is something inconsistent about adding by overlay or container */
  switch (axis_type)
  {
  case GDV_X1_AXIS:
    if (layer->priv->x1_axis)
      gtk_container_remove (GTK_CONTAINER (layer),
                            GTK_WIDGET (layer->priv->x1_axis));

    layer->priv->x1_axis = axis;
    gtk_container_add (GTK_CONTAINER (layer), GTK_WIDGET (axis));
    gtk_widget_queue_allocate (GTK_WIDGET (layer));
    break;

  case GDV_X2_AXIS:
    if (layer->priv->x2_axis)
      gtk_container_remove (GTK_CONTAINER (layer),
                            GTK_WIDGET (layer->priv->x2_axis));

    layer->priv->x2_axis = axis;
    gtk_container_add (GTK_CONTAINER (layer), GTK_WIDGET (axis));
    gtk_widget_queue_allocate (GTK_WIDGET (layer));
    break;

  case GDV_Y1_AXIS:
    if (layer->priv->y1_axis)
      gtk_container_remove (GTK_CONTAINER (layer),
                            GTK_WIDGET (layer->priv->y1_axis));

    gtk_container_add (GTK_CONTAINER (layer), GTK_WIDGET (axis));
    layer->priv->y1_axis = axis;
    gtk_widget_queue_allocate (GTK_WIDGET (layer));
    break;

  case GDV_Y2_AXIS:
    if (layer->priv->y2_axis)
      gtk_container_remove (GTK_CONTAINER (layer),
                            GTK_WIDGET (layer->priv->y2_axis));

    gtk_container_add (GTK_CONTAINER (layer), GTK_WIDGET (axis));
    layer->priv->y2_axis = axis;
    gtk_widget_queue_allocate (GTK_WIDGET (layer));
    break;

  default:
    /* unknown axis type */
    g_warning ("gdv_twod_layer_set_axis: axis_type %d unknown\n", axis_type);
    break;
  }

}

/**
 * gdv_twod_layer_unset_axis:
 * @layer: a #GdvTwodLayer
 * @axis_type: the axis-type
 *
 * Removes an axis from the @layer.
 **/
void gdv_twod_layer_unset_axis (GdvTwodLayer *layer, GdvTwodAxisType axis_type)
{
  g_return_if_fail (GDV_TWOD_IS_LAYER (layer));

  switch (axis_type)
  {
  case GDV_X1_AXIS:
    if (layer->priv->x1_axis)
      gtk_container_remove (GTK_CONTAINER (layer),
                            GTK_WIDGET (layer->priv->x1_axis));

    layer->priv->x1_axis = NULL;
    gtk_widget_queue_allocate (GTK_WIDGET (layer));
    break;

  case GDV_X2_AXIS:
    if (layer->priv->x2_axis)
      gtk_container_remove (GTK_CONTAINER (layer),
                            GTK_WIDGET (layer->priv->x2_axis));

    layer->priv->x2_axis = NULL;
    gtk_widget_queue_allocate (GTK_WIDGET (layer));
    break;

  case GDV_Y1_AXIS:
    if (layer->priv->y1_axis)
      gtk_container_remove (GTK_CONTAINER (layer),
                            GTK_WIDGET (layer->priv->y1_axis));

    layer->priv->y1_axis = NULL;
    gtk_widget_queue_allocate (GTK_WIDGET (layer));
    break;

  case GDV_Y2_AXIS:
    if (layer->priv->y2_axis)
      gtk_container_remove (GTK_CONTAINER (layer),
                            GTK_WIDGET (layer->priv->y2_axis));

    layer->priv->y2_axis = NULL;
    gtk_widget_queue_allocate (GTK_WIDGET (layer));
    break;

  default:
    /* unknown axis type */
    g_warning ("gdv_twod_layer_unset_axis: axis_type %d unknown\n", axis_type);
    break;
  }
}

static void
gdv_twod_layer_measure (
  GdvTwodLayer             *twod_layer,
  GtkOrientation       orientation,
  int                  for_size,
  int                 *minimum,
  int                 *natural,
  int                 *minimum_baseline,
  int                 *natural_baseline,
  gpointer             data)
{
  GdvTwodLayerPrivate *priv = twod_layer->priv;
  gint return_sum_nat = 0, return_sum_min = 0;
  gint tmp_nat = 0, tmp_min = 0;

  gint x1_beg_space[4] = {0, 0, 0, 0};
  gint x1_beg_space_nat[4] = {0, 0, 0, 0};
  gint x1_end_space[4] = {0, 0, 0, 0};
  gint x1_end_space_nat[4] = {0, 0, 0, 0};

  gint x2_beg_space[4] = {0, 0, 0, 0};
  gint x2_beg_space_nat[4] = {0, 0, 0, 0};
  gint x2_end_space[4] = {0, 0, 0, 0};
  gint x2_end_space_nat[4] = {0, 0, 0, 0};

  gint y1_beg_space[4] = {0, 0, 0, 0};
  gint y1_beg_space_nat[4] = {0, 0, 0, 0};
  gint y1_end_space[4] = {0, 0, 0, 0};
  gint y1_end_space_nat[4] = {0, 0, 0, 0};

  gint y2_beg_space[4] = {0, 0, 0, 0};
  gint y2_beg_space_nat[4] = {0, 0, 0, 0};
  gint y2_end_space[4] = {0, 0, 0, 0};
  gint y2_end_space_nat[4] = {0, 0, 0, 0};

  *minimum = 0;
  *natural = 0;

  if (orientation == GTK_ORIENTATION_HORIZONTAL)
  {
    if (priv->y1_axis &&
        gtk_widget_get_visible (GTK_WIDGET(priv->y1_axis)))
    {
      if (for_size > 0)
        gtk_widget_get_preferred_width_for_height (
          GTK_WIDGET(priv->y1_axis),
          for_size,
          &tmp_min,
          &tmp_nat);
      else
        gtk_widget_get_preferred_width (
          GTK_WIDGET(priv->y1_axis),
          &tmp_min,
          &tmp_nat);

      GDV_AXIS_CLASS (GDV_AXIS_GET_CLASS (
                        priv->y1_axis))->get_space_to_beg_position (
                          GDV_AXIS (priv->y1_axis), GTK_POS_LEFT, -1,
                          &y1_beg_space[2], &y1_beg_space_nat[2], NULL);
      GDV_AXIS_CLASS (GDV_AXIS_GET_CLASS (
                        priv->y1_axis))->get_space_to_beg_position (
                          GDV_AXIS (priv->y1_axis), GTK_POS_RIGHT, -1,
                          &y1_beg_space[3], &y1_beg_space_nat[3], NULL);

      GDV_AXIS_CLASS (GDV_AXIS_GET_CLASS (
                        priv->y1_axis))->get_space_to_end_position (
                          GDV_AXIS (priv->y1_axis), GTK_POS_LEFT, -1,
                          &y1_end_space[2], &y1_end_space_nat[2], NULL);
      GDV_AXIS_CLASS (GDV_AXIS_GET_CLASS (
                        priv->y1_axis))->get_space_to_end_position (
                          GDV_AXIS (priv->y1_axis), GTK_POS_RIGHT, -1,
                          &y1_end_space[3], &y1_end_space_nat[3], NULL);

      return_sum_nat += tmp_nat;
      return_sum_min += tmp_min;

    }

    if (priv->y2_axis &&
        gtk_widget_get_visible (GTK_WIDGET(priv->y2_axis)))
    {
      if (for_size > 0)
        gtk_widget_get_preferred_width_for_height (
          GTK_WIDGET(priv->y2_axis),
          for_size,
          &tmp_min,
          &tmp_nat);
      else
        gtk_widget_get_preferred_width (
          GTK_WIDGET(priv->y2_axis),
          &tmp_min,
          &tmp_nat);

      GDV_AXIS_CLASS (GDV_AXIS_GET_CLASS (
                        priv->y2_axis))->get_space_to_beg_position (
                          GDV_AXIS (priv->y2_axis), GTK_POS_LEFT, -1,
                          &y2_beg_space[2], &y2_beg_space_nat[2], NULL);
      GDV_AXIS_CLASS (GDV_AXIS_GET_CLASS (
                        priv->y2_axis))->get_space_to_beg_position (
                          GDV_AXIS (priv->y2_axis), GTK_POS_RIGHT, -1,
                          &y2_beg_space[3], &y2_beg_space_nat[3], NULL);

      GDV_AXIS_CLASS (GDV_AXIS_GET_CLASS (
                        priv->y2_axis))->get_space_to_end_position (
                          GDV_AXIS (priv->y2_axis), GTK_POS_LEFT, -1,
                          &y2_end_space[2], &y2_end_space_nat[2], NULL);
      GDV_AXIS_CLASS (GDV_AXIS_GET_CLASS (
                        priv->y2_axis))->get_space_to_end_position (
                          GDV_AXIS (priv->y2_axis), GTK_POS_RIGHT, -1,
                          &y2_end_space[3], &y2_end_space_nat[3], NULL);

      return_sum_nat += tmp_nat;
      return_sum_min += tmp_min;

    }

    if (priv->x1_axis &&
        gtk_widget_get_visible (GTK_WIDGET(priv->x1_axis)))
    {
      if (for_size > 0)
        gtk_widget_get_preferred_width_for_height (
          GTK_WIDGET(priv->x1_axis),
          for_size,
          &tmp_min,
          &tmp_nat);
      else
        gtk_widget_get_preferred_width (
          GTK_WIDGET(priv->x1_axis),
          &tmp_min,
          &tmp_nat);

      GDV_AXIS_CLASS (GDV_AXIS_GET_CLASS (
                        priv->x1_axis))->get_space_to_beg_position (
                          GDV_AXIS (priv->x1_axis), GTK_POS_LEFT, -1,
                          &x1_beg_space[2], &x1_beg_space_nat[2], NULL);
      GDV_AXIS_CLASS (GDV_AXIS_GET_CLASS (
                        priv->x1_axis))->get_space_to_beg_position (
                          GDV_AXIS (priv->x1_axis), GTK_POS_RIGHT, -1,
                          &x1_beg_space[3], &x1_beg_space_nat[3], NULL);

      GDV_AXIS_CLASS (GDV_AXIS_GET_CLASS (
                        priv->x1_axis))->get_space_to_end_position (
                          GDV_AXIS (priv->x1_axis), GTK_POS_LEFT, -1,
                          &x1_end_space[2], &x1_end_space_nat[2], NULL);
      GDV_AXIS_CLASS (GDV_AXIS_GET_CLASS (
                        priv->x1_axis))->get_space_to_end_position (
                          GDV_AXIS (priv->x1_axis), GTK_POS_RIGHT, -1,
                          &x1_end_space[3], &x1_end_space_nat[3], NULL);

      return_sum_nat = tmp_nat > return_sum_nat ? tmp_nat : return_sum_nat;
      return_sum_min = tmp_min > return_sum_min ? tmp_min : return_sum_min;

    }

    if (priv->x2_axis &&
        gtk_widget_get_visible (GTK_WIDGET(priv->x2_axis)))
    {
      if (for_size > 0)
        gtk_widget_get_preferred_width_for_height (
          GTK_WIDGET(priv->x2_axis),
          for_size,
          &tmp_min,
          &tmp_nat);
      else
        gtk_widget_get_preferred_width (
          GTK_WIDGET(priv->x2_axis),
          &tmp_min,
          &tmp_nat);

      GDV_AXIS_CLASS (GDV_AXIS_GET_CLASS (
                        priv->x2_axis))->get_space_to_beg_position (
                          GDV_AXIS (priv->x2_axis), GTK_POS_LEFT, -1,
                          &x2_beg_space[2], &x2_beg_space_nat[2], NULL);
      GDV_AXIS_CLASS (GDV_AXIS_GET_CLASS (
                        priv->x2_axis))->get_space_to_beg_position (
                          GDV_AXIS (priv->x2_axis), GTK_POS_RIGHT, -1,
                          &x2_beg_space[3], &x2_beg_space_nat[3], NULL);

      GDV_AXIS_CLASS (GDV_AXIS_GET_CLASS (
                        priv->x2_axis))->get_space_to_end_position (
                          GDV_AXIS (priv->x2_axis), GTK_POS_LEFT, -1,
                          &x2_end_space[2], &x2_end_space_nat[2], NULL);
      GDV_AXIS_CLASS (GDV_AXIS_GET_CLASS (
                        priv->x2_axis))->get_space_to_end_position (
                          GDV_AXIS (priv->x2_axis), GTK_POS_RIGHT, -1,
                          &x2_end_space[3], &x2_end_space_nat[3], NULL);

      return_sum_nat = tmp_nat > return_sum_nat ? tmp_nat : return_sum_nat;
      return_sum_min = tmp_min > return_sum_min ? tmp_min : return_sum_min;

    }

    /* FIXME: ATM this ignores the inverted border-sizes - gen. use is buggy! */
    /* adding boarder values */
    if (priv->x1_axis &&
        gtk_widget_get_visible (GTK_WIDGET(priv->x1_axis)))
    {
      if (priv->y1_axis &&
          gtk_widget_get_visible (GTK_WIDGET(priv->y1_axis)))
      {
        return_sum_nat +=
          abs (x1_beg_space_nat[2] - y1_beg_space_nat[2]);
        return_sum_min +=
          abs (x1_beg_space[2] - y1_beg_space[2]);
      }

      if (priv->y2_axis &&
          gtk_widget_get_visible (GTK_WIDGET(priv->y2_axis)))
      {
        return_sum_nat +=
          abs (x1_end_space_nat[3] - y2_beg_space_nat[3]);
        return_sum_min +=
          abs (x1_end_space[3] - y2_beg_space[3]);
      }
    }

    if (priv->x2_axis &&
        gtk_widget_get_visible (GTK_WIDGET(priv->x2_axis)))
    {
      if (priv->y1_axis &&
          gtk_widget_get_visible (GTK_WIDGET(priv->y1_axis)))
      {
        return_sum_nat +=
          abs (x2_beg_space_nat[2] - y1_end_space_nat[2]);
        return_sum_min +=
          abs (x2_beg_space[2] - y1_end_space[2]);
      }

      if (priv->y2_axis &&
          gtk_widget_get_visible (GTK_WIDGET(priv->y2_axis)))
      {
        return_sum_nat +=
          abs (x2_end_space_nat[3] - y2_end_space_nat[3]);
        return_sum_min +=
          abs (x2_beg_space[3] - y2_end_space[3]);
      }
    }
  }
  else /* orientation == GTK_ORIENTATION_VERTICAL */
  {

    if (priv->x1_axis &&
        gtk_widget_get_visible (GTK_WIDGET(priv->x1_axis)))
    {
      if (for_size > 0)
        gtk_widget_get_preferred_height_for_width (
          GTK_WIDGET(priv->x1_axis),
          for_size,
          &tmp_min,
          &tmp_nat);
      else
        gtk_widget_get_preferred_height (
          GTK_WIDGET(priv->x1_axis),
          &tmp_min,
          &tmp_nat);

      GDV_AXIS_CLASS(GDV_AXIS_GET_CLASS(
                       priv->x1_axis))->get_space_to_beg_position (
                         GDV_AXIS (priv->x1_axis), GTK_POS_TOP, -1,
                         &x1_beg_space[0], &x1_beg_space_nat[0], NULL);
      GDV_AXIS_CLASS(GDV_AXIS_GET_CLASS(
                       priv->x1_axis))->get_space_to_beg_position (
                         GDV_AXIS (priv->x1_axis), GTK_POS_BOTTOM, -1,
                         &x1_beg_space[1], &x1_beg_space_nat[1], NULL);
      GDV_AXIS_CLASS(GDV_AXIS_GET_CLASS(
                       priv->x1_axis))->get_space_to_end_position (
                         GDV_AXIS (priv->x1_axis), GTK_POS_TOP, -1,
                         &x1_end_space[0], &x1_end_space_nat[0], NULL);
      GDV_AXIS_CLASS(GDV_AXIS_GET_CLASS(
                       priv->x1_axis))->get_space_to_end_position (
                         GDV_AXIS (priv->x1_axis), GTK_POS_BOTTOM, -1,
                         &x1_end_space[1], &x1_end_space_nat[1], NULL);

      return_sum_nat += tmp_nat;
      return_sum_min += tmp_min;

    }

    if (priv->x2_axis &&
        gtk_widget_get_visible (GTK_WIDGET(priv->x2_axis)))
    {
      if (for_size > 0)
        gtk_widget_get_preferred_height_for_width (
          GTK_WIDGET(priv->x2_axis),
          for_size,
          &tmp_min,
          &tmp_nat);
      else
        gtk_widget_get_preferred_height (
          GTK_WIDGET(priv->x2_axis),
          &tmp_min,
          &tmp_nat);

      GDV_AXIS_CLASS(GDV_AXIS_GET_CLASS(
                       priv->x2_axis))->get_space_to_beg_position (
                         GDV_AXIS (priv->x2_axis), GTK_POS_TOP, -1,
                         &x2_beg_space[0], &x2_beg_space_nat[0], NULL);
      GDV_AXIS_CLASS(GDV_AXIS_GET_CLASS(
                       priv->x2_axis))->get_space_to_beg_position (
                         GDV_AXIS (priv->x2_axis), GTK_POS_BOTTOM, -1,
                         &x2_beg_space[1], &x2_beg_space_nat[1], NULL);
      GDV_AXIS_CLASS(GDV_AXIS_GET_CLASS(
                       priv->x2_axis))->get_space_to_end_position (
                         GDV_AXIS (priv->x2_axis), GTK_POS_TOP, -1,
                         &x2_end_space[0], &x2_end_space_nat[0], NULL);
      GDV_AXIS_CLASS(GDV_AXIS_GET_CLASS(
                       priv->x2_axis))->get_space_to_end_position (
                         GDV_AXIS (priv->x2_axis), GTK_POS_BOTTOM, -1,
                         &x2_end_space[1], &x2_end_space_nat[1], NULL);

      return_sum_nat += tmp_nat;
      return_sum_min += tmp_min;

    }

    if (priv->y1_axis &&
        gtk_widget_get_visible (GTK_WIDGET(priv->y1_axis)))
    {
      if (for_size > 0)
        gtk_widget_get_preferred_height_for_width (
          GTK_WIDGET(priv->y1_axis),
          for_size,
          &tmp_min,
          &tmp_nat);
      else
        gtk_widget_get_preferred_height (
          GTK_WIDGET(priv->y1_axis),
          &tmp_min,
          &tmp_nat);

      GDV_AXIS_CLASS(GDV_AXIS_GET_CLASS(
                       priv->y1_axis))->get_space_to_beg_position (
                         GDV_AXIS (priv->y1_axis), GTK_POS_TOP, -1,
                         &y1_beg_space[0], &y1_beg_space_nat[0], NULL);
      GDV_AXIS_CLASS(GDV_AXIS_GET_CLASS(
                       priv->y1_axis))->get_space_to_beg_position (
                         GDV_AXIS (priv->y1_axis), GTK_POS_BOTTOM, -1,
                         &y1_beg_space[1], &y1_beg_space_nat[1], NULL);
      GDV_AXIS_CLASS(GDV_AXIS_GET_CLASS(
                       priv->y1_axis))->get_space_to_end_position (
                         GDV_AXIS (priv->y1_axis), GTK_POS_TOP, -1,
                         &y1_end_space[0], &y1_end_space_nat[0], NULL);
      GDV_AXIS_CLASS(GDV_AXIS_GET_CLASS(
                       priv->y1_axis))->get_space_to_end_position (
                         GDV_AXIS (priv->y1_axis), GTK_POS_BOTTOM, -1,
                         &y1_end_space[1], &y1_end_space_nat[1], NULL);

      return_sum_nat = tmp_nat > return_sum_nat ? tmp_nat : return_sum_nat;
      return_sum_min = tmp_min > return_sum_min ? tmp_min : return_sum_min;

    }


    if (priv->y2_axis &&
        gtk_widget_get_visible (GTK_WIDGET(priv->y2_axis)))
    {
      if (for_size > 0)
        gtk_widget_get_preferred_height_for_width (
          GTK_WIDGET(priv->y2_axis),
          for_size,
          &tmp_min,
          &tmp_nat);
      else
        gtk_widget_get_preferred_height (
          GTK_WIDGET(priv->y2_axis),
          &tmp_min,
          &tmp_nat);

      GDV_AXIS_CLASS(GDV_AXIS_GET_CLASS(
                       priv->y2_axis))->get_space_to_beg_position (
                         GDV_AXIS (priv->y2_axis), GTK_POS_TOP, -1,
                         &y2_beg_space[0], &y2_beg_space_nat[0], NULL);
      GDV_AXIS_CLASS(GDV_AXIS_GET_CLASS(
                       priv->y2_axis))->get_space_to_beg_position (
                         GDV_AXIS (priv->y2_axis), GTK_POS_BOTTOM, -1,
                         &y2_beg_space[1], &y2_beg_space_nat[1], NULL);
      GDV_AXIS_CLASS(GDV_AXIS_GET_CLASS(
                       priv->y2_axis))->get_space_to_end_position (
                         GDV_AXIS (priv->y2_axis), GTK_POS_TOP, -1,
                         &y2_end_space[0], &y2_end_space_nat[0], NULL);
      GDV_AXIS_CLASS(GDV_AXIS_GET_CLASS(
                       priv->y2_axis))->get_space_to_end_position (
                         GDV_AXIS (priv->y2_axis), GTK_POS_BOTTOM, -1,
                         &y2_end_space[1], &y2_end_space_nat[1], NULL);

      return_sum_nat = tmp_nat > return_sum_nat ? tmp_nat : return_sum_nat;
      return_sum_min = tmp_min > return_sum_min ? tmp_min : return_sum_min;

    }

    /* adding boarder values */
    if (priv->y1_axis &&
        gtk_widget_get_visible (GTK_WIDGET(priv->y1_axis)))
    {
      if (priv->x1_axis &&
          gtk_widget_get_visible (GTK_WIDGET(priv->x1_axis)))
      {
        return_sum_nat +=
          abs (x1_beg_space_nat[1] - y1_beg_space_nat[1]);
        return_sum_min +=
          abs (x1_beg_space[1] - y1_beg_space[1]);
      }

      if (priv->x2_axis &&
          gtk_widget_get_visible (GTK_WIDGET(priv->x2_axis)))
      {
        return_sum_nat +=
          abs (x2_beg_space_nat[0] - y1_end_space_nat[0]);
        return_sum_min +=
          abs (x2_beg_space[0] - y1_end_space[0]);
      }
    }

    if (priv->y2_axis &&
        gtk_widget_get_visible (GTK_WIDGET(priv->y2_axis)))
    {
      if (priv->x1_axis &&
          gtk_widget_get_visible (GTK_WIDGET(priv->x1_axis)))
      {
        return_sum_nat +=
          abs (x1_end_space_nat[1] - y2_beg_space_nat[1]);
        return_sum_min +=
          abs (x1_end_space[1] - y2_beg_space[1]);
      }

      if (priv->x2_axis &&
          gtk_widget_get_visible (GTK_WIDGET(priv->x2_axis)))
      {
        return_sum_nat +=
          abs (x2_end_space_nat[0] - y2_end_space_nat[0]);
        return_sum_min +=
          abs (x2_beg_space[0] - y2_end_space[0]);
      }
    }
  }

  *minimum = return_sum_min;
  *natural = return_sum_nat;
}

