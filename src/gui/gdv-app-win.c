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

//  GtkListStore *file_list;
};

G_DEFINE_TYPE_WITH_PRIVATE (GdvViewerAppWindow, gdv_viewer_app_window, GTK_TYPE_APPLICATION_WINDOW);

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

static gint indicator_fc = 0;

static gboolean
timeout_cb_oned (GdvLayer *layer)
{
//  if (indicator_fc > 0)
//    return FALSE;

  if (layer && GDV_IS_LAYER (layer))
  {
    GList *ind_list, *axis_list;

//    g_object_set (layer, "center-value",
//      0.0 + 100.0 * sin(0.01 * M_PI * ((gdouble) indicator_fc)),NULL);
    g_object_set (layer, "center-value",
      0.0 + 100.0 * sin(0.01 * M_PI * ((gdouble) indicator_fc)),NULL);

    axis_list = gdv_layer_get_axis_list (layer);

    if (axis_list)
    {
//      ind_list = gdv_axis_get_indicator_list (axis_list->data);

      g_object_set (axis_list->data,
        "force-beg-end", TRUE,
        "axis-beg-at-screen-x", 0.5,
        "axis-end-at-screen-x", 0.5,
        "axis-beg-at-screen-y", 0.0,
        "axis-end-at-screen-y", 1.0,
        "scale-increment-val", 20.0,
        "scale-auto-increment", FALSE,
        NULL);

//      if (ind_list)
//      {
//        g_object_set (ind_list->data,
//          "value",0.0 + 100.0 * sin(0.01 * M_PI * ((gdouble) indicator_fc)),
//          NULL);
//      }
//      g_list_free (ind_list);

    }
    g_list_free (axis_list);
  }

  /*
  if (indicator_fc >= G_MAXINT * 0.5)
    indicator_fc = 0;
  else
    indicator_fc++;
*/

  indicator_fc++;

  return TRUE;
}

void
gdv_viewer_app_window_init (GdvViewerAppWindow *window)
{
  GtkListStore *new_list;
  GList * axis_list;
//  gint i;
//  GtkTreePath *path;
//  GtkTreeIter iter;

  g_type_ensure (gdv_special_drum_display_get_type ());

  gtk_widget_init_template (GTK_WIDGET (window));
  window->priv = gdv_viewer_app_window_get_instance_private (window);

  g_object_set (window->priv->main_layer,
                 "center-value", 0.0,
                "left-right-range", 50.0,
                //"center-value", 0.0,
                NULL);

  axis_list = gdv_layer_get_axis_list (window->priv->main_layer);
  if (axis_list)
  {
    g_object_set (axis_list->data,
              "force-beg-end", TRUE,
              "axis-beg-at-screen-x", 0.5,
              "axis-end-at-screen-x", 0.5,
              "axis-beg-at-screen-y", 0.0,
              "axis-end-at-screen-y", 1.0,
              "scale-auto-increment", FALSE,
                  NULL);
  }

  gtk_widget_show_all(GTK_WIDGET(window->priv->main_layer));

//  g_object_set (window->priv->main_layer,
//                "fill", TRUE,
//                "expand", TRUE,
//                NULL);

//  gtk_container_child_set_property

//  g_object_set(window, "show-menubar", TRUE, NULL);

//  timeout_cb_oned(window->priv->main_layer);
  //g_timeout_add (20, ((GSourceFunc) timeout_cb_oned), window->priv->main_layer);

}

GdvViewerAppWindow *
gdv_viewer_app_window_new (GdvViewerApp *app)
{
  return g_object_new (GDV_VIEWER_APP_TYPE_WINDOW, "application", app, NULL);
}

void gdv_viewer_app_window_open (GdvViewerAppWindow *win,
                                 GFile            *file)
{
  GdvViewerAppWindowPrivate *priv;

  /* FIXME: Make the actual file-loading in this function and not in selection-changed */

  priv = gdv_viewer_app_window_get_instance_private (win);

 /* GFileMonitor * mon =
    g_file_monitor_file (file,
                         GFileMonitorFlags flags,
                         GCancellable *cancellable,
                         GError **error);
*/
  g_print("File: %s\n", g_file_get_path(file));
/*
  {
    GList *menus = gtk_menu_get_for_attach_widget(win);

    if (menus != NULL)
      g_print("Reached\n");
  }
*/
}