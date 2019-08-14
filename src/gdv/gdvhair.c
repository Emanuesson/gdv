/*
 * gdvhair.c
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

#include "gdvhair.h"
#include "gdv-data-boxed.h"
#include "gdvaxis.h"
#include "gdvlayer.h"
#include "gdvrender.h"

/* TODO: I should take a look at GtkRange to see something similar and adapt it
 *       for the hair-widget. In the end there should be a similar functionality
 *       betweem am GtkRange-widget and an GdvHair
 */

/**
 * SECTION:gdvhair
 * @title: GdvHair
 * @short_description: an hair on an axis
 *
 * #GdvHair is an object-class, that displays an hair for any given
 * value on a layer. The intention is to draw a mesh for different values,
 * espcially for most of the tic-values.
 *
 * # CSS nodes
 *
 * GdvHair uses a single CSS node with name hair.
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
  PROP_FROM_AXIS,
  PROP_FROM_X,
  PROP_FROM_Y,
  PROP_TO_AXIS,
  PROP_TO_X,
  PROP_TO_Y,

  N_PROPERTIES
};

struct _GdvHairPrivate
{
  /* value */
  gdouble value;
  gdouble from_x, from_y;
  gdouble to_x, to_y;
  GdvAxis *from_axis, *to_axis;
};

static GParamSpec *hair_properties[N_PROPERTIES] = { NULL, };

static void
gdv_hair_dispose (GObject *object);

static void
gdv_hair_finalize (GObject *object);

G_DEFINE_TYPE_WITH_PRIVATE (GdvHair, gdv_hair, GTK_TYPE_CONTAINER);

static void
gdv_hair_init (GdvHair *hair)
{
  GtkWidget *widget;
  GtkStyleContext *style_context;
  GtkCssProvider *css_provider;
  GdvHairPrivate *priv = gdv_hair_get_instance_private (hair);
  hair->priv = priv;
  widget = GTK_WIDGET (hair);

  priv->value = 0.0;
  priv->from_x = 0.0;
  priv->from_y = 0.0;
  priv->to_x = 0.0;
  priv->to_y = 0.0;
  priv->from_axis = NULL;
  priv->to_axis = NULL;

  gtk_widget_set_name (widget, "new hair");

  /* initializing style-properties */
  css_provider = gtk_css_provider_new ();
  style_context = gtk_widget_get_style_context (widget);
  gtk_css_provider_load_from_resource (
    css_provider,
    "/net/gdv/libgdv/themes/default_general.css");

  gtk_style_context_add_provider (
    style_context,
    GTK_STYLE_PROVIDER (css_provider),
    GTK_STYLE_PROVIDER_PRIORITY_FALLBACK);

  gtk_widget_set_can_focus (widget, TRUE);
  gtk_widget_set_receives_default (widget, TRUE);
  gtk_widget_set_has_window (widget, FALSE);
}

/****************************************************** below needs work *****/

static void
gdv_hair_set_property (GObject      *object,
                            guint         property_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
  GdvHair *self;

  self = GDV_HAIR (object);

  switch (property_id)
  {
  case PROP_DATA_VAL:
    self->priv->value = g_value_get_double (value);
    break;
  case PROP_FROM_AXIS:
    self->priv->from_axis = g_value_get_object (value);
    break;
  case PROP_FROM_X:
    self->priv->from_x = g_value_get_double (value);
    break;
  case PROP_FROM_Y:
    self->priv->from_y = g_value_get_double (value);
    break;
  case PROP_TO_AXIS:
    self->priv->to_axis = g_value_get_object (value);
    break;
  case PROP_TO_X:
    self->priv->to_x = g_value_get_double (value);
    break;
  case PROP_TO_Y:
    self->priv->to_y = g_value_get_double (value);
    break;
  default:
    /* unknown property */
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }
}


static void
gdv_hair_get_property (GObject    *object,
                            guint       property_id,
                            GValue     *value,
                            GParamSpec *pspec)
{
  GdvHair *self = GDV_HAIR (object);

  switch (property_id)
  {
  case PROP_DATA_VAL:
    g_value_set_double (value, self->priv->value);
    break;
  case PROP_FROM_AXIS:
    g_value_set_object (value, self->priv->from_axis);
    break;
  case PROP_FROM_X:
    g_value_set_double (value, self->priv->from_x);
    break;
  case PROP_FROM_Y:
    g_value_set_double (value, self->priv->from_y);
    break;
  case PROP_TO_AXIS:
    g_value_set_object (value, self->priv->to_axis);
    break;
  case PROP_TO_X:
    g_value_set_double (value, self->priv->to_x);
    break;
  case PROP_TO_Y:
    g_value_set_double (value, self->priv->to_y);
    break;
  default:
    /* unknown property */
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }
}

