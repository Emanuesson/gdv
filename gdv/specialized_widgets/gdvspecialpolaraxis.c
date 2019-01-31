/*
 * gdvspecialpolaraxis.c
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

#include "gdvspecialpolaraxis.h"

/* Define Properties */
enum
{
  PROP_0,

  //PROP_IS_UNIX_POLAR,
  PROP_DRAW_FROM_ANGLE,
  PROP_DRAW_TO_ANGLE,

  N_PROPERTIES
};

static GParamSpec *special_polar_axis_properties[N_PROPERTIES] = { NULL, };

struct _GdvSpecialPolarAxisPrivate
{
  gdouble draw_from_angle;
  gdouble draw_to_angle;
};

static gboolean
gdv_polar_axis_on_evaluate (GdvAxis *axis,
                            gdouble value,
                            gdouble *pos_x,
                            gdouble *pos_y);

static gboolean
gdv_polar_axis_on_evaluate_inndir (GdvAxis *axis,
                                   gdouble value,
                                   gdouble *pos_x,
                                   gdouble *pos_y);

static void
gdv_polar_axis_size_allocate (GtkWidget     *widget,
                              GtkAllocation *allocation);

static gboolean
gdv_polar_axis_draw (GtkWidget    *widget,
                     cairo_t      *cr);

static void
gdv_polar_axis_get_preferred_width (GtkWidget           *widget,
                                    gint                *minimum_size,
                                    gint                *natural_size);

static void
gdv_polar_axis_get_preferred_height (GtkWidget           *widget,
                                     gint                *minimum_size,
                                     gint                *natural_size);

static void
gdv_polar_axis_get_preferred_height_for_width (GtkWidget           *widget,
                                               gint                 width,
                                               gint                *minimum_height,
                                               gint                *natural_height);

static void
gdv_polar_axis_get_preferred_width_for_height (GtkWidget           *layout,
                                               gint                 width,
                                               gint                *minimum_height,
                                               gint                *natural_height);

/* Method declaration */
//static void gdv_special_polar_axis_dispose (GObject *object);
//static void gdv_special_polar_axis_finalize (GObject *object);
static void gdv_special_polar_axis_set_property (GObject      *object,
                                                guint         property_id,
                                                const GValue *value,
                                                GParamSpec   *pspec);
static void gdv_special_polar_axis_get_property (GObject    *object,
                                                guint       property_id,
                                                GValue     *value,
                                                GParamSpec *pspec);

G_DEFINE_TYPE_WITH_PRIVATE (GdvSpecialPolarAxis, gdv_special_polar_axis,
  gdv_linear_axis_get_type());

//static void
//gdv_special_polar_axis_size_allocate (GtkWidget           *widget,
//                               GtkAllocation       *allocation);

