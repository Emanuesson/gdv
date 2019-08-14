/* tgdv-axis.c
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

#include <math.h>

#include "tgdv-axis.h"

/* Does all sanity checks. This may be insufficient for any more complex
 * setup as in 2D-plots */
void tgdv_axis_test_full (GdvAxis *axis)
{
  tgdv_axis_test_integrity (axis);
  tgdv_axis_test_alloc_usage (axis);
  tgdv_axis_test_tic_distance (axis);
}

/* Checks if all directions are consistent with presettings */
//void tgdv_axis_test_directions (GdvAxis *axis)

static gint _compare_tics (GdvTic *a, GdvTic *b)
{
  gdouble val_a, val_b;

  g_assert(GDV_IS_TIC(a));
  g_assert(GDV_IS_TIC(b));

  g_object_get(a, "value", &val_a, NULL);
  g_object_get(b, "value", &val_b, NULL);

  if (val_a > val_b)
    return 1;
  else if (val_a < val_b)
    return -1;

  return 0;
}

/* Checks if tic distance is according to style properties. This should
 * contain checks for maximum and minimum distances of. */
void tgdv_axis_test_tic_distance (GdvAxis *axis)
{
  GList *tics, *tics_cpy, *mtics, *mtics_cpy;
  GdvTic *last_tic = NULL;
  gint max_diff_pix;
  gdouble scale_incr;
  gboolean scale_auto_increment, force_beg_end, scale_limits_auto;

  g_assert(GDV_IS_AXIS(axis));

  g_object_get(axis,
              "scale-auto-increment", &scale_auto_increment,
              "force-beg-end", &force_beg_end,
               "scale-increment-val", &scale_incr,
               "scale-limits-automatic", &scale_limits_auto,
               NULL);
  gtk_widget_style_get(GTK_WIDGET(axis),
                       "scale-max-diff-pix", &max_diff_pix,
                       NULL);

  tics = gdv_axis_get_tic_list (GDV_AXIS (axis));
  tics_cpy = tics;
  mtics = gdv_axis_get_mtic_list (GDV_AXIS (axis));
  mtics_cpy = mtics;

  tics = g_list_sort(tics, (GCompareFunc) _compare_tics);
  mtics = g_list_sort(mtics, (GCompareFunc) _compare_tics);

  for (; tics; tics = tics->next)
  {
    GdvTic *tic = tics->data;

    if (last_tic != NULL)
    {
      gdouble curr_value, last_value;
      gfloat  curr_pos_in_x, last_pos_in_x;
      gfloat  curr_pos_in_y, last_pos_in_y;
      gdouble distance_between_tics;

      g_object_get (tic,
                    "value", &curr_value,
                    "pos-x", &curr_pos_in_x,
                    "pos-y", &curr_pos_in_y,
                    NULL);
      g_object_get (last_tic,
                    "value", &last_value,
                    "pos-x", &last_pos_in_x,
                    "pos-y", &last_pos_in_y,
                    NULL);

      distance_between_tics = sqrt((last_pos_in_x - curr_pos_in_x) * (last_pos_in_x - curr_pos_in_x) +
                                   (last_pos_in_y - curr_pos_in_y) * (last_pos_in_y - curr_pos_in_y));

      if (scale_auto_increment) //&& scale_limits_auto
        g_assert_cmpfloat(distance_between_tics, <=, max_diff_pix);
//      g_assert_cmpfloat_with_epsilon(scale_incr, fabs(curr_value - last_value), scale_incr * 1e-5);
    }

    last_tic = tics->data;
  }


  g_list_free(tics_cpy);
  g_list_free(mtics_cpy);
}

static gdouble _dmax(gdouble a, gdouble b)
{
  return 0.5 * (a + b + fabs(a - b));
}

