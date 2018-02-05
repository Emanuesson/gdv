/*
 * gdvindicator.c
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
#include <cairo-gobject.h>

#include "gdvindicator.h"
#include "gdv/gdv-data-boxed.h"
#include "gdvaxis.h"
#include "gdvrender.h"

/* TODO: I should take a look at GtkRange to see something similar and adapt it
 *       for the indicator-widget. In the end there should be a similar functionality
 *       betweem am GtkRange-widget and an GdvIndicator
 */

/**
 * SECTION:gdvindicator
 * @title: GdvIndicator
 * @short_description: an indicator on an axis
 *
 * #GdvIndicator is an object-class, that displays an indicator for any given
 * value on a single axis.
 *
 * # CSS nodes
 *
 * GdvIndicator uses a single CSS node with name indicator.
 */

/* Define Signals */
enum
{
  GET_POINT,
  LAST_SIGNAL
};

/* Define Properties */
enum
{
  PROP_0,

  PROP_DATA_VAL,
  PROP_TITLE,
  PROP_SHOW_IN_LEGEND,
  PROP_SHOW_IN_AOR,

  PROP_WIDGET,
  PROP_WIDGET_XALIGN,
  PROP_WIDGET_YALIGN,
  PROP_WIDGET_ROTATE_WITH_AXIS,

  N_PROPERTIES
};

struct _GdvIndicatorPrivate
{
  /* value */
  gdouble value;

  /* potentially style properties */
  gboolean show_indicator_tic;
  gboolean show_indicator_label;
  gboolean show_indicator_box;

  gchar *title;
  gboolean show_in_legend;

  GtkWidget *widget;
  gdouble widget_xalign;
  gdouble widget_yalign;
  gboolean widget_rotate_with_axis;

  gboolean show_in_out_of_range;
};

static GParamSpec *indicator_properties[N_PROPERTIES] = { NULL, };

static void
gdv_indicator_dispose (GObject *object);

static void
gdv_indicator_finalize (GObject *object);

G_DEFINE_TYPE_WITH_PRIVATE (GdvIndicator, gdv_indicator, GTK_TYPE_CONTAINER);

static void
gdv_indicator_init (GdvIndicator *indicator)
{
  GtkStyleContext *style_context;
  GtkCssProvider *css_provider;

  GtkWidget *widget;

  widget = GTK_WIDGET (indicator);

  gtk_widget_set_name (widget, "new indicator");

  css_provider = gtk_css_provider_new ();
  style_context = gtk_widget_get_style_context (widget);
  gtk_css_provider_load_from_resource (
    css_provider,
    "/net/gdv/libgdv/themes/default_general.css");

  gtk_style_context_add_provider (
    style_context,
    GTK_STYLE_PROVIDER (css_provider),
    GTK_STYLE_PROVIDER_PRIORITY_FALLBACK);

  indicator->priv = gdv_indicator_get_instance_private (indicator);

  indicator->priv->title = g_strdup ("new indicator\0");
  indicator->priv->show_in_legend = TRUE;
  indicator->priv->show_in_out_of_range = FALSE;

  gtk_widget_set_can_focus (widget, TRUE);
  gtk_widget_set_receives_default (widget, TRUE);
  gtk_widget_set_has_window (widget, FALSE);

  indicator->priv->value = 0.0;
}

static void
gdv_indicator_set_property (GObject      *object,
                            guint         property_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
  GdvIndicator *self;

  self = GDV_INDICATOR (object);

  switch (property_id)
  {
  case PROP_DATA_VAL:
    self->priv->value = g_value_get_double (value);
    break;
  case PROP_TITLE:
    if (self->priv->title)
      g_free (self->priv->title);
    self->priv->title = g_strdup (g_value_get_string (value));
    break;
  case PROP_SHOW_IN_LEGEND:
    self->priv->show_in_legend = g_value_get_boolean (value);
    break;
  case PROP_SHOW_IN_AOR:
    self->priv->show_in_out_of_range = g_value_get_boolean (value);
    break;
  case PROP_WIDGET:
    /* FIXME: implement this somehow better! */
    self->priv->widget = g_value_dup_object (value);
    break;
  case PROP_WIDGET_XALIGN:
    self->priv->widget_xalign = g_value_get_double (value);
    break;
  case PROP_WIDGET_YALIGN:
    self->priv->widget_yalign = g_value_get_double (value);
    break;
  case PROP_WIDGET_ROTATE_WITH_AXIS:
    self->priv->widget_rotate_with_axis = g_value_get_boolean (value);
    break;
  default:
    /* unknown property */
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }
}


