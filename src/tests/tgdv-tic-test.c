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

#include <math.h>

#include <stdio.h>
#include <gdv/gdv.h>

#include "tgdv-shared-functions.h"

struct _test_gdv_tic_proeprty_container {
  /* instance properties */
  gdouble value;
  gfloat  curr_pos_in_x;
  gfloat  curr_pos_in_y;
  gdouble axis_inner_dir_x;
  gdouble axis_inner_dir_y;
  gdouble label_xalign;
  gdouble label_yalign;
  GtkWidget *label_widget;
  gchar *label_string;

  /* style properties */
  gdouble line_width;
  GdkRGBA *color;
  gdouble in_length;
  gdouble out_length;
  gboolean show_label;
  gdouble label_distance;
};

static void poll_all_properties (GdvTic *tic,
                                 struct _test_gdv_tic_proeprty_container *props)
{
  g_object_get (G_OBJECT (tic),
                "value", &props->value,
                "pos-x", &props->curr_pos_in_x,
                "pos-y", &props->curr_pos_in_y,
                "axis-inner-dir-x", &props->axis_inner_dir_x,
                "axis-inner-dir-y", &props->axis_inner_dir_y,
                "label-xalign", &props->label_xalign,
                "label-yalign", &props->label_yalign,
                "label", &props->label_string,
                "label-widget", &props->label_widget,
                NULL);

  gtk_widget_style_get (GTK_WIDGET (tic),
                        "line-width", &props->line_width,
                        "color", &props->color,
                        "tics-in-length", &props->in_length,
                        "tics-out-length", &props->out_length,
                        "show-label", &props->show_label,
                        NULL);
}

static void
test_tic_plain (void)
{
  /* Test bedding */
  GtkWidget *window;
  GdvTic *tic;
  GObjectClass *tic_oclass;

  struct _test_gdv_tic_proeprty_container properties;

  /* inherited properties */
  GtkAllocation tic_allocation = {0};

  gtk_init (NULL, 0);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  tic = g_object_new (GDV_TYPE_TIC, NULL);
  g_assert_nonnull(tic);
  tic_oclass = G_OBJECT_GET_CLASS(tic);
  g_assert_nonnull(tic_oclass);
  _st_check_all_properties(G_OBJECT(tic), TRUE, "style", "events", "tooltip-markup", NULL);

  poll_all_properties (tic, &properties);

  gtk_container_add (GTK_CONTAINER (window), GTK_WIDGET (tic));

  /* Realizing and Allocating the Layer */
  gtk_widget_show_all (window);

  g_assert (gtk_widget_get_realized(window));
  g_assert (gtk_widget_get_realized(GTK_WIDGET (tic)));
  _st_check_all_properties(G_OBJECT(tic), FALSE, NULL);

  /* Now testing all automatted settings for correctness */
  gtk_widget_get_allocation (GTK_WIDGET(window), &tic_allocation);
  g_print("Alloc: %d, %d, %d, %d\n",
          tic_allocation.x, tic_allocation.y,
          tic_allocation.width, tic_allocation.height);
}

int main(int argc, char* argv[]) {
  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/Gdv/Tic/plain", test_tic_plain);

  return g_test_run ();
}