static gdouble _determine_dist_to_border (gdouble x_pos, gdouble y_pos,
                                          GtkAllocation *allocation)
{
  gdouble dist_x_neg = ((gdouble) allocation->x - x_pos);
  gdouble dist_x_pos = ((gdouble) (allocation->width + allocation->x) - x_pos);
  gdouble dist_y_neg = ((gdouble) allocation->y - y_pos);
  gdouble dist_y_pos = ((gdouble) (allocation->height + allocation->x) - y_pos);

  return _dmax(_dmax(dist_x_neg, dist_x_pos), _dmax(dist_y_neg, dist_y_pos));
}

/* Tests if the axis is using the allocated area properly */
void tgdv_axis_test_alloc_usage (GdvAxis *axis)
{
  gdouble axis_beg_pix_x = 0.0, axis_end_pix_x = 0.0;
  gdouble axis_beg_pix_y = 0.0, axis_end_pix_y = 0.0;
  gdouble distance_beg_en = 0;
  GtkAllocation allocation = {0};
  gdouble tmp_val;

  g_assert(GDV_IS_AXIS(axis));

  g_object_get (axis,
    "axis-beg-pix-x", &axis_beg_pix_x,
    "axis-beg-pix-y", &axis_beg_pix_y,
    "axis-end-pix-x", &axis_end_pix_x,
    "axis-end-pix-y", &axis_end_pix_y,
    NULL);

  gtk_widget_get_allocation(GTK_WIDGET(axis), &allocation);

  g_assert_cmpint((gint) fabs(axis_beg_pix_x - axis_end_pix_x), <=, allocation.width);
  g_assert_cmpint((gint) fabs(axis_beg_pix_y - axis_end_pix_y), <=, allocation.height);

  /* TODO: extend this test */
//  _determine_dist_to_border (axis_beg_pix_x axis_beg_pix_y, &allocation);
//  _determine_dist_to_border (axis_beg_pix_x axis_beg_pix_y, &allocation);

//  distance_beg_en = sqrt(

}

