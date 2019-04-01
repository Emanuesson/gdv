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

#include <gtksourceview/gtksource.h>
//#include <gtksourceview/gtksourcetypes.h>
#include <gio/gio.h>

#include "gui/viewer-app-win.h"
#include "gui/gdv-viewer-settings-panel.h"
#include "file/viewer-file.h"
#include "sourceview/gdve-source-view.h"

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
  GdvTwodLayer    *main_layer;
  GtkTreeView     *file_view;
  GtkScrolledWindow *file_view_window;
  GdveSourceView *file_content;
  GtkScrolledWindow *file_content_window;

  GdvViewerSettingsPanel *setting_panel;

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
_progress_file_load_cb (goffset            current_num_bytes,
                        goffset            full_num_bytes,
                        GdveSourceView  *view)
{
  g_log (NULL, G_LOG_LEVEL_DEBUG,
         "%ld/%ld\n", current_num_bytes, full_num_bytes);
}

static void
_async_file_loaded (GtkSourceFileLoader *source_loader,
                    GAsyncResult        *res,
                    GdveSourceView    *view)
{
  GtkSourceBuffer *buffer;

  buffer = gtk_source_file_loader_get_buffer(source_loader);

  if (gtk_source_file_loader_load_finish (source_loader, res, NULL))
    gtk_text_view_set_buffer (GTK_TEXT_VIEW (view), GTK_TEXT_BUFFER (buffer));
}

static void
on_treeview_selection_changed (GtkTreeSelection *tree_selection,
                               gpointer          user_data)
{
  GList *selected_rows;
  GtkTreeModel *current_model;
  GdvViewerAppWindow *window = user_data;
  GdvViewerAppWindowPrivate *priv;
  GtkTreeIter iter;
  GtkTreePath *path;
  ViewerFile *file;
  GtkSourceFile *source_file;
  GtkSourceFileLoader *source_loader;
  GtkSourceBuffer *buffer;

  priv = gdv_viewer_app_window_get_instance_private (window);

  current_model = gtk_tree_view_get_model (priv->file_view);
  selected_rows = gtk_tree_selection_get_selected_rows (tree_selection, &current_model);
  if (!selected_rows)
    return;

  path = selected_rows->data;

  if (!gtk_tree_model_get_iter (current_model, &iter, path))
    return;

   gtk_tree_model_get (current_model, &iter,
                       COLUMN_FILE, &file,
                       -1);

  g_object_set (priv->setting_panel, "file", file, NULL);

  source_file = _viewer_file_get_source_file (file);
  buffer = gtk_source_buffer_new (NULL);
  source_loader = gtk_source_file_loader_new (buffer, source_file);

  gtk_source_file_loader_load_async (source_loader,
                                     G_PRIORITY_HIGH,
                                     NULL,
                                     (GFileProgressCallback) _progress_file_load_cb,
                                     priv->file_content,
                                     NULL,
                                     (GAsyncReadyCallback) _async_file_loaded,
                                     priv->file_content);

  if (file)
    g_log (NULL, G_LOG_LEVEL_DEBUG,
           "Here: %s\n", g_type_name (G_OBJECT_TYPE (file)));
}


static void
bottom_panel_toggled (GtkToggleButton *togglebutton,
                      gpointer         user_data)
{
  gboolean button_status;
  GdvViewerAppWindow *window = GDV_VIEWER_APP_WINDOW (user_data);
  GdvViewerAppWindowPrivate *priv;

  g_object_get (togglebutton, "active", &button_status, NULL);

  priv = gdv_viewer_app_window_get_instance_private (window);

  g_object_set (priv->file_content, "visible", button_status, NULL);
  g_object_set (priv->file_content_window, "visible", button_status, NULL);
}

static void
left_panel_toggled (GtkToggleButton *togglebutton,
                      gpointer         user_data)
{
  gboolean button_status;
  GdvViewerAppWindow *window = GDV_VIEWER_APP_WINDOW (user_data);
  GdvViewerAppWindowPrivate *priv;

  g_object_get (togglebutton, "active", &button_status, NULL);

  priv = gdv_viewer_app_window_get_instance_private (window);

  g_object_set (priv->file_view, "visible", button_status, NULL);
  g_object_set (priv->file_view_window, "visible", button_status, NULL);
}

