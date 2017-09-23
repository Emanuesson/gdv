/*
 * test-app.c
 * This file is part of gdv
 *
 * Copyright (C) 2013 - Emanuel Schmidt
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cairo-gobject.h>

#include "test-app.h"

#include <gnuplot_i/gnuplot_i.h>
#include <goffice/canvas/goc-widget.h>
#include <goffice/goffice.h>

#include<X11/Xlib.h>

#define SLEEP_LGTH  5
#define NPOINTS     50
#define TEST_ACTIVE_GUI     TRUE
#define TEST_ACTIVE_GNUPLOT FALSE
#define TEST_ACTIVE_GOFFICE FALSE

/*
  this whole file is just thought as a test to compare the most convenient library for the data-viewer
*/

static void
gdv_test_regular_gui_app_activate (GApplication *application)
{
  GtkWidget *app_window;
  Gdv2dView *simple_view;
  GdvLayer *even_simpler_layer;
  GdvLayer *layer_1, *layer_2, *layer_3;
  Gdv2dStaticLayer *static_layer;
//  GdvAxis *axis1, *axis2;
  GtkWidget *test_label_1, *test_label_2;
  GtkWidget *vbox, *hbox, *button1, *button2;

  app_window = gtk_application_window_new(GTK_APPLICATION (application));

  gtk_container_set_border_width (GTK_CONTAINER (app_window), 8);
  simple_view = gdv_2d_view_new();
  static_layer = g_object_new (gdv_2d_static_layer_get_type (), NULL);

//  axis1 = g_object_new (gdv_axis_get_type (), NULL);
//  axis2 = g_object_new (gdv_axis_get_type (), NULL);
//  gtk_container_add (GTK_CONTAINER (even_simpler_layer), GTK_WIDGET (axis1));
//  gtk_container_add (GTK_CONTAINER (even_simpler_layer), GTK_WIDGET (axis2));

//  gtk_container_add (GTK_CONTAINER (simple_view), GTK_WIDGET (even_simpler_layer));

  button1 = gtk_button_new_with_label ("button1");
  button2 = gtk_button_new_with_label ("button2");
  hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_box_pack_start (GTK_BOX (hbox), button1, TRUE, TRUE, 5);
  gtk_box_pack_start (GTK_BOX (hbox), button2, TRUE, TRUE, 5);

  vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_box_pack_start (GTK_BOX (vbox), GTK_WIDGET(simple_view), TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (vbox), GTK_WIDGET(hbox), TRUE, TRUE, 0);

  gtk_container_add (GTK_CONTAINER (app_window), GTK_WIDGET (vbox));
//  gtk_container_add (GTK_CONTAINER (app_window), GTK_WIDGET (simple_view));

//  gdv_2d_view_set_background_rgba (simple_view,
//                                   0.061, 0.089, 0.171, 0.81);
  /* more mac-compatible background */
//  gdv_2d_view_set_background_rgba (simple_view,
//        0.361, 0.389, 0.671, 0.81);

  GdvElement *new_xmpl_element_1, *new_xmpl_element_2, *new_xmpl_element_3;
  new_xmpl_element_1 = g_object_new (gdv_element_get_type (), NULL);
  new_xmpl_element_2 = g_object_new (gdv_element_get_type (), NULL);
  new_xmpl_element_3 = g_object_new (gdv_element_get_type (), NULL);

  layer_1 = g_object_new (gdv_layer_get_type (), NULL);
  layer_2 = g_object_new (gdv_layer_get_type (), NULL);
  layer_3 = g_object_new (gdv_layer_get_type (), NULL);

  gtk_widget_set_size_request (GTK_WIDGET (layer_1), 200, 200);
  gtk_widget_set_size_request (GTK_WIDGET (layer_2), 200, 200);
  gtk_widget_set_size_request (GTK_WIDGET (layer_3), 200, 200);
  gtk_widget_set_size_request (GTK_WIDGET (static_layer), 200, 200);

  GtkWidget *test_grid;
  test_grid = gtk_grid_new();
  gtk_grid_attach (GTK_GRID (test_grid), GTK_WIDGET (layer_1), 0, 0, 1, 1);
  gtk_grid_attach (GTK_GRID (test_grid), GTK_WIDGET (layer_2), 0, 1, 1, 1);
  gtk_grid_attach (GTK_GRID (test_grid), GTK_WIDGET (layer_3), 1, 0, 1, 1);
  gtk_grid_attach (GTK_GRID (test_grid), GTK_WIDGET (static_layer), 1, 1, 1, 1);

//  GdvElement *new_xmpl_element_10, *new_xmpl_element_20, *new_xmpl_element_30, *new_xmpl_element_40;
//  GtkWidget *complete_strange_grid;

//  complete_strange_grid = gtk_grid_new ();
//  new_xmpl_element_10 = g_object_new (gdv_element_get_type (), NULL);
//  new_xmpl_element_20 = g_object_new (gdv_element_get_type (), NULL);
//  new_xmpl_element_30 = g_object_new (gdv_element_get_type (), NULL);
//  new_xmpl_element_40 = g_object_new (gdv_element_get_type (), NULL);

//  gtk_widget_set_size_request (GTK_WIDGET (new_xmpl_element_10), 100, 100);
//  gtk_widget_set_size_request (GTK_WIDGET (new_xmpl_element_20), 100, 100);
//  gtk_widget_set_size_request (GTK_WIDGET (new_xmpl_element_30), 100, 100);
//  gtk_widget_set_size_request (GTK_WIDGET (new_xmpl_element_40), 100, 100);

//  gtk_grid_attach (GTK_GRID (complete_strange_grid), GTK_WIDGET (new_xmpl_element_10), 0, 0, 1, 1);

//  gtk_grid_attach (GTK_GRID (test_grid), GTK_WIDGET (complete_strange_grid), 0, 0, 1, 1);


  g_object_set (G_OBJECT (layer_1), "expand", FALSE, NULL);
  g_object_set (G_OBJECT (layer_2), "expand", FALSE, NULL);
  g_object_set (G_OBJECT (layer_3), "expand", FALSE, NULL);
  g_object_set (G_OBJECT (static_layer), "expand", TRUE, NULL);

  gtk_container_add (GTK_CONTAINER (simple_view), GTK_WIDGET (test_grid));

  GdkScreen *screen_window;
  GtkCssProvider *css_provider;

//  g_object_get (G_OBJECT (simple_view), "window", &scree_window);
  screen_window = gtk_widget_get_screen (GTK_WIDGET (simple_view));
  css_provider = gtk_css_provider_get_default ();
  gtk_css_provider_load_from_path (css_provider, "../add_info/bumms.css", NULL);

  gtk_style_context_add_provider_for_screen(
    screen_window,
    GTK_STYLE_PROVIDER(css_provider),
    GTK_STYLE_PROVIDER_PRIORITY_USER);

//  gtk_widget_set_child_visible (GTK_WIDGET(axis1), FALSE);
//  gtk_widget_set_child_visible (GTK_WIDGET(axis2), FALSE);

//  gtk_widget_queue_draw (GTK_WIDGET (app_window));


  GdvLayerContent *layer_content, *layer_content_2;
  layer_content = gdv_layer_content_new();
  gdv_layer_content_add_data_point (layer_content, 10, 10, 0);
  gdv_layer_content_add_data_point (layer_content, 10, 20, 0);
  gdv_layer_content_add_data_point (layer_content, 10, 40, 0);
  gdv_layer_content_add_data_point (layer_content, 10, 80, 0);
  gdv_layer_content_add_data_point (layer_content, 10, 160, 0);
  gdv_layer_content_add_data_point (layer_content, 10, 170, 0);

  gtk_container_add(GTK_CONTAINER (static_layer), GTK_WIDGET(layer_content));


//  cairo_pattern_t *new_color = cairo_pattern_create_rgba (0.4, 0.2, 0.2, 1);
//  g_object_set (G_OBJECT(layer_content), "point-source", new_color, NULL);
//  gdv_layer_add_layer_content(GDV_LAYER (simple_layer), layer_content);

//  GtkWidgetClass *test_class = GTK_WIDGET_GET_CLASS(simple_layer);
//  test_class->get_preferred_width (GTK_WIDGET(simple_layer), &width, &nat_width);
//  test_class->get_preferred_height (GTK_WIDGET(simple_layer), &height, &nat_height);
//  g_print ("pref width: %u, %u\n", width, nat_width);
//  g_print ("pref height: %u, %u\n", height, nat_height);

//  gtk_widget_queue_draw (GTK_WIDGET (simple_view));
//  gtk_widget_queue_draw (GTK_WIDGET (simple_layer));


/* Doesn't work or is deprecated */
//  GtkStyleContext   *context;
//  context = gtk_widget_get_style_context (GTK_WIDGET(simple_view));
//  gdouble stuff;

//  GValue value = G_VALUE_INIT;
//  gtk_widget_style_get (GTK_WIDGET(simple_view), "custom-property", &stuff, NULL);

//  g_print("thing: %e\n", stuff);

  //gtk_style_context_get_property (context, "custom-property", GTK_STATE_FLAG_NORMAL, &value);
//  const gchar *my_str = g_type_name(G_VALUE_TYPE(&value));
//  g_print ("type: %s\n", my_str);
//  g_print ("thing: %f\n", g_value_get_double(&value));

  gtk_widget_show_all (GTK_WIDGET (app_window));


//  layer_content_2 = gdv_layer_content_new();
//  gdv_layer_content_add_data_point (layer_content_2, 15, 100, 0);
//  gdv_layer_content_add_data_point (layer_content_2, 20, 100, 0);
//  gdv_layer_content_add_data_point (layer_content_2, 40, 100, 0);
//  gdv_layer_content_add_data_point (layer_content_2, 80, 200, 0);
//  gdv_layer_content_add_data_point (layer_content_2, 160, 100, 0);
//  gdv_layer_content_add_data_point (layer_content_2, 170, 100, 0);
//  cairo_pattern_t *new_color_2 = cairo_pattern_create_rgba (0.2, 0.2, 0.4, 1);
//  g_object_set (G_OBJECT(layer_content_2), "point-source", new_color_2, NULL);
//  g_object_set (G_OBJECT(layer_content_2), "point-fill", FALSE, NULL);
//  g_object_set (G_OBJECT(layer_content_2), "point-width", 10.0, NULL);
//  cairo_pattern_t *new_color_3 = cairo_pattern_create_rgba (0.2, 0.7, 0.1, 1);
//  g_object_set (G_OBJECT(layer_content_2), "line-source", new_color_3, NULL);
//  g_object_set (G_OBJECT(layer_content_2), "line-width", 3.1, NULL);
//  gdv_layer_add_layer_content(GDV_LAYER (simple_layer), layer_content_2);

}


