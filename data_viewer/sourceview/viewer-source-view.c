/*
 * viewer-source-view.c
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

#include "sourceview/viewer-source-view.h"
#include "file/viewer-file.h"

//struct _ViewerSourceView
//{
//  GtkSourceView parent;
//};

/* Define Properties */
/*enum
{
  PROP_0,

  PROP_CURRENT_FILE,

  N_PROPERTIES
};*/

struct _ViewerSourceViewClass
{
  GtkSourceViewClass parent;
};

typedef struct _ViewerSourceViewPrivate ViewerSourceViewPrivate;

struct _ViewerSourceViewPrivate
{
  GtkWidget *xmpl;
  ViewerFile *file;
};

//static GParamSpec *properties [N_PROPERTIES];

G_DEFINE_TYPE_WITH_PRIVATE (ViewerSourceView,
                            viewer_source_view,
                            GTK_SOURCE_TYPE_VIEW);

static void
on_buffer_changed (ViewerSourceView *view)
{
  GtkSourceBuffer *buffer;
//  GtkTextTagTable *tag_table;
//  GtkTextTag *tag;

  g_object_get (view, "buffer", &buffer, NULL);

  gtk_source_buffer_set_highlight_syntax (buffer, TRUE);

//  tag_table = gtk_text_buffer_get_tag_table (GTK_TEXT_BUFFER (buffer));
//  tag = gtk_text_tag_table_lookup (tag_table, "gtksourceview:context-classes:string");
}

static void
viewer_source_view_init (ViewerSourceView *view)
{
  ViewerSourceViewPrivate *priv;

  priv = viewer_source_view_get_instance_private (view);

  priv->file = NULL;
  g_signal_connect(view, "notify::buffer", (GCallback) on_buffer_changed, NULL);

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
/*
static void
viewer_source_view_get_property (GObject    *object,
                                 guint       prop_id,
                                 GValue     *value,
                                 GParamSpec *pspec)
{
  ViewerSourceView *self = (ViewerSourceView *)object;
  ViewerSourceViewPrivate *priv;

  priv = viewer_source_view_get_instance_private (self);

  switch (prop_id)
    {
    case PROP_CURRENT_FILE:
      g_value_set_object (value, priv->file);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
viewer_source_view_set_property (GObject      *object,
                                 guint         prop_id,
                                 const GValue *value,
                                 GParamSpec   *pspec)
{
  ViewerSourceView *self = (ViewerSourceView *)object;
  ViewerSourceViewPrivate *priv;

  priv = viewer_source_view_get_instance_private (self);

  switch (prop_id)
    {
    case PROP_CURRENT_FILE:
      priv->file = g_value_get_object (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}
*/
static void
viewer_source_view_class_init (ViewerSourceViewClass *klass)
{
//  GObjectClass *object_class;

//  object_class = G_OBJECT_CLASS (klass);

//  gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (class),
//                                               "/org/gtk/exampleapp/window.ui");
//  gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (class), ExampleAppWindow, stack);
//  object_class->get_property = viewer_source_view_get_property;
//  object_class->set_property = viewer_source_view_set_property;

//  gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (class),
//                                               "/org/gtk/exampleapp/window.ui");
//  gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (class), ExampleAppWindow, stack);

/*

  properties [PROP_CURRENT_FILE] =
    g_param_spec_object ("file",
                         "File",
                         "The current file",
                         VIEWER_TYPE_FILE,
                         G_PARAM_READWRITE);

  g_object_class_install_properties (object_class, N_PROPERTIES, properties);
*/
}

ViewerSourceView *viewer_source_view_new (void)
{
  return g_object_new (viewer_source_view_get_type(), NULL);
}

