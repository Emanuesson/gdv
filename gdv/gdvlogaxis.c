/*
 * gdvlogaxis.c
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

#include <math.h>
#include <gsl/gsl_math.h>

/**
 * SECTION:gdvlogaxis
 * @short_description: an logarithmic axis
 * @title: GdvLogAxis
 *
 * #GdvLogAxis Provides an basic logarithmic axis. The increment-value defines
 *  the base of the applied logarithm.
 *
 * # CSS nodes
 *
 * GdvLogAxis uses a single CSS node with name logaxis.
 *
 */

#include "gdvaxis.h"
#include "gdvtic.h"
#include "gdvmtic.h"
#include "gdvlogaxis.h"
#include "gdvlayer.h"

struct _GdvLogAxisPrivate
{
  gint placeholder;
};

static void
gdv_log_axis_size_allocate (GtkWidget           *widget,
                            GtkAllocation       *allocation);
static gboolean
gdv_log_axis_on_get_point (
  GdvAxis *axis,
  gdouble value,
  gdouble *pos_x,
  gdouble *pos_y);
static gboolean
gdv_log_axis_on_get_inner_dir (
  GdvAxis *axis,
  gdouble value,
  gdouble *pos_x,
  gdouble *pos_y);
static gchar *
gdv_log_axis_make_tic_label_markup (GdvAxis *axis, gdouble  value);
static void
gdv_log_axis_get_space_to_beg_position (
  GdvAxis             *axis,
  GtkPositionType      direction,
  int                  for_size,
  int                 *minimum,
  int                 *natural,
  gpointer             data);
static void
gdv_log_axis_get_space_to_end_position (
  GdvAxis             *axis,
  GtkPositionType      direction,
  int                  for_size,
  int                 *minimum,
  int                 *natural,
  gpointer             data);

G_DEFINE_TYPE_WITH_PRIVATE (GdvLogAxis, gdv_log_axis, GDV_TYPE_AXIS);

static void
gdv_log_axis_init (GdvLogAxis *axis)
{
  axis->priv = gdv_log_axis_get_instance_private (axis);
}

static void
gdv_log_axis_dispose (GObject *object)
{
  G_OBJECT_CLASS (gdv_log_axis_parent_class)->dispose (object);
}

static void
gdv_log_axis_finalize (GObject *object)
{
  G_OBJECT_CLASS (gdv_log_axis_parent_class)->finalize (object);
}

static void
gdv_log_axis_class_init (GdvLogAxisClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
  GdvAxisClass *axis_class = GDV_AXIS_CLASS (klass);

  object_class->dispose = gdv_log_axis_dispose;
  object_class->finalize = gdv_log_axis_finalize;

  widget_class->size_allocate = gdv_log_axis_size_allocate;

  axis_class->get_point = gdv_log_axis_on_get_point;
  axis_class->get_inner_dir = gdv_log_axis_on_get_inner_dir;
  axis_class->make_tic_label_markup = gdv_log_axis_make_tic_label_markup;
  axis_class->get_space_to_beg_position =
    gdv_log_axis_get_space_to_beg_position;
  axis_class->get_space_to_end_position  =
    gdv_log_axis_get_space_to_end_position;

  gtk_widget_class_set_css_name (widget_class, "logaxis");
}

/**
 * gdv_log_axis_new:
 *
 * Just a common constructor.
 *
 * Returns: a new #GdvLogAxis
 **/
GdvLogAxis *gdv_log_axis_new (void)
{
  return g_object_new (gdv_log_axis_get_type (),
                       NULL);
}