static void
gdv_hair_dispose (GObject *object)
{
  G_OBJECT_CLASS (gdv_hair_parent_class)->dispose (object);
}

static void
gdv_hair_finalize (GObject *object)
{
  G_OBJECT_CLASS (gdv_hair_parent_class)->finalize (object);
}

static void gdv_hair_size_allocate         (GtkWidget              *widget,
    GtkAllocation          *allocation);

/*
static void gdv_hair_get_preferred_width (
  GtkWidget           *widget,
  gint                *minimum_size,
  gint                *natural_size);
static void gdv_hair_get_preferred_height (
  GtkWidget           *widget,
  gint                *minimum_size,
  gint                *natural_size);
static void gdv_hair_get_preferred_height_for_width(
  GtkWidget           *widget,
  gint                 width,
  gint                *minimum_height,
  gint                *natural_height);
static void gdv_hair_get_preferred_width_for_height(
  GtkWidget           *widget,
  gint                 height,
  gint                *minimum_width,
  gint                *natural_width);
*/

static void
gdv_hair_realize (GtkWidget *widget)
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
gdv_hair_unrealize (GtkWidget *widget)
{
  GTK_WIDGET_CLASS (gdv_hair_parent_class)->unrealize (widget);
}

static void
gdv_hair_size_allocate (GtkWidget     *widget,
                        GtkAllocation *allocation)
{
  g_return_if_fail (allocation != NULL);

  gtk_widget_set_allocation (widget, allocation);
}

static
gboolean gdv_hair_draw (GtkWidget   *widget,
                        cairo_t     *cr)
{
  GdvHair *hair = GDV_HAIR (widget);
  GdvLayer *layer = GDV_LAYER(gtk_widget_get_parent(widget));
  GdvHairPrivate *priv = gdv_hair_get_instance_private (hair);
//  GdvAxis *axis = GDV_AXIS (gtk_widget_get_parent (widget));
  gboolean on_axis;
  GtkStyleContext *context;
  GtkAllocation alloc, layer_alloc;

  if (!priv->from_axis)
    g_warning ("from-axis for %s(%p) is not set",
               g_type_name (G_TYPE_FROM_INSTANCE (widget)), widget);

  gtk_widget_get_allocation (GTK_WIDGET(hair), &layer_alloc);

  g_signal_emit_by_name (
    priv->from_axis,
    "get-point",
    priv->value,
    &priv->from_x,
    &priv->from_y,
    &on_axis);

  gtk_widget_get_allocation (GTK_WIDGET(priv->from_axis), &alloc);
  priv->from_x += alloc.x - layer_alloc.x;
  priv->from_y += alloc.y - layer_alloc.y;

  if (!priv->to_axis)
    g_warning ("to-axis for %s(%p) is not set",
               g_type_name (G_TYPE_FROM_INSTANCE (widget)), widget);

  g_signal_emit_by_name (
    priv->to_axis,
    "get-point",
    priv->value,
    &priv->to_x,
    &priv->to_y,
    &on_axis);

  gtk_widget_get_allocation (GTK_WIDGET(priv->to_axis), &alloc);
  priv->to_x += alloc.x - layer_alloc.x;
  priv->to_y += alloc.y - layer_alloc.y;


  /* TODO: remember to make this an property, if implemented as allocation */
  /* skip hairs outside the range of the axis */
//  if (on_axis || hair->priv->show_in_out_of_range)
  {
    context = gtk_widget_get_style_context (widget);
    gdv_render_line (context, cr,
                     priv->from_x, priv->from_y,
                     priv->to_x, priv->to_y);
  }

  return FALSE;
}

