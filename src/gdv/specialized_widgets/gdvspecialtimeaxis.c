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

#include "gdvspecialtimeaxis.h"

/* Define Properties */
enum
{
  PROP_0,

  PROP_IS_UNIX_TIME,

  N_PROPERTIES
};

static GParamSpec *special_time_axis_properties[N_PROPERTIES] = { NULL, };

struct _GdvSpecialTimeAxisPrivate
{
  gint placeholder;
  gboolean unix_time;
  GtkWidget *socket;
};

/* Method declaration */
//static void gdv_special_time_axis_dispose (GObject *object);
//static void gdv_special_time_axis_finalize (GObject *object);
static void gdv_special_time_axis_set_property (GObject      *object,
                                                guint         property_id,
                                                const GValue *value,
                                                GParamSpec   *pspec);
static void gdv_special_time_axis_get_property (GObject    *object,
                                                guint       property_id,
                                                GValue     *value,
                                                GParamSpec *pspec);

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
  GObjectClass *object_class;

  widget_class = GTK_WIDGET_CLASS (klass);
  axis_class = GDV_AXIS_CLASS (klass);
  object_class = G_OBJECT_CLASS (klass);

//  widget_class->size_allocate = gdv_special_time_axis_size_allocate;

  axis_class->make_tic_label_markup =
    gdv_special_time_axis_make_tic_label_markup;

//  gobject_class->dispose = gdv_special_time_axis_dispose;
//  gobject_class->finalize = gdv_special_time_axis_finalize;
  object_class->set_property = gdv_special_time_axis_set_property;
  object_class->get_property = gdv_special_time_axis_get_property;

  /* Properties */
  special_time_axis_properties[PROP_IS_UNIX_TIME] =
    g_param_spec_boolean ("is-unix-time",
                          "axis-values will be interpreted in local unit-time",
                          "axis-values will be interpreted as the time in seconds "
                          "since 1970-01-01 00:00:00 UTC (value is interpreted as "
                          "UTC-time)",
                          FALSE,
                          G_PARAM_READWRITE);

  g_object_class_install_properties (object_class,
                                     N_PROPERTIES,
                                     special_time_axis_properties);

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


static void gdv_special_time_axis_set_property (GObject      *object,
                                      guint         property_id,
                                      const GValue *value,
                                      GParamSpec   *pspec)
{
  GdvSpecialTimeAxis *self = GDV_SPECIAL_TIME_AXIS (object);

  switch (property_id)
    {
    case PROP_IS_UNIX_TIME:
      self->priv->unix_time = g_value_get_boolean (value);
      break;

    default:
      /* unknown property */
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
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
    case PROP_IS_UNIX_TIME:
      g_value_set_boolean (value, self->priv->unix_time);
      break;

    default:
      /* unknown property */
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;

    }
}


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

static gchar *
gdv_special_time_axis_make_tic_label_markup (GdvAxis *axis, gdouble  value)
{
  gchar *return_string = NULL;
  GdvSpecialTimeAxis *time_axis = GDV_SPECIAL_TIME_AXIS (axis);

  if (time_axis->priv->unix_time)
  {
    GDateTime *dtime = g_date_time_new_from_unix_utc ((gint64)value);
//    GDateTime *dtime_cpy;
    gint yr, mth, day;
    gdouble sec;
//    dtime_cpy = g_date_time_add_full (dtime, 0, 0, 0, 0, 0, fmod(value, 1.0));
//    g_date_time_unref(dtime);

    if (dtime == NULL)
      return g_strdup("NA");

    sec = fmod(value, 1.0);
    sec += g_date_time_get_seconds (dtime);
    g_date_time_get_ymd (dtime, &yr, &mth, &day);

    if (sec > 0.0)
      return_string =
        g_strdup_printf ("%d %s %d, %02d:%02d:%02.1lf",
                         day, _get_month_name (mth), yr,
                         g_date_time_get_hour (dtime), g_date_time_get_minute (dtime),
                         sec);
    else
      return_string =
        g_strdup_printf ("%d %s %d, %02d:%02d",
                         day, _get_month_name (mth), yr,
                         g_date_time_get_hour (dtime), g_date_time_get_minute (dtime));

    g_date_time_unref(dtime);
  }
  else
  {
    return_string =
      g_strdup_printf ("%2.0lf:%02.0lf:%02.0lf",
  //      floor (value / 3600.0), 0.0, 0.0);
        floor (value / 3600.0),
        floor (fmod (value, 3600.0) / 60.0),
        floor (fmod (value, 60.0)));

  }

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