static void callback( GtkWidget *widget,
               gpointer   data )
{
    gtk_widget_show (GTK_WIDGET (data));
    g_print ("new sockid: %#x\n", (guint) gtk_socket_get_id (GTK_SOCKET (data)));

    gnuplot_ctrl    *   h1,
                    *   h2,
                    *   h3,
                    *   h4 ;
    double              x[NPOINTS] ;
    double              y[NPOINTS] ;
    int                 i ;
    char gplot_command[50];

    sprintf(gplot_command, "set terminal x11 window \"%#x\"", (guint) gtk_socket_get_id (GTK_SOCKET (data)));
//    sprintf(gplot_command, "set terminal x11 window \"0x1c00018\"");

    g_print ("%s\n", gplot_command);

    /*
     * Initialize the gnuplot handle
     */
    h1 = gnuplot_init() ;

    gnuplot_cmd(h1, gplot_command);
    gnuplot_cmd(h1, "test");
    gtk_widget_show (GTK_WIDGET (data));

//    sleep(SLEEP_LGTH) ;
//    gnuplot_close(h1) ;

}

static void callback_socket( GtkWidget *widget,
                             gpointer   data )
{
    g_print ("Plug added\n");
}

static void
on_quit (GtkWidget *w)
{
	gtk_widget_destroy (w);
	gtk_main_quit ();
}

