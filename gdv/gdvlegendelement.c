/*
 * gdvlegendelement.c
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

#include <gsl/gsl_math.h>

/**
 * SECTION:gdvlegendelement
 * @title: GdvLegendElement
 *
 * #GdvLegendElement Defines the small examples for #GdvLayerContent- or
 *    #GdvIndicators, used in a Layer to the GdvLegend. It is just an helper to
 *    provide descriptive legends.
 *
 * # CSS nodes
 *
 * GdvLegendElement uses a single CSS node with name legendelement.
 *
 */

#include "gdvlegendelement.h"
#include "gdvrender.h"
#include "gdvlayercontent.h"
#include "gdvindicator.h"

/* Define Properties */
enum
{
  PROP_0,

  PROP_CONNECTED_ELEMENT,
  PROP_SAMPLEN,

  N_PROPERTIES
};

/* --- variables --- */

struct _GdvLegendElementPrivate
{
  /* list with indicators */
  GtkWidget *connected_element;

  gint samplelen;
};

static GParamSpec *legend_element_properties[N_PROPERTIES] = { NULL, };

static void
gdv_legend_element_realize (GtkWidget *widget);
static void
gdv_legend_element_unrealize (GtkWidget *widget);
static void
gdv_legend_element_size_allocate (GtkWidget     *widget,
                                  GtkAllocation *allocation);
static gboolean
gdv_legend_element_draw (GtkWidget    *widget,
                         cairo_t      *cr);

static void
gdv_legend_element_get_preferred_width (GtkWidget           *widget,
                                        gint                *minimum_size,
                                        gint                *natural_size);
static void
gdv_legend_element_get_preferred_height (GtkWidget           *widget,
    gint                *minimum_size,
    gint                *natural_size);
static void
gdv_legend_element_get_preferred_height_for_width (GtkWidget           *widget,
    gint                 width,
    gint                *minimum_height,
    gint                *natural_height);
static void
gdv_legend_element_get_preferred_width_for_height (GtkWidget           *layout,
    gint                 width,
    gint                *minimum_height,
    gint                *natural_height);

static void
gdv_legend_element_dispose (GObject *object);

static void
gdv_legend_element_finalize (GObject *object);

G_DEFINE_TYPE_WITH_PRIVATE (GdvLegendElement, gdv_legend_element, GTK_TYPE_CONTAINER);

static void
gdv_legend_element_init (GdvLegendElement *legend_element)
{
  gtk_widget_set_can_focus (GTK_WIDGET (legend_element), TRUE);
  gtk_widget_set_receives_default (GTK_WIDGET (legend_element), TRUE);
  gtk_widget_set_has_window (GTK_WIDGET (legend_element), FALSE);

  legend_element->priv = gdv_legend_element_get_instance_private (legend_element);

  legend_element->priv->connected_element = NULL;
  legend_element->priv->samplelen = 20;
}

static void
gdv_legend_element_set_property (GObject      *object,
                                 guint         property_id,
                                 const GValue *value,
                                 GParamSpec   *pspec)
{
  GdvLegendElement *self;

  self = GDV_LEGEND_ELEMENT (object);

  switch (property_id)
  {
  case PROP_CONNECTED_ELEMENT:
    self->priv->connected_element = g_value_get_object (value);
    break;

  case PROP_SAMPLEN:
    self->priv->samplelen = g_value_get_int (value);
    break;

  default:
    /* unknown property */
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }
}


static void
gdv_legend_element_get_property (GObject    *object,
                                 guint       property_id,
                                 GValue     *value,
                                 GParamSpec *pspec)
{
  GdvLegendElement *self = GDV_LEGEND_ELEMENT (object);

  switch (property_id)
  {
  case PROP_CONNECTED_ELEMENT:
    g_value_set_object (value, self->priv->connected_element);
    break;

  case PROP_SAMPLEN:
    g_value_set_int (value, self->priv->samplelen);
    break;

  default:
    /* unknown property */
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }
}

static void
gdv_legend_element_dispose (GObject *object)
{
  G_OBJECT_CLASS (gdv_legend_element_parent_class)->dispose (object);
}

static void
gdv_legend_element_finalize (GObject *object)
{
  G_OBJECT_CLASS (gdv_legend_element_parent_class)->finalize (object);
}

