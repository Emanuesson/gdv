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

#include "gui/viewer-app-win.h"
#include "sourceview/viewer-source-view.h"

/* Define Properties */
enum
{
  PROP_0,

  N_PROPERTIES
};

struct _GdvViewerAppWindowPrivate
{
  gint             placeholder;

  GdvLayerContent *content;
  GdvTwodLayer    *main_layer;
  GtkTreeView     *file_list;
  ViewerSourceView *file_view;
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
on_treeview_selection_changed (GtkTreeSelection *treeselection,
                               gpointer          user_data)
{
  g_print ("hello world\n");
}

static void
gdv_viewer_app_window_class_init (GdvViewerAppWindowClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->dispose = gdv_viewer_app_window_dispose;
  object_class->finalize = gdv_viewer_app_window_finalize;

  g_type_ensure (VIEWER_TYPE_SOURCE_VIEW);

  gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (object_class),
                                               "/net/gdv/viewerapp/ui/gui/viewer-app-win.ui");

  gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (object_class), GdvViewerAppWindow, main_layer);
  gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (object_class), GdvViewerAppWindow, file_list);
  gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (object_class), GdvViewerAppWindow, file_view);
  gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (object_class), on_treeview_selection_changed);
}

enum
{
  COLUMN_STRING,
  COLUMN_INT,
  COLUMN_BOOLEAN,
  N_COLUMNS
};

void
gdv_viewer_app_window_init (GdvViewerAppWindow *window)
{
  GtkListStore *new_list;
  gint i;
  GtkTreePath *path;
  GtkTreeIter iter;

  g_type_ensure (gdv_twod_layer_get_type ());

  gtk_widget_init_template (GTK_WIDGET (window));
  window->priv = gdv_viewer_app_window_get_instance_private (window);

//  g_object_set (window->priv->main_layer,
//                "fill", TRUE,
//                "expand", TRUE,
//                NULL);

//  gtk_container_child_set_property

  new_list = gtk_list_store_new (N_COLUMNS,
                                 G_TYPE_STRING,
                                 G_TYPE_INT,
                                 G_TYPE_BOOLEAN);

  for (i = 0; i < 10; i++)
    {
      gchar *some_data;

      some_data = g_strdup_printf ("Helllo: %d", i);

      // Add a new row to the model
      gtk_list_store_append (new_list, &iter);
      gtk_list_store_set (new_list, &iter,
                          COLUMN_STRING, some_data,
                          COLUMN_INT, i,
                          COLUMN_BOOLEAN,  FALSE,
                          -1);

      // As the store will keep a copy of the string internally,
      // we free some_data.
      g_free (some_data);
    }

  // Modify a particular row
  path = gtk_tree_path_new_from_string ("4");
  gtk_tree_model_get_iter (GTK_TREE_MODEL (new_list),
                           &iter,
                           path);
  gtk_tree_path_free (path);
  gtk_list_store_set (new_list, &iter,
                      COLUMN_BOOLEAN, TRUE,
                      -1);

  g_object_set (window->priv->file_list,
                "model", new_list,
                NULL);
}

GdvViewerAppWindow *
gdv_viewer_app_window_new (GdvViewerApp *app)
{
  return g_object_new (GDV_VIEWER_APP_TYPE_WINDOW, "application", app, NULL);
}