static void
gdv_special_polar_axis_class_init (GdvSpecialPolarAxisClass *klass)
{
  GtkWidgetClass *widget_class;
  GdvAxisClass *axis_class;
  GObjectClass *object_class;

  widget_class = GTK_WIDGET_CLASS (klass);
  axis_class = GDV_AXIS_CLASS (klass);
  object_class = G_OBJECT_CLASS (klass);

//  widget_class->size_allocate = gdv_special_polar_axis_size_allocate;

//  gobject_class->dispose = gdv_special_polar_axis_dispose;
//  gobject_class->finalize = gdv_special_polar_axis_finalize;
  object_class->set_property = gdv_special_polar_axis_set_property;
  object_class->get_property = gdv_special_polar_axis_get_property;

  widget_class->size_allocate =
    gdv_polar_axis_size_allocate;
  widget_class->draw =
    gdv_polar_axis_draw;
  widget_class->get_preferred_width =
    gdv_polar_axis_get_preferred_width;
  widget_class->get_preferred_height =
    gdv_polar_axis_get_preferred_height;
  widget_class->get_preferred_height_for_width =
    gdv_polar_axis_get_preferred_height_for_width;
  widget_class->get_preferred_width_for_height =
    gdv_polar_axis_get_preferred_width_for_height;

  axis_class->get_point =
    gdv_polar_axis_on_evaluate;
  axis_class->get_inner_dir =
    gdv_polar_axis_on_evaluate_inndir;

  /* Properties */
  special_polar_axis_properties[PROP_DRAW_FROM_ANGLE] =
    g_param_spec_double ("draw-from-angle",
                         "angle from which the axis starts",
                         "the angle that is counted clockwise which indicates the beginning of this "
                         "polar axis",
                         -G_MAXDOUBLE,
                         G_MAXDOUBLE,
                         0.0,
                         G_PARAM_READWRITE);

  special_polar_axis_properties[PROP_DRAW_TO_ANGLE] =
    g_param_spec_double ("draw-to-angle",
                          "angle where the axis stops",
                         "the angle that is counted clockwise which indicates the end of this "
                         "polar axis",
                         -G_MAXDOUBLE,
                         G_MAXDOUBLE,
                         2.0 * M_PI,
                         G_PARAM_READWRITE);

  g_object_class_install_properties (object_class,
                                     N_PROPERTIES,
                                     special_polar_axis_properties);

//  gtk_widget_class_install_style_property (widget_class,
//    g_param_spec_double ("custom-property",
//                         "background cairo source",
//                         "sets the view background source for plotting",
//                         -1000.0,
//                         10000.0,
//                         0.0,
//                         G_PARAM_READWRITE));

  gtk_widget_class_set_css_name (widget_class, "polaraxis");

}

static void
gdv_special_polar_axis_init (GdvSpecialPolarAxis *polar_axis)
{
  GtkWidget *widget;

  GtkCssProvider *css_provider;
  GdkScreen *screen;

  widget = GTK_WIDGET (polar_axis);

  polar_axis->priv = gdv_special_polar_axis_get_instance_private (polar_axis);

	/* initialize window group */
  css_provider = gtk_css_provider_new ();
//  gtk_css_provider_load_from_resource (
//    css_provider,
//    "/net/gevseexe/gevseapp/ui/gui/default_general.css");
//  gtk_style_context_add_provider (
//    style_context,
//    GTK_STYLE_PROVIDER (css_provider),
//    GTK_STYLE_PROVIDER_PRIORITY_FALLBACK);
  screen = gtk_widget_get_screen (widget);
  if (screen)
    gtk_style_context_add_provider_for_screen (
      screen,
      GTK_STYLE_PROVIDER (css_provider),
      GTK_STYLE_PROVIDER_PRIORITY_USER);

  g_object_set (polar_axis,
    "scale-increment-base", 60.0,
    "scale-beg-val", -100.0,
    NULL);

}

static gboolean
gdv_polar_axis_on_evaluate (GdvAxis *axis,
                            gdouble value,
                            gdouble *pos_x,
                            gdouble *pos_y)
{
  return TRUE;
}

static gboolean
gdv_polar_axis_on_evaluate_inndir (GdvAxis *axis,
                                   gdouble value,
                                   gdouble *pos_x,
                                   gdouble *pos_y)
{
  return TRUE;
}

static void
gdv_polar_axis_size_allocate (GtkWidget     *widget,
                              GtkAllocation *allocation)
{
}

static gboolean
gdv_polar_axis_draw (GtkWidget    *widget,
                     cairo_t      *cr)
{
  g_print("REACHED!\n");
  exit(0);
  return TRUE;
}


