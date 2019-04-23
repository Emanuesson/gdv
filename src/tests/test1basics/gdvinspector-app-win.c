/*
 * gdvinspector-app-win.c
 * This file is part of gevse
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
#include <stdlib.h>

#include <gdv/gdv.h>

#include "gdvinspector-app-win.h"
#include "gdvinspector-test3.h"
#include "gdvinspector-test4.h"
#include "gdvinspector-test5a.h"
#include "test6/gdvinspector-test6.h"

#include "tests/gemu_sub/gemu-glib-util.h"
#include "tests/gemu_sub/gemu-gtk-util.h"

/* Define Properties */
enum
{
  PROP_0,

  N_PROPERTIES
};

struct _GdvInspectorAppWindowPrivate
{
  GtkWidget *content_box;
  gint placeholder;

  GtkWidget *calendar;

  GtkWidget *settings_stack;
  GtkWidget *nautilus;
  GtkWidget *textedit;

  GdvOnedLayer *onedlayer;
  GdvOnedLayer *loglayer;
  GdvTwodLayer *twodlayer;
  GdvOnedLayer *polarlayer;

  GdvOnedLayer *consummeter;
  guint         changed_cb;
  guint         changed_cb_oned;
};

G_DEFINE_TYPE_WITH_PRIVATE (
  GdvInspectorAppWindow,
  gdv_inspector_app_window,
  GTK_TYPE_APPLICATION_WINDOW);

static void
gdv_inspector_app_window_dispose (GObject *object)
{
  GdvInspectorAppWindow *window = GDV_INSPECTOR_APP_WINDOW (object);
  GdvInspectorAppWindowPrivate *priv;
	priv = gdv_inspector_app_window_get_instance_private (window);

  if (priv->changed_cb != 0)
  {
    GSource *source = g_main_context_find_source_by_id (NULL, priv->changed_cb);
    g_source_destroy (source);
    priv->changed_cb = 0;
  }

  if (priv->changed_cb_oned != 0)
  {
    GSource *source = g_main_context_find_source_by_id (NULL, priv->changed_cb_oned);
    g_source_destroy (source);
    priv->changed_cb_oned = 0;
  }

	G_OBJECT_CLASS (gdv_inspector_app_window_parent_class)->dispose (object);
}

static void
gdv_inspector_app_window_finalize (GObject *object)
{
	G_OBJECT_CLASS (gdv_inspector_app_window_parent_class)->finalize (object);
}

static void
gdv_inspector_app_window_class_init (GdvInspectorAppWindowClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->dispose = gdv_inspector_app_window_dispose;
  object_class->finalize = gdv_inspector_app_window_finalize;

  g_type_ensure (GDV_ONED_LAYER_TYPE);
  g_type_ensure (GDV_TWOD_LAYER_TYPE);
  g_type_ensure (GDV_INSPECTOR_TYPE_TEST3);
  g_type_ensure (GDV_INSPECTOR_TYPE_TEST4);
  g_type_ensure (GDV_INSPECTOR_TYPE_TEST5A);
  g_type_ensure (GDV_INSPECTOR_TYPE_TEST6);

  g_type_ensure (GDV_TYPE_SPECIAL_DRUM_DISPLAY);

  gtk_widget_class_set_template_from_resource (
    GTK_WIDGET_CLASS (object_class),
    "/net/gdv/gdvinspectorapp/ui/gui/gdvinspec-app-win.ui");

  gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (klass),
    GdvInspectorAppWindow, loglayer);
  gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (klass),
    GdvInspectorAppWindow, twodlayer);
  gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (klass),
    GdvInspectorAppWindow, onedlayer);
  gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (klass),
    GdvInspectorAppWindow, polarlayer);
}

static gint time_fc = 0;
static gint indicator_fc = 0;

static GdvLayerContent *global_content;
static GdvTwodLayer *global_layer;