static void
gdv_indicator_get_property (GObject    *object,
                            guint       property_id,
                            GValue     *value,
                            GParamSpec *pspec)
{
  GdvIndicator *self = GDV_INDICATOR (object);

  switch (property_id)
  {
  case PROP_DATA_VAL:
    g_value_set_double (value, self->priv->value);
    break;
  case PROP_TITLE:
    if (self->priv->title)
      g_value_set_string (value, self->priv->title);
    else
      g_value_set_static_string (value, "");
    break;
  case PROP_SHOW_IN_LEGEND:
    g_value_set_boolean (value, self->priv->show_in_legend);
    break;
  case PROP_SHOW_IN_AOR:
    g_value_set_boolean (value, self->priv->show_in_out_of_range);
    break;
  case PROP_WIDGET:
    g_value_set_object (value, self->priv->widget);
    break;
  case PROP_WIDGET_XALIGN:
    g_value_set_double (value, self->priv->widget_xalign);
    break;
  case PROP_WIDGET_YALIGN:
    g_value_set_double (value, self->priv->widget_yalign);
    break;
  case PROP_WIDGET_ROTATE_WITH_AXIS:
    g_value_set_boolean (value, self->priv->widget_rotate_with_axis);
    break;
  default:
    /* unknown property */
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }
}

static void
gdv_indicator_dispose (GObject *object)
{
  G_OBJECT_CLASS (gdv_indicator_parent_class)->dispose (object);
}

static void
gdv_indicator_finalize (GObject *object)
{
  G_OBJECT_CLASS (gdv_indicator_parent_class)->finalize (object);
}

static void gdv_indicator_size_allocate         (GtkWidget              *widget,
    GtkAllocation          *allocation);



static void gdv_indicator_get_preferred_width (
  GtkWidget           *widget,
  gint                *minimum_size,
  gint                *natural_size);
static void gdv_indicator_get_preferred_height (
  GtkWidget           *widget,
  gint                *minimum_size,
  gint                *natural_size);
static void gdv_indicator_get_preferred_height_for_width(
  GtkWidget           *widget,
  gint                 width,
  gint                *minimum_height,
  gint                *natural_height);
static void gdv_indicator_get_preferred_width_for_height(
  GtkWidget           *widget,
  gint                 height,
  gint                *minimum_width,
  gint                *natural_width);


static void
gdv_indicator_realize (GtkWidget *widget)
{
  GtkAllocation allocation;
  GdkWindow *window;

  gtk_widget_get_allocation (widget, &allocation);


  gtk_widget_set_realized (widget, TRUE);


  window = gtk_widget_get_parent_window (widget);
  gtk_widget_set_window (widget, window);
  g_object_ref (window);
}

static void
gdv_indicator_unrealize (GtkWidget *widget)
{
  GTK_WIDGET_CLASS (gdv_indicator_parent_class)->unrealize (widget);
}

static void
gdv_indicator_size_allocate (GtkWidget     *widget,
                             GtkAllocation *allocation)
{
  gtk_widget_set_allocation (widget, allocation);

}

static
gboolean gdv_indicator_draw (GtkWidget   *widget,
                             cairo_t     *cr)
{
  GdvIndicator *indicator = GDV_INDICATOR (widget);
  GdvAxis *axis = GDV_AXIS (gtk_widget_get_parent (widget));
  gboolean on_axis;
  gdouble x_position, y_position;
  GtkStyleContext *context;

  if (!axis)
    g_warning ("parent-widget for %s(%p) is not set",
               g_type_name (G_TYPE_FROM_INSTANCE (widget)), widget);

  g_signal_emit_by_name (
    axis,
    "get-point",
    indicator->priv->value,
    &x_position,
    &y_position,
    &on_axis);

  /* TODO: remember to make this an property, if implemented as allocation */
  /* skip indicators outside the range of the axis */
  if (on_axis || indicator->priv->show_in_out_of_range)
  {
    context = gtk_widget_get_style_context (widget);
    gdv_render_line (context, cr,
                     x_position, y_position + 10.0,
                     x_position, y_position - 10.0);
    gdv_render_line (context, cr,
                     x_position + 10.0, y_position,
                     x_position - 10.0, y_position);
  }

  return FALSE;
}

