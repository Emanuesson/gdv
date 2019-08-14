/*
 * gdvlinearaxis.c
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

#include <math.h>
#include <stdarg.h>

#include "gdvaxis.h"
#include "gdvlinearaxis.h"
#include "gdvtic.h"
#include "gdvmtic.h"
#include "gdvrender.h"

/**
 * SECTION:gdvlinearaxis
 * @short_description: a plain linear axis
 * @title: GdvLinearAxis
 *
 * #GdvLinearAxis defines an widget, that plots a simple linear axis. It is the
 * base-class for all axes, that are simply straight.
 *
 * # CSS nodes
 *
 * GdvLinearAxis uses a single CSS node with name linearaxis.
 */

/* Define Properties */
enum
{
  PROP_0,

  PROP_SC_INCR_BASE,

  N_PROPERTIES
};

struct _GdvLinearAxisPrivate
{
//  GSList *tics_list;
//  GSList *mtics_list;

  gdouble scale_increment_base;
};

static GParamSpec *linear_axis_properties [N_PROPERTIES] = {NULL, };

static void
gdv_linear_axis_size_allocate (GtkWidget     *widget,
                               GtkAllocation *allocation);
static gchar *
gdv_linear_axis_make_tic_label_markup (GdvAxis *axis,
                                       gdouble  value);
static gboolean
gdv_linear_axis_on_get_point (GdvAxis *axis,
                              gdouble  value,
                              gdouble *pos_x,
                              gdouble *pos_y);
static gboolean
gdv_linear_axis_on_get_inner_dir (GdvAxis *axis,
                                  gdouble  value,
                                  gdouble *pos_x,
                                  gdouble *pos_y);
static void
gdv_linear_axis_get_space_to_beg_position (GdvAxis        *axis,
                                           GtkPositionType direction,
                                           int             for_size,
                                           int            *minimum,
                                           int            *natural,
                                           gpointer        data);
static void
gdv_linear_axis_get_space_to_end_position (GdvAxis        *axis,
                                           GtkPositionType direction,
                                           int             for_size,
                                           int            *minimum,
                                           int            *natural,
                                           gpointer        data);
static void
gdv_linear_axis_set_property (GObject      *object,
                              guint         property_id,
                              const GValue *value,
                              GParamSpec   *pspec);
static void
gdv_linear_axis_get_property (GObject    *object,
                              guint       property_id,
                              GValue     *value,
                              GParamSpec *pspec);

G_DEFINE_TYPE_WITH_PRIVATE (GdvLinearAxis, gdv_linear_axis, GDV_TYPE_AXIS);

static void
gdv_linear_axis_init (GdvLinearAxis *axis)
{
  GdvLinearAxisPrivate *priv;

  axis->priv = gdv_linear_axis_get_instance_private (axis);

  priv = axis->priv;

  gtk_widget_set_can_focus (GTK_WIDGET (axis), TRUE);
  gtk_widget_set_receives_default (GTK_WIDGET (axis), TRUE);
  gtk_widget_set_has_window (GTK_WIDGET (axis), FALSE);

  priv->scale_increment_base = 10.0;
}

static void
gdv_linear_axis_class_init (GdvLinearAxisClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
  GdvAxisClass *axis_class = GDV_AXIS_CLASS (klass);

  widget_class->size_allocate = gdv_linear_axis_size_allocate;

  object_class->set_property = gdv_linear_axis_set_property;
  object_class->get_property = gdv_linear_axis_get_property;

  axis_class->get_point = gdv_linear_axis_on_get_point;
  axis_class->get_inner_dir = gdv_linear_axis_on_get_inner_dir;
  axis_class->make_tic_label_markup = gdv_linear_axis_make_tic_label_markup;

  axis_class->get_space_to_beg_position =
    gdv_linear_axis_get_space_to_beg_position;
  axis_class->get_space_to_end_position =
    gdv_linear_axis_get_space_to_end_position;

  gtk_widget_class_set_css_name (widget_class, "linearaxis");

  /**
   * GdvLinearAxis:scale-increment-base:
   *
   * Sets the base value that should be used to determine the
   * scale-increment-value during automatic setting. Usually when
   * working with decimal values this is 10.0.
   * However, in some cases it is usefull to change this value, e.g. when
   * you want to implement a time-scale with the base of 60sec or an
   * angle-distribution with axis-tics on multiples of Pi.
   *
   */
  linear_axis_properties [PROP_SC_INCR_BASE] =
    g_param_spec_double ("scale-increment-base",
                         "scale auto increment base value",
                         "The base-value to calculate automatically tic-values",
                         0.0,
                         G_MAXDOUBLE,
                         10.0,
                         G_PARAM_READWRITE);

  g_object_class_install_properties (object_class,
                                     N_PROPERTIES,
                                     linear_axis_properties);
}

static void
gdv_linear_axis_set_property (GObject      *object,
                              guint         property_id,
                              const GValue *value,
                              GParamSpec   *pspec)
{
  GdvLinearAxis *self = GDV_LINEAR_AXIS (object);

  switch (property_id)
  {
  case PROP_SC_INCR_BASE:
    self->priv->scale_increment_base = g_value_get_double (value);
    break;

  default:
    /* unknown property */
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }
}

static void
gdv_linear_axis_get_property (GObject    *object,
                              guint       property_id,
                              GValue     *value,
                              GParamSpec *pspec)
{
  GdvLinearAxis *self = GDV_LINEAR_AXIS (object);

  switch (property_id)
  {
  case PROP_SC_INCR_BASE:
    g_value_set_double (value, self->priv->scale_increment_base);
    break;

  default:
    /* unknown property */
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }
}

static gboolean
_is_present_in_approved_list (GdvTic *tic, GList *list)
{
  GList *list_copy;
  gdouble tic_value;

  g_object_get (tic, "value", &tic_value, NULL);

  /* Removing the excess of tic s*/
  for (list_copy = list; list_copy; list_copy = list_copy->next)
  {
    gdouble current_tic_value;

    g_object_get (
      G_OBJECT (list_copy->data), "value", &current_tic_value, NULL);

    /* FIXME: This is dangerous - it should be substituted by a more
     *        tolerant solution
     */
    if (tic_value == current_tic_value)
      return TRUE;
  }

  return FALSE;
}

/*
 * Simply provides the tic for a given Axis, according to the provided value.
 * This scales quadratic, which is not suited for invoking this function
 * multiple times.
 */
static void
_determine_tic_by_val(GdvLinearAxis *linear_axis,
                      gdouble tics_val,
                      GdvTic **tic)
{
  GList *tic_list, *tic_list_start;

  /* looking for the beg- and end-tic */
  tic_list = gdv_axis_get_tic_list (GDV_AXIS (linear_axis));
  tic_list_start = tic_list;

  while (tic_list)
  {
    gdouble current_value;
    g_object_get (G_OBJECT (tic_list->data), "value", &current_value, NULL);

    /* FIXME: this might not be correctly reached due to round-off errors */
    if (current_value == tics_val)
      *tic = tic_list->data;

    tic_list = tic_list->next;
  }

  g_list_free (tic_list_start);
}

struct _linear_axis_scale_definition {
  gdouble beg_val;
  gdouble end_val;
  gdouble increment;
};

static gboolean comp_equal_values (gdouble value_to_compare,
                                   gdouble tic_value,
                                   struct _linear_axis_scale_definition *scale)
{
  gdouble local_val_resid =
    fmod ((tic_value - scale->beg_val) / scale->increment, 1.0);

  if (tic_value < fmin (scale->beg_val, scale->end_val) ||
      tic_value > fmax (scale->beg_val, scale->end_val) ||
      !((fabs(local_val_resid) * fabs(scale->increment) < 1e-10) ||
        (fabs(1.0 - local_val_resid) * fabs(scale->increment) < 1e-10)))
      //_is_present_in_approved_list (tics_copy->data, tics_approved_list))
    /* TODO: separate the round-off problems to own function */
    return FALSE;
  else
    return TRUE;

}

