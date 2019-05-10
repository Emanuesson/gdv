/* test-gdv-linearxis.c
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

/* A function to test if a certain value lies on a linear scale. */
static void
test_tic_value_on_regression (gdouble value_under_test,
                              gdouble axis_start, gdouble axis_end,
                              gdouble axis_increment)
{
  gdouble high_border = axis_end > axis_start ? axis_end : axis_start;
  gdouble low_border = axis_end > axis_start ? axis_start : axis_end;
  gdouble residuum = fmodl(value_under_test - low_border, axis_increment);

  g_assert_cmpfloat (value_under_test, <=, high_border);
  g_assert_cmpfloat (value_under_test, >=, low_border);
  g_assert_cmpfloat_with_epsilon (residuum, 0.0, axis_increment * 1e-20);
}

static const guint cb_time = 300;

static gboolean
teardown_cb (GtkWidget *widget)
{
  gtk_widget_destroy (widget);
  gtk_main_quit();
  return FALSE;
}

/*
 * This test has maaybe no real value aas it checks simplay the default values
 * for every property of the linear axis.
 */
static void
test_lin_axis_correct_default (void)
{
  GtkWidget *window;
  GdvLinearAxis *lin_axis;
  GdvLayer *layer;
  GList *tics, *mtics;
  GList *tics_cpy, *mtics_cpy;

  gdouble orientation, direction_outside;
  gdouble scale_min_val = 0.0, scale_max_val = 0.0, scale_incr = 0.0;
  gdouble axis_beg_pix_x = 0.0, axis_end_pix_x = 0.0, axis_beg_pix_y = 0.0, axis_end_pix_y = 0.0;
  gdouble axis_beg_screen_x = 0.0, axis_end_screen_x = 0.0,
          axis_beg_screen_y = 0.0, axis_end_screen_y = 0.0;
  gdouble tic_beg_val = 0.0, tic_end_val = 0.0, mtics_beg_val = 0.0, mtics_end_val = 0.0;
  gboolean scale_auto_increment, scale_limits_auto, tics_auto, mtics_auto;
  guint no_of_mtics;
  gboolean force_beg_end;
  gint scale_min_diff_pix = 0, scale_max_diff_pix = 0;
  gdouble scale_increment_x, scale_increment_y;

  gtk_init (NULL, 0);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  lin_axis = g_object_new (GDV_LINEAR_TYPE_AXIS, NULL);
  _st_check_all_properties(G_OBJECT(lin_axis), TRUE,
                           "style", "events", "tooltip-markup",
                           "can-focus", "receives-default", NULL);

  layer = g_object_new (GDV_TYPE_LAYER, NULL);
  gtk_window_set_title (GTK_WINDOW (window), "a");

  tgdv_linearaxis_test_integrity(lin_axis);

  tics = gdv_axis_get_tic_list (GDV_AXIS (lin_axis));
  mtics = gdv_axis_get_mtic_list (GDV_AXIS (lin_axis));

  g_object_get (lin_axis,
    "axis-orientation", &orientation,
    "axis-direction-outside", &direction_outside,
    "scale-beg-val", &scale_min_val,
    "scale-end-val", &scale_max_val,
    "scale-increment-val", &scale_incr,
    "scale-auto-increment", &scale_auto_increment,
    "scale-limits-automatic", &scale_limits_auto,
    "axis-beg-pix-x", &axis_beg_pix_x,
    "axis-beg-pix-y", &axis_beg_pix_y,
    "axis-end-pix-x", &axis_end_pix_x,
    "axis-end-pix-y", &axis_end_pix_y,
    "axis-beg-at-screen-x", &axis_beg_screen_x,
    "axis-end-at-screen-x", &axis_end_screen_x,
    "axis-beg-at-screen-y", &axis_beg_screen_y,
    "axis-end-at-screen-y", &axis_end_screen_y,
    "tics-beg-val", &tic_beg_val,
    "tics-end-val", &tic_end_val,
    "tics-automatic", &tics_auto,
    "mtics-beg-val", &mtics_beg_val,
    "mtics-end-val", &mtics_end_val,
    "mtics-automatic", &mtics_auto,
    "mtics", &no_of_mtics,
    "force-beg-end", &force_beg_end,
    NULL);

  gtk_widget_style_get (GTK_WIDGET (lin_axis),
                        "scale-min-diff-pix", &scale_min_diff_pix,
                        "scale-max-diff-pix", &scale_max_diff_pix,
                        NULL);

  g_assert_null (tics);
  g_assert_null (mtics);
  g_assert_true (orientation == 0.0);
  g_assert_true (direction_outside == 0.5 * M_PI);
  g_assert_true (scale_min_val == 0.0);
  g_assert_true (scale_max_val == 100.0);
  g_assert_true (scale_incr == 100.0);
  g_assert_true (scale_auto_increment == TRUE);
  g_assert_true (scale_limits_auto == TRUE);
  g_assert_true (axis_beg_pix_x == 0.0);
  g_assert_true (axis_beg_pix_y == 0.0);
  g_assert_true (axis_end_pix_x == 0.0);
  g_assert_true (axis_end_pix_y == 0.0);
  g_assert_true (axis_beg_screen_x == 0.0);
  g_assert_true (axis_beg_screen_y == 0.0);
  g_assert_true (axis_end_screen_x == 0.0);
  g_assert_true (axis_end_screen_y == 0.0);
  g_assert_true (tic_beg_val == 0.0);
  g_assert_true (tic_end_val == 100.0);
  g_assert_true (tics_auto == TRUE);
  g_assert_true (mtics_beg_val == 0.0);
  g_assert_true (mtics_end_val == 100.0);
  g_assert_true (mtics_auto == TRUE);
  g_assert_true (no_of_mtics == 4);
  g_assert_true (force_beg_end == FALSE);

  g_assert_true (scale_min_diff_pix == 40);
  g_assert_true (scale_max_diff_pix == 300);

  gtk_container_add (GTK_CONTAINER (window), GTK_WIDGET (layer));
  gtk_container_add (GTK_CONTAINER (layer), GTK_WIDGET (lin_axis));

  /* Realizing and Allocating the Layer */
  gtk_widget_show_all (window);

  g_assert (gtk_widget_get_realized(window));
  g_assert (gtk_widget_get_realized(GTK_WIDGET (layer)));
  g_assert (gtk_widget_get_realized(GTK_WIDGET (lin_axis)));
  _st_check_all_properties(G_OBJECT(lin_axis), FALSE, NULL);

  tgdv_linearaxis_test_integrity(lin_axis);

  /* Now testing all automatted settings for correctness */
  g_object_get (lin_axis,
    "axis-orientation", &orientation,
    "axis-direction-outside", &direction_outside,
    "scale-beg-val", &scale_min_val,
    "scale-end-val", &scale_max_val,
    "scale-increment-val", &scale_incr,
    "scale-auto-increment", &scale_auto_increment,
    "scale-limits-automatic", &scale_limits_auto,
    "axis-beg-pix-x", &axis_beg_pix_x,
    "axis-beg-pix-y", &axis_beg_pix_y,
    "axis-end-pix-x", &axis_end_pix_x,
    "axis-end-pix-y", &axis_end_pix_y,
    "axis-beg-at-screen-x", &axis_beg_screen_x,
    "axis-end-at-screen-x", &axis_end_screen_x,
    "axis-beg-at-screen-y", &axis_beg_screen_y,
    "axis-end-at-screen-y", &axis_end_screen_y,
    "tics-beg-val", &tic_beg_val,
    "tics-end-val", &tic_end_val,
    "tics-automatic", &tics_auto,
    "mtics-beg-val", &mtics_beg_val,
    "mtics-end-val", &mtics_end_val,
    "mtics-automatic", &mtics_auto,
    "mtics", &no_of_mtics,
    "force-beg-end", &force_beg_end,
    NULL);

  g_assert_true (scale_min_val == 0.0);
  g_assert_true (scale_max_val == 100.0);
  g_assert_true (scale_incr == 100.0);
  g_assert_true (tic_beg_val == 0.0);
  g_assert_true (tic_end_val == 100.0);
  g_assert_true (mtics_beg_val == 0.0);
  g_assert_true (mtics_end_val == 100.0);
  g_assert_true ((axis_beg_screen_x >= 0.3) && (axis_beg_screen_x <= 0.7));
  g_assert_true ((axis_end_screen_x >= 0.3) && (axis_end_screen_x <= 0.7));
  g_assert_true (gdv_is_nearly_identical (axis_beg_screen_x,
                                          axis_end_screen_x,
                                          1e-10 * axis_end_screen_x));
  g_assert_true ((axis_beg_screen_y >= 0.0) && (axis_beg_screen_y <= 0.3));
  g_assert_true ((axis_end_screen_y >= 0.7) && (axis_end_screen_y <= 1.0));

  tics = gdv_axis_get_tic_list (GDV_AXIS (lin_axis));
  mtics = gdv_axis_get_mtic_list (GDV_AXIS (lin_axis));

  g_assert (g_list_length(tics) == 2);

  for (tics_cpy = tics; tics; tics = tics->next)
  {
    gdouble value;
    gfloat pos_x, pos_y;

    g_object_get (tics->data,
                  "value", &value,
                  "pos-x", &pos_x,
                  "pos-y", &pos_y,
                  NULL);

    tgdv_linearaxis_test_tic_on_regression (value, tic_beg_val, tic_end_val, scale_incr);
//    tgdv_linearaxis_test_tic_on_regression (pos_x, axis_beg_pix_x, axis_end_pix_x, scale_incr);
//    tgdv_linearaxis_test_tic_on_regression (pos_y, axis_beg_pix_y, axis_end_pix_y, scale_incr);

    g_assert_true (scale_min_val == value || scale_max_val == value);
    if (scale_min_val == value)
    {
      g_assert_true (pos_x == (gfloat) axis_beg_pix_x);
      g_assert_true (pos_y == (gfloat) axis_beg_pix_y);
    }
    else
    {
      g_assert_true (pos_x == (gfloat) axis_end_pix_x);
      g_assert_true (pos_y == (gfloat) axis_end_pix_y);
    }

  }
  g_list_free(tics_cpy);

  g_assert (g_list_length(mtics) == no_of_mtics);

//  guint iter;
//  iter = 0;
  for (mtics_cpy = mtics; mtics; mtics = mtics->next)
  {
    gdouble value;
    gfloat pos_x, pos_y;

    g_object_get (mtics->data,
                  "value", &value,
                  "pos-x", &pos_x,
                  "pos-y", &pos_y,
                  NULL);
    g_assert_true (value > scale_min_val);
    g_assert_true (value < scale_max_val);
    g_assert_true ((pos_x >= (gfloat) axis_beg_pix_x) && (pos_x <= (gfloat) axis_end_pix_x));
    g_assert_true ((pos_y >= (gfloat) axis_beg_pix_y) && (pos_y <= (gfloat) axis_end_pix_y));

//    iter++;
  }
  g_list_free(mtics_cpy);

//  g_assert_cmpuint(no_of_tics, ==, iter);

  g_timeout_add (cb_time, ((GSourceFunc) teardown_cb), window);
  gtk_main();
}