static gboolean
timeout_cb_oned (GdvLayer *layer)
{
  if (layer && GDV_IS_LAYER (layer))
  {
    GList *ind_list, *axis_list;

    g_object_set (layer, "center-value",
      0.0 + 100.0 * sin(0.01 * M_PI * ((gdouble) indicator_fc)),NULL);

    axis_list = gdv_layer_get_axis_list (layer);

    if (axis_list)
    {
      ind_list = gdv_axis_get_indicator_list (axis_list->data);

      g_object_set (axis_list->data,
        "force-beg-end", TRUE,
        "axis-beg-at-screen-x", 0.5,
        "axis-end-at-screen-x", 0.5,
        "axis-beg-at-screen-y", 0.0,
        "axis-end-at-screen-y", 1.0,
        "scale-increment-val", 20.0,
        "scale-auto-increment", FALSE,
        NULL);

      if (ind_list)
      {
        g_object_set (ind_list->data,
          "value",0.0 + 100.0 * sin(0.01 * M_PI * ((gdouble) indicator_fc)),
          NULL);
      }
      g_list_free (ind_list);

    }
    g_list_free (axis_list);
  }

  if (indicator_fc >= G_MAXINT * 0.5)
    indicator_fc = 0;
  else
    indicator_fc++;

  return TRUE;
}

static gboolean
timeout_cb (GdvIndicator *indicator)
{
  gdouble tmp_min, tmp_max;

  if (time_fc == 3)
    gdv_twod_layer_set_xrange (GDV_TWOD_LAYER (global_layer), 0.0, 1200.0);


  if (global_content && GDV_LAYER_IS_CONTENT (global_content) &&
      indicator && GDV_IS_INDICATOR (indicator) &&
      global_layer && GDV_IS_LAYER (global_layer))
  {
    g_object_set (indicator,
      "value",50.0 + 50.0 * sin(0.01 * M_PI * ((gdouble) time_fc)),
      NULL);

    gdv_layer_content_add_data_point (global_content,
      time_fc * (((gdouble) rand()) / ((gdouble) RAND_MAX)),
      time_fc * (((gdouble) rand()) / ((gdouble) RAND_MAX)),
//      time_fc * gsl_rng_uniform (global_rng),
//      time_fc * gsl_rng_uniform (global_rng),
//      90.0 + 0.5 * ((gdouble) time_fc),
//      90.0 + 0.5 * ((gdouble) time_fc),
      0.0);
    gdv_layer_content_get_min_max_x (global_content, &tmp_min, &tmp_max);
    tmp_min = tmp_min < 0 ? tmp_min : 0;
    tmp_max = tmp_max > 100 ? tmp_max : 100;
    gdv_twod_layer_set_xrange (GDV_TWOD_LAYER (global_layer), tmp_min, tmp_max);
    gdv_layer_content_get_min_max_y (global_content, &tmp_min, &tmp_max);
    tmp_min = tmp_min < 0 ? tmp_min : 0;
    tmp_max = tmp_max > 100 ? tmp_max : 100;
    gdv_twod_layer_set_yrange (GDV_TWOD_LAYER (global_layer), tmp_min, tmp_max);

    gtk_widget_queue_allocate (GTK_WIDGET (global_layer));

  }
//  g_print ("HELLO AGAIN\n");

  if (time_fc >= G_MAXINT * 0.5)
    time_fc = 0;
  else
    time_fc++;

  return TRUE;
}