/*
 * Simply provides the tic for a given Axis, according to the provided value. This scales
 * quadratic, which is not suited for invoking this function multiple times.
 */
//static GdvTic **
static void
_determine_tics_by_values(GList *tics_list,
                          gsize no_of_vals,
                          gdouble tics_val[],
                          GdvTic *tics[],
                          struct _linear_axis_scale_definition *scale)
{
//  GdvTic *returned_array[] = (GdvTic *) g_new0(gpointer, no_of_vals);
  GList * copy_list;

  for (copy_list = tics_list;
       copy_list != NULL;
       copy_list = copy_list->next)
  {
    GdvTic *local_tic = GDV_TIC(copy_list->data);
    gdouble local_tic_value;
    gsize i;

    g_object_get (local_tic, "value", &local_tic_value, NULL);

    for (i = 0; i < no_of_vals; i++)
    {
      if (comp_equal_values(local_tic_value, tics_val[i],
                            scale) == TRUE)
        tics[i] = local_tic;
    }

  }
}

/* Function that is getting the maximum necessary space for any of the given Tics in the
 * list of GdvTics in the given direcation. */
static gint _determine_max_border_on_list (GtkPositionType dir, GList *tics_list)
{
  gint max_border = 0;

  for (GList *list_copy = tics_list;
       list_copy != NULL;
       list_copy = list_copy->next)
    {
      GdvTic * tic = list_copy->data;
      gint tmp_min, tmp_nat;

      gdv_tic_get_space_to_tic_position (
        GDV_TIC (tic), dir, -1, &tmp_min, &tmp_nat, NULL);
      max_border = (max_border > tmp_nat ? max_border : tmp_nat);
      max_border = (tmp_min > max_border ? tmp_min : max_border);
    }

  return max_border;
}

/* Function that simplyfies the setting of a new value to a newly created GdvTic. This involves
 * the according properties as well as the markup. */
static void
_set_value_on_pristine_tic (GdvLinearAxis *linear_axis, GdvTic *tic,
                            gdouble tic_val, gboolean visible)
{
  gchar *new_tic_label;

  g_object_set (G_OBJECT (tic),
                "value", tic_val,
                "visible", visible,
                NULL);
  new_tic_label =
    GDV_AXIS_GET_CLASS (GDV_AXIS (linear_axis))->make_tic_label_markup (
      GDV_AXIS (linear_axis), tic_val);
  gdv_tic_label_set_markup (tic, new_tic_label);
  g_free (new_tic_label);
}

