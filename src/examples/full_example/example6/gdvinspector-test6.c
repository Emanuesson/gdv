/*
 * gdvinspector-test6.c
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

#include <gdv/gdv.h>

#include "gdvinspector-test6.h"
#include "math.h"

//#include "gemu_sub/gemu-glib-util.h"
//#include "gemu_sub/gemu-gtk-util.h"


struct _GdvInspectorTest6Private
{
  gint placeholder;
};

G_DEFINE_TYPE_WITH_PRIVATE (
  GdvInspectorTest6,
  gdv_inspector_test6,
  GTK_TYPE_FRAME)

static void
gdv_inspector_test6_class_init (GdvInspectorTest6Class *klass)
{
}

void
gdv_inspector_test6_init (GdvInspectorTest6 *frame)
{

  GtkWidget *left_box, *right_box, *main_box;

  GdvLayer *layer;
  GdvLogAxis *new_x1_axis, *new_x2_axis;
  GtkWidget *tmp_label;
  GdvAxis *current_axis;
  GdvIndicator *test_indicator;
  gint i;
  GdvLayerContent *new_content, *new_content2;

  GtkStyleContext *style_context;
  GtkCssProvider *css_provider;
  GdvLegend *new_legend;

//  gemu_gtk_util_signal_connect_to_widget_children (
//    GTK_CONTAINER (main_window),
//    gemu_gtk_util_signal_use_append_and_remove,
//    NULL,
//    NULL);

  left_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  right_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  main_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_box_pack_start (GTK_BOX (main_box), GTK_WIDGET (left_box), TRUE, TRUE, 2);
  gtk_box_pack_start (GTK_BOX (main_box), GTK_WIDGET (right_box), FALSE, FALSE, 2);
  gtk_container_add (GTK_CONTAINER (frame), GTK_WIDGET (main_box));

  layer = g_object_new (gdv_twod_layer_get_type(), NULL);
//  gtk_container_add (GTK_CONTAINER (main_window), GTK_WIDGET (layer));

  gtk_box_pack_start (GTK_BOX (left_box), GTK_WIDGET (layer), TRUE, TRUE, 0);

  new_legend = gdv_legend_new();
  g_object_set (new_legend, "layer", layer, NULL);
  gtk_box_pack_start (GTK_BOX (right_box),
                      GTK_WIDGET (new_legend),
                      TRUE,
                      FALSE,
                      2);

//  gemu_glib_util_connect_to_all_signals (axis_list->data,
//                                         gemu_glib_util_show_details,
//                                         NULL,
//                                         NULL);

//  gemu_gtk_util_signal_connect_to_widget_children (
//    axis_list->data,
//    gemu_glib_util_show_details,
//    NULL, NULL);

//  gemu_glib_util_connect_to_all_signals (
//   G_OBJECT (layer),
//   gemu_glib_util_show_details,
//   NULL,
//   NULL);


//  gtk_window_set_title (GTK_WINDOW (main_window), "gdv test-app");

//  main_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
//  gtk_container_add (GTK_CONTAINER (main_window), GTK_WIDGET (main_box));

//  layer = g_object_new (GDV_ONED_LAYER_TYPE, NULL);
//  layer = g_object_new (GDV_TWOD_LAYER_TYPE, NULL);

//////////


//  gtk_box_pack_start (GTK_BOX (main_box), GTK_WIDGET (layer), TRUE, TRUE, 5);


/////////////////////////////////////////

  new_x1_axis =
    g_object_new (GDV_LOG_TYPE_AXIS,
                  "halign", GTK_ALIGN_FILL,
                  "valign", GTK_ALIGN_END,
                  "axis-orientation", -0.5 * M_PI,
                  "axis-direction-outside", M_PI,
                  NULL);
  new_x2_axis =
    g_object_new (GDV_LOG_TYPE_AXIS,
                  "halign", GTK_ALIGN_FILL,
                  "valign", GTK_ALIGN_START,
                  "axis-orientation", -0.5 * M_PI,
                  "axis-direction-outside", 0.0,
                  NULL);

  gdv_twod_layer_unset_axis (GDV_TWOD_LAYER (layer), GDV_X1_AXIS);
  gdv_twod_layer_set_axis (GDV_TWOD_LAYER (layer), GDV_AXIS (new_x1_axis), GDV_X1_AXIS);
  gdv_twod_layer_unset_axis (GDV_TWOD_LAYER (layer), GDV_X2_AXIS);
  gdv_twod_layer_set_axis (GDV_TWOD_LAYER (layer), GDV_AXIS (new_x2_axis), GDV_X2_AXIS);




/////////////////////////////////////////






//  GList *axes_list = gdv_layer_get_axis_list (layer);

//  g_object_set (axes_list->data,
//    "axis-direction-outside", M_PI,
//    "axis-orientation", 0.5 * M_PI,
//    NULL);

//  g_object_set (axes_list->data, "title", "hello World", NULL);

  current_axis = gdv_twod_layer_get_axis (GDV_TWOD_LAYER (layer), GDV_X1_AXIS);
  tmp_label = g_object_new (gtk_label_get_type(), NULL);
  gtk_label_set_markup (GTK_LABEL (tmp_label), "X<sub>1</sub>-Axis");
//  g_object_set (tmp_label, "xalign", -50.0, "yalign", -50.0, NULL);
  gtk_label_set_angle (GTK_LABEL (tmp_label), 0.0);
  gdv_axis_set_title_widget (GDV_AXIS (current_axis), tmp_label);

  test_indicator = g_object_new (GDV_TYPE_INDICATOR, NULL);
  gtk_container_add (GTK_CONTAINER (current_axis), GTK_WIDGET (test_indicator));
  g_object_set (test_indicator, "value", 50.0, NULL);

  current_axis = gdv_twod_layer_get_axis (GDV_TWOD_LAYER (layer), GDV_Y1_AXIS);
  tmp_label = g_object_new (gtk_label_get_type(), NULL);
  gtk_label_set_markup (GTK_LABEL (tmp_label), "Y<sub>1</sub>-Axis");
  gtk_label_set_angle (GTK_LABEL (tmp_label), 90.0);
  gdv_axis_set_title_widget (GDV_AXIS (current_axis), tmp_label);

  current_axis = gdv_twod_layer_get_axis (GDV_TWOD_LAYER (layer), GDV_X2_AXIS);
  tmp_label = g_object_new (gtk_label_get_type(), NULL);
  gtk_label_set_markup (GTK_LABEL (tmp_label), "X<sub>2</sub>-Axis");
  gtk_label_set_angle (GTK_LABEL (tmp_label), 0.0);
  gdv_axis_set_title_widget (GDV_AXIS (current_axis), tmp_label);

  current_axis = gdv_twod_layer_get_axis (GDV_TWOD_LAYER (layer), GDV_Y2_AXIS);
  tmp_label = g_object_new (gtk_label_get_type(), NULL);
  gtk_label_set_markup (GTK_LABEL (tmp_label), "Y<sub>2</sub>-Axis");
  gtk_label_set_angle (GTK_LABEL (tmp_label), -90.0);
  gdv_axis_set_title_widget (GDV_AXIS (current_axis), tmp_label);

//  gtk_container_add (GTK_CONTAINER (left_box), GTK_WIDGET (layer));




/*
  gint tmp_nat, tmp_min;

  GDV_AXIS_GET_CLASS (GDV_AXIS (axes_list->data))->get_space_to_beg_position (
    GDV_AXIS (axes_list->data),
    GTK_POS_RIGHT, -1,
    &tmp_min, &tmp_nat, NULL);
  GDV_AXIS_GET_CLASS (GDV_AXIS (axes_list->data))->get_space_to_end_position (
    GDV_AXIS (axes_list->data),
    GTK_POS_RIGHT, -1,
    &tmp_min, &tmp_nat, NULL);
  GDV_AXIS_GET_CLASS (GDV_AXIS (axes_list->data))->get_space_to_beg_position (
    GDV_AXIS (axes_list->data),
    GTK_POS_LEFT, -1,
    &tmp_min, &tmp_nat, NULL);
  GDV_AXIS_GET_CLASS (GDV_AXIS (axes_list->data))->get_space_to_end_position (
    GDV_AXIS (axes_list->data),
    GTK_POS_LEFT, -1,
    &tmp_min, &tmp_nat, NULL);
  GDV_AXIS_GET_CLASS (GDV_AXIS (axes_list->data))->get_space_to_beg_position (
    GDV_AXIS (axes_list->data),
    GTK_POS_TOP, -1,
    &tmp_min, &tmp_nat, NULL);
  GDV_AXIS_GET_CLASS (GDV_AXIS (axes_list->data))->get_space_to_end_position (
    GDV_AXIS (axes_list->data),
    GTK_POS_TOP, -1,
    &tmp_min, &tmp_nat, NULL);
  GDV_AXIS_GET_CLASS (GDV_AXIS (axes_list->data))->get_space_to_beg_position (
    GDV_AXIS (axes_list->data),
    GTK_POS_BOTTOM, -1,
    &tmp_min, &tmp_nat, NULL);
  GDV_AXIS_GET_CLASS (GDV_AXIS (axes_list->data))->get_space_to_end_position (
    GDV_AXIS (axes_list->data),
    GTK_POS_BOTTOM, -1,
    &tmp_min, &tmp_nat, NULL);

*/