static void
gdv_log_axis_size_allocate (GtkWidget           *widget,
                            GtkAllocation       *allocation)
{
  /* FIXME: This is necessary to apply splint on the code; maybe it is possible
   *        to run a newer version of splint over the code somewhere without
   *        the crashes I got from Ver3.1.2-4 from the AUR
   */
  #ifndef S_SPLINT_S

  GdvLogAxis  *log_axis = GDV_LOG_AXIS (widget);

  gboolean     scale_automatic, scale_auto_increment;
  gdouble      scale_beg_val, scale_end_val;
  gdouble      scale_increment_val;
  gdouble      angle_to_outer_dir, angle_to_start;

  gboolean     tics_automatic;
  gdouble      tmp_tics_val;
  gdouble      tics_beg_val,
               tics_end_val;
  gboolean     mtics_automatic;
  guint        mtics_number;
  gdouble      mtics_beg_val,
               mtics_end_val;

  gdouble      init_increment_val;

  gdouble      scale_beg_x, scale_beg_y,
               scale_end_x, scale_end_y;

  gint         scale_min_diff_pix, scale_max_diff_pix;

  gdouble      inner_dir_x = 0.0, inner_dir_y = 0.0;
//  gdouble      axis_dir_x = 0.0, axis_dir_y = 0.0;
  gdouble      tic_label_halign, tic_label_valign;

  GdvTic      *beg_tic = NULL, *end_tic = NULL;
  GList       *tic_list, *tic_list_start;

  gdouble      current_diff_pix = G_MAXDOUBLE;

  GtkAllocation space_without_border;
  gdouble      line_height, line_width, line_length;

  gboolean     beg_is_new = FALSE,
               end_is_new = FALSE;

  /* usefull to ceep track about tic-val residua */
  gdouble      signed_scale_increment_val;

  /* maximum distances against borders of tics and mtics */
  gint         max_top_border, max_bot_border,
               max_left_border, max_right_border;

  gdouble      actual_pos_val;

  GtkWidget   *title_widget;
  gboolean     axis_title_on;
  gint         title_height = 0, title_width = 0;
  gint         title_height_nat = 0, title_width_nat = 0;
  GtkAllocation title_allocation;

  GList *previouse_tics, *tics_copy;
  GList *previouse_mtics, *mtics_copy;

  /* needed for the autoadjustment of scale- and tic-values */
  gdouble exp_suppr = 0.0;

  gtk_widget_set_allocation (widget, allocation);

  g_object_get (G_OBJECT (widget),
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
                "mtics-automatic", &mtics_automatic,
                "mtics", &mtics_number,
                "mtics-beg-val", &mtics_beg_val,
                "mtics-end-val", &mtics_end_val,
                "title-widget", &title_widget,
                "axis-beg-pix-x", &scale_beg_x,
                "axis-beg-pix-y", &scale_beg_y,
                "axis-end-pix-x", &scale_end_x,
                "axis-end-pix-y", &scale_end_y,
                NULL);

  init_increment_val = scale_increment_val;

  gtk_widget_style_get (widget,
                        "show-title", &axis_title_on,
                        "scale-min-diff-pix", &scale_min_diff_pix,
                        "scale-max-diff-pix", &scale_max_diff_pix,
                        /* TODO: extensively test this! */
                        NULL);

  /* FIXME: maybe better to overwrite the scale-increment-val property */
  /* catching negative base-values */
  if (!scale_auto_increment && scale_increment_val <= 0.0)
  {
    g_warning ("Negative values for GdvLogAxis::scale-increment-val are not "
               "accepted. Reconsider use of automatic axis-properties");
    return;
  }

  /* setting the scale */
  if (scale_beg_val <= 0.0 && scale_end_val > 0.0)
    scale_beg_val = 1e-2 * scale_end_val;
  else if (scale_end_val <= 0.0 && scale_beg_val > 0.0)
    scale_end_val = 1e2 * scale_beg_val;
  else if (scale_beg_val <= 0.0 && scale_end_val <= 0.0)
  {
    /* FIXME: might warn in this case too!? */
    scale_beg_val = 1e-2;
    scale_end_val = 1e+2;
  }

  g_object_set (G_OBJECT (widget),
                "scale-beg-val", scale_beg_val,
                "scale-end-val", scale_end_val,
                NULL);

  axis_title_on =
    axis_title_on &&
    title_widget &&
    gtk_widget_get_visible (title_widget);

  /* calculating basic geometric values */
  inner_dir_x = -1.0 * sin (angle_to_outer_dir);
  inner_dir_y = cos (angle_to_outer_dir);
//  axis_dir_x = -1.0 * sin (angle_to_start);
//  axis_dir_y = cos (angle_to_start);

  tic_label_halign = -0.5 * fabs(inner_dir_y);
  tic_label_valign = -0.5 * fabs(inner_dir_x);

  /* looking for the beg- and end-tic */
  tic_list = gdv_axis_get_tic_list (GDV_AXIS (log_axis));
  tic_list_start = tic_list;

  while (tic_list)
  {
    gdouble current_value;
    gdouble local_val_resid;

    g_object_get (G_OBJECT (tic_list->data), "value", &current_value, NULL);

    /* FIXME: this might not be correctly reached due to round-off errors */

    /* round-off problems */
    local_val_resid =
      log10(current_value / tics_beg_val) / log10(scale_increment_val);

    if (local_val_resid < 1e-13 && local_val_resid > -1e-13)
      beg_tic = tic_list->data;

    local_val_resid =
      log10(current_value / tics_end_val) / log10(scale_increment_val);

    if (local_val_resid < 1e-13 && local_val_resid > -1e-13)
      end_tic = tic_list->data;

    tic_list = tic_list->next;
  }

  g_list_free (tic_list_start);

  /* if not present, make new beg- and end-tics */
  if (!beg_tic)
  {
    beg_tic = g_object_new (GDV_TYPE_TIC,
                            "value", tics_beg_val, NULL);
    gtk_container_add (GTK_CONTAINER (log_axis), GTK_WIDGET (beg_tic));
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
    end_tic = g_object_new (GDV_TYPE_TIC,
                            "value", tics_end_val, NULL);
    gtk_container_add (GTK_CONTAINER (log_axis), GTK_WIDGET (end_tic));
    end_is_new = TRUE;
  }

  g_object_set (end_tic,
                "axis-inner-dir-x", inner_dir_x,
                "axis-inner-dir-y", inner_dir_y,
                "label-xalign", tic_label_halign,
                "label-yalign", tic_label_valign,
                NULL);


  while (TRUE)
  {
    gdouble down_scaled_difference;
    gdouble down_scaled_beg, down_scaled_end;
    gchar *new_tic_label;
    gint tmp_min, tmp_nat;
    gdouble scale_length;

    /* FIXME: catch trivial case (scale_end_val == scale_beg_val) */
    down_scaled_difference =
      pow(10.0, floor (log10 (scale_end_val > scale_beg_val ?
                              scale_end_val / scale_beg_val :
                              scale_beg_val / scale_end_val)) - exp_suppr);

    if (down_scaled_difference == 1.0)
      break;

    if (scale_auto_increment)
    {
      scale_increment_val = down_scaled_difference;
      signed_scale_increment_val =
        scale_beg_val < scale_end_val ?
        1.0 / scale_increment_val : scale_increment_val;
    }

    if (scale_automatic)
    {
      if (scale_beg_val <= scale_end_val)
      {
        scale_beg_val =
          pow(scale_increment_val,
              floor (log10 (scale_beg_val) / log10 (scale_increment_val)));
        scale_end_val =
          pow(scale_increment_val,
              ceil (log10 (scale_end_val) / log10 (scale_increment_val)));
      }
      else
      {
        scale_beg_val =
          pow(scale_increment_val,
              ceil (log10 (scale_beg_val) / log10 (scale_increment_val)));
        scale_end_val =
          pow(scale_increment_val,
              floor (log10 (scale_end_val) / log10 (scale_increment_val)));
      }

    }

    if (scale_beg_val <= scale_end_val)
    {
      down_scaled_beg =
        pow(down_scaled_difference,
            floor (log10 (scale_beg_val) / log10 (down_scaled_difference)));
      down_scaled_end =
        pow(down_scaled_difference,
            ceil (log10 (scale_end_val) / log10 (down_scaled_difference)));
    }
    else
    {
      down_scaled_beg =
        pow(down_scaled_difference,
            ceil (log10 (scale_beg_val) / log10 (down_scaled_difference)));
      down_scaled_end =
        pow(down_scaled_difference,
            floor (log10 (scale_end_val) / log10 (down_scaled_difference)));
    }

    if (tics_automatic)
    {
      tics_beg_val = down_scaled_beg;
      tics_end_val = down_scaled_end;
    }

    /* set the beg/end-tic to the new value */
    g_object_get (G_OBJECT (beg_tic),
                  "value", &tmp_tics_val,
                  NULL);

    if (beg_is_new || tmp_tics_val != tics_beg_val)
    {
      g_object_set (G_OBJECT (beg_tic),
                    "value", tics_beg_val,
                    NULL);
      new_tic_label =
        GDV_AXIS_GET_CLASS (GDV_AXIS (log_axis))->make_tic_label_markup (
          GDV_AXIS (log_axis), tics_beg_val);
      gdv_tic_label_set_markup (beg_tic, new_tic_label);
      g_free (new_tic_label);
    }

    g_object_get (G_OBJECT (end_tic),
                  "value", &tmp_tics_val,
                  NULL);

    if (end_is_new || tmp_tics_val != tics_end_val)
    {
      g_object_set (G_OBJECT (end_tic),
                    "value", tics_end_val,
                    NULL);
      new_tic_label =
        GDV_AXIS_GET_CLASS (GDV_AXIS (log_axis))->make_tic_label_markup (
          GDV_AXIS (log_axis), tics_end_val);
      gdv_tic_label_set_markup (end_tic, new_tic_label);
      g_free (new_tic_label);
    }

    /* lookup the space that the beg- and end-tic need
     * Please remind: this is not looking for the very narrowest solution.
     *                It's just a good way to organise the tics at minimum cost's
     */
    gdv_tic_get_space_to_tic_position (
      GDV_TIC (beg_tic), GTK_POS_TOP, -1, &tmp_min, &tmp_nat, NULL);
    max_top_border = (tmp_min > tmp_nat ? tmp_min : tmp_nat);
    gdv_tic_get_space_to_tic_position (
      GDV_TIC (end_tic), GTK_POS_TOP, -1, &tmp_min, &tmp_nat, NULL);
    max_top_border = (max_top_border > tmp_nat ? max_top_border : tmp_nat);
    max_top_border = (tmp_min > max_top_border ? tmp_min : max_top_border);

    gdv_tic_get_space_to_tic_position (
      GDV_TIC (beg_tic), GTK_POS_BOTTOM, -1, &tmp_min, &tmp_nat, NULL);
    max_bot_border = (tmp_min > tmp_nat ? tmp_min : tmp_nat);
    gdv_tic_get_space_to_tic_position (
      GDV_TIC (end_tic), GTK_POS_BOTTOM, -1, &tmp_min, &tmp_nat, NULL);
    max_bot_border = (max_bot_border > tmp_nat ? max_bot_border : tmp_nat);
    max_bot_border = (tmp_min > max_bot_border ? tmp_min : max_bot_border);

    gdv_tic_get_space_to_tic_position (
      GDV_TIC (beg_tic), GTK_POS_LEFT, -1, &tmp_min, &tmp_nat, NULL);
    max_left_border = (tmp_min > tmp_nat ? tmp_min : tmp_nat);
    gdv_tic_get_space_to_tic_position (
      GDV_TIC (end_tic), GTK_POS_LEFT, -1, &tmp_min, &tmp_nat, NULL);
    max_left_border = (max_left_border > tmp_nat ? max_left_border : tmp_nat);
    max_left_border = (tmp_min > max_left_border ? tmp_min : max_left_border);

    gdv_tic_get_space_to_tic_position (
      GDV_TIC (beg_tic), GTK_POS_RIGHT, -1, &tmp_min, &tmp_nat, NULL);
    max_right_border = (tmp_min > tmp_nat ? tmp_min : tmp_nat);
    gdv_tic_get_space_to_tic_position (
      GDV_TIC (end_tic), GTK_POS_RIGHT, -1, &tmp_min, &tmp_nat, NULL);
    max_right_border = (max_right_border > tmp_nat ? max_right_border : tmp_nat);
    max_right_border = (tmp_min > max_right_border ? tmp_min : max_right_border);

    /* calculating the free space */
    space_without_border.x = max_left_border;
    space_without_border.y = max_top_border;
    space_without_border.width =
      allocation->width - max_left_border - max_right_border;
    space_without_border.height =
      allocation->height - max_top_border - max_bot_border;

    /* measuring title */
    if (axis_title_on)
    {
      gtk_widget_get_preferred_width (
        title_widget, &title_width, &title_width_nat);
      gtk_widget_get_preferred_height (
        title_widget, &title_height, &title_height_nat);

      space_without_border.width -=
        (gint) (fabs(sin(angle_to_outer_dir)) * (gdouble) title_width);
      space_without_border.height -=
        (gint) (fabs(cos(angle_to_outer_dir)) * (gdouble) title_height);

      if (sin(angle_to_outer_dir) < 0.0)
        space_without_border.x +=
          (gint) (-1.0 * sin(angle_to_outer_dir) * (gdouble) title_width);

      if (cos(angle_to_outer_dir) >= 0.0)
        space_without_border.y +=
          (gint) (+1.0 * cos(angle_to_outer_dir) * (gdouble) title_height);
    }

    /* binning to zero */
    space_without_border.width =
      space_without_border.width < 0 ? 0 : space_without_border.width;
    space_without_border.height =
      space_without_border.height < 0 ? 0 : space_without_border.height;

    /* FIXME: This must be adapted, if we manage the geometry-handling by the
     * layer
     */
    line_height =
      fmin (fabs(tan (0.5 * M_PI - angle_to_start)) *
            (gdouble) (space_without_border.width + 0.5),
            (gdouble) space_without_border.height);
    line_width =
      fmin (fabs(tan (angle_to_start)) * (gdouble) (space_without_border.height + 0.5),
            (gdouble) space_without_border.width);
    line_length = sqrt (line_width * line_width + line_height * line_height);

    /* compare difference to scale_min_diff_pix */
    scale_length = fabs (scale_end_val - scale_beg_val);
    current_diff_pix = line_length * scale_increment_val / scale_length;

    /* abort if the style-property cannot be reached */
    if ((signed_scale_increment_val == down_scaled_end / down_scaled_beg) &&
        current_diff_pix < (gdouble) scale_min_diff_pix)
    {
      if (scale_auto_increment)
        g_object_set (G_OBJECT (log_axis),
                      "scale-increment-val", scale_increment_val,
                      NULL);

      if (scale_automatic)
        g_object_set (G_OBJECT (log_axis),
                      "scale-beg-val", scale_beg_val,
                      "scale-end-val", scale_end_val,
                      NULL);

      if (tics_automatic)
        g_object_set (G_OBJECT (log_axis),
                      "tics-beg-val", tics_beg_val,
                      "tics-end-val", tics_end_val,
                      NULL);

      if (gtk_widget_get_realized (widget))
        g_warning ("Style-property GdvAxis::scale-min-diff-pix and "
                   "GdvAxis::scale-max-diff-pix cannot be fullfilled. "
                   "Reconsider change of automatic axis-properties");

      break;
    }

    /* aborting criteria - optimisation successful */
    if ((current_diff_pix < (gdouble) scale_min_diff_pix) || !scale_auto_increment)
      break;
    /* autoscaling successfull; setting all parameters to the axis */
    else if (current_diff_pix < (gdouble) scale_max_diff_pix)
    {
      if (scale_auto_increment)
        g_object_set (G_OBJECT (log_axis),
                      "scale-increment-val", scale_increment_val,
                      NULL);

      if (scale_automatic)
        g_object_set (G_OBJECT (log_axis),
                      "scale-beg-val", scale_beg_val,
                      "scale-end-val", scale_end_val,
                      NULL);

      if (tics_automatic)
        g_object_set (G_OBJECT (log_axis),
                      "tics-beg-val", tics_beg_val,
                      "tics-end-val", tics_end_val,
                      NULL);
    }

    /* preparing the next base value */
    exp_suppr = exp_suppr + 1.0;
  }

  /* Restoring properties after break condition */
  g_object_get (G_OBJECT (log_axis),
                "scale-increment-val", &scale_increment_val,
                NULL);
  g_object_get (G_OBJECT (log_axis),
                "scale-beg-val", &scale_beg_val,
                "scale-end-val", &scale_end_val,
                NULL);
  g_object_get (G_OBJECT (log_axis),
                "tics-beg-val", &tics_beg_val,
                "tics-end-val", &tics_end_val,
                NULL);

  /* Just in case someone the tic-label dependents on the incr-val */
  if (init_increment_val != scale_increment_val)
  {
    gchar *new_tic_label;

    new_tic_label =
      GDV_AXIS_GET_CLASS (GDV_AXIS (log_axis))->make_tic_label_markup (
        GDV_AXIS (log_axis), tics_beg_val);
    gdv_tic_label_set_markup (beg_tic, new_tic_label);
    g_free (new_tic_label);

    new_tic_label =
      GDV_AXIS_GET_CLASS (GDV_AXIS (log_axis))->make_tic_label_markup (
        GDV_AXIS (log_axis), tics_end_val);
    gdv_tic_label_set_markup (end_tic, new_tic_label);
    g_free (new_tic_label);
  }

  signed_scale_increment_val =
    scale_beg_val < scale_end_val ?
    scale_increment_val : 1.0 / scale_increment_val;

  /* Determining the number of mtics per interval
   * (will adjust automatically to the base)
   */
  if (mtics_automatic)
  {
    gdouble exponent, mantissa;

    mtics_beg_val = tics_beg_val;
    mtics_end_val = tics_end_val;

    exponent = floor(log10(scale_increment_val));
    mantissa = scale_increment_val / pow (10.0, exponent);

    /* This is just a best practice, if a different engine of automatism is
     * required, it should be implemented by an own class-type.
     */
    if (mantissa == 2.5 || mantissa == 5.0)
    {
      mtics_number = 4;
    }
    else if ((mantissa == 4.0) || (mantissa == 2.0))
    {
      mtics_number = 3;
    }
    else if (mantissa == 1.0)
    {
      mtics_number = 9;
    }
    else
    {
      mtics_number =
        (guint) (scale_increment_val / pow (10.0, exponent - 1.0) - 1.0);
    }

    g_object_set (G_OBJECT (log_axis),
                  "mtics", mtics_number,
                  "mtics-beg-val", mtics_beg_val,
                  "mtics-end-val", mtics_end_val,
                  NULL);
  }

  /* check and delete all unnecessary tics of the axis-instance! */
  previouse_tics = gdv_axis_get_tic_list (GDV_AXIS (log_axis));
  tics_copy = previouse_tics;

  while (tics_copy)
  {
    gdouble local_tic_value;
    gdouble local_val_resid;

    g_object_get (tics_copy->data, "value", &local_tic_value, NULL);
    local_val_resid =
      fmod(log10(local_tic_value / tics_beg_val) / log10(signed_scale_increment_val), 1.0);

    if (local_tic_value < GSL_MIN (tics_beg_val, tics_end_val) ||
        local_tic_value > GSL_MAX (tics_beg_val, tics_end_val) ||
        !((local_val_resid < 1e-13 && local_val_resid > -1e-13) ||
          ((1.0 - local_val_resid) < 1e-13 &&
           (1.0 - local_val_resid) > -1e-13)))
      /* round-off problems */
    {
      gtk_container_remove (GTK_CONTAINER (log_axis), tics_copy->data);
    }

    tics_copy = tics_copy->next;
  }

  g_list_free (previouse_tics);

  /* check and delete all unnecessary mtics of the axis-instance! */
  previouse_mtics = gdv_axis_get_mtic_list (GDV_AXIS (log_axis));
  mtics_copy = previouse_mtics;

  while (mtics_copy)
  {
    gdouble local_mtic_value;
    gdouble local_val_resid, local_mtic_resid;

    g_object_get (mtics_copy->data, "value", &local_mtic_value, NULL);
    local_val_resid =
      pow (signed_scale_increment_val,
           fmod(log10(local_mtic_value / tics_beg_val) /
             log10(signed_scale_increment_val), 1.0));

    local_mtic_resid =
      fmod(local_val_resid / (scale_increment_val / (mtics_number + 1)), 1.0);

    if (local_mtic_value < GSL_MIN (mtics_beg_val, mtics_end_val) ||
        local_mtic_value > GSL_MAX (mtics_beg_val, mtics_end_val) ||
        !((local_mtic_resid < 1e-13 && local_mtic_resid > -1e-13) ||
          ((1.0 - local_mtic_resid) < 1e-13 &&
           (1.0 - local_mtic_resid) > -1e-13) ||
          ((local_mtic_resid - 1.0) < 1e-13 &&
           (local_mtic_resid - 1.0) > -1e-13)))
      /* round-off problems */
    {
      gtk_container_remove (GTK_CONTAINER (log_axis), mtics_copy->data);
    }

    mtics_copy = mtics_copy->next;
  }

  g_list_free (previouse_mtics);

  /* adding or deleting all the necessary tics and mtics */
  actual_pos_val = tics_beg_val;

  /* adding all tics and mtics between the begin and end value */
  while (actual_pos_val <= GSL_MAX (tics_end_val, tics_beg_val) &&
         actual_pos_val >= GSL_MIN (tics_end_val, tics_beg_val))
  {
    GdvTic *local_tic = NULL;
    gint i;

    /* get the correct tic */
    previouse_tics = gdv_axis_get_tic_list (GDV_AXIS (log_axis));
    tics_copy = previouse_tics;

    while (tics_copy)
    {
      gdouble local_tic_value;
      gdouble local_val_resid;

      /* floating-point precision struggle */
      g_object_get (tics_copy->data, "value", &local_tic_value, NULL);

      local_val_resid =
        fabs(actual_pos_val - local_tic_value) / scale_increment_val;

      if (local_val_resid < 1e-13 && local_val_resid > -1e-13)
      {
        local_tic = tics_copy->data;
        break;
      }

      tics_copy =  tics_copy->next;
    }

    g_list_free (previouse_tics);

    if (!local_tic)
    {
      gchar *new_tic_label;

      local_tic =
        g_object_new (gdv_tic_get_type(),
                      "value", actual_pos_val,
                      NULL);

      new_tic_label =
        GDV_AXIS_GET_CLASS (GDV_AXIS (log_axis))->make_tic_label_markup (
          GDV_AXIS (log_axis), actual_pos_val);

      gdv_tic_label_set_markup (local_tic, new_tic_label);
      g_free (new_tic_label);

      gtk_container_add (GTK_CONTAINER (log_axis), GTK_WIDGET (local_tic));
    }

    /* Setting the mtics */
    previouse_mtics = gdv_axis_get_mtic_list (GDV_AXIS (log_axis));

    for (i = 1; i < mtics_number + 1; i++)
    {
      GdvTic *local_mtic = NULL;
      gdouble local_mtic_val =
        actual_pos_val * i * signed_scale_increment_val / (mtics_number + 1);

      /* get the correct mtic */
      mtics_copy = previouse_mtics;

      while (mtics_copy)
      {
        gdouble local_mtic_value;

        gdouble local_val_resid;

        /* floating-point precision struggle */
        g_object_get (mtics_copy->data, "value", &local_mtic_value, NULL);

        local_val_resid =
          (local_mtic_value - local_mtic_val) / scale_increment_val;

        if (local_val_resid < 1e-13 && local_val_resid > -1e-13)
        {
          local_mtic = mtics_copy->data;
          break;
        }

        mtics_copy = mtics_copy->next;
      }

      /* we are not discussing about one pixel again ! */
      if (
        local_mtic_val <= GSL_MAX (mtics_end_val, mtics_beg_val) &&
        local_mtic_val >= GSL_MIN (mtics_end_val, mtics_beg_val))
      {
        if (!local_mtic)
        {
          local_mtic =
            g_object_new (gdv_mtic_get_type(),
                          "value", local_mtic_val,
                          NULL);
          gtk_container_add (GTK_CONTAINER (log_axis), GTK_WIDGET (local_mtic));
        }
      }

    }

    g_list_free (previouse_mtics);

    /* stepping to next scale-point */
    actual_pos_val *= signed_scale_increment_val;
  }

  /* make the final measurement of all tics! */
  max_top_border = 0;
  max_bot_border = 0;
  max_left_border = 0;
  max_right_border = 0;

  previouse_tics = gdv_axis_get_tic_list (GDV_AXIS (log_axis));

  for (tics_copy = previouse_tics; tics_copy; tics_copy = tics_copy->next)
  {
    gint tmp_min, tmp_nat;

    /* lookup the space that the beg- and end-tic need
     * Please remind: this is not looking for the very narrowest solution.
     *                It's just a good way to organise the tics at minimum
     *                cost's
     */
    gdv_tic_get_space_to_tic_position (
      GDV_TIC (tics_copy->data), GTK_POS_TOP, -1, &tmp_min, &tmp_nat, NULL);
    max_top_border = (max_top_border > tmp_nat ? max_top_border : tmp_nat);
    max_top_border = (tmp_min > max_top_border ? tmp_min : max_top_border);

    gdv_tic_get_space_to_tic_position (
      GDV_TIC (tics_copy->data), GTK_POS_BOTTOM, -1, &tmp_min, &tmp_nat, NULL);
    max_bot_border = (max_bot_border > tmp_nat ? max_bot_border : tmp_nat);
    max_bot_border = (tmp_min > max_bot_border ? tmp_min : max_bot_border);

    gdv_tic_get_space_to_tic_position (
      GDV_TIC (tics_copy->data), GTK_POS_LEFT, -1, &tmp_min, &tmp_nat, NULL);
    max_left_border = (max_left_border > tmp_nat ? max_left_border : tmp_nat);
    max_left_border = (tmp_min > max_left_border ? tmp_min : max_left_border);

    gdv_tic_get_space_to_tic_position (
      GDV_TIC (tics_copy->data), GTK_POS_RIGHT, -1, &tmp_min, &tmp_nat, NULL);
    max_right_border =
      (max_right_border > tmp_nat ? max_right_border : tmp_nat);
    max_right_border =
      (tmp_min > max_right_border ? tmp_min : max_right_border);
  }

  g_list_free (previouse_tics);

  /* calculating the free space */
  space_without_border.x = max_left_border;
  space_without_border.y = max_top_border;
  space_without_border.width =
    allocation->width - max_left_border - max_right_border;
  space_without_border.height =
    allocation->height - max_top_border - max_bot_border;

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

    space_without_border.width -=
      (gint) (fabs(sin(angle_to_outer_dir)) * (gdouble) title_width);
    space_without_border.height -=
      (gint) (fabs(cos(angle_to_outer_dir)) * (gdouble) title_height);

    if (sin(angle_to_outer_dir) < 0.0)
      space_without_border.x +=
        (gint) (-1.0 * sin(angle_to_outer_dir) * (gdouble) title_width);

    if (cos(angle_to_outer_dir) >= 0.0)
      space_without_border.y +=
        (gint) (+1.0 * cos(angle_to_outer_dir) * (gdouble) title_height);

    /* FIXME: This is not the function I intented */
    title_allocation.x =
      allocation->x +
      space_without_border.x - max_left_border - title_width +
      (gint) ((0.5 + 0.5 * sin(angle_to_outer_dir)) *
              (gdouble) (
                space_without_border.width +
                max_right_border +
                max_left_border +
                title_width));

    title_allocation.y =
      allocation->y +
      space_without_border.y - max_top_border - title_height +
      (gint) ((0.5 - 0.5 * cos(angle_to_outer_dir)) *
              (gdouble) (
                space_without_border.height +
                max_bot_border +
                max_top_border +
                title_height));

    gtk_widget_size_allocate (
      GTK_WIDGET (title_widget), &title_allocation);
  }

  /* FIXME: this should be guarded, to ensure correct setting by layer */
  scale_beg_x =
    fmax (fmin (
            (sin(angle_to_start) / fabs(cos(angle_to_start))) *
            0.5 * ((gdouble) space_without_border.height + 0.5) +
            (gdouble) (space_without_border.x +
              0.5 * space_without_border.width),
            (gdouble) (space_without_border.x + space_without_border.width)),
          (gdouble) space_without_border.x);
  scale_end_x =
    fmax (fmin (
            (-1.0 * sin(angle_to_start) / fabs(cos(angle_to_start))) *
            0.5 * ((gdouble) space_without_border.height + 0.5) +
            (gdouble) (space_without_border.x +
              0.5 * space_without_border.width),
            (gdouble) (space_without_border.x + space_without_border.width)),
          (gdouble) space_without_border.x);

  scale_beg_y =
    fmax (fmin (
            (-1.0 * cos (angle_to_start) / fabs(sin(angle_to_start))) *
            0.5 * ((gdouble) space_without_border.width + 0.5) +
            (gdouble) (space_without_border.y +
              0.5 * space_without_border.height),
            (gdouble) (space_without_border.y + space_without_border.height)),
          (gdouble) space_without_border.y);

  scale_end_y =
    fmax (fmin (
            (cos (angle_to_start) / fabs(sin(angle_to_start))) *
            0.5 * ((gdouble) space_without_border.width + 0.5) +
            (gdouble) (space_without_border.y +
              0.5 * space_without_border.height),
            (gdouble) (space_without_border.y + space_without_border.height)),
          (gdouble) space_without_border.y);

  /* Finally setting the values */
  g_object_set (G_OBJECT (widget),
                "axis-beg-pix-x", scale_beg_x,
                "axis-beg-pix-y", scale_beg_y,
                "axis-end-pix-x", scale_end_x,
                "axis-end-pix-y", scale_end_y,
                NULL);

  #endif /* S_SPLINT_S */

  GTK_WIDGET_CLASS (gdv_log_axis_parent_class)->size_allocate (
    widget, allocation);
}