static void
gdv_polar_axis_measure (
  GdvAxis             *axis,
  GtkOrientation       orientation,
  int                  for_size,
  int                 *minimum,
  int                 *natural,
  int                 *minimum_baseline,
  int                 *natural_baseline,
  gpointer             data)
{
  GtkWidget * title_widget;
  GList *tics_list;
  gint glob_tic_min_start = 0, glob_tic_nat_start = 0,
       glob_tic_min_stop = 0, glob_tic_nat_stop = 0;
  gint scale_min_diff, projected_min_diff = 0;
  gdouble axis_line_width;
//  gdouble axis_direction = axis->priv->direction_start;
  gboolean axis_title_on;
  gint title_height = 0, title_width = 0;
  gint title_height_nat = 0, title_width_nat = 0;
  gdouble outside_dir;

/*
  *minimum = 200;
  *natural = 200;
  return;
*/

  g_object_get (G_OBJECT(axis), "title-widget", &title_widget, NULL);

  *minimum = 0;
  *natural = 0;

  gtk_widget_style_get (GTK_WIDGET (axis),
                        "scale-min-diff-pix", &scale_min_diff,
                        "line-width", &axis_line_width,
                        NULL);

/*
  if (orientation == GTK_ORIENTATION_HORIZONTAL)
    projected_min_diff =
      (gint) (fabs(sin (axis_direction)) * ((gdouble) scale_min_diff + 0.5) +
              fabs(cos (axis_direction)) * axis_line_width);
  else
    projected_min_diff =
      (gint) ((fabs(cos (axis_direction)) * ((gdouble) scale_min_diff + 0.5))
              + fabs(sin (axis_direction)) * axis_line_width);
*/

    projected_min_diff =
      (gint) ((gdouble) scale_min_diff + axis_line_width);

  /* FIXME: this is not really not an elegant solution here */
  for (
    tics_list = gdv_axis_get_tic_list (axis);
    tics_list;
    tics_list = tics_list->next)
  {
    GdvTic *local_tic = tics_list->data;
    gint loc_tic_min_start, loc_tic_nat_start,
         loc_tic_min_stop, loc_tic_nat_stop;
    gint loc_tic_min_size = 0, loc_tic_nat_size = 0;

    if (orientation == GTK_ORIENTATION_HORIZONTAL)
    {
      gdv_tic_get_space_to_tic_position (
        local_tic, GTK_POS_LEFT, for_size,
        &loc_tic_min_start, &loc_tic_nat_start, data);
      gdv_tic_get_space_to_tic_position (
        local_tic, GTK_POS_RIGHT, for_size,
        &loc_tic_min_stop, &loc_tic_nat_stop, data);

      /* this is more for the record and not obligatoric */
      if (for_size > 0)
        gtk_widget_get_preferred_width_for_height (
          GTK_WIDGET (local_tic), for_size, &loc_tic_min_size, &loc_tic_nat_size);
      else
        gtk_widget_get_preferred_width (
          GTK_WIDGET (local_tic), &loc_tic_min_size, &loc_tic_nat_size);
    }
    else
    {
      gdv_tic_get_space_to_tic_position (
        local_tic, GTK_POS_TOP, for_size,
        &loc_tic_min_start, &loc_tic_nat_start, data);
      gdv_tic_get_space_to_tic_position (
        local_tic, GTK_POS_BOTTOM, for_size,
        &loc_tic_min_stop, &loc_tic_nat_stop, data);

      /* this is more fore the record and not obligatoric */
      if (for_size > 0)
        gtk_widget_get_preferred_height_for_width (
          GTK_WIDGET (local_tic), for_size, &loc_tic_min_size, &loc_tic_nat_size);
      else
        gtk_widget_get_preferred_height (
          GTK_WIDGET (local_tic), &loc_tic_min_size, &loc_tic_nat_size);
    }

    glob_tic_min_start =
      (glob_tic_min_start < loc_tic_min_start ?
       loc_tic_min_start : glob_tic_min_start);
    glob_tic_min_stop =
      glob_tic_min_stop < loc_tic_min_stop ?
      loc_tic_min_stop : glob_tic_min_stop;
    glob_tic_nat_start =
      glob_tic_nat_start < loc_tic_nat_start ?
      loc_tic_nat_start : glob_tic_nat_start;
    glob_tic_nat_stop =
      glob_tic_nat_stop < loc_tic_nat_stop ?
      loc_tic_nat_stop : glob_tic_nat_stop;
  }

  /* Getting the size of the minor tics */
  /*  for (
      tics_list = axis->priv->mtics;
      tics_list;
      tics_list = tics_list->next)
    {
      GdvTic *local_tic = tics_list->data;
      gint loc_tic_min_start, loc_tic_nat_start,
           loc_tic_min_stop, loc_tic_nat_stop;
      gint loc_tic_min_size = 0, loc_tic_nat_size = 0;

      if (orientation == GTK_ORIENTATION_HORIZONTAL)
      {
        gdv_tic_get_space_to_tic_position (
          local_tic, GTK_POS_LEFT, for_size,
          &loc_tic_min_start, &loc_tic_nat_start, data);
        gdv_tic_get_space_to_tic_position (
          local_tic, GTK_POS_RIGHT, for_size,
          &loc_tic_nat_start, &loc_tic_nat_stop, data);
  */
  /* this is more fore the record and not obligatoric */
  /*      if (for_size > 0)
          gtk_widget_get_preferred_width_for_height (
            GTK_WIDGET (local_tic), for_size, &loc_tic_min_size, &loc_tic_nat_size);
        else
          gtk_widget_get_preferred_width (
            GTK_WIDGET (local_tic), &loc_tic_min_size, &loc_tic_nat_size);
      }
      else
      {
        gdv_tic_get_space_to_tic_position (
          local_tic, GTK_POS_TOP, for_size,
          &loc_tic_min_start, &loc_tic_nat_start, data);
        gdv_tic_get_space_to_tic_position (
          local_tic, GTK_POS_BOTTOM, for_size,
          &loc_tic_nat_start, &loc_tic_nat_stop, data);
  */
  /* this is more fore the record and not obligatoric */
  /*      if (for_size > 0)
          gtk_widget_get_preferred_height_for_width (
            GTK_WIDGET (local_tic), for_size, &loc_tic_min_size, &loc_tic_nat_size);
        else
          gtk_widget_get_preferred_height (
            GTK_WIDGET (local_tic), &loc_tic_min_size, &loc_tic_nat_size);
      }

      glob_tic_min_start =
        glob_tic_min_start < loc_tic_min_start ?
          loc_tic_min_start : glob_tic_min_start;
      glob_tic_min_stop =
        glob_tic_min_stop < loc_tic_min_stop ?
          loc_tic_min_stop : glob_tic_min_stop;
      glob_tic_nat_start =
        glob_tic_nat_start < loc_tic_nat_start ?
          loc_tic_nat_start : glob_tic_nat_start;
      glob_tic_nat_stop =
        glob_tic_nat_stop < loc_tic_nat_stop ?
          loc_tic_nat_stop : glob_tic_nat_stop;
    }
  */
  *minimum =
    glob_tic_min_start + glob_tic_min_stop + projected_min_diff;
  *natural =
    glob_tic_nat_start + glob_tic_nat_stop + projected_min_diff;

  /* Measuring the title */
  g_object_get (axis, "axis-direction-outside", &outside_dir, NULL);

  axis_title_on = gdv_axis_get_show_title (axis);

  if (axis_title_on)
  {
    if (orientation == GTK_ORIENTATION_VERTICAL)
    {
      if (for_size > 0)
        gtk_widget_get_preferred_height_for_width (
          title_widget, for_size, &title_height, &title_height_nat);
      else
        gtk_widget_get_preferred_height (
          title_widget, &title_height, &title_height_nat);

      *minimum += (gint) (fabs(cos(outside_dir)) * (gdouble) title_height);
      *natural += (gint) (fabs(cos(outside_dir)) * (gdouble) title_height_nat);

      *minimum =
        (gint) fmax(
          (gdouble) * minimum,
          fabs(sin(outside_dir)) * (gdouble) title_height);
      *natural =
        (gint) fmax(
          (gdouble) * natural,
          fabs(sin(outside_dir)) * (gdouble) title_height_nat);
    }
    else
    {
      if (for_size > 0)
        gtk_widget_get_preferred_width_for_height (
          title_widget, for_size, &title_width, &title_width_nat);
      else
        gtk_widget_get_preferred_width (
          title_widget, &title_width, &title_width_nat);

      *minimum += (gint) (fabs(sin(outside_dir)) * (gdouble) title_width);
      *natural += (gint) (fabs(sin(outside_dir)) * (gdouble) title_width_nat);

      *minimum =
        (gint) fmax(
          (gdouble) * minimum,
          fabs(cos(outside_dir)) * (gdouble) title_width);
      *natural =
        (gint) fmax(
          (gdouble) * natural,
          fabs(cos(outside_dir)) * (gdouble) title_width_nat);
    }
  }
}