static void
gdv_legend_element_class_init (GdvLegendElementClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->dispose = gdv_legend_element_dispose;
  object_class->finalize = gdv_legend_element_finalize;
  object_class->set_property = gdv_legend_element_set_property;
  object_class->get_property = gdv_legend_element_get_property;

  widget_class->realize =
    gdv_legend_element_realize;
  widget_class->unrealize =
    gdv_legend_element_unrealize;
  widget_class->size_allocate =
    gdv_legend_element_size_allocate;
  widget_class->draw =
    gdv_legend_element_draw;

  widget_class->get_preferred_width =
    gdv_legend_element_get_preferred_width;
  widget_class->get_preferred_height =
    gdv_legend_element_get_preferred_height;
  widget_class->get_preferred_height_for_width =
    gdv_legend_element_get_preferred_height_for_width;
  widget_class->get_preferred_width_for_height =
    gdv_legend_element_get_preferred_width_for_height;

  gtk_widget_class_set_css_name (widget_class, "legendelement");

  /* Properties */

  /**
   * GdvLegendElement:element:
   *
   * The connected element (layer-content or indicator) that will be displayed.
   *
   * Since: 0.1
   */
  legend_element_properties[PROP_CONNECTED_ELEMENT] =
    g_param_spec_object ("element",
                         "displayed element",
                         "the layer element (layer-content or indicator) that "
                         "should be displayed as a legend-element",
                         GTK_TYPE_WIDGET,
                         G_PARAM_READWRITE);


  /**
   * GdvLegend:sample-length:
   *
   * The layer, that the legend is connected to.
   *
   * Since: 0.1
   */
  legend_element_properties[PROP_SAMPLEN] =
    g_param_spec_int ("sample-length",
                      "the example length",
                      "the length of sample-length in pixels",
                      0,
                      G_MAXINT,
                      20,
                      G_PARAM_READWRITE);


  g_object_class_install_properties (object_class,
                                     N_PROPERTIES,
                                     legend_element_properties);

}

/*
 */
GdvLegendElement *gdv_legend_element_new (void)
{
  return g_object_new (gdv_legend_element_get_type (),
                       NULL);
}

static void
gdv_legend_element_measure (
  GdvLegendElement    *legend_element,
  GtkOrientation       orientation,
  int                  for_size,
  int                 *minimum,
  int                 *natural,
  int                 *minimum_baseline,
  int                 *natural_baseline,
  gpointer             data)
{
  /* Measuring the indicators */
  if (GDV_IS_INDICATOR (legend_element->priv->connected_element))
  {
    if (orientation == GTK_ORIENTATION_HORIZONTAL)
    {
      if (for_size > 0)
        gtk_widget_get_preferred_width_for_height (
          GTK_WIDGET (legend_element->priv->connected_element),
          for_size, minimum, natural);
      else
        gtk_widget_get_preferred_width (
          GTK_WIDGET (legend_element->priv->connected_element),
          minimum, natural);
    }
    else if (for_size > 0)
      gtk_widget_get_preferred_height_for_width (
        GTK_WIDGET (legend_element->priv->connected_element),
        for_size, minimum, natural);
    else
      gtk_widget_get_preferred_height (
        GTK_WIDGET (legend_element->priv->connected_element),
        minimum, natural);

  }

  /* Measuring the layer-content */
  if (GDV_LAYER_IS_CONTENT (legend_element->priv->connected_element))
  {
    GtkStyleContext *content_context =
      gtk_widget_get_style_context (GTK_WIDGET (legend_element->priv->connected_element));
    gint minimum_point = 0,
         natural_point = 0;

    if (orientation == GTK_ORIENTATION_HORIZONTAL)
    {
      if (for_size > 0)
        gtk_widget_get_preferred_width_for_height (
          GTK_WIDGET (legend_element->priv->connected_element),
          for_size, &minimum_point, &natural_point);
      else
        gtk_widget_get_preferred_width (
          GTK_WIDGET (legend_element->priv->connected_element),
          &minimum_point, &natural_point);

      *natural =
        legend_element->priv->samplelen > natural_point ?
        legend_element->priv->samplelen : natural_point;
      *minimum =
        legend_element->priv->samplelen > minimum_point ?
        legend_element->priv->samplelen : minimum_point;
    }
    else
    {
      gdouble axis_line_width;
      GValue value_lw = G_VALUE_INIT;

      if (for_size > 0)
        gtk_widget_get_preferred_height_for_width (
          GTK_WIDGET (legend_element->priv->connected_element),
          for_size, &minimum_point, &natural_point);
      else
        gtk_widget_get_preferred_height (
          GTK_WIDGET (legend_element->priv->connected_element),
          &minimum_point, &natural_point);

      g_value_init (&value_lw, G_TYPE_DOUBLE);

      gtk_style_context_get_style_property (content_context,
                                            "line-width", &value_lw);
      axis_line_width = g_value_get_double (&value_lw);

      *natural =
        (gint) axis_line_width > natural_point ?
        (gint) axis_line_width : natural_point;
      *minimum =
        (gint) axis_line_width > minimum_point ?
        (gint) axis_line_width : minimum_point;
    }
  }
}