/*

  axes_list = axes_list->next;
  test_indicator = g_object_new (GDV_TYPE_INDICATOR, NULL);
  gtk_container_add (GTK_CONTAINER (axes_list->data), GTK_WIDGET (test_indicator));
  g_object_set (test_indicator, "value", 50.0, NULL);
  g_object_set (axes_list->data, "scale-auto-increment", FALSE, NULL);

  axes_list = axes_list->next;
  test_indicator = g_object_new (GDV_TYPE_INDICATOR, NULL);
  gtk_container_add (GTK_CONTAINER (axes_list->data), GTK_WIDGET (test_indicator));
  g_object_set (test_indicator, "value", 50.0, NULL);
  g_object_set (axes_list->data, "scale-auto-increment", FALSE, NULL);

  axes_list = axes_list->next;
  test_indicator = g_object_new (GDV_TYPE_INDICATOR, NULL);
  gtk_container_add (GTK_CONTAINER (axes_list->data), GTK_WIDGET (test_indicator));
  g_object_set (test_indicator, "value", 50.0, NULL);
  g_object_set (axes_list->data, "scale-auto-increment", FALSE, NULL);

//  while (axes_list)
//  {
//    g_print ("Another axis is present\n");
//    axes_list = axes_list->next;
//    exit(0);
//  }
*/

  new_content = g_object_new (GDV_LAYER_TYPE_CONTENT, "title", "sin", NULL);
  new_content2 = g_object_new (GDV_LAYER_TYPE_CONTENT, "title", "cos", NULL);

  for (i = 0; i < 500; i++)
  {
    gdouble double_i = (gdouble) i;
    gdv_layer_content_add_data_point (new_content,
      double_i / 5.0,
      50.0 * sin (double_i * M_PI / 250.0) + 50.0,
      50.0);
    gdv_layer_content_add_data_point (new_content2,
      double_i / 5.0,
      50.0 * cos (double_i * M_PI / 250.0) + 50.0,
      50.0);
  }