static void
right_panel_toggled (GtkToggleButton *togglebutton,
                     gpointer         user_data)
{
  gboolean button_status;
  GdvViewerAppWindow *window = GDV_VIEWER_APP_WINDOW (user_data);
  GdvViewerAppWindowPrivate *priv;

  g_object_get (togglebutton, "active", &button_status, NULL);

  priv = gdv_viewer_app_window_get_instance_private (window);

  g_object_set (priv->setting_panel, "visible", button_status, NULL);
//  g_object_set (priv->file_view_window, "visible", button_status, NULL);
}

static void
gdv_viewer_app_window_class_init (GdvViewerAppWindowClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->dispose = gdv_viewer_app_window_dispose;
  object_class->finalize = gdv_viewer_app_window_finalize;

  g_type_ensure (GDVE_TYPE_SOURCE_VIEW);

  gtk_widget_class_set_template_from_resource (
    GTK_WIDGET_CLASS (object_class),
    "/net/gdv/viewerapp/ui/gui/viewer-app-win.ui");

  gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (object_class),
                                                GdvViewerAppWindow,
                                                main_layer);
  gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (object_class),
                                                GdvViewerAppWindow,
                                                file_view);
  gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (object_class),
                                                GdvViewerAppWindow,
                                                file_view_window);
  gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (object_class),
                                                GdvViewerAppWindow,
                                                file_content);
  gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (object_class),
                                                GdvViewerAppWindow,
                                                file_content_window);
  gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (object_class),
                                                GdvViewerAppWindow,
                                                setting_panel);
  gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (object_class),
                                           bottom_panel_toggled);
  gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (object_class),
                                           left_panel_toggled);
  gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (object_class),
                                           right_panel_toggled);
  gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (object_class),
                                           on_treeview_selection_changed);
}

void
gdv_viewer_app_window_init (GdvViewerAppWindow *window)
{
  GtkListStore *new_list;
//  gint i;
//  GtkTreePath *path;
//  GtkTreeIter iter;

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
                                 VIEWER_TYPE_FILE,
//                                 G_TYPE_FILE,
                                 G_TYPE_BOOLEAN,
                                 GTK_SOURCE_TYPE_BUFFER);


  // Modify a particular row
//  path = gtk_tree_path_new_from_string ("4");
//  gtk_tree_model_get_iter (GTK_TREE_MODEL (new_list),
//                           &iter,
//                           path);
//  gtk_tree_path_free (path);
//  gtk_list_store_set (new_list, &iter,
//                      COLUMN_BOOLEAN, TRUE,
//                      -1);

  g_object_set (window->priv->file_view,
                "model", new_list,
                NULL);
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
  GtkListStore *new_list;
  gchar *file_name;
  GtkTreeIter iter;
  ViewerFile *new_viewer_file;
  GtkTreeSelection *selection;

  /* FIXME: Make the actual file-loading in this function and not in selection-changed */

  priv = gdv_viewer_app_window_get_instance_private (win);

  g_object_get (priv->file_view,
                "model", &new_list,
                NULL);

//  new_viewer_file = viewer_file_new ();
//  g_object_set (new_viewer_file, "file", file, NULL);
  new_viewer_file = g_object_new (viewer_file_get_type(), "file", file, NULL);

  viewer_file_set_file (new_viewer_file, file);
  priv->files = g_list_append (priv->files, new_viewer_file);

  selection = gtk_tree_view_get_selection (priv->file_view);

//  some_data = g_strdup_printf ("Helllo: %d", i);
  file_name = g_file_get_basename (file);

  gtk_list_store_append (new_list, &iter);
  gtk_list_store_set (new_list, &iter,
                      COLUMN_STRING, file_name,
                      COLUMN_FILE, new_viewer_file,
                      COLUMN_BOOLEAN,  FALSE,
                      -1);

  gtk_tree_selection_select_iter (selection, &iter);

  g_free (file_name);


/*
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
*/

}