static void
test_lin_axis_horizontal (void)
{
  GtkWidget *window;
  GdvLinearAxis *lin_axis;
  GdvLayer *layer;
  GList *tics, *mtics;
  GList *tics_cpy, *mtics_cpy;

  gdouble orientation, direction_outside;
  gdouble scale_min_val = 0.0, scale_max_val = 0.0, scale_incr = 0.0;
  gdouble axis_beg_pix_x = 0.0, axis_end_pix_x = 0.0, axis_beg_pix_y = 0.0, axis_end_pix_y = 0.0;
  gdouble axis_beg_screen_x = 0.0, axis_end_screen_x = 0.0,
          axis_beg_screen_y = 0.0, axis_end_screen_y = 0.0;
  gdouble tic_beg_val = 0.0, tic_end_val = 0.0, mtics_beg_val = 0.0, mtics_end_val = 0.0;
  gboolean scale_auto_increment, scale_limits_auto, tics_auto, mtics_auto;
  guint no_of_mtics;
  gboolean force_beg_end;

  gtk_init (NULL, 0);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  lin_axis = g_object_new (GDV_LINEAR_TYPE_AXIS, NULL);
  layer = g_object_new (GDV_TYPE_LAYER, NULL);

  g_object_set (lin_axis,
    "axis-orientation", 0.5 * M_PI,
    "axis-direction-outside", 1.0 * M_PI,
    NULL);

  gtk_container_add (GTK_CONTAINER (window), GTK_WIDGET (layer));
  gtk_container_add (GTK_CONTAINER (layer), GTK_WIDGET (lin_axis));
  gtk_container_set_border_width (GTK_CONTAINER (window), 80);
  gtk_window_set_title (GTK_WINDOW (window), "b");

  /* Realizing and Allocating the Layer */
  gtk_widget_show_all (window);

  g_assert (gtk_widget_get_realized(window));
  g_assert (gtk_widget_get_realized(GTK_WIDGET (layer)));
  g_assert (gtk_widget_get_realized(GTK_WIDGET (lin_axis)));

  tgdv_linearaxis_test_integrity(lin_axis);

  /* Now testing all automatted settings for correctness */
  g_object_get (lin_axis,
    "axis-orientation", &orientation,
    "axis-direction-outside", &direction_outside,
    "scale-beg-val", &scale_min_val,
    "scale-end-val", &scale_max_val,
    "scale-increment-val", &scale_incr,
    "scale-auto-increment", &scale_auto_increment,
    "scale-limits-automatic", &scale_limits_auto,
    "axis-beg-pix-x", &axis_beg_pix_x,
    "axis-beg-pix-y", &axis_beg_pix_y,
    "axis-end-pix-x", &axis_end_pix_x,
    "axis-end-pix-y", &axis_end_pix_y,
    "axis-beg-at-screen-x", &axis_beg_screen_x,
    "axis-end-at-screen-x", &axis_end_screen_x,
    "axis-beg-at-screen-y", &axis_beg_screen_y,
    "axis-end-at-screen-y", &axis_end_screen_y,
    "tics-beg-val", &tic_beg_val,
    "tics-end-val", &tic_end_val,
    "tics-automatic", &tics_auto,
    "mtics-beg-val", &mtics_beg_val,
    "mtics-end-val", &mtics_end_val,
    "mtics-automatic", &mtics_auto,
    "mtics", &no_of_mtics,
    "force-beg-end", &force_beg_end,
    NULL);

  g_assert_true (scale_min_val == 0.0);
  g_assert_true (scale_max_val == 100.0);
  g_assert_true (scale_incr == 100.0);
  g_assert_true (tic_beg_val == 0.0);
  g_assert_true (tic_end_val == 100.0);
  g_assert_true (mtics_beg_val == 0.0);
  g_assert_true (mtics_end_val == 100.0);
  g_assert_true ((axis_beg_screen_y >= 0.1) && (axis_beg_screen_y <= 0.9));
  g_assert_true ((axis_end_screen_y >= 0.1) && (axis_end_screen_y <= 0.9));
  g_assert_true (gdv_is_nearly_identical (axis_beg_screen_y,
                                          axis_end_screen_y,
                                          1e-10 * axis_end_screen_y));
  g_assert_true ((axis_beg_screen_x >= 0.7) && (axis_beg_screen_x <= 1.0));
  g_assert_true ((axis_end_screen_x >= 0.0) && (axis_end_screen_x <= 0.3));

  tics = gdv_axis_get_tic_list (GDV_AXIS (lin_axis));
  mtics = gdv_axis_get_mtic_list (GDV_AXIS (lin_axis));

  g_assert (g_list_length(tics) == 2);

  for (tics_cpy = tics; tics; tics = tics->next)
  {
    gdouble value;
    gfloat pos_x, pos_y;

    g_object_get (tics->data,
                  "value", &value,
                  "pos-x", &pos_x,
                  "pos-y", &pos_y,
                  NULL);
    g_assert_true (scale_min_val == value || scale_max_val == value);
    if (scale_min_val == value)
    {
      g_assert_true (gdv_is_nearly_identical_f (pos_x,
                                                (gfloat) axis_beg_pix_x,
                                                1e-10 * pos_x));
      g_assert_true (gdv_is_nearly_identical_f (pos_y,
                                                (gfloat) axis_beg_pix_y,
                                                1e-10 * pos_y));
    }
    else
    {
      g_assert_true (gdv_is_nearly_identical_f (pos_x,
                                                (gfloat) axis_end_pix_x,
                                                1e-10 * pos_x));
      g_assert_true (gdv_is_nearly_identical_f (pos_y,
                                                (gfloat) axis_end_pix_y,
                                                1e-10 * pos_y));
    }

  }
  g_list_free(tics_cpy);

  g_assert (g_list_length(mtics) == no_of_mtics);
  for (mtics_cpy = mtics; mtics; mtics = mtics->next)
  {
    gdouble value;
    gfloat pos_x, pos_y;

    g_object_get (mtics->data,
                  "value", &value,
                  "pos-x", &pos_x,
                  "pos-y", &pos_y,
                  NULL);
    g_assert_true (value > scale_min_val);
    g_assert_true (value < scale_max_val);
    g_assert_true (gdv_is_within_range_f (pos_x, (gfloat) axis_beg_pix_x, (gfloat) axis_end_pix_x));
    g_assert_true (gdv_is_within_range_f (pos_y, (gfloat) axis_beg_pix_y, (gfloat) axis_end_pix_y));
  }
  g_list_free(mtics_cpy);

  g_timeout_add (cb_time, ((GSourceFunc) teardown_cb), window);
  gtk_main();
}

