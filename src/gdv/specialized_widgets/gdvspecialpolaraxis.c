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
#include "gdvrender.h"

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

  gdouble cent_pix_x;
  gdouble cent_pix_y;
  gdouble radius;

//  gdouble axis_beg_pix_x;
//  gdouble axis_beg_pix_y;
//  gdouble axis_end_pix_x;
//  gdouble axis_end_pix_y;
  gboolean resize_during_redraw;
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

static gboolean gdv_special_polar_axis_get_point (
  GdvAxis            *axis,
  gdouble             value,
  gdouble            *pos_x,
  gdouble            *pos_y);

static gboolean gdv_special_polar_axis_get_inner_dir(
  GdvAxis            *axis,
  gdouble             value,
  gdouble            *pos_x,
  gdouble            *pos_y);

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
//  widget_class->get_preferred_height =
//    gdv_polar_axis_get_preferred_height;
//  widget_class->get_preferred_height_for_width =
//    gdv_polar_axis_get_preferred_height_for_width;
//  widget_class->get_preferred_width_for_height =
//    gdv_polar_axis_get_preferred_width_for_height;

  axis_class->get_point =
    gdv_special_polar_axis_get_point;
  axis_class->get_inner_dir =
    gdv_special_polar_axis_get_inner_dir;

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
  GdvSpecialPolarAxis *polar_axis = GDV_SPECIAL_POLAR_AXIS (widget);
  GdvSpecialPolarAxisPrivate *priv = gdv_special_polar_axis_get_instance_private(polar_axis);

  GdvLinearAxis *linear_axis = GDV_LINEAR_AXIS (widget);
  GtkAllocation axis_allocation = *allocation;

  gboolean scale_automatic;

  gboolean scale_auto_increment;
  gdouble scale_beg_val,
          scale_end_val;
  gdouble scale_increment_val;
  gdouble signed_scale_increment_val;
  gdouble scale_increment_base;
  gdouble init_scale_beg_val, init_scale_end_val;

  gint scale_min_diff_pix;
  gint scale_max_diff_pix;
  gboolean first_iteration = TRUE;

  gdouble angle_to_outer_dir, angle_to_start;

  gdouble init_increment_val;
//  gdouble   init_tic_beg_val, init_tic_end_val;

  gdouble inner_dir_x = 0.0, inner_dir_y = 0.0;
//  gdouble   axis_dir_x = 0.0, axis_dir_y = 0.0;

  gboolean force_beg_end;

  gboolean tics_automatic;
  gdouble tmp_tics_val_beg, tmp_tics_val_end;
  gdouble tics_beg_val,
          tics_end_val;
  gboolean beg_is_new = FALSE, end_is_new = FALSE;

  gdouble tic_label_halign, tic_label_valign;

  gboolean mtics_automatic;
  gdouble mtics_beg_val,
          mtics_end_val;
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

  /* needed for the autoadjustment of scale- and tic-values */
  GList *tic_list, *tic_list_start;
  GtkAllocation space_without_border;

  gboolean visible;

  g_object_get (
    G_OBJECT (polar_axis),
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

  priv->cent_pix_x = allocation->x + 0.5 * allocation->width;
  priv->cent_pix_y = allocation->y + 0.5 * allocation->height;
  priv->radius = allocation->width < allocation->height ?
                   allocation->width * 0.5 :
                   allocation->height * 0.5;

  previouse_tics = gdv_axis_get_tic_list (GDV_AXIS (linear_axis));

  for (tics_copy = previouse_tics;
       tics_copy != NULL;
       tics_copy = tics_copy->next)
  {
  }

  GTK_WIDGET_CLASS (gdv_special_polar_axis_parent_class)->size_allocate (widget, allocation);
}