/* Function that overwrites the size_allocate-method of the GtkWidget parent class */
static void
gdv_linear_axis_size_allocate (GtkWidget     *widget,
                               GtkAllocation *allocation)
{
  /* FIXME: This is necessary to apply splint on the code; maybe it is possible
   *        to run a newer version of splint over the code somewhere without
   *        the crashes I got from Ver3.1.2-4 from the AUR
   */
#ifndef S_SPLINT_S

  GdvLinearAxis *linear_axis = GDV_LINEAR_AXIS (widget);
  GtkAllocation axis_allocation = *allocation;

  gboolean scale_automatic;

  gboolean scale_auto_increment;
  gdouble scale_beg_val, scale_end_val;
  gdouble scale_increment_val;
  gdouble signed_scale_increment_val;
  gdouble scale_increment_base;
  gdouble init_scale_beg_val, init_scale_end_val;

  gint scale_min_diff_pix, scale_max_diff_pix;
  gboolean first_iteration = TRUE;

  gdouble angle_to_outer_dir, angle_to_start;

  gdouble init_increment_val;
//  gdouble   init_tic_beg_val, init_tic_end_val;

  gdouble inner_dir_x = 0.0, inner_dir_y = 0.0;
//  gdouble   axis_dir_x = 0.0, axis_dir_y = 0.0;

  gboolean force_beg_end;

  gboolean tics_automatic;
  gdouble tics_beg_val, tics_end_val;
  gboolean beg_is_new = FALSE, end_is_new = FALSE;

  gdouble tic_label_halign, tic_label_valign;

  gboolean mtics_automatic;
  gdouble mtics_beg_val, mtics_end_val;
  guint mtics_number;
  gdouble current_diff_pix = G_MAXDOUBLE;
  gint max_top_border, max_bot_border, max_left_border, max_right_border;

  gboolean set_tics = TRUE;

  GList *tics_copy, *previouse_tics;
  GList *tics_approved_list = NULL;
  GList *mtics_copy, *previouse_mtics;
  gdouble actual_pos_val;

  gdouble line_height, line_width, line_length;

  GtkWidget *title_widget;
  gboolean axis_title_on;
  gint title_height = 0, title_width = 0;
  gint title_height_nat = 0, title_width_nat = 0;
  GtkAllocation title_allocation;

  /* needed for the autoadjustment of scale- and tic-values */
  gdouble mantissa = 1.0; /* 2, 4, 5, 10, 20, 40, 50, 100, ..... */
  gdouble exponent = 0.0; /* 1, 2, 3, 4, 5, 6,...*/
  gdouble sign = -1.0; /* -1.0, +1.0 */
  GdvTic *beg_tic = NULL, *end_tic = NULL;
  GList *tic_list, *tic_list_start;
  GtkAllocation space_without_border;

  /* final begin- and end-values of the axis */
  gdouble scale_beg_x, scale_beg_y;
  gdouble scale_end_x, scale_end_y;

  gboolean visible;

  /* Some annotations:
   *   The allocation process is the point where axes determine their
   *   available space for the tics. This process can be controlled by
   *   the user/developer with the public widget properties. The process of
   *   automatic tic-setting is rather complex. Therefore a short sketch will
   *   illustrate the several steps:
   *  I.    Receive the properties and starting values
   *  II.   Determine the begin and end of the scale and produce first
   *        temporary tics for just these two values
   *  III.  Iterating a loop and measure the available tics. Start with the
   *        temporary begin- and end-tics
   *  IV.   If the remaining space is large enough, add more tics and make the
   *        scale more detailed and repeat the measurement process
   *  V.    If the remaining space is too small, make the scale less detailed by
   *        using larger increment values and repeat the measurement process
   *        if the abortion criteria is met, the other tics will be added/removed
   *        from the axis and their position will be calculated and allocated
   *  VI.   It is definitely necessary to redetermine the space needed by the axis
   *        after this process, due to the newly added tics. This is made by the
   *        internal resize-during-redraw property
   *  VII.  We assume that the process converges after the first iteration
   */

  /*  I.    Receive the properties and starting values */
  g_debug ("ALLOC WITH: %d, %d, %d, %d",
           allocation->x, allocation->y, allocation->width, allocation->height);

  /* setting the allocation at the real beginning */
  gtk_widget_set_allocation (widget, &axis_allocation);

  /* Receiving all the starting values */
  g_object_get (
    G_OBJECT (linear_axis),
    "scale-beg-val", &scale_beg_val,
    "scale-end-val", &scale_end_val,
    "scale-auto-increment", &scale_auto_increment,
    "scale-limits-automatic", &scale_automatic,
    "scale-increment-val", &scale_increment_val,
    "axis-direction-outside", &angle_to_outer_dir,
    "axis-orientation", &angle_to_start,
    "tics-beg-val", &tics_beg_val,
    "tics-end-val", &tics_end_val,
    "tics-automatic", &tics_automatic,
    "mtics-beg-val", &mtics_beg_val,
    "mtics-end-val", &mtics_end_val,
    "mtics-automatic", &mtics_automatic,
    "mtics", &mtics_number,
    "title-widget", &title_widget,
    "scale-increment-base", &scale_increment_base,
    "force-beg-end", &force_beg_end,
    "visible", &visible,
    NULL);

  /* Inspecting the starting values */
  g_debug ("BEGW SCALE: fr %e to %e TICS: fr %e to %e SCIB %e SCIV %e",
           scale_beg_val, scale_end_val, tics_beg_val, tics_end_val,
           scale_increment_base, scale_increment_val);

  /* safing all necessary values */
  init_increment_val = scale_increment_val;
  init_scale_beg_val = scale_beg_val;
  init_scale_end_val = scale_end_val;

  /* TODO: extensively test the max and min diff! */
  gtk_widget_style_get (
    widget,
    "show-title", &axis_title_on,
    "scale-min-diff-pix", &scale_min_diff_pix,
    "scale-max-diff-pix", &scale_max_diff_pix,
    NULL);

  /* testing for tilte-visibility */
  axis_title_on =
    axis_title_on &&
    title_widget &&
    gtk_widget_get_visible (title_widget);

  /* calculating basic geometric values */
  inner_dir_x = -1.0 * sin (angle_to_outer_dir);
  inner_dir_y = cos (angle_to_outer_dir);

  tic_label_halign = -0.5 * fabs (inner_dir_y);
  tic_label_valign = -0.5 * fabs (inner_dir_x);

  /* Check the initial boundaries and set flag for skipping a lot
   *   of functions
   * FIXME: it might be a faster algorithm to implement the inverse case or even use a
   *        bisect-search */
  set_tics =
    (tics_automatic || /* FIXME: potentially wrong! */
     !((tics_beg_val <= fmin (scale_beg_val, scale_end_val) &&
        tics_end_val <= fmin (scale_beg_val, scale_end_val)) ||
       (tics_beg_val >= fmax (scale_beg_val, scale_end_val) &&
        tics_end_val >= fmax (scale_beg_val, scale_end_val))));

  /*  II.   Determine the begin and end of the scale and produce first
   *        temporary tics for just these two values */
  if (set_tics)
  {
    /* looking for the beg- and end-tic */
    _determine_tic_by_val (linear_axis, tics_beg_val, &beg_tic);
    _determine_tic_by_val (linear_axis, tics_end_val, &end_tic);

    /* if not present, make new beg- and end-tics */
    if (!beg_tic)
    {
      beg_tic = g_object_new (GDV_TYPE_TIC, NULL);
      gtk_container_add (GTK_CONTAINER (linear_axis), GTK_WIDGET (beg_tic));
      beg_is_new = TRUE;
    }

    g_object_set (beg_tic,
                  "axis-inner-dir-x", inner_dir_x,
                  "axis-inner-dir-y", inner_dir_y,
                  "label-xalign", tic_label_halign,
                  "label-yalign", tic_label_valign,
                  NULL);

    if (!end_tic)
    {
      end_tic = g_object_new (GDV_TYPE_TIC, NULL);
      gtk_container_add (GTK_CONTAINER (linear_axis), GTK_WIDGET (end_tic));
      end_is_new = TRUE;
    }

    g_object_set (end_tic,
                  "axis-inner-dir-x", inner_dir_x,
                  "axis-inner-dir-y", inner_dir_y,
                  "label-xalign", tic_label_halign,
                  "label-yalign", tic_label_valign,
                  NULL);
  }

  g_log (NULL, G_LOG_LEVEL_DEBUG, "START INCREMENT_DET-LOOP");

  /*  III.  Iterating a loop and measure the available tics. Start with the
   *        temporary begin- and end-tics
   * This will optimise the scale-parameters, until scale-min-diff-pix are
   * reached */
  while (set_tics)
  {
    /* negative sign -> exponent grows, positive sign vice versa */
    gdouble base_value =
      pow (10.0, exponent) * (sign < 0.0 ? mantissa : 1.0 / mantissa);
    gdouble down_scaled_difference = 0;
    gdouble down_scaled_beg, down_scaled_end;
    gdouble scale_length;

    /* catch the trivial case (scale_end_val == scale_beg_val) */
    scale_length = scale_end_val - scale_beg_val;
    if (scale_length != 0)
      down_scaled_difference =
          base_value *
          pow (scale_increment_base,
               floor (
                 log (fabs (scale_length)) /
                 log (scale_increment_base)
                     ));

    g_debug ("\tLOOP: DSD %e BAV %e SCALE: fr %e to %e",
             down_scaled_difference, base_value, scale_beg_val, scale_end_val);
    g_debug ("\tMORE MAGEIC: FAC %e SIB %e",
             sign < 0.0 ? floor (
               log (fabs (scale_length)) /
               log (scale_increment_base)) : ceil (
               log (fabs (scale_length)) /
               log (scale_increment_base)
                  ), scale_increment_base);
    g_debug ("\tBV MAGIC: SGN %e MTSS %e EXPO %e", sign, mantissa, exponent);

    /* FIXME: This is not usefull! */
    if (isnan (down_scaled_difference))
    {
      g_warning ("Unhandled problem with the linear-axis, this should be reworked internally");
      return;
    }

    if (scale_auto_increment)
    {
      scale_increment_val = down_scaled_difference;
      signed_scale_increment_val =
        scale_increment_val * (scale_beg_val < scale_end_val ? 1.0 : -1.0);
    }

    /* Calculating the scale begin- and end-values */
    if ((scale_automatic && (scale_beg_val <= scale_end_val)) ||
        (!scale_automatic && (scale_beg_val > scale_end_val)))
    {
      scale_beg_val =
        (gdouble)floor (init_scale_beg_val / scale_increment_val);
      scale_end_val =
        (gdouble)ceil (init_scale_end_val / scale_increment_val);
    }
    else
    {
      scale_beg_val =
        (gdouble)ceil (init_scale_beg_val / scale_increment_val);
      scale_end_val =
        (gdouble)floor (init_scale_end_val / scale_increment_val);
    }

    scale_beg_val *= scale_increment_val;
    scale_end_val *= scale_increment_val;

    /* Scaling the begin and end */
    if (scale_beg_val <= scale_end_val)
    {
      down_scaled_beg =
        down_scaled_difference *
        (gdouble)floor (scale_beg_val / down_scaled_difference);
      down_scaled_end =
        down_scaled_difference *
        (gdouble)ceil (scale_end_val / down_scaled_difference);
    }
    else
    {
      down_scaled_beg =
        down_scaled_difference *
        (gdouble)ceil (scale_beg_val / down_scaled_difference);
      down_scaled_end =
        down_scaled_difference *
        (gdouble)floor (scale_end_val / down_scaled_difference);
    }

    /* This Output could be used to debug the retic-problem */
    g_debug("\tSET SCV DONWSCALED fr %e to %e ; SCALE fr %e to %e ; DSD %e SIV %e",
            down_scaled_beg, down_scaled_end, scale_beg_val, scale_end_val,
            down_scaled_difference, scale_increment_val);

    if (scale_automatic && scale_auto_increment)
    {
      scale_beg_val = down_scaled_beg;
      scale_end_val = down_scaled_end;
    }

    if (tics_automatic)
    {
      tics_beg_val = scale_beg_val;
      tics_end_val = scale_end_val;
      g_debug("\tRecalculated Scale-Values %e %e Tics-Values %e %e",
              scale_beg_val, scale_end_val,
              tics_beg_val, tics_end_val);
    }

    /* set the beg/end-tic to the new value */
    /* FIXME: This constrains the geometry-handling here... maybe there is a
     *        more robust solution in the future
     */
    if (beg_is_new)
      _set_value_on_pristine_tic (linear_axis, beg_tic, tics_beg_val, visible);

    if (end_is_new)
      _set_value_on_pristine_tic (linear_axis, end_tic, tics_end_val, visible);

    /*  IV.   If the remaining space is large enough, add more tics and make the
     *        scale more detailed and repeat the measurement process
     * lookup the space that the beg- and end-tic need
     * Please remind: this is not looking for the very narrowest solution.
     *                It's just a good way to organise the tics at minimum
     *                cost's
     */
    if (force_beg_end)
    {
      gdouble rel_beg_x, rel_beg_y, rel_end_x, rel_end_y;

      /* FIXME: This whole setup here! */

      g_object_get (G_OBJECT (linear_axis),
                    "axis-beg-pix-x", &rel_beg_x,
                    "axis-beg-pix-y", &rel_beg_y,
                    "axis-end-pix-x", &rel_end_x,
                    "axis-end-pix-y", &rel_end_y,
                    NULL);

      space_without_border.x = (gint)(rel_beg_x);
      space_without_border.y = (gint)(rel_beg_y);
      space_without_border.width =
        (gint)(fmin (rel_end_x - rel_beg_x, 1.0));
      space_without_border.height =
        (gint)(fmin (rel_end_y - rel_beg_y, 1.0));
    }
    else
    {
      GList beg_tic_list_element = {.data = beg_tic, .prev = NULL};
      GList end_tic_list_element = {.data = end_tic, .next = NULL,
                                    .prev = &beg_tic_list_element};
      beg_tic_list_element.next = &end_tic_list_element;

      /* calculating the borders */
      max_top_border = _determine_max_border_on_list(GTK_POS_TOP, &beg_tic_list_element);
      max_bot_border = _determine_max_border_on_list(GTK_POS_BOTTOM, &beg_tic_list_element);
      max_left_border = _determine_max_border_on_list(GTK_POS_LEFT, &beg_tic_list_element);
      max_right_border = _determine_max_border_on_list(GTK_POS_RIGHT, &beg_tic_list_element);

      /* setting the free space */
      space_without_border.x = max_left_border;
      space_without_border.y = max_top_border;
      space_without_border.width = allocation->width - max_left_border - max_right_border;
      space_without_border.height = allocation->height - max_top_border - max_bot_border;

      /* measuring title */
      if (axis_title_on)
      {
        gtk_widget_get_preferred_width (
          title_widget, &title_width, &title_width_nat);
        gtk_widget_get_preferred_height (
          title_widget, &title_height, &title_height_nat);

        space_without_border.width -=
          (gint)(fabs (sin (angle_to_outer_dir)) * (gdouble)title_width);
        space_without_border.height -=
          (gint)(fabs (cos (angle_to_outer_dir)) * (gdouble)title_height);

        if (sin (angle_to_outer_dir) < 0.0)
          space_without_border.x +=
            (gint)(-1.0 * sin (angle_to_outer_dir) * (gdouble)title_width);

        if (cos (angle_to_outer_dir) >= 0.0)
          space_without_border.y +=
            (gint)(+1.0 * cos (angle_to_outer_dir) * (gdouble)title_height);
      }

      /* binning to zero */
      space_without_border.width =
        space_without_border.width < 0 ? 0 : space_without_border.width;
      space_without_border.height =
        space_without_border.height < 0 ? 0 : space_without_border.height;
    }

    /* FIXME: This must be adapted, if we manage the geometry-handling by the
     * layer
     */
    line_height =
      fmin (fabs (tan (0.5 * M_PI - angle_to_start)) *
            (gdouble)(space_without_border.width + 0.5),
            (gdouble)space_without_border.height);
    line_width =
      fmin (fabs (tan (angle_to_start)) *
            (gdouble)(space_without_border.height + 0.5),
            (gdouble)space_without_border.width);
    line_length = sqrt (line_width * line_width + line_height * line_height);

    /* compare difference to scale_min_diff_pix */
    scale_length = fabs (scale_end_val - scale_beg_val);
    current_diff_pix = line_length * scale_increment_val / scale_length;

    g_log (NULL, G_LOG_LEVEL_DEBUG,
           "\tREACHPOS W CDP %f SID %d SAD %d",
           current_diff_pix, scale_min_diff_pix, scale_max_diff_pix);

   /*  V.    If the remaining space is too small, make the scale less detailed by
    *        using larger increment values and repeat the measurement process
    *        if the abortion criteria is met, the other tics wwill be added/removed
    *        from the axis and their position will be calculated and allocated
    * aborting criteria - optimisation successful */
    if (((current_diff_pix < (gdouble)scale_min_diff_pix &&
          sign > 0.0) ||
         (current_diff_pix > (gdouble)scale_max_diff_pix &&
          sign < 0.0)) &&
        !first_iteration)
      break;
    /* autoscaling successfull; setting all parameters to the axis */
    else if ((current_diff_pix >= (gdouble)scale_min_diff_pix &&
              current_diff_pix <= (gdouble)scale_max_diff_pix))
    {
      g_log (NULL, G_LOG_LEVEL_DEBUG,"\tSUCC SET");

      if (scale_auto_increment)
        g_object_set (G_OBJECT (linear_axis),
                      "scale-increment-val", scale_increment_val,
                      NULL);

      if (scale_automatic)
        g_object_set (G_OBJECT (linear_axis),
                      "scale-beg-val", scale_beg_val,
                      "scale-end-val", scale_end_val,
                      NULL);

      if (tics_automatic)
        g_object_set (G_OBJECT (linear_axis),
                      "tics-beg-val", tics_beg_val,
                      "tics-end-val", tics_end_val,
                      NULL);

      /* this is a nasty hack to prevent wrong settings for a correct
       * solution */
      first_iteration = FALSE;
    }


    g_log (NULL, G_LOG_LEVEL_DEBUG,
           "\tREACH ITER COND CDP %e DSD %e DOWN_SCALE: BEG %e END %e",
           current_diff_pix, down_scaled_difference,
           down_scaled_beg, down_scaled_end);
    g_log (NULL, G_LOG_LEVEL_DEBUG,
           "\tSIGN %e", sign);

//      if (first_iteration && sign > 0.0)
  //      {
  //        first_iteration = FALSE;
  //    g_print ("FIRST ITER CONT\n");
  //        continue;
  //      }
  //    else

    /* abort if the style-property cannot be reached */
    /* FIXME: the statement in the last line implies, that there will be no
     * changes to scale_increment_val under the conditions, even if necessary!
     * Thats unlikeliy to be correct - however, it is a very fast fix to the
     * issue.
     */
    if (((down_scaled_difference == fabs (down_scaled_end - down_scaled_beg)) &&
         ((current_diff_pix < (gdouble)scale_min_diff_pix))) ||
          (!first_iteration && sign < 0.0) ||
//          (!first_iteration && sign < 0.0 && (last_diff_pix == current_diff_pix)) ||
//         ((current_diff_pix > (gdouble)scale_max_diff_pix) ||
//          (!first_iteration && sign < 0.0)) ||
        !scale_auto_increment || force_beg_end || !scale_automatic)
//    if (current_diff_pix < (gdouble)scale_min_diff_pix)
    {
      g_log (NULL, G_LOG_LEVEL_DEBUG,
             "SUCC2 SET SCALE BEG %e END %e SIV %e",
             scale_beg_val, scale_end_val, scale_increment_val);

//    } if (FALSE) {
      if (scale_auto_increment)
        g_object_set (G_OBJECT (linear_axis),
                      "scale-increment-val", scale_increment_val,
                      NULL);

      if (scale_automatic)
        g_object_set (G_OBJECT (linear_axis),
                      "scale-beg-val", scale_beg_val,
                      "scale-end-val", scale_end_val,
                      NULL);

      if (tics_automatic)
        g_object_set (G_OBJECT (linear_axis),
                      "tics-beg-val", tics_beg_val,
                      "tics-end-val", tics_end_val,
                      NULL);

      /* FIXME: This should be warning usually. Maybe add scale_automatic */
      if (gtk_widget_get_realized (widget) &&
          !scale_auto_increment &&
          (current_diff_pix < (gdouble)scale_min_diff_pix ||
           current_diff_pix > (gdouble)scale_max_diff_pix))
        g_message ("Style-property GdvAxis::scale-min-diff-pix or "
                   "GdvAxis::scale-max-diff-pix cannot be fullfilled. "
                   "Reconsider change of automatic axis-properties");

      break;
    }
//    last_diff_pix = current_diff_pix;

//    g_print ("\tSET NEW mnt&exp - OLD: MNT %e EXP %e\n", mantissa, exponent);

    /* setting the sign-value */
    if (first_iteration &&
        current_diff_pix <= (gdouble)scale_min_diff_pix)
      sign = -1.0;
    else if (first_iteration)
    {
      sign = 1.0;
    }

    /* preparing the next base value */
    if (mantissa == 1.0)
      mantissa = 2.0;
    else if (mantissa == 2.0)
      mantissa = 4.0;
    else if (mantissa == 4.0)
      mantissa = 5.0;
    else if (mantissa == 5.0)
    {
      mantissa = 1.0;

      if (sign < 0.0)
        exponent++;
      else
        exponent--;
    }

    first_iteration = FALSE;
  }

  /* Restoring properties after break condition */
  g_object_get (G_OBJECT (linear_axis),
                "scale-increment-val", &scale_increment_val,
                NULL);
  g_object_get (G_OBJECT (linear_axis),
                "scale-beg-val", &scale_beg_val,
                "scale-end-val", &scale_end_val,
                NULL);
  g_object_get (G_OBJECT (linear_axis),
                "tics-beg-val", &tics_beg_val,
                "tics-end-val", &tics_end_val,
                NULL);

  g_log (NULL, G_LOG_LEVEL_DEBUG,
         "!!!FIN WITH SC %.8e %.8e TI %e %e SIV %e",
         scale_beg_val, scale_end_val,
         tics_beg_val, tics_end_val,
         scale_increment_val);

  if (set_tics)
  {
    gdouble tmp_tics_val_beg, tmp_tics_val_end;

    /* Just in case the scale is different from last settings */
    g_object_get (G_OBJECT (beg_tic),
                  "value", &tmp_tics_val_beg,
                  NULL);

    if (init_increment_val != scale_increment_val ||
        tmp_tics_val_beg != tics_beg_val)
      _set_value_on_pristine_tic (linear_axis, beg_tic, tics_beg_val, visible);

    g_object_get (G_OBJECT (end_tic),
                  "value", &tmp_tics_val_end,
                  NULL);

    if (init_increment_val != scale_increment_val ||
        tmp_tics_val_end != tics_end_val)
      _set_value_on_pristine_tic (linear_axis, end_tic, tics_end_val, visible);
  }

  signed_scale_increment_val =
    scale_increment_val * (scale_beg_val < scale_end_val ? 1.0 : -1.0);

  /* Determining the number of mtics per interval
   * (will adjust automatically to the base)
   */
  if (mtics_automatic && set_tics)
  {
    mtics_beg_val = scale_beg_val;
    mtics_end_val = scale_end_val;

    exponent = floor (log (scale_increment_val) / log (scale_increment_base));
    mantissa = scale_increment_val / pow (scale_increment_base, exponent);

    /* This is just a best practice, if a different engine of automatism is
     * required, it should be implemented by an own class-type.
     */
    if (mantissa == 2.5 || mantissa == 5.0)
      mtics_number = 4;
    else if (mantissa == 4.0 || mantissa == 2.0)
      mtics_number = 3;
    else if (mantissa == 1.0)
      mtics_number = 9;
    else
      /* FIXME: make this somehow usefull! */
      mtics_number = 3;

    g_object_set (G_OBJECT (linear_axis),
                  "mtics", mtics_number,
                  "mtics-beg-val", mtics_beg_val,
                  "mtics-end-val", mtics_end_val,
                  NULL);
  }

  /*
   * FIXME: This should check for any doubled values!
   * check and delete all unnecessary tics of the axis-instance!
   */
  previouse_tics = gdv_axis_get_tic_list (GDV_AXIS (linear_axis));
  tics_copy = previouse_tics;

  while (tics_copy)
  {
    gdouble local_tic_value;
    gdouble local_val_resid;

    g_object_get (tics_copy->data, "value", &local_tic_value, NULL);
    local_val_resid =
      fmod ((local_tic_value - tics_beg_val) / signed_scale_increment_val, 1.0);

    if (local_tic_value < fmin (tics_beg_val, tics_end_val) ||
        local_tic_value < fmin (scale_beg_val, scale_end_val) ||
        local_tic_value > fmax (tics_beg_val, tics_end_val) ||
        local_tic_value > fmax (scale_beg_val, scale_end_val) ||
        !((fabs(local_val_resid) * scale_increment_val < 1e-10) ||
          (fabs(1.0 - local_val_resid) * scale_increment_val < 1e-10)) ||
        _is_present_in_approved_list (tics_copy->data, tics_approved_list))
      /* TODO: separate the round-off problems to own function */
      gtk_container_remove (GTK_CONTAINER (linear_axis), tics_copy->data);
    else
      tics_approved_list = g_list_append (tics_approved_list, tics_copy->data);

    tics_copy = tics_copy->next;
  }

  g_list_free (previouse_tics);
  g_list_free (tics_approved_list);
  tics_approved_list = NULL;

  /*
   * FIXME: See previous comment!
   * check and delete all unnecessary mtics of the axis-instance!
   */
  previouse_mtics = gdv_axis_get_mtic_list (GDV_AXIS (linear_axis));
  mtics_copy = previouse_mtics;

  while (mtics_copy)
  {
    gdouble local_mtic_value;
    gdouble local_val_resid, local_mtic_resid;

    g_object_get (mtics_copy->data, "value", &local_mtic_value, NULL);
    local_val_resid =
      fmod (local_mtic_value - tics_beg_val, signed_scale_increment_val);
    local_mtic_resid =
      fmod (local_val_resid / (scale_increment_val / (mtics_number + 1)), 1.0);

    if (local_mtic_value < fmin (mtics_beg_val, mtics_end_val) ||
        local_mtic_value < fmin (scale_beg_val, scale_end_val) ||
        local_mtic_value > fmax (mtics_beg_val, mtics_end_val) ||
        local_mtic_value > fmax (scale_beg_val, scale_end_val) ||
        !((fabs (local_mtic_resid * scale_increment_val) < 1e-9) ||
          (fabs((1.0 - local_mtic_resid) * scale_increment_val) < 1e-9)) ||
        fabs (local_val_resid / scale_increment_val) < 1e-10 ||
        fabs ((scale_increment_val - local_val_resid) /
              scale_increment_val) < 1e-10 ||
        _is_present_in_approved_list (mtics_copy->data, tics_approved_list))
      /* round-off problems */
      gtk_container_remove (GTK_CONTAINER (linear_axis), mtics_copy->data);
    else
      tics_approved_list = g_list_append (tics_approved_list, mtics_copy->data);

    mtics_copy = mtics_copy->next;
  }

  g_list_free (previouse_mtics);
  g_list_free (tics_approved_list);
  tics_approved_list = NULL;

  /* Setting the mtics before the actual scale begins */
  actual_pos_val = tics_beg_val - signed_scale_increment_val;

  if (set_tics)
  {
    gint i;

    /* Setting the mtics */
    previouse_mtics = gdv_axis_get_mtic_list (GDV_AXIS (linear_axis));

    for (i = 1; i < mtics_number + 1; i++)
    {
      GdvTic *local_mtic = NULL;
      gdouble local_mtic_val =
        actual_pos_val + i * signed_scale_increment_val / (mtics_number + 1);

      /* get the correct mtic and check and delete all unnecessary mtics of
       * the axis-instance!
       */
      mtics_copy = previouse_mtics;

      while (mtics_copy)
      {
        gdouble local_mtic_value;

        gdouble local_val_resid;

        /* floating-point precision struggle */
        g_object_get (mtics_copy->data, "value", &local_mtic_value, NULL);

        local_val_resid =
          (local_mtic_value - local_mtic_val) / scale_increment_val;

        if (fabs(local_val_resid) < 1e-10)
        {
          local_mtic = mtics_copy->data;
          break;
        }

        mtics_copy = mtics_copy->next;
      }

      /* we are not discussing about one pixel again ! */
      if (
        local_mtic_val <= fmax (mtics_end_val, mtics_beg_val) &&
        local_mtic_val <= fmax (scale_beg_val, scale_end_val) &&
        local_mtic_val >= fmin (mtics_end_val, mtics_beg_val) &&
        local_mtic_val >= fmin (scale_beg_val, scale_end_val))
      {
        if (!local_mtic)
        {
          local_mtic =
            g_object_new (gdv_mtic_get_type (),
                          "value", local_mtic_val,
                          "visible", visible,
                          NULL);
          gtk_container_add (
            GTK_CONTAINER (linear_axis), GTK_WIDGET (local_mtic));
        }
      }
      else if (local_mtic)
        gtk_container_remove (GTK_CONTAINER (linear_axis),
                              GTK_WIDGET (local_mtic));
    }

    g_list_free (previouse_mtics);
  }

  actual_pos_val = tics_beg_val;

  /* adding axis-tics */
  while (actual_pos_val <= fmax (tics_end_val, tics_beg_val) &&
         actual_pos_val <= fmax (scale_beg_val, scale_end_val) &&
         actual_pos_val >= fmin (tics_end_val, tics_beg_val) &&
         actual_pos_val >= fmin (scale_beg_val, scale_end_val) &&
         set_tics)
  {
    GdvTic *local_tic = NULL;
    gint i;

    /* get the correct mtic and check and delete all unnecessary tics of
     * the axis-instance!
     */
    previouse_tics = gdv_axis_get_tic_list (GDV_AXIS (linear_axis));
    tics_copy = previouse_tics;

    while (tics_copy)
    {
      gdouble local_tic_value;
      gdouble local_val_resid;

      /* floating-point precision struggle */
      g_object_get (tics_copy->data, "value", &local_tic_value, NULL);

      local_val_resid =
        fabs (actual_pos_val - local_tic_value) / scale_increment_val;

      if (fabs(local_val_resid) < 1e-10)
      {
        local_tic = tics_copy->data;
        break;
      }

      tics_copy = tics_copy->next;
    }

    g_list_free (previouse_tics);

    if (!local_tic)
    {
      local_tic =
        g_object_new (gdv_tic_get_type (),
                      NULL);
      _set_value_on_pristine_tic (linear_axis, local_tic, actual_pos_val, visible);
      gtk_container_add (GTK_CONTAINER (linear_axis), GTK_WIDGET (local_tic));
    }

    /* Setting the mtics */
    previouse_mtics = gdv_axis_get_mtic_list (GDV_AXIS (linear_axis));

    for (i = 1; i < mtics_number + 1; i++)
    {
      GdvTic *local_mtic = NULL;
      gdouble local_mtic_val =
        actual_pos_val + i * signed_scale_increment_val / (mtics_number + 1);

      /* get the correct mtic and check and delete all unnecessary mtics of
       * the axis-instance!
       */
      mtics_copy = previouse_mtics;

      while (mtics_copy)
      {
        gdouble local_mtic_value;

        gdouble local_val_resid;

        /* floating-point precision struggle */
        g_object_get (mtics_copy->data, "value", &local_mtic_value, NULL);

        local_val_resid =
          (local_mtic_value - local_mtic_val) / scale_increment_val;

        if (fabs(local_val_resid) < 1e-10)
        {
          local_mtic = mtics_copy->data;
          break;
        }

        mtics_copy = mtics_copy->next;
      }

      /* we are not discussing about one pixel again ! */
      if (
        local_mtic_val <= fmax (mtics_end_val, mtics_beg_val) &&
        local_mtic_val <= fmax (scale_beg_val, scale_end_val) &&
        local_mtic_val >= fmin (mtics_end_val, mtics_beg_val) &&
        local_mtic_val >= fmin (scale_beg_val, scale_end_val))
      {
        if (!local_mtic)
        {
          g_log (NULL, G_LOG_LEVEL_DEBUG,
                 "New MTIC VALUE: %e", local_mtic_val);

          local_mtic =
            g_object_new (gdv_mtic_get_type (),
                          "value", local_mtic_val,
                          "visible", visible,
                          NULL);
          gtk_container_add (
            GTK_CONTAINER (linear_axis), GTK_WIDGET (local_mtic));
        }
      }
      else if (local_mtic)
        gtk_container_remove (GTK_CONTAINER (linear_axis),
                              GTK_WIDGET (local_mtic));
    }

    g_list_free (previouse_mtics);

    /* stepping to next scale-point */
    actual_pos_val += signed_scale_increment_val;
  }

  /*  VI.   Make the final measurement of all tics for approval. */
  if (!force_beg_end)
  {
    previouse_tics = gdv_axis_get_tic_list (GDV_AXIS (linear_axis));

    /* lookup the space that the beg- and end-tic need
     * Please remind: this is not about looking for the very narrowest
     *                solution. It's just a good way to organise the tics
     *                at minimum cost's
     */
    max_top_border = _determine_max_border_on_list(GTK_POS_TOP, previouse_tics);
    max_bot_border = _determine_max_border_on_list(GTK_POS_BOTTOM, previouse_tics);
    max_left_border = _determine_max_border_on_list(GTK_POS_LEFT, previouse_tics);
    max_right_border = _determine_max_border_on_list(GTK_POS_RIGHT, previouse_tics);

    g_list_free (previouse_tics);
  } else {
    max_top_border = 0;
    max_bot_border = 0;
    max_left_border = 0;
    max_right_border = 0;
  }

  /* calculating the free space */
  space_without_border.x = (gint) (
    allocation->width > max_left_border + max_right_border ? max_left_border :
//    0.5 * allocation->width - 0.5 * (max_left_border + max_right_border));
    allocation->width * max_left_border / (max_left_border + max_right_border));
  space_without_border.y =
    allocation->height > max_top_border + max_bot_border ? max_top_border :
    allocation->height * max_top_border / (max_top_border + max_bot_border);
  space_without_border.width =
    allocation->width > max_left_border + max_right_border ?
    allocation->width - max_left_border - max_right_border : 1;
  space_without_border.height =
    allocation->height > max_top_border + max_bot_border ?
    allocation->height - max_top_border - max_bot_border : 1;

  /* binning to zero */
  space_without_border.width =
    space_without_border.width < 0 ? 0 : space_without_border.width;
  space_without_border.height =
    space_without_border.height < 0 ? 0 : space_without_border.height;

  /* allocating the title */
  if (axis_title_on)
  {
    title_allocation.width = title_width;
    title_allocation.height = title_height;

    gtk_widget_get_preferred_width (
      title_widget, &title_width, &title_width_nat);
    gtk_widget_get_preferred_height (
      title_widget, &title_height, &title_height_nat);

    if (!force_beg_end)
    {

      if (allocation->width > max_left_border + max_right_border)
      {
        space_without_border.width -=
          (gint)(fabs (sin (angle_to_outer_dir)) * (gdouble)title_width);
        if (sin (angle_to_outer_dir) < 0.0)
          space_without_border.x +=
            (gint)(-1.0 * sin (angle_to_outer_dir) * (gdouble)title_width);
      }

      if (allocation->height > max_top_border + max_bot_border)
      {
        space_without_border.height -=
          (gint)(fabs (cos (angle_to_outer_dir)) * (gdouble)title_height);

        if (cos (angle_to_outer_dir) >= 0.0)
          space_without_border.y +=
            (gint)(+1.0 * cos (angle_to_outer_dir) * (gdouble)title_height);
      }
    }

    /* FIXME: This is not the function I intented */
    title_allocation.x =
      allocation->x + space_without_border.x - max_left_border - title_width +
      (gint)((0.5 + 0.5 * sin (angle_to_outer_dir)) *
             (gdouble)(
               space_without_border.width +
               max_right_border +
               max_left_border +
               title_width));

    title_allocation.y =
      allocation->y + space_without_border.y - max_top_border - title_height +
      (gint)((0.5 - 0.5 * cos (angle_to_outer_dir)) *
             (gdouble)(
               space_without_border.height +
               max_bot_border +
               max_top_border +
               title_height));

    gtk_widget_size_allocate (
      GTK_WIDGET (title_widget), &title_allocation);
  }

  /*  VII.  We assume that the process converges after the first iteration and store the
   *        new values */
  if (!force_beg_end)
  {
    /* FIXME: this should be guarded, to ensure correct setting by layer */
    scale_beg_x =
      fmax (fmin (
              (sin (angle_to_start) / fabs (cos (angle_to_start))) *
              0.5 * ((gdouble)space_without_border.height + 0.5) +
              (gdouble)(space_without_border.x +
                        0.5 * space_without_border.width),
              (gdouble)(space_without_border.x + space_without_border.width)),
            (gdouble)space_without_border.x);
    scale_end_x =
      fmax (fmin (
              (-1.0 * sin (angle_to_start) / fabs (cos (angle_to_start))) *
              0.5 * ((gdouble)space_without_border.height + 0.5) +
              (gdouble)(space_without_border.x +
                        0.5 * space_without_border.width),
              (gdouble)(space_without_border.x + space_without_border.width)),
            (gdouble)space_without_border.x);

    scale_beg_y =
      fmax (fmin (
              (-1.0 * cos (angle_to_start) / fabs (sin (angle_to_start))) *
              0.5 * ((gdouble)space_without_border.width + 0.5) +
              (gdouble)(space_without_border.y +
                        0.5 * space_without_border.height),
              (gdouble)(space_without_border.y + space_without_border.height)),
            (gdouble)space_without_border.y);

    scale_end_y =
      fmax (fmin (
              (cos (angle_to_start) / fabs (sin (angle_to_start))) *
              0.5 * ((gdouble)space_without_border.width + 0.5) +
              (gdouble)(space_without_border.y +
                        0.5 * space_without_border.height),
              (gdouble)(space_without_border.y + space_without_border.height)),
            (gdouble)space_without_border.y);

    /* FIXME: These type-casts might be the reason for the differences of
     *        single pixels by drawing a 2d-layer!
     */
    g_object_set (G_OBJECT (widget),
                  "axis-beg-pix-x", scale_beg_x,
                  "axis-beg-pix-y", scale_beg_y,
                  "axis-end-pix-x", scale_end_x,
                  "axis-end-pix-y", scale_end_y,
                  NULL);
  }

  #endif /* S_SPLINT_S */

  GTK_WIDGET_CLASS (gdv_linear_axis_parent_class)->size_allocate (widget, allocation);
}