//  gdv_layer_content_add_data_point (new_content, 48.0, 50.0, 50.0);
//  gdv_layer_content_add_data_point (new_content, 50.0, 52.0, 50.0);
//  gdv_layer_content_add_data_point (new_content, 52.0, 50.0, 50.0);
//  gdv_layer_content_add_data_point (new_content, 50.0, 48.0, 50.0);
//  gdv_layer_content_add_data_point (new_content, 48.0, 50.0, 50.0);

  gtk_widget_show(GTK_WIDGET (new_content));
//  gtk_overlay_add_overlay (GTK_OVERLAY (layer), GTK_WIDGET (new_content));
  gtk_container_add (GTK_CONTAINER (layer), GTK_WIDGET (new_content));
  gtk_overlay_reorder_overlay (GTK_OVERLAY (layer), GTK_WIDGET (new_content), -1);

  gtk_widget_show(GTK_WIDGET (new_content2));
//  gtk_overlay_add_overlay (GTK_OVERLAY (layer), GTK_WIDGET (new_content2));
  gtk_container_add (GTK_CONTAINER (layer), GTK_WIDGET (new_content2));
  gtk_overlay_reorder_overlay (GTK_OVERLAY (layer), GTK_WIDGET (new_content2), -1);



  css_provider = gtk_css_provider_new ();
  style_context = gtk_widget_get_style_context (GTK_WIDGET (new_content));
  gtk_css_provider_load_from_data (css_provider,
  "*{"
  "  -GdvLayerContent-line-color: #ff00ff;"
  "  -GdvLayerContent-point-color: #ff00ff;"
  "  -GdvLayerContent-line-dash-portion: 10.0;"
  "  -GdvLayerContent-line-secondary-dash-portion: 10.0;"
  "                                 }\0", -1, NULL);

  gtk_style_context_add_provider (
    style_context,
    GTK_STYLE_PROVIDER (css_provider),
    GTK_STYLE_PROVIDER_PRIORITY_FALLBACK);

//  g_object_set (axes_list->data,
//                "scale-auto-increment", TRUE,
//                "scale-limits-automatic", TRUE,
//                "tics-automatic", TRUE,
//                "mtics-automatic", TRUE,
//                "scale-auto-increment", FALSE,
//                "scale-limits-automatic", FALSE,
//                "tics-automatic", FALSE,
//                "mtics-automatic", FALSE,
//                NULL);

  // second object
