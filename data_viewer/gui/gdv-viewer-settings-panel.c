/*
 * gdv-viewer-settings-panel.c
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

#include <gtk/gtk.h>

#include "gui/gdv-viewer-settings-panel.h"
#include "file/viewer-file.h"

//struct _ViewerSourceView
//{
//  GtkSourceView parent;
//};

struct _GdvViewerSettingsPanelClass
{
  GtkBoxClass parent;
};

typedef struct _GdvViewerSettingsPanelPrivate GdvViewerSettingsPanelPrivate;

struct _GdvViewerSettingsPanelPrivate
{
  GtkWidget *xmpl;

  ViewerFile *set_for_file;
};

G_DEFINE_TYPE_WITH_PRIVATE (GdvViewerSettingsPanel, gdv_viewer_settings_panel, GTK_TYPE_BOX);

static void
gdv_viewer_settings_panel_init (GdvViewerSettingsPanel *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
//  window->priv = gdv_viewer_app_window_get_instance_private (window);


/*
//  gtk_widget_init_template (GTK_WIDGET (view));
//  GtkTextBuffer *new_buffer = gtk_text_buffer_new (NULL);
  GtkSourceBuffer *new_buffer = gtk_source_buffer_new (NULL);
//  GtkTextTag *new_tag = gtk_text_tag_new ("New Tag");
  GtkTextIter start, end;
//  GdkRGBA rgba;
  GtkTextTag *tag;
*/

/*

  gtk_text_buffer_set_text (GTK_TEXT_BUFFER (new_buffer),
    "Hello my world\n"
    "Hello my other world\tsafrasfa\tfsafsa\n", -1);

  tag = gtk_text_buffer_create_tag (GTK_TEXT_BUFFER (new_buffer),
                                    "blue_foreground",
                                    "foreground", "blue", NULL);
  gtk_text_buffer_get_iter_at_offset (GTK_TEXT_BUFFER (new_buffer), &start, 7);
  gtk_text_buffer_get_iter_at_offset (GTK_TEXT_BUFFER (new_buffer), &end, 12);
  gtk_text_buffer_apply_tag (GTK_TEXT_BUFFER (new_buffer), tag, &start, &end);
  tag = gtk_text_buffer_create_tag (GTK_TEXT_BUFFER (new_buffer),
                                    "blackbackground",
                                    "background", "black", NULL);
  gtk_text_buffer_apply_tag (GTK_TEXT_BUFFER (new_buffer), tag, &start, &end);

  gtk_text_view_set_buffer (GTK_TEXT_VIEW (view), GTK_TEXT_BUFFER (new_buffer));
*/
}

static void
gdv_viewer_settings_panel_class_init (GdvViewerSettingsPanelClass *klass)
{

  gtk_widget_class_set_template_from_resource (
    GTK_WIDGET_CLASS (klass),
    "/net/gdv/viewerapp/ui/gui/gdv-viewer-settings-panel.ui");

//  gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (class),
//                                               "/org/gtk/exampleapp/window.ui");
//  gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (class), ExampleAppWindow, stack);
}

GdvViewerSettingsPanel *gdv_viewer_settings_panel_new (void)
{
  return g_object_new (gdv_viewer_settings_panel_get_type(), NULL);
}