static void
gdv_indicator_class_init (GdvIndicatorClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->dispose = gdv_indicator_dispose;
  object_class->finalize = gdv_indicator_finalize;
  object_class->set_property = gdv_indicator_set_property;
  object_class->get_property = gdv_indicator_get_property;

  widget_class->get_preferred_width =
    gdv_indicator_get_preferred_width;
  widget_class->get_preferred_height =
    gdv_indicator_get_preferred_height;
  widget_class->get_preferred_height_for_width =
    gdv_indicator_get_preferred_height_for_width;
  widget_class->get_preferred_width_for_height =
    gdv_indicator_get_preferred_width_for_height;

  widget_class->realize = gdv_indicator_realize;
  widget_class->unrealize = gdv_indicator_unrealize;
  widget_class->size_allocate = gdv_indicator_size_allocate;
  widget_class->draw = gdv_indicator_draw;

  gtk_widget_class_set_css_name (widget_class, "indicator");


  /* Properties */

  /**
   * GdvIndicator:scale-min-val:
   *
   * Determines the minimum value of the indicator.
   */
  indicator_properties[PROP_DATA_VAL] =
    g_param_spec_double ("value",
                         "data value",
                         "Determines the current value of the indicator",
                         -G_MAXDOUBLE,
                         G_MAXDOUBLE,
                         0.0,
                         G_PARAM_READWRITE);

  /**
   * GdvIndicator:title:
   *
   * The title of an indicator.
   *
   * This property is pretty useless for the moment.
   */
  indicator_properties[PROP_TITLE] =
    g_param_spec_string ("title",
                         "content-title",
                         "the title that will be shown in the legend",
                         "new indicator",
                         G_PARAM_READWRITE);

  /**
   * GdvIndicator:show-in-legend:
   *
   * If the indicator should be listed in the legend of the affilliated layer.
   */
  indicator_properties[PROP_SHOW_IN_LEGEND] =
    g_param_spec_boolean ("show-in-legend",
                          "indicator shown in legend",
                          "determines if the indicator-title is shown or hidden "
                          "in the legend",
                          TRUE,
                          G_PARAM_READWRITE);

  /**
   * GdvIndicator:show-in-out-of-range:
   *
   * If the indicator should be shown even if the value is out of the range of
   * the assigned axis. In some technical applications and if the indicator value
   * is more important, than an actual remark, this should be considered.
   */
  indicator_properties[PROP_SHOW_IN_AOR] =
    g_param_spec_boolean ("show-in-out-of-range",
                          "show indicator even after leaving the axis-range",
                          "determines if the indicator is shown or hidden "
                          "after leaving the range of the assigned axis",
                          FALSE,
                          G_PARAM_READWRITE);

  /**
   * GdvIndicator:widget:
   *
   * A widget, that should be displayed instead of the indicator.
   */
  indicator_properties[PROP_WIDGET] =
    g_param_spec_object ("widget",
                         "the widget related to the indicator",
                         "determines the widget, that will be displayed on "
                         "the position of the indicator",
                          GTK_TYPE_WIDGET,
                          G_PARAM_READWRITE);


  /**
   * GdvIndicator:widget-xalign:
   *
   * The horizontal alignment of the widget.
   */
  indicator_properties[PROP_WIDGET_XALIGN] =
    g_param_spec_double ("widget-xalign",
                         "horizontal widget offset",
                         "determines the offset to diplay the widget",
                         -G_MAXDOUBLE,
                         G_MAXDOUBLE,
                         0.5,
                         G_PARAM_READWRITE);

  /**
   * GdvIndicator:widget-yalign:
   *
   * The vertical alignment of the widget.
   */
  indicator_properties[PROP_WIDGET_YALIGN] =
    g_param_spec_double ("widget-yalign",
                         "vertical widget offset",
                         "determines the offset to diplay the widget",
                         -G_MAXDOUBLE,
                         G_MAXDOUBLE,
                         0.5,
                         G_PARAM_READWRITE);

  /**
   * GdvIndicator:rotate-widget-with-axis:
   *
   * If possible, rotate the widget along the local axis-slope.
   */
  indicator_properties[PROP_WIDGET_ROTATE_WITH_AXIS] =
    g_param_spec_boolean ("rotate-widget-with-axis",
                          "rotates widget automatically",
                          "the widget will be rotated according to the "
                          "local axis geometry",
                          FALSE,
                          G_PARAM_READWRITE);

  g_object_class_install_properties (object_class,
                                     N_PROPERTIES,
                                     indicator_properties);

  /* Style-Properties */

  /**
   * GdvIndicator:color:
   *
   * Sets line-color for indicator.
   */
  gtk_widget_class_install_style_property (widget_class,
      g_param_spec_boxed ("color",
                          _("color of indicator"),
                          _("Color to draw indicator"),
                          GDK_TYPE_RGBA,
                          G_PARAM_READWRITE));

  /**
   * GdvIndicator:line-wdith:
   *
   * Sets line-width for the indicator-line.
   */
  gtk_widget_class_install_style_property (widget_class,
      g_param_spec_double ("line-width",
                           "line-width of the main-line",
                           "line-width to draw the main line",
                           0.0,
                           G_MAXDOUBLE,
                           1.0,
                           G_PARAM_READWRITE));

  /* TODO: Implement this correctly */
  /*
   * GdvIndicator:style-type:
   *
   * Sets the geometry of the indicator.
   */
/*  gtk_widget_class_install_style_property (widget_class,
      g_param_spec_int ("style-type",
                        "indicator style",
                        "determines the indicator symbol",
                        0,
                        0,
                        0,
                        G_PARAM_READWRITE));
*/
}