static gboolean
gdv_polar_axis_draw (GtkWidget    *widget,
                     cairo_t      *cr)
{
//  gdouble   scale_beg_val, scale_end_val;
  gdouble axis_line_width = 0.0;
  GtkStyleContext *context;
  GdvSpecialPolarAxis *polar_axis = GDV_SPECIAL_POLAR_AXIS (widget);
  GdvSpecialPolarAxisPrivate *priv = gdv_special_polar_axis_get_instance_private(polar_axis);

  context = gtk_widget_get_style_context (widget);

  gtk_widget_style_get (widget,
                        "line-width", &axis_line_width,
                        NULL);

//  if (axis_line_width && cr != NULL &&
//      (!axis->priv->resize_during_redraw || axis->priv->force_beg_end))
  if (axis_line_width && cr != NULL)
  {
    /* plotting axis-line */
    gdv_render_arc (
      context, cr, priv->cent_pix_x,
                   priv->cent_pix_y,
                   priv->radius, 0.0, 2.0 * M_PI);

//  if (axis->priv->resize_during_redraw && !axis->priv->force_beg_end)
//  {
//    gtk_widget_queue_resize (GTK_WIDGET (widget));
//    axis->priv->resize_during_redraw = FALSE;
//    return TRUE;
  }

  return FALSE;
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

  *minimum = 100;
  *natural = 100;
  return;


  gtk_widget_style_get (GTK_WIDGET (axis),
                        "scale-min-diff-pix", &scale_min_diff,
                        "line-width", &axis_line_width,
                        NULL);

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

static gboolean gdv_special_polar_axis_get_point (
  GdvAxis            *axis,
  gdouble             value,
  gdouble            *pos_x,
  gdouble            *pos_y)
{
  GdvSpecialPolarAxis *self = GDV_SPECIAL_POLAR_AXIS (axis);
  GdvSpecialPolarAxisPrivate * priv = gdv_special_polar_axis_get_instance_private (self);
  gdouble beg_val, end_val,
          //begin_x, end_x, begin_y, end_y,
          orientation;
  gdouble tmp_slope_angle;
  gdouble pos_angle;

  g_object_get(G_OBJECT(axis),
               "scale-beg-val", &beg_val,
               "scale-end-val", &end_val,
//               "axis-beg-pix-x", &begin_x,
//               "axis-end-pix-x", &end_x,
//               "axis-beg-pix-y", &begin_y,
//               "axis-end-pix-y", &end_y,
               "axis-orientation", &orientation,
               NULL);

  /* The following part assumes that the allocation process fixed the values for the
   * angle boundaries of the axis already correctly (draw_from_angle and draw_to_angle members).
   * The same applies for the (radial symmetric) center of this axis
   */

  if (priv->draw_from_angle != priv->draw_to_angle)
  {
    tmp_slope_angle = (beg_val - end_val) / (priv->draw_from_angle - priv->draw_to_angle);
    pos_angle = priv->draw_from_angle + (value - beg_val) / tmp_slope_angle;
  }
  else
  {
    pos_angle = priv->draw_from_angle;
  }

  *pos_x = priv->cent_pix_x + priv->radius * sin(pos_angle);
  *pos_y = priv->cent_pix_x + priv->radius * cos(pos_angle);

  return (
    (value <= (beg_val >= end_val ? beg_val : end_val)) &&
    (value >= (beg_val <= end_val ? beg_val : end_val)));
}

static gboolean gdv_special_polar_axis_get_inner_dir(
  GdvAxis            *axis,
  gdouble             value,
  gdouble            *pos_x,
  gdouble            *pos_y)
{
  GdvSpecialPolarAxis *self = GDV_SPECIAL_POLAR_AXIS (axis);
  GdvSpecialPolarAxisPrivate * priv = gdv_special_polar_axis_get_instance_private (self);
  gdouble beg_val, end_val,
          //begin_x, end_x, begin_y, end_y,
          orientation;
  gdouble tmp_slope_angle;
  gdouble pos_angle;

  g_object_get(G_OBJECT(axis),
               "scale-beg-val", &beg_val,
               "scale-end-val", &end_val,
//               "axis-beg-pix-x", &begin_x,
//               "axis-end-pix-x", &end_x,
//               "axis-beg-pix-y", &begin_y,
//               "axis-end-pix-y", &end_y,
               "axis-orientation", &orientation,
               NULL);

  if (priv->draw_from_angle != priv->draw_to_angle)
  {
    tmp_slope_angle = (beg_val - end_val) / (priv->draw_from_angle - priv->draw_to_angle);
    pos_angle = priv->draw_from_angle + (value - beg_val) / tmp_slope_angle;
  }
  else
  {
    pos_angle = priv->draw_from_angle;
  }

  *pos_x = -1.0 * sin(pos_angle);
  *pos_y = -1.0 * cos(pos_angle);

  return (
    (value <= (beg_val >= end_val ? beg_val : end_val)) &&
    (value >= (beg_val <= end_val ? beg_val : end_val)));

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