/* Function that overwrites the make_tic_label_markup-method of the GdvAxis parent class */
static gchar *
gdv_linear_axis_make_tic_label_markup (GdvAxis *axis, gdouble value)
{
  gdouble tics_beg, tics_end;
  GdvLinearAxis *linear_axis;
  gchar *return_string = NULL;
  gdouble base_val;

  linear_axis = GDV_LINEAR_AXIS (axis);

  g_object_get (G_OBJECT (linear_axis),
                "tics-beg-val", &tics_beg,
                "tics-end-val", &tics_end,
                "scale-increment-base", &base_val,
                NULL);

  if (value == 0.0)
    return_string = g_strdup_printf ("0");
  else if (((fabs (tics_beg) >= 1.0e4 || fabs (tics_end) >= 1.0e4) &&
            fabs (tics_beg - tics_end) >= 1.0e4) ||
           ((fabs (tics_beg) <= 1e-3 || fabs (tics_end) <= 1e-3) &&
            fabs (tics_beg - tics_end) <= 1e-3) ||
           (fabs (tics_beg - tics_end) <= 1.0e-3 * fabs (tics_beg)))
  {
    gdouble exponent, mantissa;

    exponent = floor (log (fabs (value)) / log (base_val));
    mantissa = value / pow (base_val, exponent);

    if (exponent != 0.0)
      return_string =
        g_markup_printf_escaped ("%gx%g<sup>%g</sup>", mantissa, base_val, exponent);
    else
      return_string =
        g_markup_printf_escaped ("%g", mantissa);
  }
  else
    return_string = g_strdup_printf ("%g", value);

  return return_string;
}