static gboolean
gdv_log_axis_on_get_point (
  GdvAxis *axis,
  gdouble value,
  gdouble *pos_x,
  gdouble *pos_y)
{
  gdouble begin_val, end_val;
  gdouble begin_x, begin_y, end_x, end_y;
  gdouble tmp_slope_x, tmp_slope_y;
  gdouble scale_incrmenet;

  if (value <= 0.0)
    return FALSE;

  g_object_get (axis,
                "scale-beg-val", &begin_val,
                "scale-end-val", &end_val,
                "scale-increment-val", &scale_incrmenet,
                "axis-beg-pix-x", &begin_x,
                "axis-end-pix-x", &end_x,
                "axis-beg-pix-y", &begin_y,
                "axis-end-pix-y", &end_y,
                NULL);

  /* TODO: automatically correct the values depending on the others */
  if (!begin_val && end_val)
  {
    begin_val = fmax (begin_val, 1e-1);
  }
  else if (!end_val && begin_val)
  {
    end_val = fmax (end_val, 1e2);
  }
  else if (!begin_val && !end_val)
  {
    begin_val = fmax (begin_val, 1e-1);
    end_val = fmax (end_val, 1e2);
  }

  if (begin_x != end_x && begin_val != end_val)
  {
    tmp_slope_x =
      (log(begin_val) - log(end_val)) /
      ((begin_x - end_x));
    *pos_x =
      begin_x + (log(value) -
                 log(begin_val)) / tmp_slope_x;
  }
  else
    *pos_x = (end_x + begin_x) / 2.0;

  if (begin_y != end_y && begin_val != end_val)
  {
    tmp_slope_y =
      (log(begin_val) - log(end_val)) /
      ((begin_y - end_y));
    *pos_y =
      begin_y + (log(value) -
                 log(begin_val)) / tmp_slope_y;
  }
  else
    *pos_y = (end_y + begin_y) / 2.0;

  return (
           (value <= (begin_val >= end_val ? begin_val : end_val)) &&
           (value >= (begin_val <= end_val ? begin_val : end_val)));
}