static void
if_test_goffice_app_activate (GApplication *application)
{
  GtkWidget *app_window;

  app_window = gtk_application_window_new(GTK_APPLICATION (application));

//  libgoffice_init ();

	GtkWidget *window, *box, *w;
	GogChart *chart;
	GogGraph *graph;
	GogLabel *label;
	GogPlot *pie;
	GogSeries *series;
	GOStyle *style;
	GOData *data;
	GError *error;
	PangoFontDescription *desc;
	char const *title = "Some statistics";
	char const * const legends[] = {"first", "second", "third", "fourth"};
	double values[] = {10., 20., 30., 40.};

//	gtk_init (&argc, &argv);
	/* Initialize libgoffice */
	libgoffice_init ();
	/* Initialize plugins manager */
	go_plugins_init (NULL, NULL, NULL, NULL, TRUE, GO_TYPE_PLUGIN_LOADER_MODULE);

//	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_resize (GTK_WINDOW (app_window), 300, 340);
	gtk_window_set_title (GTK_WINDOW (app_window), "pie demo");
//	g_signal_connect (app_window, "destroy", gtk_main_quit, NULL);

	box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	w = gtk_button_new_from_stock (GTK_STOCK_QUIT);
	g_signal_connect_swapped (w, "clicked", G_CALLBACK (on_quit), app_window);
	gtk_box_pack_end (GTK_BOX (box), w, FALSE, FALSE, 0);

	w = gtk_separator_new (GTK_ORIENTATION_HORIZONTAL);
	gtk_box_pack_end (GTK_BOX (box), w, FALSE, FALSE, 2);

	/* Create a graph widget and add it to the GtkVBox */
	w = go_graph_widget_new (NULL);
	gtk_box_pack_end (GTK_BOX (box), w, TRUE, TRUE, 0);
	/* Get the embedded graph */
	graph = go_graph_widget_get_graph (GO_GRAPH_WIDGET (w));
	/* Add a title */
	label = (GogLabel *) g_object_new (GOG_TYPE_LABEL, NULL);
	data = go_data_scalar_str_new (title, FALSE);
	gog_dataset_set_dim (GOG_DATASET (label), 0, data, NULL);
	gog_object_add_by_name (GOG_OBJECT (graph), "Title", GOG_OBJECT (label));
	/* Change the title font */
	style = go_styled_object_get_style (GO_STYLED_OBJECT (label));
	desc = pango_font_description_from_string ("Sans bold 16");
	go_style_set_font_desc (style, desc);
	/* Get the chart created by the widget initialization */
	chart = go_graph_widget_get_chart (GO_GRAPH_WIDGET (w));
	/* Create a pie plot and add it to the chart */
	pie = (GogPlot *) gog_plot_new_by_name ("GogPiePlot");
	gog_object_add_by_name (GOG_OBJECT (chart), "Plot", GOG_OBJECT (pie));
	/* Create a series for the plot and populate it with some simple data */
	series = gog_plot_new_series (pie);
	data = go_data_vector_str_new (legends, 4, NULL);
	gog_series_set_dim (series, 0, data, &error);
	data = go_data_vector_val_new (values, 4, NULL);
	gog_series_set_dim (series, 1, data, &error);
	/* Add a legend to the chart */
	gog_object_add_by_name (GOG_OBJECT (chart), "Legend", NULL);

	gtk_container_add (GTK_CONTAINER (app_window), box);
	gtk_widget_show_all (GTK_WIDGET (app_window));

	w = gtk_separator_new (GTK_ORIENTATION_HORIZONTAL);
	gtk_box_pack_start (GTK_BOX (box), w, FALSE, FALSE, 0);

	/* Clean libgoffice stuff */
//  libgoffice_shutdown ();











}