/* Function that overwrites the get_point-method of the GdvAxis parent class */
static gboolean
gdv_linear_axis_on_get_point (GdvAxis *axis,
                              gdouble  value,
                              gdouble *pos_x,
                              gdouble *pos_y)
{
  GtkAllocation allocation;
  gdouble begin_val, end_val;
  gdouble begin_x, begin_y, end_x, end_y;
  gdouble tmp_slope_x, tmp_slope_y;

  /* FIXME: why? */
  gtk_widget_get_allocation (GTK_WIDGET (axis), &allocation);

  g_object_get (axis,
                "scale-beg-val", &begin_val,
                "scale-end-val", &end_val,
                "axis-beg-pix-x", &begin_x,
                "axis-end-pix-x", &end_x,
                "axis-beg-pix-y", &begin_y,
                "axis-end-pix-y", &end_y,
                NULL);

  if (pos_x && begin_x != end_x && begin_val != end_val)
  {
    tmp_slope_x = (begin_val - end_val) / (begin_x - end_x);
    *pos_x = begin_x + (value - begin_val) / tmp_slope_x;
  }
  else if (pos_x)
    *pos_x = begin_x;

  if (pos_y && begin_y != end_y && begin_val != end_val)
  {
    tmp_slope_y = (begin_val - end_val) / (begin_y - end_y);
    *pos_y = begin_y + (value - begin_val) / tmp_slope_y;
  }
  else if (pos_y)
    *pos_y = begin_y;

  return (
    (value <= (begin_val >= end_val ? begin_val : end_val)) &&
    (value >= (begin_val <= end_val ? begin_val : end_val)));
}

