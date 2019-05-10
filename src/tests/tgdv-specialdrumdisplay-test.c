/* tgdv-specialdrumdisplay-test.c
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
#include "tgdv-specialdrumdisplay.h"

static const guint cb_time = 300;

struct _tgdv_specialddisplay_data {
  GtkWidget *window;
  GdvSpecialDrumDisplay *layer;
  GdvAxis *axis;
};

static gboolean
teardown_cb (GtkWidget *widget)
{
  gtk_widget_destroy (widget);
  gtk_main_quit();
  return FALSE;
}

/*
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
*/

static guint endless_loop_prevention = 0;
static gboolean
teardown_wrapper_cb (GtkWidget *widget)
{
//  g_print("REACHED: %e\n", current_orientation);
/*
  endless_loop_prevention++;

  if (endless_loop_prevention > 2)
  {
    g_assert_not_reached();
    return FALSE;
  }

  if (current_orientation > 3.5)
    return teardown_cb(widget);
*/
  return TRUE;
}

static void
test_drum_displ_plain (void)
{
  struct _tgdv_specialddisplay_data data_str;
  struct _tgdv_specialddisplay_data * data = &data_str;

  gtk_init (NULL, 0);

  data->window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  data->layer = g_object_new (GDV_TYPE_SPECIAL_DRUM_DISPLAY, NULL);
  gtk_container_add (GTK_CONTAINER (data->window), GTK_WIDGET (data->layer));
  gtk_widget_set_size_request (GTK_WIDGET (data->window), 800, 800);
  gtk_window_set_title (GTK_WINDOW (data->window), "Test window");
  //g_object_unref(data->layer);
  gtk_widget_show_all(data->window);


  g_object_get(G_OBJECT(data->layer), "axis", &data->axis, NULL);

//  g_timeout_add (10, ((GSourceFunc) timeout_cb_oned), data);
  g_timeout_add (cb_time * 10, ((GSourceFunc) teardown_cb), data->window); // cb_time

  gtk_main ();
  data = NULL;

}

int main(int argc, char* argv[]) {
  g_test_init (&argc, &argv, NULL);

//  g_test_add_func ("/Gdv/LinearAxis/correct_default", test_lin_axis_correct_default);
//  g_test_add_func ("/Gdv/LinearAxis/horizontal", test_lin_axis_horizontal);
//  g_test_add_func ("/Gdv/LinearAxis/cross_settings", test_lin_axis_cross_settings);
  g_test_add_func ("/Gdv/Special/DrumDisplay/plain", test_drum_displ_plain);
/*  g_test_add_data_func_full ("/Gdv/LinearAxis/manual_tics", setting,
                             test_lin_axis_manual_tics, test_lin_axis_reset_window);
*/
  return g_test_run ();
}

