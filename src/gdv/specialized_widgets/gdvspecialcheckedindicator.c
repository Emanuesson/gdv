/*
 * gdvspecialcheckedindicator.c
 * This file is part of gdv
 *
 * Copyright (C) 2017 - Emanuel Schmidt
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

#include "gdvaxis.h"
#include "gdvrender.h"
#include "gdv-enums.h"

#include "gdvspecialcheckedindicator.h"

/**
 * SECTION:gdvspecialcheckedindicator
 * @title: GdvSpecialCheckedIndicator
 * @short_description: an special indicator, with checked design
 *
 * # CSS nodes
 *
 * GdvSpecialCheckedIndicator uses a single CSS node with name special-checked-indicator.
 */

/* Define Properties */
enum
{
  PROP_0,

  PROP_IN_L,
  PROP_OUT_L,

  N_PROPERTIES
};

struct _GdvSpecialCheckedIndicatorPrivate
{
  /* potentially style properties */
  gdouble in_length;
  gdouble out_length;

  GdvDistributionType distro;
  gdouble check_angle;
  gdouble inner_offset;
  gdouble arrow_height;

  gdouble bottom_broad;
  gdouble top_broad;

};

static GParamSpec *indicator_properties[N_PROPERTIES] = { NULL, };

G_DEFINE_TYPE_WITH_PRIVATE (GdvSpecialCheckedIndicator,
  gdv_special_checked_indicator, GDV_TYPE_INDICATOR)

static void
gdv_special_checked_indicator_init (GdvSpecialCheckedIndicator *indicator)
{
  indicator->priv =
    gdv_special_checked_indicator_get_instance_private (indicator);

  indicator->priv->in_length = 10.0;
  indicator->priv->out_length = 1.0;

  indicator->priv->distro = GDV_SYMMETRIC_DIST;
  indicator->priv->check_angle = 0.25 * M_PI;
  indicator->priv->inner_offset = 2.0;
  indicator->priv->arrow_height = 5.0;

  indicator->priv->bottom_broad = 2.0;
  indicator->priv->top_broad = 5.0;
}

static void
gdv_special_checked_indicator_set_property (
  GObject      *object,
  guint         property_id,
  const GValue *value,
  GParamSpec   *pspec)
{
  GdvSpecialCheckedIndicator *self;

  self = GDV_SPECIAL_CHECKED_INDICATOR (object);

  switch (property_id)
  {
  case PROP_IN_L:
    self->priv->in_length = g_value_get_double (value);
    break;
  case PROP_OUT_L:
    self->priv->out_length = g_value_get_double (value);
    break;
  default:
    /* unknown property */
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }
}


static void
gdv_special_checked_indicator_get_property (
  GObject    *object,
  guint       property_id,
  GValue     *value,
  GParamSpec *pspec)
{
  GdvSpecialCheckedIndicator *self =
    GDV_SPECIAL_CHECKED_INDICATOR (object);

  switch (property_id)
  {
  case PROP_IN_L:
    g_value_set_double (value, self->priv->in_length);
    break;
  case PROP_OUT_L:
    g_value_set_double (value, self->priv->out_length);
    break;
  default:
    /* unknown property */
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }
}