static gboolean
gdv_log_axis_on_get_inner_dir (
  GdvAxis *axis,
  gdouble value,
  gdouble *dir_x,
  gdouble *dir_y)
{
  gdouble begin_val, end_val;
  gdouble outside_dir;

  g_object_get (axis,
                "scale-beg-val", &begin_val,
                "scale-end-val", &end_val,
                "axis-direction-outside", &outside_dir,
                NULL);

  *dir_x = - sin(outside_dir);
  *dir_y = cos(outside_dir);

  return (
           (value <= (begin_val >= end_val ? begin_val : end_val)) &&
           (value >= (begin_val <= end_val ? begin_val : end_val)));
}

static gchar *
gdv_log_axis_make_tic_label_markup (GdvAxis *axis, gdouble  value)
{
  gdouble tics_beg, tics_end;
  gdouble scale_increment_val;
  GdvLogAxis *log_axis = GDV_LOG_AXIS (axis);
  gchar *return_string = NULL;
  gdouble exponent, mantissa;

  g_object_get (G_OBJECT (log_axis),
                "tics-beg-val", &tics_beg,
                "tics-end-val", &tics_end,
                "scale-increment-val", &scale_increment_val,
                NULL);

  exponent = floor(log10(fabs(value)) / log10(scale_increment_val));
  mantissa = value / pow (scale_increment_val, exponent);

  if (exponent == 0.0) /* Magic words everywhere */
    return_string = g_strdup_printf ("1");
  else
    return_string =
      g_markup_printf_escaped ("%gx%g<sup>%g</sup>",
                               mantissa,
                               scale_increment_val,
                               exponent);

  return return_string;
}

