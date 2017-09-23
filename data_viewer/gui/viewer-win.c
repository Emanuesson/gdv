/*
 * viewer-app.c
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

//#include <cairo-gobject.h>

#include "viewer-app.h"

#define SLEEP_LGTH  5
#define NPOINTS     50

struct _GdvViewerAppPrivate
{
  GObject           *settings;
  GtkCssProvider    *css_provider;
  GtkStyleContext   *style_context;
};

G_DEFINE_TYPE_WITH_PRIVATE (GdvViewerApp, gdv_viewer_app, GTK_TYPE_APPLICATION);

void callback( GtkWidget *widget,
               gpointer   data )
{
    gtk_widget_show (GTK_SOCKET (data));
    g_print ("new sockid: %x\n", gtk_socket_get_id (GTK_SOCKET (data)));

    gnuplot_ctrl    *   h1,
                    *   h2,
                    *   h3,
                    *   h4 ;
    double              x[NPOINTS] ;
    double              y[NPOINTS] ;
    int                 i ;
    char gplot_command[50];

    sprintf(gplot_command, "set terminal x11 window \"%#x\"\0", gtk_socket_get_id (GTK_SOCKET (data)));
//    sprintf(gplot_command, "set terminal x11 window \"0x4000003\"");
    g_print ("%s\n", gplot_command);


    /*
     * Initialize the gnuplot handle
     */
    h1 = gnuplot_init() ;

    gnuplot_cmd(h1, gplot_command);
    gnuplot_cmd(h1, "test");
    gtk_widget_show (GTK_SOCKET (data));

//    sleep(SLEEP_LGTH) ;
//    gnuplot_close(h1) ;

}

void callback_socket( GtkWidget *widget,
               gpointer   data )
{
    g_print ("Plug added\n");
}

static void
gdv_viewer_app_activate (GApplication *application)
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
//    gtk_container_add (GTK_CONTAINER (app_window), socket);

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

  g_print ("Display id: %x\n", display_id);
  g_print ("gdk_win id: %x\n", gdk_id);
  g_print ("socket  id: %x\n", (int) socket_id);
}

static void
show_about_activated (GSimpleAction *action,
                      GVariant      *parameter,
                      gpointer       user_data)
{
  gtk_show_about_dialog (NULL,
                         "program-name", "Gdv-Viewer-Suite",
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
_gdv_viewer_app_has_app_menu (GdvViewerApp *app)
{
  GtkSettings *gtk_settings;
  gboolean show_app_menu;
  gboolean show_menubar;

  g_return_val_if_fail (GDV_VIEWER_IS_APP (app), FALSE);

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
gdv_viewer_app_startup (GApplication *application)
{
  GdvViewerApp *app = GDV_VIEWER_APP (application);

  G_APPLICATION_CLASS (gdv_viewer_app_parent_class)->startup (application);

  /* initialize app menu on application startup */
  if (_gdv_viewer_app_has_app_menu (app))
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
gdv_viewer_app_init (GdvViewerApp *app)
{

  app->priv = gdv_viewer_app_get_instance_private (app);


}

static void
gdv_viewer_app_dispose (GObject *object)
{
  GdvViewerApp *app = GDV_VIEWER_APP (object);

  g_clear_object (&app->priv->settings);

  G_OBJECT_CLASS (gdv_viewer_app_parent_class)->dispose (object);
}

static void
gdv_viewer_app_finalize (GObject *object)
{
  GdvViewerApp *app = GDV_VIEWER_APP (object);

  G_OBJECT_CLASS (gdv_viewer_app_parent_class)->finalize (object);
}

static void
gdv_viewer_app_class_init (GdvViewerAppClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GApplicationClass *application_class = G_APPLICATION_CLASS (klass);

  object_class->dispose = gdv_viewer_app_dispose;
  object_class->finalize = gdv_viewer_app_finalize;

  application_class->startup = gdv_viewer_app_startup;
  application_class->activate = gdv_viewer_app_activate;
}


/**
 * gdv_viewer_app_new:
 *
 * Creates a new #GdvViewerApp. A simple wrapper-function of
 * the #gtk_application_new function.
 *
 * Returns: (transfer none): a new #GdvViewerApp
 */
GdvViewerApp *
gdv_viewer_app_new (void)
{
  /*FIXME: change application-id in correct nomenclature; look for other appearence */
  return g_object_new (gdv_viewer_app_get_type (),
                       "application-id", "org.gtk.Gdv.GdvViewerApp",
                       NULL);
}
