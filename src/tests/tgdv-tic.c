/* tgdv-tic.c
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

#include "tgdv-tic.h"

void tgdv_tic_test_full (GdvTic *tic)
{
  gdouble value;
  gfloat xpos, ypos;
  gdouble axis_inner_dir_x, axis_inner_dir_y;
  gdouble xalign, yalign;
  GString label;
  GtkWidget label_widget;
  GtkAllocation allocation = {0};

  gdouble linewidth, in_length, out_length;
  gdouble label_distance;
  gboolean show_label;

  gdouble inner_max_x, inner_max_y;
  gdouble outer_max_x, outer_max_y;

  g_assert(GDV_IS_TIC(tic));

  g_object_get (tic,
    "value", &value,
    "pos-x", &xpos,
    "pos-y", &ypos,
    "axis-inner-dir-x", &axis_inner_dir_x,
    "axis-inner-dir-y", &axis_inner_dir_y,
    "label-xalign", &xalign,
    "label-yalign", &yalign,
    "label-widget", &label_widget,
    "label", &label,
    NULL);

  gtk_widget_get_allocation(GTK_WIDGET(tic), &allocation);

  /* Verification that tic lies within allocated area */
  g_assert_cmpint((gint) xpos, <=, allocation.x + allocation.width);
  g_assert_cmpint((gint) xpos, >=, allocation.x);
  g_assert_cmpint((gint) ypos, <=, allocation.y + allocation.height);
  g_assert_cmpint((gint) ypos, >=, allocation.y);

  gtk_widget_style_get(
    GTK_WIDGET(tic),
    "label-distance", &label_distance,
    "show-label", &show_label,
    "tics-in-length", &in_length,
    "tics-out-length", &out_length,
    "line-width", &linewidth,
    NULL);

  /* Calculating the inner length of the tic line */
  inner_max_x = xpos + axis_inner_dir_x * in_length;
  inner_max_y = ypos + axis_inner_dir_y * in_length;

  g_assert_cmpint(inner_max_x, <=, allocation.x + allocation.width);
  g_assert_cmpint(inner_max_x, >=, allocation.x);
  g_assert_cmpint(inner_max_y, <=, allocation.y + allocation.height);
  g_assert_cmpint(inner_max_y, >=, allocation.y);

  /* Calculating the outer length of the tic line */
  outer_max_x = xpos - axis_inner_dir_x * out_length;
  outer_max_y = ypos - axis_inner_dir_y * out_length;

  g_assert_cmpint(outer_max_x, <=, allocation.x + allocation.width);
  g_assert_cmpint(outer_max_x, >=, allocation.x);
  g_assert_cmpint(outer_max_y, <=, allocation.y + allocation.height);
  g_assert_cmpint(outer_max_y, >=, allocation.y);
}