/* FIXME: This is just a working solution and nothing solid! */
/* TODO: Assign this to the normal axis-class! */
static void
gdv_log_axis_get_space_to_beg_position (
  GdvAxis             *axis,
  GtkPositionType      direction,
  int                  for_size,
  int                 *minimum,
  int                 *natural,
  gpointer             data)
{
  GList *tic_list, *tic_list_start;
  gdouble tics_beg_val, axis_orientation;
  gdouble outside_dir;
  gboolean axis_title_on;
  GtkWidget *title_widget;
  gint title_height = 0, title_width = 0;
  gint title_height_nat = 0, title_width_nat = 0;

  *minimum = 0;
  *natural = 0;

  g_object_get (axis,
                "tics-beg-val", &tics_beg_val,
                "axis-orientation", &axis_orientation,
                "axis-direction-outside", &outside_dir,
                "title-widget", &title_widget,
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
      gdv_tic_get_space_to_tic_position (
        tic_list->data, direction, for_size,
        &local_min, &local_nat, data);

      *minimum = *minimum < local_min ? local_min : *minimum;
      *natural = *natural < local_nat ? local_nat : *natural;
    }

    tic_list = tic_list->next;
  }

  g_list_free (tic_list_start);

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
      if (direction == GTK_POS_LEFT && sin(outside_dir) < 0.0)
      {
        *minimum +=
          (gint) (-1.0 * sin(outside_dir) * (gdouble) title_width);
        *natural +=
          (gint) (-1.0 * sin(outside_dir) * (gdouble) title_width_nat);
      }
      else if (direction == GTK_POS_RIGHT && sin(outside_dir) > 0.0)
      {
        *minimum +=
          (gint) (sin(outside_dir) * (gdouble) title_width);
        *natural +=
          (gint) (sin(outside_dir) * (gdouble) title_width_nat);
      }
    }
    else
    {
      if (direction == GTK_POS_BOTTOM && cos(outside_dir) < 0.0)
      {
        *minimum +=
          (gint) (-1.0 * cos(outside_dir) * (gdouble) title_height);
        *natural +=
          (gint) (-1.0 * cos(outside_dir) * (gdouble) title_height_nat);
      }
      else if (direction == GTK_POS_TOP && cos(outside_dir) > 0.0)
      {
        *minimum +=
          (gint) (cos(outside_dir) * (gdouble) title_height);
        *natural +=
          (gint) (cos(outside_dir) * (gdouble) title_height_nat);
      }
    }
  }
}