static void
gdv_polar_axis_get_preferred_width (GtkWidget           *widget,
                              gint                *minimum_size,
                              gint                *natural_size)
{
  gdv_polar_axis_measure (GDV_AXIS (widget),
                    GTK_ORIENTATION_HORIZONTAL,
                    -1,
                    minimum_size, natural_size,
                    NULL, NULL, NULL);
}

static void
gdv_polar_axis_get_preferred_height (GtkWidget           *widget,
                               gint                *minimum_size,
                               gint                *natural_size)
{
  gdv_polar_axis_measure (GDV_AXIS (widget),
                    GTK_ORIENTATION_VERTICAL,
                    -1,
                    minimum_size, natural_size,
                    NULL, NULL, NULL);
}

static void
gdv_polar_axis_get_preferred_height_for_width (GtkWidget           *widget,
    gint                 width,
    gint                *minimum_height,
    gint                *natural_height)
{
  gdv_polar_axis_measure (GDV_AXIS (widget),
                    GTK_ORIENTATION_VERTICAL,
                    width,
                    minimum_height, natural_height,
                    NULL, NULL, NULL);
}


static void
gdv_polar_axis_get_preferred_width_for_height (GtkWidget           *widget,
    gint                 height,
    gint                *minimum_width,
    gint                *natural_width)
{
  gdv_polar_axis_measure (GDV_AXIS (widget),
                    GTK_ORIENTATION_HORIZONTAL,
                    height,
                    minimum_width, natural_width,
                    NULL, NULL, NULL);
}