static void
test_lin_axis_cross_settings (void)
{
  GtkWidget *window;
  GdvLinearAxis *lin_axis;
  GdvLayer *layer;

  gdouble scale_min_val = 0.0, scale_max_val = 0.0, scale_incr = 0.0;

  gtk_init (NULL, 0);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  lin_axis = g_object_new (GDV_LINEAR_TYPE_AXIS, NULL);
  layer = g_object_new (GDV_TYPE_LAYER, NULL);

  g_object_set (lin_axis,
    "scale-increment-val", 10.0,
    NULL);

  gtk_container_add (GTK_CONTAINER (window), GTK_WIDGET (layer));
  gtk_container_add (GTK_CONTAINER (layer), GTK_WIDGET (lin_axis));
  gtk_widget_set_size_request (GTK_WIDGET (window), 1800, 1800);
  gtk_window_set_title (GTK_WINDOW (window), "c");

  /* Realizing and Allocating the Layer */
  gtk_widget_show_all (window);

  /* Now testing all automatted settings for correctness */
  g_object_get (lin_axis,
    "scale-beg-val", &scale_min_val,
    "scale-end-val", &scale_max_val,
    "scale-increment-val", &scale_incr,
    NULL);

  g_assert_true (scale_min_val == 0.0);
  g_assert_true (scale_max_val == 100.0);
  g_assert_true (scale_incr == 2.5);  /* TODO: Investigate this value, it seems unlikely */

  g_object_set (lin_axis,
    "scale-increment-base", 30.0,
    NULL);

  gtk_widget_hide (window);
  gtk_widget_show_all (window);

  g_object_get (lin_axis,
    "scale-beg-val", &scale_min_val,
    "scale-end-val", &scale_max_val,
    "scale-increment-val", &scale_incr,
    NULL);

  g_assert_true (scale_min_val == 0.0);
  g_assert_true (scale_max_val == 102.0);
  g_assert_true (scale_incr == 3.0);

  g_object_set (lin_axis,
    "scale-increment-base", 10.0,
    "scale-beg-val", -10.0,
    "scale-end-val", 10.0,
    "scale-increment-val", 1.0,
    NULL);

  gtk_widget_hide (window);
  gtk_widget_show_all (window);

  g_object_get (lin_axis,
    "scale-beg-val", &scale_min_val,
    "scale-end-val", &scale_max_val,
    "scale-increment-val", &scale_incr,
    NULL);

  g_assert_true (scale_min_val == -10.0);
  g_assert_true (scale_max_val == 10.0);
  g_assert_true (scale_incr == 0.5);

  g_object_set (lin_axis,
                "scale-increment-base", 10.0,
                "scale-beg-val", -10.000001,
                "scale-end-val", -9.999990,
                "scale-increment-val", 1.0,
                NULL);

  gtk_widget_set_size_request (GTK_WIDGET (window), 1800, 1800);

  gtk_widget_hide (window);
  gtk_widget_show_all (window);

  g_object_get (lin_axis,
    "scale-beg-val", &scale_min_val,
    "scale-end-val", &scale_max_val,
    "scale-increment-val", &scale_incr,
    NULL);

  g_print ("SCALE MIN VAL %.8f\n", scale_min_val);
  g_print ("SCALE DIFF VAL %.8e\n", scale_min_val + 10.00000100);
  g_assert_true (gdv_is_nearly_identical (scale_min_val, -10.00000100, 1e-10));
  g_assert_true (gdv_is_nearly_identical (scale_max_val, -9.99998950, 1e-10));
  g_assert_true (gdv_is_nearly_identical (scale_incr, 5.000000e-07, 1e-17));

  g_print ("SCALE: BEG %.8f END %.8f SIV %e\n",
           scale_min_val, scale_max_val, scale_incr);

  g_timeout_add (cb_time, ((GSourceFunc) teardown_cb), window);
  gtk_main();

}