static void
if_test_x11_app_activate (GApplication *application)
{
  GtkWidget *app_window;
  int display_id;

  app_window = gtk_application_window_new(GTK_APPLICATION (application));

  gtk_widget_set_size_request (GTK_WIDGET (app_window), 800, 800);
  gtk_container_set_border_width (GTK_CONTAINER (app_window), 8);

  Display *disp;
  GtkWidget *button1, *hbox;
  XID gdk_id, socket_id;

  GdkWindow *gdk_win;
  GdkDisplay *gdk_disp;

  GtkWidget *socket = gtk_socket_new ();

  button1 = gtk_button_new_with_label ("button1");
  hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_box_pack_start (GTK_BOX (hbox), button1, TRUE, TRUE, 5);
  gtk_box_pack_start (GTK_BOX (hbox), socket, TRUE, TRUE, 5);
  gtk_container_add (GTK_CONTAINER (  app_window), hbox);

  g_signal_connect (GTK_WIDGET (button1), "clicked",
                    (GCallback) callback, socket);
  g_signal_connect (GTK_WIDGET (socket), "plug-added",
                    (GCallback) callback_socket, NULL);

  gtk_widget_realize (socket);

  gtk_widget_show_all (GTK_WIDGET (app_window));
  gtk_widget_realize (app_window);
  gdk_win = gtk_widget_get_window (app_window);

  gdk_win = gtk_widget_get_window (app_window);
  gdk_disp = gdk_display_get_default ();
  disp = gdk_x11_display_get_xdisplay (gdk_disp);
  display_id = XConnectionNumber(disp);
  gdk_id = gdk_x11_window_get_xid (gdk_win);
  socket_id = gtk_socket_get_id (GTK_SOCKET (socket));

  g_print ("Display id: %#x\n", (guint) display_id);
  g_print ("gdk_win id: %#x\n", (guint) gdk_id);
  g_print ("socket  id: %#x\n", (guint) socket_id);

}


struct _GdvTestAppPrivate
{
  GObject           *settings;
  GtkCssProvider    *css_provider;
  GtkStyleContext   *style_context;
};

