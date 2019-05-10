/* tgdv-linearaxis.c
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


/* A function to test if a certain value lies on a linear scale. */
void tgdv_linearaxis_test_tic_on_regression (gdouble value_under_test,
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

void tgdv_linearaxis_test_integrity (GdvLinearAxis *laxis)
{
  GList *tics, *mtics;
  GList *tics_cpy, *mtics_cpy;

  gdouble orientation, direction_outside;
  gdouble scale_min_val = 0.0, scale_max_val = 0.0, scale_incr = 0.0;
  gdouble axis_beg_pix_x = 0.0, axis_end_pix_x = 0.0;
  gdouble axis_beg_pix_y = 0.0, axis_end_pix_y = 0.0;
  gdouble axis_beg_screen_x = 0.0, axis_end_screen_x = 0.0,
          axis_beg_screen_y = 0.0, axis_end_screen_y = 0.0;
  gdouble tic_beg_val = 0.0, tic_end_val = 0.0, mtics_beg_val = 0.0, mtics_end_val = 0.0;
  gboolean scale_auto_increment, scale_limits_auto, tics_auto, mtics_auto;
  guint no_of_mtics;
  gboolean force_beg_end;

  g_object_get (laxis,
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

  tics = gdv_axis_get_tic_list (GDV_AXIS (laxis));
  mtics = gdv_axis_get_mtic_list (GDV_AXIS (laxis));

  if (gtk_widget_get_realized (GTK_WIDGET(laxis)))
    g_assert_cmpuint(g_list_length(tics), >=, 2);

  for (tics_cpy = tics; tics; tics = tics->next)
  {
    gdouble value;
    gfloat pos_x, pos_y;

    g_object_get (tics->data,
                  "value", &value,
                  "pos-x", &pos_x,
                  "pos-y", &pos_y,
                  NULL);

    if (scale_auto_increment)
      tgdv_linearaxis_test_tic_on_regression (value, tic_beg_val, tic_end_val, scale_auto_increment);
  }
  g_list_free(tics_cpy);

  /* TODO: make this test universal */
  if (g_list_length(tics) == 2 && mtics_auto)
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
    g_assert_true (gdv_is_within_range_f (pos_x, (gfloat) axis_beg_pix_x,
                                          (gfloat) axis_end_pix_x));
    g_assert_true (gdv_is_within_range_f (pos_y, (gfloat) axis_beg_pix_y,
                                          (gfloat) axis_end_pix_y));
  }
  g_list_free(mtics_cpy);
}

