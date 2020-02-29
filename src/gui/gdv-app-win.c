/*
 * viewer-app-win.c
 * This file is part of gdv
 *
 * Copyright (C) 2013 - Emanuel Schmidt
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

//#include <gtksourceview/gtksourcetypes.h>
#include <gio/gio.h>
#include <math.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <inttypes.h>
#include <errno.h>

#include "gui/gdv-app-win.h"

enum
{
  COLUMN_STRING,
//  COLUMN_INT,
  COLUMN_FILE,
  COLUMN_BOOLEAN,
  COLUMN_FILE_BUFFER,
  N_COLUMNS
};


/* Define Properties */
enum
{
  PROP_0,

  PROP_VIEWER_WIN_FILE,

  N_PROPERTIES
};

struct _GdvViewerAppWindowPrivate
{
  gint             placeholder;

  GdvLayerContent *content;
  GdvLayer    *main_layer;
  GtkTreeView     *file_view;
  GtkScrolledWindow *file_view_window;
  GtkScrolledWindow *file_content_window;

//  ViewerFile *file;
  GList *files;

  int open_fd;
  uint8_t curr;

//  GtkListStore *file_list;
};

G_DEFINE_TYPE_WITH_PRIVATE (GdvViewerAppWindow, gdv_viewer_app_window,
                            GTK_TYPE_APPLICATION_WINDOW)

static void
gdv_viewer_app_window_dispose (GObject *object)
{
  G_OBJECT_CLASS (gdv_viewer_app_window_parent_class)->dispose (object);
}

static void
gdv_viewer_app_window_finalize (GObject *object)
{
  G_OBJECT_CLASS (gdv_viewer_app_window_parent_class)->finalize (object);
}

static void
gdv_viewer_app_window_class_init (GdvViewerAppWindowClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->dispose = gdv_viewer_app_window_dispose;
  object_class->finalize = gdv_viewer_app_window_finalize;

//  g_type_ensure (GDV_TWOD_TYPE_LAYER);

  gtk_widget_class_set_template_from_resource (
    GTK_WIDGET_CLASS (object_class),
    "/net/gdv/viewerapp/ui/gui/gdv-app-win.ui");

  gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (object_class),
                                                GdvViewerAppWindow,
                                                main_layer);
}

void
gdv_viewer_app_window_init (GdvViewerAppWindow *window)
{
  GdvAxis *axis;
//  GList * axis_list;
//  gint i;
//  GtkTreePath *path;
//  GtkTreeIter iter;

  g_type_ensure (gdv_special_drum_display_get_type ());

  gtk_widget_init_template (GTK_WIDGET (window));

  window->priv = gdv_viewer_app_window_get_instance_private (window);

  gtk_widget_show_all(GTK_WIDGET(window->priv->main_layer));

  g_object_set (window->priv->main_layer,
                 "center-value", 0.0,
                "left-right-range", 50.0,
                //"center-value", 0.0,
                NULL);

  g_object_get(G_OBJECT(window->priv->main_layer), "axis", &axis, NULL);
  g_object_set (axis,
                "scale-limits-automatic", FALSE,
                "force-beg-end", TRUE,
                "axis-beg-at-screen-x", 0.5,
                "axis-end-at-screen-x", 0.5,
                "axis-beg-at-screen-y", 0.0,
                "axis-end-at-screen-y", 1.0,
                "scale-auto-increment", FALSE,
                NULL);

  window->priv->open_fd = 0;
  window->priv->curr = 0;

//  g_object_set (window->priv->main_layer,
//                "fill", TRUE,
//                "expand", TRUE,
//                NULL);

//  gtk_container_child_set_property

//  g_object_set(window, "show-menubar", TRUE, NULL);

}

GdvViewerAppWindow *
gdv_viewer_app_window_new (GdvViewerApp *app)
{
  return g_object_new (GDV_VIEWER_APP_TYPE_WINDOW, "application", app, NULL);
}

static gboolean
timeout_cb_opend (GdvViewerAppWindow *win)
{
  GdvViewerAppWindowPrivate *priv = gdv_viewer_app_window_get_instance_private (win);
  uint8_t read_data;

//  g_print("nothing\n");

  if (0 < read(priv->open_fd, &read_data, sizeof(read_data))) {
    if (errno != 0)
    {
      printf("Oh dear, something went wrong with read()! %s\n", strerror(errno));
      //return TRUE;
    }
  }

  if (priv->curr != read_data && read_data != 0) {
    g_print("REACHED: %u\n", read_data);
    g_object_set (win->priv->main_layer,
                  "center-value", (double) read_data,
                  NULL);
    priv->curr = read_data;
  }

  return TRUE;
}

void gdv_viewer_app_window_open (GdvViewerAppWindow *win,
                                 GFile            *file)
{
  GdvViewerAppWindowPrivate *priv;

  /* FIXME: Make the actual file-loading in this function and not in selection-changed */
  /* It should be possible to open and display e.g.
   * /sys/bus/iio/devices/iio\:device2/in_magn_y_raw */

  priv = gdv_viewer_app_window_get_instance_private (win);

 /* GFileMonitor * mon =
    g_file_monitor_file (file,
                         GFileMonitorFlags flags,
                         GCancellable *cancellable,
                         GError **error);
*/
  g_print("File: %s\n", g_file_get_path(file));
  priv->open_fd = open(g_file_get_path(file), O_RDONLY | O_NONBLOCK);
  g_timeout_add (1, ((GSourceFunc) timeout_cb_opend), win);

/*
  {
    GList *menus = gtk_menu_get_for_attach_widget(win);

    if (menus != NULL)
      g_print("Reached\n");
  }
*/
}