/* TODO: Geometry-handling for the indicators still needs a lot of work */
static void
gdv_indicator_measure (
  GdvIndicator        *indicator,
  GtkOrientation       orientation,
  int                  for_size,
  int                 *minimum,
  int                 *natural,
  int                 *minimum_baseline,
  int                 *natural_baseline,
  gpointer             data)
{
  *minimum = 20;
  *natural = 20;
}

static void
gdv_indicator_get_preferred_width (
  GtkWidget           *widget,
  gint                *minimum,
  gint                *natural)
{
  gdv_indicator_measure (GDV_INDICATOR (widget),
                         GTK_ORIENTATION_HORIZONTAL,
                         -1,
                         minimum, natural,
                         NULL, NULL, NULL);
}

static void gdv_indicator_get_preferred_width_for_height (
  GtkWidget           *widget,
  gint                 height,
  gint                *minimum,
  gint                *natural)
{
  gdv_indicator_measure (GDV_INDICATOR (widget),
                         GTK_ORIENTATION_HORIZONTAL,
                         height,
                         minimum, natural,
                         NULL, NULL, NULL);
}

static void gdv_indicator_get_preferred_height          (
  GtkWidget           *widget,
  gint                *minimum,
  gint                *natural)
{
  gdv_indicator_measure (GDV_INDICATOR (widget),
                         GTK_ORIENTATION_VERTICAL,
                         -1,
                         minimum, natural,
                         NULL, NULL, NULL);
}

static void gdv_indicator_get_preferred_height_for_width(
  GtkWidget           *widget,
  gint                 width,
  gint                *minimum,
  gint                *natural)
{
  gdv_indicator_measure (GDV_INDICATOR (widget),
                         GTK_ORIENTATION_VERTICAL,
                         width,
                         minimum, natural,
                         NULL, NULL, NULL);
}

/**
 * gdv_indicator_new:
 *
 * Just a common constructor.
 *
 * Returns: a new #GdvIndicator
 **/
GdvIndicator *gdv_indicator_new (void)
{
  return g_object_new (gdv_indicator_get_type (),
                       NULL);
}