/*

static struct _tgdv_linearaxis_data *test_lin_axis_setup_data (void)
{
  struct _tgdv_linearaxis_data * data =
    g_new0(struct _tgdv_linearaxis_data, 1);

  gtk_init (NULL, 0);

  data->window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  data->layer = g_object_new (GDV_TYPE_LAYER, NULL);

  gtk_container_add (GTK_CONTAINER (window), GTK_WIDGET (layer));
  g_object_unref (layer);

  return data;
}

static void test_lin_axis_reset_data (struct _tgdv_linearaxis_data *window)
{

}
*/

struct _tgdv_linearaxis_data {
  GtkWidget *window;
  GtkWidget *layer;
  GdvLinearAxis *axis;
};

static gdouble current_orientation = 0;

static gboolean
timeout_cb_oned (struct _tgdv_linearaxis_data *data)
{
  if (!data->window || !GTK_IS_CONTAINER (data->window))
    return FALSE;

  g_object_set (data->axis,
                "axis-orientation", current_orientation,
                "axis-direction-outside", current_orientation + 0.5 * M_PI,
                NULL);
  current_orientation += 0.02;
  tgdv_linearaxis_test_integrity(data->axis);

  return TRUE;
}

static guint endless_loop_prevention = 0;
static gboolean
teardown_wrapper_cb (GtkWidget *widget)
{
//  g_print("REACHED: %e\n", current_orientation);

  endless_loop_prevention++;

  if (endless_loop_prevention > 2)
  {
    g_assert_not_reached();
    return FALSE;
  }

  if (current_orientation > 3.5)
    return teardown_cb(widget);

  return TRUE;
}