static void
gdv_hair_class_init (GdvHairClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->dispose = gdv_hair_dispose;
  object_class->finalize = gdv_hair_finalize;
  object_class->set_property = gdv_hair_set_property;
  object_class->get_property = gdv_hair_get_property;

//  widget_class->get_preferred_width =
//    gdv_hair_get_preferred_width;
//  widget_class->get_preferred_height =
//    gdv_hair_get_preferred_height;
//  widget_class->get_preferred_height_for_width =
//    gdv_hair_get_preferred_height_for_width;
//  widget_class->get_preferred_width_for_height =
//    gdv_hair_get_preferred_width_for_height;

  //widget_class->realize = gdv_hair_realize;
  //widget_class->unrealize = gdv_hair_unrealize;
  widget_class->size_allocate = gdv_hair_size_allocate;
  widget_class->draw = gdv_hair_draw;

  gtk_widget_class_set_css_name (widget_class, "hair");

  /* Properties */

  /**
   * GdvHair:value:
   *
   * Determines the value, this hair represents.
   */
  hair_properties[PROP_DATA_VAL] =
    g_param_spec_double ("value",
                         "data value",
                         "Determines the current value this hair is representing",
                         -G_MAXDOUBLE,
                         G_MAXDOUBLE,
                         0.0,
                         G_PARAM_READWRITE);

  /**
   * GdvHair:from-axis:
   */
  hair_properties[PROP_FROM_AXIS] =
    g_param_spec_object ("from-axis",
                         "from axis",
                         "An axis to start this hair-line",
                          GDV_TYPE_AXIS,
                          G_PARAM_READWRITE);

  /**
   * GdvHair:from-x:
   */
  hair_properties[PROP_FROM_X] =
    g_param_spec_double ("from-x",
                         "from x-pos",
                         "determines the x-position, this hair-line is "
                         "starting",
                         -G_MAXDOUBLE,
                         G_MAXDOUBLE,
                         0.0,
                         G_PARAM_READWRITE);

  /**
   * GdvHair:from-y:
   */
  hair_properties[PROP_FROM_Y] =
    g_param_spec_double ("from-y",
                         "from y-pos",
                         "determines the y-position, this hair-line is "
                         "starting",
                         -G_MAXDOUBLE,
                         G_MAXDOUBLE,
                         0.0,
                         G_PARAM_READWRITE);

  /**
   * GdvHair:to-axis:
   */
  hair_properties[PROP_TO_AXIS] =
    g_param_spec_object ("to-axis",
                         "to axis",
                         "An axis to end this hair-line",
                          GDV_TYPE_AXIS,
                          G_PARAM_READWRITE);

  /**
   * GdvHair:to-x:
   */
  hair_properties[PROP_TO_X] =
    g_param_spec_double ("to-x",
                         "to x-pos",
                         "determines the x-position, this hair-line is "
                         "ending",
                         -G_MAXDOUBLE,
                         G_MAXDOUBLE,
                         0.0,
                         G_PARAM_READWRITE);

  /**
   * GdvHair:to-y:
   */
  hair_properties[PROP_TO_Y] =
    g_param_spec_double ("to-y",
                         "to y-pos",
                         "determines the y-position, this hair-line is "
                         "ending",
                         -G_MAXDOUBLE,
                         G_MAXDOUBLE,
                         0.0,
                         G_PARAM_READWRITE);

  g_object_class_install_properties (object_class,
                                     N_PROPERTIES,
                                     hair_properties);

  /* Style-Properties */

  /**
   * GdvHair:color:
   *
   * Sets line-color for hair.
   */
  gtk_widget_class_install_style_property (widget_class,
      g_param_spec_boxed ("color",
                          _("color of hair"),
                          _("Color to draw hair"),
                          GDK_TYPE_RGBA,
                          G_PARAM_READWRITE));

  /**
   * GdvHair:line-wdith:
   *
   * Sets line-width for the hair-line.
   */
  gtk_widget_class_install_style_property (widget_class,
      g_param_spec_double ("line-width",
                           "line-width of the main-line",
                           "line-width to draw the main line",
                           0.0,
                           G_MAXDOUBLE,
                           1.0,
                           G_PARAM_READWRITE));
}

/* TODO: Geometry-handling for the hairs still needs a lot of work */
/*static void
gdv_hair_measure (
  GdvHair        *hair,
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
gdv_hair_get_preferred_width (
  GtkWidget           *widget,
  gint                *minimum,
  gint                *natural)
{
  gdv_hair_measure (GDV_HAIR (widget),
                         GTK_ORIENTATION_HORIZONTAL,
                         -1,
                         minimum, natural,
                         NULL, NULL, NULL);
}

static void gdv_hair_get_preferred_width_for_height (
  GtkWidget           *widget,
  gint                 height,
  gint                *minimum,
  gint                *natural)
{
  gdv_hair_measure (GDV_HAIR (widget),
                         GTK_ORIENTATION_HORIZONTAL,
                         height,
                         minimum, natural,
                         NULL, NULL, NULL);
}

static void gdv_hair_get_preferred_height          (
  GtkWidget           *widget,
  gint                *minimum,
  gint                *natural)
{
  gdv_hair_measure (GDV_HAIR (widget),
                         GTK_ORIENTATION_VERTICAL,
                         -1,
                         minimum, natural,
                         NULL, NULL, NULL);
}

static void gdv_hair_get_preferred_height_for_width(
  GtkWidget           *widget,
  gint                 width,
  gint                *minimum,
  gint                *natural)
{
  gdv_hair_measure (GDV_HAIR (widget),
                         GTK_ORIENTATION_VERTICAL,
                         width,
                         minimum, natural,
                         NULL, NULL, NULL);
}
*/

/**
 * gdv_hair_new:
 *
 * Just a common constructor.
 *
 * Returns: a new #GdvHair
 **/
GdvHair *gdv_hair_new (void)
{
  return g_object_new (gdv_hair_get_type (),
                       NULL);
}

