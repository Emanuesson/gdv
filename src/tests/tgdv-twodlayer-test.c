/* tgdv-twodlayer-test.c
 *
 * Copyright © 2018 Emanuel Schmidt <eschmidt216@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include<math.h>

#include<stdio.h>
#include<gdv/gdv.h>

#include "tgdv-shared-functions.h"
#include "tgdv-linearaxis.h"
#include "tgdv-axis.h"
#include "tgdv-layer.h"

static const guint cb_time = 800;

struct _tgdv_twodlayer_data {
  GtkWidget *window;
  GdvTwodLayer *layer;
//  GdvAxis *axis;
};

static gboolean
teardown_cb (GtkWidget *widget)
{
  gtk_widget_destroy (widget);
  gtk_main_quit();
  return FALSE;
}


static void
test_twodlayer_pre_normal (void)
{
  struct _tgdv_twodlayer_data data_str;
  struct _tgdv_twodlayer_data * data = &data_str;
  GList *laxis, *laxis_cpy;

  gtk_init (NULL, 0);

  data->window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  data->layer = g_object_new (GDV_TWOD_LAYER_TYPE, NULL);
  gtk_container_add (GTK_CONTAINER (data->window), GTK_WIDGET (data->layer));
  gtk_widget_set_size_request (GTK_WIDGET (data->window), 800, 800);
  gtk_window_set_title (GTK_WINDOW (data->window), "Test window");
  gtk_widget_show_all(data->window);

  tgdv_layer_test_integrity(GDV_LAYER(data->layer));
  laxis_cpy = gdv_layer_get_axis_list(GDV_LAYER(data->layer));

  for (laxis = laxis_cpy; laxis; laxis = laxis->next)
  {
    g_assert_nonnull(laxis->data);

    tgdv_linearaxis_test_integrity(GDV_LINEAR_AXIS(laxis->data));
    tgdv_axis_test_full(GDV_AXIS(laxis->data));
  }

  while (gtk_events_pending ())
    gtk_main_iteration ();

  g_timeout_add (cb_time, ((GSourceFunc) teardown_cb), data->window); // cb_time
  tgdv_layer_test_integrity(GDV_LAYER(data->layer));

  for (laxis = laxis_cpy; laxis; laxis = laxis->next)
  {
    g_assert_nonnull(laxis->data);

    tgdv_linearaxis_test_integrity(GDV_LINEAR_AXIS(laxis->data));
    tgdv_axis_test_full(GDV_AXIS(laxis->data));
  }

  gtk_main ();

  g_list_free(laxis_cpy);
  data = NULL;
}

static void
test_twodlayer_log_legend (void)
{
  struct _tgdv_twodlayer_data data_str;
  struct _tgdv_twodlayer_data * data = &data_str;

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

  gtk_init (NULL, 0);

  data->window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_widget_set_size_request (GTK_WIDGET (data->window), 800, 800);
  gtk_window_set_title (GTK_WINDOW (data->window), "Test window");


  left_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  right_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  main_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_box_pack_start (GTK_BOX (main_box), GTK_WIDGET (left_box), TRUE, TRUE, 2);
  gtk_box_pack_start (GTK_BOX (main_box), GTK_WIDGET (right_box), FALSE, FALSE, 2);
  gtk_container_add (GTK_CONTAINER (data->window), GTK_WIDGET (main_box));

  layer = g_object_new (gdv_twod_layer_get_type(), NULL);
  data->layer = GDV_TWOD_LAYER(layer);

  gtk_box_pack_start (GTK_BOX (left_box), GTK_WIDGET (layer), TRUE, TRUE, 0);

  new_legend = gdv_legend_new();
  g_object_set (new_legend, "layer", layer, NULL);
  gtk_box_pack_start (GTK_BOX (right_box),
                      GTK_WIDGET (new_legend),
                      TRUE,
                      FALSE,
                      2);

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

  current_axis = gdv_twod_layer_get_axis (GDV_TWOD_LAYER (layer), GDV_X1_AXIS);
  tmp_label = g_object_new (gtk_label_get_type(), NULL);
  gtk_label_set_markup (GTK_LABEL (tmp_label), "X<sub>1</sub>-Axis");
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

  gtk_widget_show(GTK_WIDGET (new_content));
  gtk_container_add (GTK_CONTAINER (layer), GTK_WIDGET (new_content));
  gtk_overlay_reorder_overlay (GTK_OVERLAY (layer), GTK_WIDGET (new_content), -1);

  gtk_widget_show(GTK_WIDGET (new_content2));
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

  gtk_widget_show_all(data->window);

  /* Run the actual tests */
  tgdv_layer_test_integrity(GDV_LAYER(data->layer));

  while (gtk_events_pending ())
    gtk_main_iteration ();

  g_timeout_add (cb_time, ((GSourceFunc) teardown_cb), data->window); // cb_time
  tgdv_layer_test_integrity(GDV_LAYER(data->layer));
  while (gtk_events_pending ())
    gtk_main_iteration ();
  tgdv_layer_test_integrity(GDV_LAYER(data->layer));

  gtk_main ();

  data = NULL;
}

int main(int argc, char* argv[]) {

  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/Gdv/TwodLayer/prenormal", test_twodlayer_pre_normal);
  g_test_add_func ("/Gdv/TwodLayer/loglegend", test_twodlayer_log_legend);

  return g_test_run ();
}