static void
test_lin_axis_manual_tics (void)
{
  struct _tgdv_linearaxis_data data_str;
  struct _tgdv_linearaxis_data * data = &data_str;

  GdvLinearAxis *lin_axis;

  gtk_init (NULL, 0);

  data->window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  data->layer = g_object_new (GDV_TYPE_LAYER, NULL);
  gtk_container_add (GTK_CONTAINER (data->window), GTK_WIDGET (data->layer));
  gtk_widget_set_size_request (GTK_WIDGET (data->window), 800, 800);
  gtk_window_set_title (GTK_WINDOW (data->window), "Test window");
  //g_object_unref(data->layer);
  gtk_widget_show_all(data->window);

  lin_axis = g_object_new (GDV_LINEAR_TYPE_AXIS,
                           "scale-increment-val", 10.0,
                           NULL);

  data->axis = lin_axis;
  gtk_container_add (GTK_CONTAINER (data->layer), GTK_WIDGET (lin_axis));
//  tgdv_linearaxis_test_integrity(lin_axis);

  g_timeout_add (10, ((GSourceFunc) timeout_cb_oned), data);
  g_timeout_add (cb_time * 10, ((GSourceFunc) teardown_wrapper_cb), data->window); // cb_time

  gtk_main ();
  data = NULL;
}

int main(int argc, char* argv[]) {
//  void * setting = NULL;
//  setting = test_lin_axis_setup_data();

  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/Gdv/LinearAxis/correct_default", test_lin_axis_correct_default);
  g_test_add_func ("/Gdv/LinearAxis/horizontal", test_lin_axis_horizontal);
  g_test_add_func ("/Gdv/LinearAxis/cross_settings", test_lin_axis_cross_settings);
  g_test_add_func ("/Gdv/LinearAxis/manual_tics", test_lin_axis_manual_tics);
/*  g_test_add_data_func_full ("/Gdv/LinearAxis/manual_tics", setting,
                             test_lin_axis_manual_tics, test_lin_axis_reset_window);
*/
  return g_test_run ();
}