/* Function that overwrites the get_inner_dir-method of the GdvAxis parent class */
static gboolean
gdv_linear_axis_on_get_inner_dir (GdvAxis *axis,
                                  gdouble  value,
                                  gdouble *dir_x,
                                  gdouble *dir_y)
{
  GtkAllocation allocation;
  gdouble begin_val, end_val;
  gdouble outside_dir;

  /* FIXME: why? */
  gtk_widget_get_allocation (GTK_WIDGET (axis), &allocation);

  g_object_get (axis,
                "scale-beg-val", &begin_val,
                "scale-end-val", &end_val,
                "axis-direction-outside", &outside_dir,
                NULL);

  *dir_x = -sin (outside_dir);
  *dir_y = cos (outside_dir);

  return (
    (value <= (begin_val >= end_val ? begin_val : end_val)) &&
    (value >= (begin_val <= end_val ? begin_val : end_val)));
}

/*
 * FIXME: This is just a working solution and nothing solid!
 * TODO: Assign this to the linear-axis-class!
 */
/* Function that overwrites the get_space_to_beg-method of the GdvAxis parent class */
static void
gdv_linear_axis_get_space_to_beg_position (GdvAxis        *axis,
                                           GtkPositionType direction,
                                           int             for_size,
                                           int            *minimum,
                                           int            *natural,
                                           gpointer        data)
{
  GList *tic_list, *tic_list_start;
//  gdouble axis_orientation;
  gdouble outside_dir;
  gboolean axis_title_on;
  GtkWidget *title_widget;
  gint title_height = 0, title_width = 0;
  gint title_height_nat = 0, title_width_nat = 0;

  gdouble scale_beg_x, scale_beg_y, scale_end_x, scale_end_y;
  gdouble scale_beg_val, scale_end_val;

  *minimum = 0;
  *natural = 0;

  g_object_get (axis,
//                "axis-orientation", &axis_orientation,
                "axis-direction-outside", &outside_dir,
                "title-widget", &title_widget,
                "axis-beg-pix-x", &scale_beg_x,
                "axis-beg-pix-y", &scale_beg_y,
                "axis-end-pix-x", &scale_end_x,
                "axis-end-pix-y", &scale_end_y,
                "scale-beg-val", &scale_beg_val,
                "scale-end-val", &scale_end_val,
                NULL);

  gtk_widget_style_get (GTK_WIDGET (axis),
                        "show-title", &axis_title_on,
                        NULL);

  axis_title_on =
    axis_title_on &&
    title_widget &&
    gtk_widget_get_visible (title_widget);

  tic_list = gdv_axis_get_tic_list (GDV_AXIS (axis));
  tic_list_start = tic_list;

  while (tic_list)
  {
    gint local_min, local_nat;

    /* FIXME: this might not be correctly reached due to round-off errors */
    if (tic_list->data)
    {
      gdouble local_val;

      g_object_get (tic_list->data, "value", &local_val, NULL);

      gdv_tic_get_space_to_tic_position (
        tic_list->data, direction, for_size,
        &local_min, &local_nat, data);

      *minimum = *minimum < local_min ? local_min : *minimum;
      *natural = *natural < local_nat ? local_nat : *natural;
    }

    tic_list = tic_list->next;
  }

  g_list_free (tic_list_start);

  /* Measuring the title */
  if (axis_title_on)
  {
    if (direction == GTK_POS_LEFT || direction == GTK_POS_RIGHT)
    {
      if (for_size > 0)
        gtk_widget_get_preferred_width_for_height (
          title_widget, for_size, &title_width, &title_width_nat);
      else
        gtk_widget_get_preferred_width (
          title_widget, &title_width, &title_width_nat);
    }
    else
    {
      if (for_size > 0)
        gtk_widget_get_preferred_height_for_width (
          title_widget, for_size, &title_height, &title_height_nat);
      else
        gtk_widget_get_preferred_height (
          title_widget, &title_height, &title_height_nat);
    }

    if (direction == GTK_POS_LEFT || direction == GTK_POS_RIGHT)
    {
      if (direction == GTK_POS_LEFT && sin (outside_dir) < 0.0)
      {
        *minimum +=
          (gint)(-1.0 * sin (outside_dir) * (gdouble)title_width);
        *natural +=
          (gint)(-1.0 * sin (outside_dir) * (gdouble)title_width_nat);
      }
      else if (direction == GTK_POS_RIGHT && sin (outside_dir) > 0.0)
      {
        *minimum +=
          (gint)(sin (outside_dir) * (gdouble)title_width);
        *natural +=
          (gint)(sin (outside_dir) * (gdouble)title_width_nat);
      }
    }
    else
    {
      if (direction == GTK_POS_BOTTOM && cos (outside_dir) < 0.0)
      {
        *minimum +=
          (gint)(-1.0 * cos (outside_dir) * (gdouble)title_height);
        *natural +=
          (gint)(-1.0 * cos (outside_dir) * (gdouble)title_height_nat);
      }
      else if (direction == GTK_POS_TOP && cos (outside_dir) > 0.0)
      {
        *minimum +=
          (gint)(cos (outside_dir) * (gdouble)title_height);
        *natural +=
          (gint)(cos (outside_dir) * (gdouble)title_height_nat);
      }
    }
  }
}