void
gdv_inspector_app_window_init (GdvInspectorAppWindow *window)
{
  GdvAxis *new_axis;
  GdvSpecialCheckedIndicator *new_ind_ondl, *new_ind;
//  const gsl_rng_type * T;

  gtk_widget_init_template (GTK_WIDGET (window));

//  gemu_gtk_util_signal_connect_to_widget_children (
//    GTK_CONTAINER (window),
//    gemu_gtk_util_signal_use_append_and_remove,
//    NULL,
//    NULL);

	window->priv = gdv_inspector_app_window_get_instance_private (window);

// FIXME: These are essential!
  gtk_widget_show_all (GTK_WIDGET (window));
//  gtk_widget_show_all (GTK_WIDGET (window->priv->polarlayer));

  //  g_object_get (window->priv->onedlayer, "axis", &new_axis, NULL);
//  g_object_set (new_axis,
//    "mtics-beg-val", 0.0,
//    "mtics-end-val", 7000.0,
//    "mtics-automatic", FALSE,
//    "mtics", 0,

//    "tics-beg-val", 142141.0,
//    "tics-end-val", 3294010432.0,
//    "tics-automatic", TRUE,
//    "scale-auto-increment", TRUE,
//    "scale-increment-val", 5000.0,
//    "scale-limits-automatic", FALSE,
//    "scale-beg-val", -529.5668,
//    "scale-end-val", 5280.0,
//    "scale_increment_base", 60.0,

//    "force-beg-end", TRUE,
//    "axis-beg-pix-x", 0.0,
//    "axis-beg-pix-y", 0.0,
//    "axis-end-pix-x", 100.0,
//    "axis-end-pix-y", 100.0,
//    NULL);

  /* initialize window group */
  new_axis = GDV_AXIS (gdv_log_axis_new());
  gdv_oned_layer_replace_axis (window->priv->loglayer, new_axis);
  gtk_widget_show (GTK_WIDGET(new_axis));

  new_ind = gdv_special_checked_indicator_new();
  g_object_set (new_ind, "value", 50.0, NULL);
  gtk_container_add (GTK_CONTAINER (new_axis), GTK_WIDGET (new_ind));

  window->priv->changed_cb =
    g_timeout_add (20, ((GSourceFunc) timeout_cb), new_ind);

  /* initialize window group */
  g_object_get (window->priv->onedlayer, "axis", &new_axis, NULL);

  new_ind_ondl = gdv_special_checked_indicator_new();
  g_object_set (new_ind_ondl, "value", 50.0, NULL);
  gtk_container_add (GTK_CONTAINER (new_axis), GTK_WIDGET (new_ind_ondl));

  window->priv->changed_cb_oned = g_timeout_add (20, ((GSourceFunc) timeout_cb_oned),
                                                 window->priv->onedlayer);


  new_axis = (GdvAxis *) gdv_special_polar_axis_new ();
  gtk_widget_show_all (GTK_WIDGET (new_axis));
  gdv_oned_layer_replace_axis(window->priv->polarlayer, new_axis);






  global_layer = GDV_TWOD_LAYER (window->priv->twodlayer);

  new_axis =
    gdv_twod_layer_get_axis (
      GDV_TWOD_LAYER (window->priv->twodlayer),
      GDV_X1_AXIS);
  g_object_set (G_OBJECT (new_axis),
    "scale-limits-automatic", FALSE,
    "scale-end-val", 1200.0,
    NULL);

  new_axis =
    gdv_twod_layer_get_axis (
      GDV_TWOD_LAYER (window->priv->twodlayer),
      GDV_X2_AXIS);
  g_object_set (G_OBJECT (new_axis),
    "scale-limits-automatic", FALSE,
    "scale-end-val", 1200.0,
//    "visible", FALSE,
    NULL);

  new_axis =
    gdv_twod_layer_get_axis (
      GDV_TWOD_LAYER (window->priv->twodlayer),
      GDV_Y1_AXIS);
  g_object_set (G_OBJECT (new_axis),
    "scale-limits-automatic", FALSE,
//    "visible", TRUE,
//    "visible", FALSE,
    NULL);

  new_axis =
    gdv_twod_layer_get_axis (
      GDV_TWOD_LAYER (window->priv->twodlayer),
      GDV_Y2_AXIS);
  g_object_set (G_OBJECT (new_axis),
    "scale-limits-automatic", FALSE,
//    "scale-end-val", 1200.0,
//    "visible", FALSE,
    NULL);
//  gtk_widget_hide (new_axis);

  global_content = gdv_layer_content_new ();
  gdv_layer_content_add_data_point (global_content, 0.0, 0.0, 0.0);
  gtk_container_add (GTK_CONTAINER (window->priv->twodlayer), GTK_WIDGET (global_content));

//  gdv_twod_layer_set_xrange (GDV_TWOD_LAYER (global_layer), 0.0, 1200.0);

//  gtk_widget_hide (window->priv->twodlayer);
//  gtk_widget_show (window->priv->twodlayer);

  /* gsl rng */
//  gsl_rng_env_setup();
//  T = gsl_rng_default;
//  global_rng = gsl_rng_alloc (T);


/*
  g_object_get (window->priv->onedlayer, "axis", &new_axis, NULL);
  g_object_set (new_axis,
    "scale-beg-val", 0.0,
    "scale-end-val", 100.0,
    "scale-increment-val", 50.0,
//    "scale-auto-increment", FALSE,
    "axis-orientation", 1.5 * M_PI,
    "axis-direction-outside", 1.0 * M_PI,
    NULL);
*/
}

GdvInspectorAppWindow *
gdv_inspector_app_window_new (GdvInspectorApp *app)
{
  return g_object_new (GDV_INSPECTOR_APP_TYPE_WINDOW, "application", app, NULL);
}

