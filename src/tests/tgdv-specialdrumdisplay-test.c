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
#include "tgdv-linearaxis.h"
#include "tgdv-layer.h"
#include "tgdv-axis.h"

static const guint cb_time = 500;

struct _tgdv_specialddisplay_data {
  GtkWidget *window;
  GdvSpecialDrumDisplay *layer;
  GdvAxis *axis;
  guint current_size;
};

static gboolean
teardown_cb (GtkWidget *widget)
{
  gtk_widget_destroy (widget);
  gtk_main_quit();
  return FALSE;
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
  gtk_widget_show_all(data->window);

  g_object_get(G_OBJECT(data->layer), "axis", &data->axis, NULL);
  g_assert_nonnull(data->axis);
  tgdv_specialdrumdisplay_test_integrity(GDV_SPECIAL_DRUM_DISPLAY(data->layer));
  tgdv_linearaxis_test_integrity(GDV_LINEAR_AXIS(data->axis));

  while (gtk_events_pending ())
    gtk_main_iteration ();

  g_timeout_add (cb_time, ((GSourceFunc) teardown_cb), data->window); // cb_time
  tgdv_specialdrumdisplay_test_integrity(GDV_SPECIAL_DRUM_DISPLAY(data->layer));
  tgdv_linearaxis_test_integrity(GDV_LINEAR_AXIS(data->axis));

  gtk_main ();
  data = NULL;
}


static void
test_drum_displ_pre_set (void)
{
  struct _tgdv_specialddisplay_data data_str;
  struct _tgdv_specialddisplay_data * data = &data_str;

  gtk_init (NULL, 0);

  data->window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  data->layer = g_object_new (GDV_TYPE_SPECIAL_DRUM_DISPLAY, NULL);
  gtk_container_add (GTK_CONTAINER (data->window), GTK_WIDGET (data->layer));
  gtk_widget_set_size_request (GTK_WIDGET (data->window), 800, 800);
  gtk_window_set_title (GTK_WINDOW (data->window), "Test window");
  gtk_widget_show_all(data->window);

  g_object_get(G_OBJECT(data->layer), "axis", &data->axis, NULL);
  g_assert_nonnull(data->axis);
  tgdv_specialdrumdisplay_test_integrity(GDV_SPECIAL_DRUM_DISPLAY(data->layer));
  tgdv_linearaxis_test_integrity(GDV_LINEAR_AXIS(data->axis));

  /* These settings contain a more common scenario for e.g. Head-Up Displays */
  g_object_set (data->layer,
                "center-value", 0.0,
                "left-right-range", 50.0,
                //"center-value", 0.0,
                NULL);

  g_object_set (data->axis,
                "force-beg-end", TRUE,
                "axis-beg-at-screen-x", 0.5,
                "axis-end-at-screen-x", 0.5,
                "axis-beg-at-screen-y", 0.0,
                "axis-end-at-screen-y", 1.0,
                "scale-auto-increment", FALSE,
                NULL);

  while (gtk_events_pending ())
    gtk_main_iteration ();

  g_timeout_add (cb_time, ((GSourceFunc) teardown_cb), data->window); // cb_time

  while (gtk_events_pending ())
    gtk_main_iteration ();

  tgdv_specialdrumdisplay_test_integrity(GDV_SPECIAL_DRUM_DISPLAY(data->layer));
  tgdv_axis_test_full(GDV_AXIS(data->axis));

  gtk_main ();

  data = NULL;
}

static gboolean
sizerequest_cb (struct _tgdv_specialddisplay_data * data)
{

  if (data->current_size == 800)
    data->current_size = 1500;
  else if (data->current_size == 1500)
    data->current_size = 200;


  gtk_widget_set_size_request (GTK_WIDGET (data->window),
                               data->current_size, data->current_size);

  while (gtk_events_pending ())
    gtk_main_iteration ();

  tgdv_specialdrumdisplay_test_integrity(GDV_SPECIAL_DRUM_DISPLAY(data->layer));
  tgdv_axis_test_full(GDV_AXIS(data->axis));

  if (data->current_size == 200)
  {
    g_timeout_add (cb_time, ((GSourceFunc) teardown_cb), data->window); // cb_time
    return FALSE;
  }
  else
    return TRUE;
}

static void
test_drum_displ_force_test (void)
{
  struct _tgdv_specialddisplay_data data_str;
  struct _tgdv_specialddisplay_data * data = &data_str;

  gtk_init (NULL, 0);

  data->window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  data->layer = g_object_new (GDV_TYPE_SPECIAL_DRUM_DISPLAY, NULL);
  gtk_container_add (GTK_CONTAINER (data->window), GTK_WIDGET (data->layer));
  data->current_size = 800;
  gtk_widget_set_size_request (GTK_WIDGET (data->window),
                               data->current_size, data->current_size);
  gtk_window_set_title (GTK_WINDOW (data->window), "Test window");
  gtk_widget_show_all(data->window);

  g_object_get(G_OBJECT(data->layer), "axis", &data->axis, NULL);
  g_assert_nonnull(data->axis);
  tgdv_specialdrumdisplay_test_integrity(GDV_SPECIAL_DRUM_DISPLAY(data->layer));
  tgdv_linearaxis_test_integrity(GDV_LINEAR_AXIS(data->axis));

  /* These settings contain a more common scenario for e.g. Head-Up Displays */
  g_object_set (data->layer,
                "center-value", 0.0,
                "left-right-range", 50.0,
                //"center-value", 0.0,
                NULL);

  g_object_set (data->axis,
                "force-beg-end", TRUE,
                "axis-beg-at-screen-x", 0.5,
                "axis-end-at-screen-x", 0.5,
                "axis-beg-at-screen-y", 0.0,
                "axis-end-at-screen-y", 1.0,
                "scale-auto-increment", FALSE,
                NULL);

  while (gtk_events_pending ())
    gtk_main_iteration ();

  g_timeout_add (cb_time, ((GSourceFunc) sizerequest_cb), data); // cb_time

  while (gtk_events_pending ())
    gtk_main_iteration ();

  tgdv_specialdrumdisplay_test_integrity(GDV_SPECIAL_DRUM_DISPLAY(data->layer));
  tgdv_axis_test_full(GDV_AXIS(data->axis));

  gtk_main ();

  data = NULL;
}

int main(int argc, char* argv[]) {
  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/Gdv/Special/DrumDisplay/plain", test_drum_displ_plain);
  g_test_add_func ("/Gdv/Special/DrumDisplay/preset", test_drum_displ_pre_set);
  g_test_add_func ("/Gdv/Special/DrumDisplay/force", test_drum_displ_force_test);

  return g_test_run ();
}

