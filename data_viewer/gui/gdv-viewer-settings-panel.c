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

/* Define Properties */
enum
{
  PROP_0,

  PROP_CURRENT_FILE,

  N_PROPERTIES
};

struct _GdvViewerSettingsPanelClass
{
  GtkBoxClass parent;
};

typedef struct _GdvViewerSettingsPanelPrivate GdvViewerSettingsPanelPrivate;

struct _GdvViewerSettingsPanelPrivate
{
  GtkWidget *xmpl;

  ViewerFile *current_file;
  GtkFrame   *file_frame;
};

static GParamSpec *properties [N_PROPERTIES];

G_DEFINE_TYPE_WITH_PRIVATE (GdvViewerSettingsPanel, gdv_viewer_settings_panel, GTK_TYPE_BOX);

static void
gdv_viewer_settings_panel_init (GdvViewerSettingsPanel *self)
{
  GdvViewerSettingsPanelPrivate *priv;

  gtk_widget_init_template (GTK_WIDGET (self));
  priv = gdv_viewer_settings_panel_get_instance_private (self);

  priv->current_file = NULL;
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
gdv_viewer_settings_panel_get_property (GObject    *object,
                                        guint       prop_id,
                                        GValue     *value,
                                        GParamSpec *pspec)
{
  GdvViewerSettingsPanel *self = (GdvViewerSettingsPanel *)object;
  GdvViewerSettingsPanelPrivate *priv;

  priv = gdv_viewer_settings_panel_get_instance_private (self);

  switch (prop_id)
    {
    case PROP_CURRENT_FILE:
      g_value_set_object (value, priv->current_file);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
_set_file (GdvViewerSettingsPanel *self,
           ViewerFile             *file)
{
  GdvViewerSettingsPanelPrivate *priv;

  priv = gdv_viewer_settings_panel_get_instance_private (self);

  priv->current_file = file;

  if (file)
  {
    gchar *file_name;
    GFile *curr_g_file;

    g_object_get (file, "file", &curr_g_file, NULL);
    if (curr_g_file)
    {
      file_name = g_file_get_basename (curr_g_file);

      gtk_frame_set_label (priv->file_frame, file_name);
      g_object_set (G_OBJECT (priv->file_frame), "visible", TRUE, NULL);
      return;
    }
  }

  g_object_set (G_OBJECT (priv->file_frame), "visible", FALSE, NULL);
}

static void
gdv_viewer_settings_panel_set_property (GObject      *object,
                                        guint         prop_id,
                                        const GValue *value,
                                        GParamSpec   *pspec)
{
  GdvViewerSettingsPanel *self = (GdvViewerSettingsPanel *)object;

  switch (prop_id)
    {
    case PROP_CURRENT_FILE:
      _set_file(self, g_value_get_object (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gdv_viewer_settings_panel_class_init (GdvViewerSettingsPanelClass *klass)
{
  GObjectClass *object_class;

  object_class = G_OBJECT_CLASS (klass);

  gtk_widget_class_set_template_from_resource (
    GTK_WIDGET_CLASS (klass),
    "/net/gdv/viewerapp/ui/gui/gdv-viewer-settings-panel.ui");

  object_class->get_property = gdv_viewer_settings_panel_get_property;
  object_class->set_property = gdv_viewer_settings_panel_set_property;

//  gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (class),
//                                               "/org/gtk/exampleapp/window.ui");
//  gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (class), ExampleAppWindow, stack);

  properties [PROP_CURRENT_FILE] =
    g_param_spec_object ("file",
                         "File",
                         "The current file",
                         VIEWER_TYPE_FILE,
                         G_PARAM_READWRITE);

  g_object_class_install_properties (object_class, N_PROPERTIES, properties);

  gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (object_class),
                                                GdvViewerSettingsPanel,
                                                file_frame);
}

GdvViewerSettingsPanel *gdv_viewer_settings_panel_new (void)
{
  return g_object_new (gdv_viewer_settings_panel_get_type(), NULL);
}