static
gboolean gdv_special_checked_indicator_draw (
  GtkWidget   *widget,
  cairo_t     *cr)
{
  GdvSpecialCheckedIndicator *indicator =
    GDV_SPECIAL_CHECKED_INDICATOR (widget);
  GdvAxis *axis = GDV_AXIS (gtk_widget_get_parent (widget));

  gboolean on_axis, tmp_on_axis;

  gdouble x_position, y_position;
  gdouble x_inner, y_inner;
  GtkStyleContext *context;

  gdouble value;

  gboolean show_in_out_of_range;

  /* FIXME: Does this even make sense? Look up examples from Gtk+ */
  if (!axis)
  {
    g_warning ("parent-widget for %s is not set",
               g_type_name (G_TYPE_FROM_INSTANCE (widget)));
    return TRUE;
  }

  g_object_get (indicator,
    "value", &value,
    "show-in-out-of-range", &show_in_out_of_range,
    NULL);

  g_signal_emit_by_name (
    axis,
    "get-point",
    value,
    &x_position,
    &y_position,
    &on_axis);

  g_signal_emit_by_name (
    axis,
    "get-inner-dir",
    value,
    &x_inner,
    &y_inner,
    &tmp_on_axis);

/*
  indicator->priv->distro = GDV_SYMMETRIC_DIST;
  indicator->priv->check_angle = 0.25 * M_PI;
  indicator->priv->inner_offset = 2.0;
  indicator->priv->arrow_height = 5.0;

  indicator->priv->bottom_broad = 2.0;
  indicator->priv->top_broad = 5.0;
*/

//  {
//
//    cairo_move_to (cr, x0 + 0.5, y0 + 0.5);
//    cairo_line_to (cr, x1 + 0.5, y1 + 0.5);
//    cairo_set_line_width (cr, line_width);
//
//    g_print ("LC\n");
//    gdk_cairo_set_source_rgba(cr, color);
//
//    cairo_set_dash (cr, dash_array, dash_length, 0.0);
//
//    cairo_stroke (cr);
//
//  }


  /* TODO: remember to make this an property, if implemented as allocation */
  /* skip indicators outside the range of the axis */
  if ((on_axis && tmp_on_axis) || show_in_out_of_range)
  {
/*
    GdkRGBA *color;
    gdouble axis_line_width;
    GValue value_lw = G_VALUE_INIT;
    GValue value_lc = G_VALUE_INIT;
//    GValue *value_lw = NULL;
//    GValue *value_lc = NULL;

    g_value_init (&value_lw, G_TYPE_DOUBLE);
    g_value_init (&value_lc, GDK_TYPE_RGBA);

    cairo_save (cr);
*/
    context = gtk_widget_get_style_context (widget);
/*
    gtk_style_context_get_style_property (context,
                                          "line-width", &value_lw);
    gtk_style_context_get_style_property (context,
                                          "color", &value_lc);

    axis_line_width = g_value_get_double (&value_lw);
    color = g_value_get_boxed (&value_lc);

    cairo_set_line_cap (cr, CAIRO_LINE_CAP_SQUARE);
    cairo_set_line_width (cr, axis_line_width);

    cairo_move_to (cr, x0 + 0.5, y0 + 0.5);
    cairo_line_to (cr, x1 + 0.5, y1 + 0.5);
    gdk_cairo_set_source_rgba(cr, color);

    cairo_stroke (cr);

    cairo_restore (cr);

    g_value_unset (&value_lw);
    g_value_unset (&value_lc);
*/



    gdv_render_line (context, cr,
      x_position + indicator->priv->in_length * x_inner,
      y_position + indicator->priv->in_length * y_inner,
      x_position - indicator->priv->out_length * x_inner,
      y_position - indicator->priv->out_length * y_inner);
  }

  return FALSE;
}

static void
gdv_special_checked_indicator_class_init (GdvSpecialCheckedIndicatorClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->set_property = gdv_special_checked_indicator_set_property;
  object_class->get_property = gdv_special_checked_indicator_get_property;

  widget_class->draw = gdv_special_checked_indicator_draw;

  gtk_widget_class_set_css_name (widget_class,
    "special-checked-indicator");


  /* Properties */

  /**
   * GdvSpecialCheckedIndicator:in-length:
   *
   * Determines inner length of the indicator.
   */
  indicator_properties[PROP_IN_L] =
    g_param_spec_double ("in-length",
                         "data value",
                         "Determines the current value of the indicator",
                         -G_MAXDOUBLE,
                         G_MAXDOUBLE,
                         10.0,
                         G_PARAM_READWRITE);

  /**
   * GdvSpecialCheckedIndicator:scale-min-val:
   *
   * Determines the minimum value of the indicator.
   */
  indicator_properties[PROP_OUT_L] =
    g_param_spec_double ("out-length",
                         "data value",
                         "Determines the current value of the indicator",
                         -G_MAXDOUBLE,
                         G_MAXDOUBLE,
                         1.0,
                         G_PARAM_READWRITE);


  g_object_class_install_properties (object_class,
                                     N_PROPERTIES,
                                     indicator_properties);

  /* Style-Properties */

  /**
   * GdvSpecialCheckedIndicator:sec-color:
   *
   * Sets a secondary color to draw the indicator.
   */
  gtk_widget_class_install_style_property (widget_class,
      g_param_spec_boxed ("sec-color",
                          _("secondary color of indicator"),
                          _("Second Color to draw indicator"),
                          GDK_TYPE_RGBA,
                          G_PARAM_READWRITE));

}

/**
 * gdv_special_checked_indicator_new:
 *
 * Just a common constructor.
 *
 * Returns: a new #GdvSpecialCheckedIndicator
 **/
GdvSpecialCheckedIndicator *gdv_special_checked_indicator_new (void)
{
  return g_object_new (gdv_special_checked_indicator_get_type (),
                       NULL);
}