void tgdv_axis_test_integrity (GdvAxis *axis)
{
  GList *tics, *mtics;
  GList *tics_cpy, *mtics_cpy;

  gdouble orientation, direction_outside;
  gdouble scale_min_val = 0.0, scale_max_val = 0.0, scale_incr = 0.0;
  gdouble axis_beg_pix_x = 0.0, axis_end_pix_x = 0.0;
  gdouble axis_beg_pix_y = 0.0, axis_end_pix_y = 0.0;
  gdouble axis_beg_screen_x = 0.0, axis_end_screen_x = 0.0,
          axis_beg_screen_y = 0.0, axis_end_screen_y = 0.0;
  gdouble tic_beg_val = 0.0, tic_end_val = 0.0, mtic_beg_val = 0.0, mtic_end_val = 0.0;
  gboolean scale_auto_increment, scale_limits_auto, tics_auto, mtics_auto;
  guint no_of_mtics;
  gboolean force_beg_end, resize_during_redraw;

  GtkAllocation allocation = {0};

  gint max_diff_pix;
  gdouble line_width;

  GString title;

  g_assert(GDV_IS_AXIS(axis));

  g_object_get (axis,
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
    "mtics-beg-val", &mtic_beg_val,
    "mtics-end-val", &mtic_end_val,
    "mtics-automatic", &mtics_auto,
    "mtics", &no_of_mtics,
    "force-beg-end", &force_beg_end,
    "resize-during-redraw", &resize_during_redraw,
    // title-widget
    "title", &title,
    // resize-during-redraw
    NULL);

  gtk_widget_get_allocation(GTK_WIDGET(axis), &allocation);

  /* TODO: Evaluate if these values should always be within the borders. E.g.:
   * axis_beg_pix_x <= allocation.x + allocation.width
   */
  g_assert_cmpint(axis_beg_pix_x, <=, allocation.width);
  g_assert_cmpint(axis_beg_pix_x, >=, 0.0);
  g_assert_cmpint(axis_end_pix_x, <=, allocation.width);
  g_assert_cmpint(axis_end_pix_x, >=, 0.0);
  g_assert_cmpint(axis_beg_pix_y, <=, allocation.height);
  g_assert_cmpint(axis_beg_pix_y, >=, 0.0);
  g_assert_cmpint(axis_end_pix_y, <=, allocation.height);
  g_assert_cmpint(axis_end_pix_y, >=, 0.0);

  gtk_widget_style_get(GTK_WIDGET(axis),
    "scale-max-diff-pix", &max_diff_pix,
    "line-width", &line_width,
    //"color", &max_diff_pix,
    //"show-title", ,
    NULL);

  tics = gdv_axis_get_tic_list (GDV_AXIS (axis));
  mtics = gdv_axis_get_mtic_list (GDV_AXIS (axis));

  if (gtk_widget_get_realized (GTK_WIDGET(axis)) &&
      tics_auto &&
      scale_limits_auto)
    g_assert_cmpuint(g_list_length(tics), >=, 2);
  else if (gtk_widget_get_realized (GTK_WIDGET(axis)) &&
           tics_auto)
    g_assert_cmpuint(g_list_length(tics), >=, 1);


//force_beg_end
//scale_auto_increment
//scale_limits_auto
//  if (!force_beg_end)
//  {
//  }

  for (tics_cpy = tics; tics && tics_auto == TRUE; tics = tics->next)
  {
    gfloat x_pos, y_pos;
    gdouble value;

    g_assert(GDV_IS_TIC(tics->data));

    g_object_get(tics->data,
                 "pos-x", &x_pos,
                 "pos-y", &y_pos,
                 "value", &value,
                 NULL);

    g_assert_cmpfloat(x_pos - allocation.x, >=, fmin(axis_beg_pix_x, axis_end_pix_x));
    g_assert_cmpfloat(x_pos - allocation.x, <=, fmax(axis_beg_pix_x, axis_end_pix_x));
    g_assert_cmpfloat(y_pos - allocation.y, >=, fmin(axis_beg_pix_y, axis_end_pix_y));
    g_assert_cmpfloat(y_pos - allocation.y, <=, fmax(axis_beg_pix_y, axis_end_pix_y));

    g_assert_cmpfloat(value, >=, tic_beg_val);
    g_assert_cmpfloat(value, <=, tic_end_val);
  }
  g_list_free(tics_cpy);

  /* TODO: make this test universal */
  if (g_list_length(tics) == 2 && mtics_auto)
    g_assert (g_list_length(mtics) == no_of_mtics);

  if (gtk_widget_get_realized (GTK_WIDGET(axis)) &&
      mtics_auto)
    g_assert (g_list_length(mtics) >= 1);

  for (mtics_cpy = mtics; mtics; mtics = mtics->next)
  {
    gfloat x_pos, y_pos;
    gdouble value;

    g_assert(GDV_IS_MTIC(mtics->data));

    g_object_get(mtics->data,
                 "pos-x", &x_pos,
                 "pos-y", &y_pos,
                 "value", &value,
                 NULL);

    g_assert_cmpfloat(x_pos - allocation.x, >=, fmin(axis_beg_pix_x, axis_end_pix_x));
    g_assert_cmpfloat(x_pos - allocation.x, <=, fmax(axis_beg_pix_x, axis_end_pix_x));
    g_assert_cmpfloat(y_pos - allocation.y, >=, fmin(axis_beg_pix_y, axis_end_pix_y));
    g_assert_cmpfloat(y_pos - allocation.y, <=, fmax(axis_beg_pix_y, axis_end_pix_y));

    g_assert_cmpfloat(value, >=, mtic_beg_val);
    g_assert_cmpfloat(value, <=, mtic_end_val);
  }
  g_list_free(mtics_cpy);

}

void tgdv_axis_test_unity (GdvAxis *axis)
{
  /* TODO: test, e.g. if get_point-signal accepts NULL-pointer for output
   *       values */
}