static void
gdv_legend_element_realize (GtkWidget *widget)
{
  GtkAllocation allocation;
  GdkWindow *window;
  GdkWindowAttr attributes;

  gtk_widget_get_allocation (widget, &allocation);

  gtk_widget_set_realized (widget, TRUE);

  attributes.window_type = GDK_WINDOW_CHILD;
  attributes.x = (gint) allocation.x;
  attributes.y = (gint) allocation.y;
  attributes.width = (gint) (allocation.width * 1.0);
  attributes.height = (gint) (allocation.height * 1.0);

  attributes.wclass = GDK_INPUT_OUTPUT;
  attributes.visual = gtk_widget_get_visual (widget);
  attributes.event_mask = gtk_widget_get_events (widget);
  attributes.event_mask |= (GDK_EXPOSURE_MASK |
                            GDK_BUTTON_MOTION_MASK |
                            GDK_BUTTON_PRESS_MASK |
                            GDK_BUTTON_RELEASE_MASK |
                            GDK_ENTER_NOTIFY_MASK |
                            GDK_LEAVE_NOTIFY_MASK);

  window = gtk_widget_get_parent_window (widget);
  gtk_widget_set_window (widget, window);
  g_object_ref (window);


  attributes.wclass = GDK_INPUT_ONLY;
}

static void
gdv_legend_element_unrealize (GtkWidget *widget)
{
  /* TODO: Write this function! */

  GTK_WIDGET_CLASS (gdv_legend_element_parent_class)->unrealize (widget);
}

static void
gdv_legend_element_size_allocate (GtkWidget     *widget,
                                  GtkAllocation *allocation)
{
  gtk_widget_set_allocation (widget, allocation);

}

static gboolean
gdv_legend_element_draw (GtkWidget    *widget,
                         cairo_t      *cr)
{
  GdvLegendElement *element = GDV_LEGEND_ELEMENT (widget);
  GtkAllocation allocation;

  gtk_widget_get_allocation (widget, &allocation);

  /* TODO: this bothers me every time */
  allocation.x = 0;
  allocation.y = 0;

  if (GDV_IS_INDICATOR (element->priv->connected_element))
  {
    /* FIXME: this is currently the official indicator-draw function */
    GtkStyleContext *context =
      gtk_widget_get_style_context (GTK_WIDGET (element->priv->connected_element));
    gdouble x_position = allocation.x + 0.5 * allocation.width;
    gdouble y_position = allocation.y + 0.5 * allocation.height;

    gdv_render_line (context, cr,
                     x_position, y_position + 10.0,
                     x_position, y_position - 10.0);
    gdv_render_line (context, cr,
                     x_position + 10.0, y_position,
                     x_position - 10.0, y_position);
  }
  else if (GDV_LAYER_IS_CONTENT (element->priv->connected_element))
  {
    GtkStyleContext *context =
      gtk_widget_get_style_context (GTK_WIDGET (element->priv->connected_element));

    gdv_render_data_point (
      context,
      cr,
      allocation.x + 0.5 * allocation.width,
      allocation.y + 0.5 * allocation.height);
    gdv_render_data_line (
      context,
      cr,
      allocation.x + 0.5 * allocation.width - 0.5 * element->priv->samplelen,
      allocation.y + 0.5 * allocation.height,
      allocation.x + 0.5 * allocation.width + 0.5 * element->priv->samplelen,
      allocation.y + 0.5 * allocation.height);
  }

  return FALSE;
}

static void
gdv_legend_element_get_preferred_width (GtkWidget           *widget,
                                        gint                *minimum_size,
                                        gint                *natural_size)
{
  gdv_legend_element_measure (GDV_LEGEND_ELEMENT (widget),
                              GTK_ORIENTATION_HORIZONTAL,
                              -1,
                              minimum_size, natural_size,
                              NULL, NULL, NULL);
}

static void
gdv_legend_element_get_preferred_height (GtkWidget           *widget,
    gint                *minimum_size,
    gint                *natural_size)
{
  gdv_legend_element_measure (GDV_LEGEND_ELEMENT (widget),
                              GTK_ORIENTATION_VERTICAL,
                              -1,
                              minimum_size, natural_size,
                              NULL, NULL, NULL);
}

static void
gdv_legend_element_get_preferred_height_for_width (GtkWidget           *widget,
    gint                 width,
    gint                *minimum_height,
    gint                *natural_height)
{
  gdv_legend_element_measure (GDV_LEGEND_ELEMENT (widget),
                              GTK_ORIENTATION_VERTICAL,
                              width,
                              minimum_height, natural_height,
                              NULL, NULL, NULL);
}


static void
gdv_legend_element_get_preferred_width_for_height (GtkWidget           *widget,
    gint                 height,
    gint                *minimum_width,
    gint                *natural_width)
{
  gdv_legend_element_measure (GDV_LEGEND_ELEMENT (widget),
                              GTK_ORIENTATION_HORIZONTAL,
                              height,
                              minimum_width, natural_width,
                              NULL, NULL, NULL);
}

