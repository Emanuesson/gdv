/*
 * gdvspecialtimeaxis.c
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

#include <gsl/gsl_math.h>

#include "gdvspecialtimeaxis.h"

/* Define Properties */
enum
{
  PROP_0,

  N_PROPERTIES
};

struct _GdvSpecialTimeAxisPrivate
{
  gint placeholder;
  GtkWidget *socket;
};

/* Method declaration */
//static void gdv_special_time_axis_dispose (GObject *object);
//static void gdv_special_time_axis_finalize (GObject *object);
//static void gdv_special_time_axis_set_property (GObject      *object,
//                                      guint         property_id,
//                                      const GValue *value,
//                                      GParamSpec   *pspec);
//static void gdv_special_time_axis_get_property (GObject    *object,
//                                      guint       property_id,
//                                      GValue     *value,
//                                      GParamSpec *pspec);

G_DEFINE_TYPE_WITH_PRIVATE (GdvSpecialTimeAxis, gdv_special_time_axis,
  gdv_linear_axis_get_type());

//static void
//gdv_special_time_axis_size_allocate (GtkWidget           *widget,
//                               GtkAllocation       *allocation);

static gchar *
gdv_special_time_axis_make_tic_label_markup (GdvAxis *axis, gdouble  value);

static void
gdv_special_time_axis_class_init (GdvSpecialTimeAxisClass *klass)
{
  GtkWidgetClass *widget_class;
  GdvAxisClass *axis_class;

  widget_class = GTK_WIDGET_CLASS (klass);
  axis_class = GDV_AXIS_CLASS (klass);

//  widget_class->size_allocate = gdv_special_time_axis_size_allocate;

  axis_class->make_tic_label_markup =
    gdv_special_time_axis_make_tic_label_markup;

//  gobject_class->dispose = gdv_special_time_axis_dispose;
//  gobject_class->finalize = gdv_special_time_axis_finalize;
//  gobject_class->set_property = gdv_special_time_axis_set_property;
//  gobject_class->get_property = gdv_special_time_axis_get_property;

  /* Properties */
//  view_properties[PROP_MULTIPLOT] =
//    g_param_spec_boolean ("multiplot",
//                          "multiplotting more than one layer",
//                          "activates or deactivates multiplotting",
//                          FALSE,
//                          G_PARAM_READWRITE);

//  g_object_class_install_properties (gobject_class,
//                                     N_PROPERTIES,
//                                     view_properties);

//  gtk_widget_class_install_style_property (widget_class,
//    g_param_spec_double ("custom-property",
//                         "background cairo source",
//                         "sets the view background source for plotting",
//                         -1000.0,
//                         10000.0,
//                         0.0,
//                         G_PARAM_READWRITE));

  gtk_widget_class_set_css_name (widget_class, "timeaxis");

}

static void
gdv_special_time_axis_init (GdvSpecialTimeAxis *time_axis)
{
  GtkWidget *widget;

  GtkCssProvider *css_provider;
  GdkScreen *screen;

  widget = GTK_WIDGET (time_axis);

  time_axis->priv = gdv_special_time_axis_get_instance_private (time_axis);

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

  g_object_set (time_axis,
    "scale-increment-base", 60.0,
    NULL);

}

/*
static void gdv_special_time_axis_set_property (GObject      *object,
                                      guint         property_id,
                                      const GValue *value,
                                      GParamSpec   *pspec)
{
  GdvSpecialTimeAxis *self = GDV_SPECIAL_TIME_AXIS (object);

  switch (property_id)
    {
//    case PROP_MULTIPLOT:
//      self->priv->multiplot = g_value_get_boolean (value);
//      break;

    default:
*/      /* unknown property */
/*      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

static void gdv_special_time_axis_get_property (GObject    *object,
                                      guint       property_id,
                                      GValue     *value,
                                      GParamSpec *pspec)
{
  GdvSpecialTimeAxis *self = GDV_SPECIAL_TIME_AXIS (object);

  switch (property_id)
    {
//    case PROP_MULTIPLOT:
//      g_value_set_boolean (value, self->priv->multiplot);
//      break;

    default:
*/      /* unknown property */
/*      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;

    }
}
*/

/*
static void
gdv_special_time_axis_size_allocate (GtkWidget           *widget,
                               GtkAllocation       *allocation)
{
  g_print ("*");

  GTK_WIDGET_CLASS (gdv_special_time_axis_parent_class)->
    size_allocate (widget, allocation);
}
*/

static gchar *
gdv_special_time_axis_make_tic_label_markup (GdvAxis *axis, gdouble  value)
{
  gchar *return_string = NULL;

  return_string =
    g_strdup_printf ("%2.0lf:%02.0lf:%02.0lf",
//      floor (value / 3600.0), 0.0, 0.0);
      floor (value / 3600.0),
      floor (fmod (value, 3600.0) / 60.0),
      floor (fmod (value, 60.0)));

  return return_string;
}


/**
 * gdv_special_time_axis_new:
 *
 * Just a common constructor.
 *
 * Returns: a new #GdvSpecialTimeAxis
 **/
GdvSpecialTimeAxis *gdv_special_time_axis_new (void)
{
  return g_object_new (gdv_special_time_axis_get_type (),
                       NULL);
}