//  layer = g_object_new (GDV_ONED_LAYER_TYPE, NULL);
//  gtk_box_pack_start (GTK_BOX (main_box), GTK_WIDGET (layer), TRUE, TRUE, 5);
//  GList *axes_list = gdv_layer_get_axis_list (layer);
//  test_indicator = g_object_new (GDV_TYPE_INDICATOR, NULL);
//  gtk_container_add (GTK_CONTAINER (axes_list->data), GTK_WIDGET (test_indicator));
//  g_object_set (test_indicator, "value", 55.0, NULL);


/**/
/*


  gtk_container_set_border_width (GTK_CONTAINER (main_window), 8);

  layer = g_object_new (gdv_layer_get_type (), NULL);


  GdvAxis *x2_axis = g_object_new (gdv_linear_axis_get_type(), NULL);
  spin_button = gtk_spin_button_new_with_range (-20.0, 20.0, 0.01);
  gtk_spin_button_set_digits (GTK_SPIN_BUTTON (spin_button), 2);
  g_signal_connect (spin_button, "value-changed",
	      G_CALLBACK (activate_button), x2_axis);
  gtk_box_pack_start (GTK_BOX (left_box), GTK_WIDGET (spin_button), FALSE, TRUE, 0);

  gdv_axis_title_set_markup (x2_axis, "hello World\0");

  gtk_container_add (GTK_CONTAINER (layer), GTK_WIDGET (x2_axis));
  g_object_set (G_OBJECT(x2_axis),
    "halign", GTK_ALIGN_FILL,
//    "halign", GTK_ALIGN_END,
    "scale-beg-val", 00.000,
    "scale-end-val", 100.0,
//    "tics-beg-val", -1000.0,
//    "tics-end-val", 20000.0,
 //   "margin-end", 10,
//    "axis-orientation", -0.0 * M_PI,
//    "axis-direction-outside", -0.5 * M_PI,
    "axis-orientation", 1.48 * M_PI,
    "axis-direction-outside", 0.98 * M_PI,
//    "valign", GTK_ALIGN_END,
    "valign", GTK_ALIGN_FILL,
 //   "margin-bottom", 10,
 //   "margin-top", 30,
    NULL);
  gtk_box_pack_start (GTK_BOX (left_box), GTK_WIDGET (layer), TRUE, TRUE, 2);

  gdouble tmp_butt_start_val;
  g_object_get (G_OBJECT(x2_axis), "axis-orientation", &tmp_butt_start_val, NULL);
  gtk_spin_button_set_value (GTK_SPIN_BUTTON (spin_button), tmp_butt_start_val);
*/
/**/
/*

  GdvLayer *second_layer = g_object_new (gdv_layer_get_type (), NULL);
  GdvAxis *second_axis = g_object_new (gdv_linear_axis_get_type(), NULL);
  gtk_container_add (GTK_CONTAINER (second_layer), GTK_WIDGET (second_axis));
  g_object_set (G_OBJECT(second_axis),
    "halign", GTK_ALIGN_FILL,
    "scale-beg-val", 00.000,
    "scale-end-val", 100.0,
    "axis-orientation", 1.0 * M_PI,
    "axis-direction-outside", 0.5 * M_PI,
    "valign", GTK_ALIGN_FILL,
    NULL);
//  gtk_box_pack_start (GTK_BOX (left_box), GTK_WIDGET (second_layer), TRUE, TRUE, 2);


  GdvLayer *third_layer = g_object_new (gdv_layer_get_type (), NULL);
  GdvAxis *third_axis = g_object_new (gdv_linear_axis_get_type(), NULL);
  gtk_container_add (GTK_CONTAINER (third_layer), GTK_WIDGET (third_axis));
  g_object_set (G_OBJECT(third_axis),
    "halign", GTK_ALIGN_FILL,
    "scale-beg-val", 00.000,
    "scale-end-val", 100.0,
    "axis-orientation", -0.5 * M_PI,
    "axis-direction-outside", 0.0 * M_PI,
    "valign", GTK_ALIGN_FILL,
    NULL);
//  gtk_box_pack_start (GTK_BOX (right_box), GTK_WIDGET (third_layer), TRUE, TRUE, 2);

  GdvLayer *fourth_layer = g_object_new (gdv_layer_get_type (), NULL);
  GdvAxis *fourth_axis = g_object_new (gdv_linear_axis_get_type(), NULL);
  gtk_container_add (GTK_CONTAINER (fourth_layer), GTK_WIDGET (fourth_axis));
  g_object_set (G_OBJECT(fourth_axis),
    "halign", GTK_ALIGN_FILL,
    "scale-beg-val", 00.000,
    "scale-end-val", 100.0,
    "axis-orientation", 0.5 * M_PI,
    "axis-direction-outside", 1.0 * M_PI,
    "valign", GTK_ALIGN_FILL,
    NULL);
//  gtk_box_pack_start (GTK_BOX (right_box), GTK_WIDGET (fourth_layer), TRUE, TRUE, 2);


//  gtk_box_pack_start (GTK_BOX (main_box), GTK_WIDGET (spin_button), FALSE, TRUE, 0);


//  GdvTic *test_tic = gdv_tic_new ("hello world");

//  GtkWidget *test_button = gtk_button_new_with_label("hello world");
//  gtk_overlay_add_overlay (GTK_OVERLAY (layer), GTK_WIDGET (test_tic));
//  g_object_set (G_OBJECT(test_tic),
//    "pos-x", 100.0,
//    "pos-y", 100.0,
//    "axis-dir-x", 0.0,
//    "axis-dir-y", 1.0,
//    "axis-inner-dir-x", -100.0,
//    "axis-inner-dir-y", -50.0,
//    "label-xalign", 0.0,
//    "label-yalign", 0.0,
//    "halign", GTK_ALIGN_END,
//    "margin-end", 10,
//    "valign", GTK_ALIGN_FILL,
//    "valign", GTK_ALIGN_END,
//    "margin-bottom", 10,
//    "margin-top", 10,
//    NULL);

//  gtk_container_add (GTK_CONTAINER (main_window), GTK_WIDGET (test_tic));


//  GList *children_list = gtk_container_get_children (GTK_CONTAINER (layer));
//  while (children_list)
//  {
//    g_print ("chld of type: %s\n", g_type_name (G_OBJECT_TYPE (children_list->data)));
//    children_list = children_list->next;
//  }
//  gtk_container_remove (GTK_CONTAINER (layer), GTK_WIDGET (x2_axis));


//  gtk_widget_set_name (GTK_WIDGET (layer), "test-layer");

//  gtk_box_pack_start (
//    GTK_BOX (main_box), GTK_WIDGET (layer),
//    TRUE, TRUE, 0);
//  gtk_box_pack_start (
//    GTK_BOX (main_box), GTK_WIDGET (test_legend),
//    FALSE, TRUE, 0);

//  overlay = gtk_overlay_new ();
//  gtk_container_add (GTK_CONTAINER (overlay), GTK_WIDGET (layer));
//  gtk_container_add (GTK_CONTAINER (frame), GTK_WIDGET (overlay));
//  gtk_overlay_add_overlay (GTK_OVERLAY (overlay), GTK_WIDGET(test_legend));
  //gtk_widget_set_size (GTK_WIDGET(test_legend), 40, 40);
//  g_object_set (G_OBJECT(test_legend),
//    "margin-start", 180, "margin-end", 180,
//    "margin-top", 180, "margin-bottom", 70, NULL);

//  content1 = gdv_layer_content_new();
//  g_object_set (content1, "title", "sin(x)", NULL);
//  gtk_container_add(GTK_CONTAINER (layer), GTK_WIDGET(content1));
//  content2 = gdv_layer_content_new();
//  g_object_set (content2, "title", "cos(x)", NULL);
//  gtk_container_add(GTK_CONTAINER (layer), GTK_WIDGET(content2));

//  gint i;
//  for (i = 0; i < 400; i++)
//  {
//    gdv_layer_content_add_data_point (
//      content1,
//      M_PI * (gdouble) i  / 200.0,
//      sin (M_PI * ((gdouble) i) / 200.0), 0.0);
//    gdv_layer_content_add_data_point (
//      content2,
//      M_PI * (gdouble) i / 200.0,
//      cos (M_PI * ((gdouble) i) / 200.0), 0.0);
//  }

//  g_object_set (test_legend, "layer", layer, NULL);
*/

//  new_legend = gdv_legend_new();
//  g_object_set (new_legend, "layer", layer, NULL);
//  gtk_box_pack_start (GTK_BOX (right_box), GTK_WIDGET (new_legend), TRUE, FALSE, 2);

//  report_axis_props (x2_axis);
}

GdvInspectorTest6 *
gdv_inspector_test6_new (void)
{
  return g_object_new (GDV_INSPECTOR_TYPE_TEST6, NULL);
}