G_DEFINE_TYPE_WITH_PRIVATE (GdvTestApp, gdv_test_app, GTK_TYPE_APPLICATION);

static void
show_about_activated (GSimpleAction *action,
                      GVariant      *parameter,
                      gpointer       user_data)
{
  gtk_show_about_dialog (NULL,
                         "program-name", "Gdv-Test-Suite",
                         "title", "About Gdv",
                         "logo-icon-name", "gdv",
                         "comments", "Prerelease version gdv 0.0",
                         NULL);
}


static void
quit_app_activated (GSimpleAction *action,
                    GVariant      *parameter,
                    gpointer       user_data)
{
  GList *list, *next;
  GtkWindow *win;

  list = gtk_application_get_windows (GTK_APPLICATION (
                                        g_application_get_default ()));

  while (list)
    {
      win = list->data;
      next = list->next;

      gtk_widget_destroy (GTK_WIDGET (win));

      list = next;
    }
}

static void
new_activated (GSimpleAction *action,
               GVariant      *parameter,
               gpointer       user_data)
{
  GApplication *app = user_data;

  g_application_activate (app);
}

static GActionEntry app_entries[] =
{
  { "about", show_about_activated, NULL, NULL, NULL },
  { "quit", quit_app_activated, NULL, NULL, NULL },
  { "new", new_activated, NULL, NULL, NULL }
};

static gboolean
_gdv_test_app_has_app_menu (GdvTestApp *app)
{
  GtkSettings *gtk_settings;
  gboolean show_app_menu;
  gboolean show_menubar;

  g_return_val_if_fail (GDV_TEST_IS_APP (app), FALSE);

  /* thanks to Gedit source-code
   * We have three cases:
   * - GNOME 3: show-app-menu true, show-menubar false -> use the app menu
   * - Unity, OSX: show-app-menu and show-menubar true -> use the normal menu
   * - Other WM, Windows: show-app-menu and show-menubar false -> use the normal menu
   */
  gtk_settings = gtk_settings_get_default ();
  g_object_get (G_OBJECT (gtk_settings),
                "gtk-shell-shows-app-menu", &show_app_menu,
                "gtk-shell-shows-menubar", &show_menubar,
                NULL);

  return show_app_menu && !show_menubar;
}

static void
gdv_test_app_startup (GApplication *application)
{
  GdvTestApp *app = GDV_TEST_APP (application);

  G_APPLICATION_CLASS (gdv_test_app_parent_class)->startup (application);

  /* initialize app menu on application startup */
  if (_gdv_test_app_has_app_menu (app))
    {
      GtkBuilder *builder;
      GError *error = NULL;

      g_action_map_add_action_entries (G_ACTION_MAP (app),
                                       app_entries,
                                       G_N_ELEMENTS (app_entries),
                                       app);
    }
}

static void
gdv_test_app_init (GdvTestApp *app)
{

  app->priv = gdv_test_app_get_instance_private (app);


}

static void
gdv_test_app_dispose (GObject *object)
{
  GdvTestApp *app = GDV_TEST_APP (object);

  g_clear_object (&app->priv->settings);

  G_OBJECT_CLASS (gdv_test_app_parent_class)->dispose (object);
}

static void
gdv_test_app_finalize (GObject *object)
{
  GdvTestApp *app = GDV_TEST_APP (object);

  G_OBJECT_CLASS (gdv_test_app_parent_class)->finalize (object);
}

static void
gdv_test_app_class_init (GdvTestAppClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GApplicationClass *application_class = G_APPLICATION_CLASS (klass);

  object_class->dispose = gdv_test_app_dispose;
  object_class->finalize = gdv_test_app_finalize;

  application_class->startup = gdv_test_app_startup;

  if (TEST_ACTIVE_GUI)
    application_class->activate = gdv_test_regular_gui_app_activate;
  if (TEST_ACTIVE_GNUPLOT)
    application_class->activate = if_test_x11_app_activate;
  if (TEST_ACTIVE_GOFFICE)
    application_class->activate = if_test_goffice_app_activate;
}


/**
 * gdv_test_app_new:
 *
 * Creates a new #GdvTestApp. A simple wrapper-function of
 * the #gtk_application_new function.
 *
 * Returns: (transfer none): a new #GdvTestApp
 */
GdvTestApp *
gdv_test_app_new (void)
{
  /*FIXME: change application-id in correct nomenclature; look for other appearence */
  return g_object_new (gdv_test_app_get_type (),
                       "application-id", "org.gtk.Gdv.GdvTestApp",
                       NULL);
}