/* FIXME: see gdv_log_axis_get_space_to_beg_position! */
static void
gdv_log_axis_get_space_to_end_position (
  GdvAxis             *axis,
  GtkPositionType      direction,
  int                  for_size,
  int                 *minimum,
  int                 *natural,
  gpointer             data)
{
  GList *tic_list, *tic_list_start;
  gdouble tics_end_val, axis_orientation;
  gdouble outside_dir;

  gboolean axis_title_on;
  GtkWidget *title_widget;
  gint title_height = 0, title_width = 0;
  gint title_height_nat = 0, title_width_nat = 0;

  *minimum = 0;
  *natural = 0;

  g_object_get (axis,
                "tics-end-val", &tics_end_val,
                "axis-orientation", &axis_orientation,
                "axis-direction-outside", &outside_dir,
                "title-widget", &title_widget,
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
      gdv_tic_get_space_to_tic_position (
        tic_list->data, direction, for_size,
        &local_min, &local_nat, data);

      *minimum = *minimum < local_min ? local_min : *minimum;
      *natural = *natural < local_nat ? local_nat : *natural;
    }

    tic_list = tic_list->next;
  }

  g_list_free (tic_list_start);

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
      if (direction == GTK_POS_LEFT && sin(outside_dir) < 0.0)
      {
        *minimum +=
          (gint) (-1.0 * sin(outside_dir) * (gdouble) title_width);
        *natural +=
          (gint) (-1.0 * sin(outside_dir) * (gdouble) title_width_nat);
      }
      else if (direction == GTK_POS_RIGHT && sin(outside_dir) > 0.0)
      {
        *minimum +=
          (gint) (sin(outside_dir) * (gdouble) title_width);
        *natural +=
          (gint) (sin(outside_dir) * (gdouble) title_width_nat);
      }
    }
    else
    {
      if (direction == GTK_POS_BOTTOM && cos(outside_dir) < 0.0)
      {
        *minimum +=
          (gint) (-1.0 * cos(outside_dir) * (gdouble) title_height);
        *natural +=
          (gint) (-1.0 * cos(outside_dir) * (gdouble) title_height_nat);
      }
      else if (direction == GTK_POS_TOP && cos(outside_dir) > 0.0)
      {
        *minimum +=
          (gint) (cos(outside_dir) * (gdouble) title_height);
        *natural +=
          (gint) (cos(outside_dir) * (gdouble) title_height_nat);
      }
    }
  }
}