/* FIXME: This is just a working solution and nothing solid! */
/* Function that overwrites the get_space_to_end-method of the GdvAxis parent class */
static void
gdv_linear_axis_get_space_to_end_position (GdvAxis        *axis,
                                           GtkPositionType direction,
                                           int             for_size,
                                           int            *minimum,
                                           int            *natural,
                                           gpointer        data)
{
  GList *tic_list, *tic_list_start;
//  gdouble tics_end_val, axis_orientation;
  gdouble outside_dir;

  gboolean axis_title_on;
  GtkWidget *title_widget;
  gint title_height = 0, title_width = 0;
  gint title_height_nat = 0, title_width_nat = 0;

  gdouble scale_beg_x, scale_beg_y, scale_end_x, scale_end_y;
  gdouble scale_beg_val, scale_end_val;

  *minimum = 0;
  *natural = 0;

  g_object_get (axis,
                //"axis-orientation", &axis_orientation,
                "axis-direction-outside", &outside_dir,
                "title-widget", &title_widget,
                "axis-beg-pix-x", &scale_beg_x,
                "axis-beg-pix-y", &scale_beg_y,
                "axis-end-pix-x", &scale_end_x,
                "axis-end-pix-y", &scale_end_y,
                "scale-beg-val", &scale_beg_val,
                "scale-end-val", &scale_end_val,
                NULL);

  gtk_widget_style_get (GTK_WIDGET (axis),
                        "show-title", &axis_title_on,
                        NULL);

  axis_title_on =
    axis_title_on &&
    title_widget &&
    gtk_widget_get_visible (title_widget);

  tic_list = gdv_axis_get_tic_list (GDV_AXIS (axis));
  tic_list_start = tic_list;

  while (tic_list)
  {
    gint local_min, local_nat;

    /* FIXME: this might not be correctly reached due to round-off errors */
    if (tic_list->data)
    {
      gdouble local_val;

      g_object_get (tic_list->data, "value", &local_val, NULL);

      gdv_tic_get_space_to_tic_position (
        tic_list->data, direction, for_size,
        &local_min, &local_nat, data);

      *minimum = *minimum < local_min ? local_min : *minimum;
      *natural = *natural < local_nat ? local_nat : *natural;
    }

    tic_list = tic_list->next;
  }

  g_list_free (tic_list_start);

  /* Measuring the title */
  if (axis_title_on)
  {
    if (direction == GTK_POS_LEFT || direction == GTK_POS_RIGHT)
    {
      if (for_size > 0)
        gtk_widget_get_preferred_width_for_height (
          title_widget, for_size, &title_width, &title_width_nat);
      else
        gtk_widget_get_preferred_width (
          title_widget, &title_width, &title_width_nat);
    }
    else
    {
      if (for_size > 0)
        gtk_widget_get_preferred_height_for_width (
          title_widget, for_size, &title_height, &title_height_nat);
      else
        gtk_widget_get_preferred_height (
          title_widget, &title_height, &title_height_nat);
    }

    if (direction == GTK_POS_LEFT || direction == GTK_POS_RIGHT)
    {
      if (direction == GTK_POS_LEFT && sin (outside_dir) < 0.0)
      {
        *minimum +=
          (gint)(-1.0 * sin (outside_dir) * (gdouble)title_width);
        *natural +=
          (gint)(-1.0 * sin (outside_dir) * (gdouble)title_width_nat);
      }
      else if (direction == GTK_POS_RIGHT && sin (outside_dir) > 0.0)
      {
        *minimum +=
          (gint)(sin (outside_dir) * (gdouble)title_width);
        *natural +=
          (gint)(sin (outside_dir) * (gdouble)title_width_nat);
      }
    }
    else
    {
      if (direction == GTK_POS_BOTTOM && cos (outside_dir) < 0.0)
      {
        *minimum +=
          (gint)(-1.0 * cos (outside_dir) * (gdouble)title_height);
        *natural +=
          (gint)(-1.0 * cos (outside_dir) * (gdouble)title_height_nat);
      }
      else if (direction == GTK_POS_TOP && cos (outside_dir) > 0.0)
      {
        *minimum +=
          (gint)(cos (outside_dir) * (gdouble)title_height);
        *natural +=
          (gint)(cos (outside_dir) * (gdouble)title_height_nat);
      }
    }
  }
}

/**
 * gdv_linear_axis_new:
 *
 * Just a common constructor.
 *
 * Returns: a new #GdvLinearAxis
 **/
GdvLinearAxis *
gdv_linear_axis_new (void)
{
  return g_object_new (gdv_linear_axis_get_type (),
                       NULL);
}