static void gdv_special_polar_axis_set_property (GObject      *object,
                                      guint         property_id,
                                      const GValue *value,
                                      GParamSpec   *pspec)
{
  GdvSpecialPolarAxis *self = GDV_SPECIAL_POLAR_AXIS (object);

  switch (property_id)
    {
    case PROP_DRAW_FROM_ANGLE:
      self->priv->draw_from_angle = g_value_get_double (value);
      break;

    case PROP_DRAW_TO_ANGLE:
      self->priv->draw_to_angle = g_value_get_double (value);
      break;

    default:
      /* unknown property */
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

static void gdv_special_polar_axis_get_property (GObject    *object,
                                      guint       property_id,
                                      GValue     *value,
                                      GParamSpec *pspec)
{
  GdvSpecialPolarAxis *self = GDV_SPECIAL_POLAR_AXIS (object);

  switch (property_id)
    {
    case PROP_DRAW_FROM_ANGLE:
      g_value_set_double (value, self->priv->draw_from_angle);
      break;

    case PROP_DRAW_TO_ANGLE:
      g_value_set_double (value, self->priv->draw_to_angle);
      break;

    default:
      /* unknown property */
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;

    }
}


/*
static void
gdv_special_polar_axis_size_allocate (GtkWidget           *widget,
                               GtkAllocation       *allocation)
{
  g_print ("*");

  GTK_WIDGET_CLASS (gdv_special_polar_axis_parent_class)->
    size_allocate (widget, allocation);
}
*/

static const gchar *
_get_month_name (gint month)
{
    switch (month)
    {
    case 1:
      return "Jan";
    case 2:
      return "Feb";
    case 3:
      return "Mar";
    case 4:
      return "Apr";
    case 5:
      return "May";
    case 6:
      return "Jun";
    case 7:
      return "Jul";
    case 8:
      return "Aug";
    case 9:
      return "Sep";
    case 10:
      return "Oct";
    case 11:
      return "Nov";
    case 12:
      return "Dec";
    default:
      return "UKN";
    }
}

/**
 * gdv_special_polar_axis_new:
 *
 * Just a common constructor.
 *
 * Returns: a new #GdvSpecialPolarAxis
 **/
GdvSpecialPolarAxis *gdv_special_polar_axis_new (void)
{
  return g_object_new (gdv_special_polar_axis_get_type (),
                       NULL);
}

