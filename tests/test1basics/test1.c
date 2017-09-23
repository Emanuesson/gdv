/*
 * example-main.c
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

#include <stdlib.h>
#include <math.h>
#include <gtk/gtk.h>

#include <gdv/gdv.h>

static void destroy( GtkWidget *widget,
                     gpointer   data )
{
  gtk_main_quit ();
}

static GdvLayer *layer;
static GdvOnedLayer *normal_layer_sl_sai_ta;
static GdvOnedLayer *normal_layer_sl_sai;
static GdvOnedLayer *normal_layer_sl;
static GdvOnedLayer *normal_layer_sl_ta;
static GdvOnedLayer *normal_layer_sai_ta;

static GdvIndicator *indicator;
static GdvIndicator *normal_indicator_sl_sai_ta;
static GdvIndicator *normal_indicator_sl_sai;
static GdvIndicator *normal_indicator_sl;
static GdvIndicator *normal_indicator_sl_ta;
static GdvIndicator *normal_indicator_sai_ta;

static GdvLayerContent *content;
static GdvLayerContent *content_sl_sai_ta;
static GdvLayerContent *content_sl_sai;
static GdvLayerContent *content_sl;
static GdvLayerContent *content_sl_ta;
static GdvLayerContent *content_sai_ta;


static GtkWidget *main_box;

static gint var_index;
static GtkWidget *main_window;
static GtkWidget *frame;
static GdvAxis *test_axis;

static gboolean
timeout_cb (void)
{
  if (var_index == 100) //100
  {
    GtkStyleContext *style_context;
    GtkCssProvider *settings;
    GList *axes_list;
    GdvAxis *loc_axis;

    g_print ("Gdv::Testing one dimensional layer with indicator\n");

//    gtk_container_remove (GTK_CONTAINER (frame), GTK_WIDGET (layer));
    gtk_container_remove (GTK_CONTAINER (frame), GTK_WIDGET (main_box));
//    g_object_unref (layer);

    main_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);

    layer = g_object_new (gdv_oned_layer_get_type (), NULL);
    axes_list = gtk_container_get_children(GTK_CONTAINER (layer));
    loc_axis = axes_list->data;
    g_list_free (axes_list);
    g_object_set (loc_axis,
//                  "scale-auto-increment", FALSE,
//                  "scale-limits-automatic", FALSE,
//                  "tics-automatic", FALSE,
                  "mtics-automatic", TRUE,
                  NULL);
    gtk_box_pack_start (GTK_BOX (main_box), GTK_WIDGET (layer), TRUE, TRUE, 0);
    indicator = g_object_new (gdv_indicator_get_type (), NULL);
    g_object_set (indicator, "value", 50.0, NULL);
    g_object_get (layer, "axis", &test_axis, NULL);
//    g_object_set (layer, "title", "SLA:0 - SAI:0 - TA:0\0", NULL);
    gtk_container_add (GTK_CONTAINER (test_axis), GTK_WIDGET (indicator));

    normal_layer_sl_sai_ta = g_object_new (gdv_oned_layer_get_type (), NULL);
    axes_list = gtk_container_get_children(GTK_CONTAINER(normal_layer_sl_sai_ta));
    loc_axis = axes_list->data;
    g_list_free (axes_list);
    g_object_set (loc_axis,
                  "scale-auto-increment", TRUE,
                  "scale-limits-automatic", TRUE,
                  "tics-automatic", TRUE,
                  "mtics-automatic", TRUE,
                  NULL);
    gtk_box_pack_start (GTK_BOX (main_box), GTK_WIDGET (normal_layer_sl_sai_ta), TRUE, TRUE, 0);
    normal_indicator_sl_sai_ta = g_object_new (gdv_indicator_get_type (), NULL);
    g_object_set (normal_indicator_sl_sai_ta, "value", 50.0, NULL);
    g_object_get (normal_layer_sl_sai_ta, "axis", &test_axis, NULL);
    gtk_container_add (GTK_CONTAINER (test_axis),
                       GTK_WIDGET (normal_indicator_sl_sai_ta));

    normal_layer_sl_sai = g_object_new (gdv_oned_layer_get_type (), NULL);
    axes_list = gtk_container_get_children(GTK_CONTAINER(normal_layer_sl_sai));
    loc_axis = axes_list->data;
    g_list_free (axes_list);
    style_context = gtk_widget_get_style_context (GTK_WIDGET(loc_axis));
    g_object_set (loc_axis,
                  "scale-auto-increment", TRUE,
                  "scale-limits-automatic", TRUE,
                  "tics-automatic", TRUE,
                  "mtics-automatic", FALSE,
                  NULL);
    gtk_box_pack_start (GTK_BOX (main_box), GTK_WIDGET (normal_layer_sl_sai), TRUE, TRUE, 0);
    normal_indicator_sl_sai = g_object_new (gdv_indicator_get_type (), NULL);
    g_object_set (normal_indicator_sl_sai, "value", 50.0, NULL);
    g_object_get (normal_layer_sl_sai, "axis", &test_axis, NULL);
    gtk_container_add (GTK_CONTAINER (test_axis), GTK_WIDGET (normal_indicator_sl_sai));

    normal_layer_sl = g_object_new (gdv_oned_layer_get_type (), NULL);
    axes_list = gtk_container_get_children(GTK_CONTAINER (normal_layer_sl));
    loc_axis = axes_list->data;
    g_list_free (axes_list);
    g_object_set (loc_axis,
                  "scale-auto-increment", TRUE,
                  "scale-limits-automatic", FALSE,
                  "tics-automatic", FALSE,
                  "mtics-automatic", TRUE,
                  NULL);
    gtk_box_pack_start (GTK_BOX (main_box), GTK_WIDGET (normal_layer_sl), TRUE, TRUE, 0);
    normal_indicator_sl = g_object_new (gdv_indicator_get_type (), NULL);
    g_object_set (normal_indicator_sl, "value", 50.0, NULL);
    g_object_get (normal_layer_sl, "axis", &test_axis, NULL);
    gtk_container_add (GTK_CONTAINER (test_axis), GTK_WIDGET (normal_indicator_sl));

    normal_layer_sl_ta = g_object_new (gdv_oned_layer_get_type (), NULL);
    axes_list = gtk_container_get_children(GTK_CONTAINER (normal_layer_sl_ta));
    loc_axis = axes_list->data;
    g_list_free (axes_list);
    g_object_set (loc_axis,
                  "scale-auto-increment", TRUE,
                  "scale-limits-automatic", FALSE,
                  "tics-automatic", TRUE,
                  "mtics-automatic", TRUE,
                  NULL);
    gtk_box_pack_start (GTK_BOX (main_box), GTK_WIDGET (normal_layer_sl_ta), TRUE, TRUE, 0);
    normal_indicator_sl_ta = g_object_new (gdv_indicator_get_type (), NULL);
    g_object_set (normal_indicator_sl_ta, "value", 50.0, NULL);
    g_object_get (normal_layer_sl_ta, "axis", &test_axis, NULL);
    gtk_container_add (GTK_CONTAINER (test_axis), GTK_WIDGET (normal_indicator_sl_ta));

    normal_layer_sai_ta = g_object_new (gdv_oned_layer_get_type (), NULL);
    axes_list = gtk_container_get_children(GTK_CONTAINER (normal_layer_sai_ta));
    loc_axis = axes_list->data;
    g_list_free (axes_list);
    g_object_set (loc_axis,
                  "scale-auto-increment", FALSE,
                  "scale-limits-automatic", TRUE,
                  "tics-automatic", TRUE,
                  "mtics-automatic", TRUE,
                  NULL);
    gtk_box_pack_start (GTK_BOX (main_box), GTK_WIDGET (normal_layer_sai_ta), TRUE, TRUE, 0);
    normal_indicator_sai_ta = g_object_new (gdv_indicator_get_type (), NULL);
    g_object_set (normal_indicator_sai_ta, "value", 50.0, NULL);
    g_object_get (normal_layer_sai_ta, "axis", &test_axis, NULL);
    gtk_container_add (GTK_CONTAINER (test_axis), GTK_WIDGET (normal_indicator_sai_ta));

    gtk_container_add (GTK_CONTAINER (frame), GTK_WIDGET (main_box));

    GList *children = gtk_container_get_children (GTK_CONTAINER (layer));
    gint no_of_child = 0;
    while (children)
    {
      GtkWidgetPath* path = gtk_widget_get_path (children->data);

//      g_print ("Children no %d of type %s\n",
//        no_of_child,
//        g_type_name (G_TYPE_FROM_INSTANCE (children->data)));
//      g_print ("child path: %s\n", gtk_widget_path_to_string (path));
      children = children->next;
      no_of_child++;
    }

//    gtk_widget_show_all (GTK_WIDGET (frame));
    gtk_widget_show_all (GTK_WIDGET (main_window));

  }
  else if (FALSE) //var_index == 7
  {
    if (main_window)
    g_print ("reached\n");
    g_printerr ("reached2\n");
    gtk_widget_destroy (main_window);
    g_printerr ("reached3\n");
    exit(0);
  }
  else if (var_index == 2000)
  {
//    GtkWidget *frame;
    GtkWidget *old_layer;
    GdvTwodLayer *new_layer;

    g_print ("Gdv::Testing a two dimensional layer with indicator\n");

//    frame = gtk_bin_get_child (GTK_BIN (main_window));
//    old_layer = gtk_bin_get_child (GTK_BIN (frame));
//    old_layer = gtk_bin_get_child (GTK_BIN (main_window));
//    gtk_container_remove (GTK_CONTAINER (main_box), old_layer);
    gtk_container_remove (GTK_CONTAINER (frame), main_box);
    new_layer = gdv_twod_layer_new ();
    layer = GDV_LAYER (new_layer);

    gtk_container_add (GTK_CONTAINER (frame), GTK_WIDGET (new_layer));

    content = gdv_layer_content_new();

    gtk_container_add(GTK_CONTAINER (new_layer), GTK_WIDGET(content));
    gdv_layer_content_add_data_point (content, 0.0, 0.0, 0.0);

    gtk_widget_show_all (GTK_WIDGET (main_window));

    GList *children = gtk_container_get_children (GTK_CONTAINER (new_layer));
    gint no_of_child = 0;
    while (children)
    {
      GtkWidgetPath* path = gtk_widget_get_path (children->data);

//      g_print ("Children no %d of type %s\n",
//        no_of_child,
//        g_type_name (G_TYPE_FROM_INSTANCE (children->data)));
//      g_print ("child path: %s\n", gtk_widget_path_to_string (path));
      children = children->next;
      no_of_child++;
    }


  }
  else if (var_index == 3000)
  {
//    return TRUE;
    GtkWidget *frame;
    GtkWidget *old_layer;
    GdvTwodLayer *new_layer;
    GdvAxis *x2_axis, *x1_axis, *y1_axis, *y2_axis;
    GtkStyleContext *style_context;
    GtkCssProvider *settings;

    g_print ("Gdv::Testing plotting without axis\n");

//    frame = gtk_bin_get_child (GTK_BIN (main_window));
//    old_layer = gtk_bin_get_child (GTK_BIN (frame));
    old_layer = GTK_WIDGET (layer);
    new_layer = GDV_TWOD_LAYER (old_layer);
    gdv_twod_layer_unset_axis (new_layer, GDV_X2_AXIS);

    x1_axis = g_object_new (gdv_log_axis_get_type (), NULL);
    x2_axis =
      g_object_new (gdv_log_axis_get_type (),
        "tic-labels", FALSE,
        NULL);

    style_context = gtk_widget_get_style_context (GTK_WIDGET(x2_axis));
    settings = gtk_css_provider_new();
    gtk_css_provider_load_from_data (settings,
                                     "GdvLogAxis {"
                                     "    -GdvAxis-tics-out-length: 0;"
                                     "}\0", -1, NULL);
    gtk_style_context_add_provider (style_context,
      GTK_STYLE_PROVIDER (settings),
      GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    gdv_twod_layer_set_axis (new_layer, x1_axis, GDV_X1_AXIS);
    gtk_widget_set_name (GTK_WIDGET (x1_axis), "x1-axis");
    gdv_twod_layer_set_axis (new_layer, x2_axis, GDV_X2_AXIS);
    gtk_widget_set_name (GTK_WIDGET (x2_axis), "x2-axis");

    gtk_container_remove(GTK_CONTAINER (new_layer), GTK_WIDGET(content));
    content = gdv_layer_content_new();
    gtk_container_add(GTK_CONTAINER (new_layer), GTK_WIDGET(content));

    gtk_widget_show_all (GTK_WIDGET (main_window));
  }
  else if (var_index >= 3000)
  {
    if (GDV_LAYER_IS_CONTENT (content))
      gdv_layer_content_add_data_point (
        content,
        (gdouble) 100.0 * (var_index - 3000),
        sin (0.01 * (gdouble) (var_index - 3000)), 0.0);
  }
  else if (var_index >= 2000)
  {
    if (GDV_LAYER_IS_CONTENT (content))
      gdv_layer_content_add_data_point (
        content,
        (gdouble) 100.0 * (var_index - 2000),
        sin (0.01 * (gdouble) (var_index - 2000)), 0.0);

//    GList *children = gtk_container_get_children (GTK_CONTAINER (layer));
//    while (children) {
//      if (GDV_IS_AXIS (children->data))
//        report_axis_props (children->data);
//      children = children->next;
//    }


  }
  else if (var_index > 100)
  {
    g_object_set (
      indicator,
      "value", 50.0 + 55.0 * sin (0.010 * (gdouble) var_index),
      NULL);
    g_object_set (
      normal_indicator_sl_sai_ta,
      "value", 50.0 + 55.0 * sin (0.011 * (gdouble) var_index),
      NULL);
    g_object_set (
      normal_indicator_sl_sai,
      "value", 50.0 + 55.0 * sin (0.012 * (gdouble) var_index),
      NULL);
    g_object_set (
      normal_indicator_sl,
      "value", 50.0 + 55.0 * sin (0.013 * (gdouble) var_index),
      NULL);
    g_object_set (
      normal_indicator_sl_ta,
      "value", 50.0 + 55.0 * sin (0.014 * (gdouble) var_index),
      NULL);
    g_object_set (
      normal_indicator_sai_ta,
      "value", 50.0 + 55.0 * sin (0.015 * (gdouble) var_index),
      NULL);

  }

  var_index++;

//  gboolean *new_boolean = g_new (gboolean, 1);
//  *new_boolean = TRUE;

  return TRUE;
}

static gboolean
button_pressed (GtkWidget *widget,
                GdkEventButton *event,
                gpointer data)
{
  if (event->type == GDK_BUTTON_PRESS)
    {
//      guint begin_x, end_x, begin_y, end_y;

//      g_print ("clicked on WINDOW\n");

      gtk_widget_grab_focus (widget);
      gtk_widget_queue_draw (widget);
    }

  return TRUE;
}


int
main (int argc, char **argv)
{
  GdvLayerContent *layer_content;

  GtkWidget *test_indicator_label;

  GdkScreen *screen_window;
  GtkCssProvider *css_provider;

  gtk_init (&argc, &argv);

  main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);


  g_signal_connect (G_OBJECT (main_window), "button_press_event",
                    G_CALLBACK (button_pressed), NULL);

  frame = gtk_frame_new (NULL);
  g_signal_connect (main_window, "destroy",
	      G_CALLBACK (destroy), NULL);
  gtk_container_set_border_width (GTK_CONTAINER (main_window), 8);
  gtk_container_add (GTK_CONTAINER (main_window), GTK_WIDGET (frame));

//  g_object_set (main_window, "default-width", 100, "default-height", 100, NULL);

  layer = g_object_new (gdv_layer_get_type (), NULL);
  test_axis = g_object_new (gdv_axis_get_type (), NULL);
  gtk_container_add (GTK_CONTAINER (layer), GTK_WIDGET (test_axis));

  main_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_container_add (GTK_CONTAINER (main_box), GTK_WIDGET (layer));
  gtk_container_add (GTK_CONTAINER (frame), GTK_WIDGET (main_box));

//  gtk_container_add (GTK_CONTAINER (frame), GTK_WIDGET (layer));
//  gtk_container_add (GTK_CONTAINER (main_window), GTK_WIDGET (fr));

  g_print ("Gdv::Testing the next layer with indicator\n");

//  var_index = 2000;
  var_index = 0;
  g_timeout_add (10, ((GSourceFunc) timeout_cb), NULL);

  gtk_widget_show_all (GTK_WIDGET (main_window));

  gtk_main ();

  return 0;
}

