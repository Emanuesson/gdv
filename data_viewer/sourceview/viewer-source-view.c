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

//struct _ViewerSourceView
//{
//  GtkSourceView parent;
//};

struct _ViewerSourceViewClass
{
  GtkSourceViewClass parent;
};

typedef struct _ViewerSourceViewPrivate ViewerSourceViewPrivate;

struct _ViewerSourceViewPrivate
{
  GtkWidget *xmpl;
};

G_DEFINE_TYPE_WITH_PRIVATE (ViewerSourceView, viewer_source_view, GTK_SOURCE_TYPE_VIEW);

static void
viewer_source_view_init (ViewerSourceView *view)
{
//  gtk_widget_init_template (GTK_WIDGET (view));
//  GtkTextBuffer *new_buffer = gtk_text_buffer_new (NULL);
  GtkSourceBuffer *new_buffer = gtk_source_buffer_new (NULL);
//  GtkTextTag *new_tag = gtk_text_tag_new ("New Tag");
  GtkTextIter start, end;
//  GdkRGBA rgba;
  GtkTextTag *tag;

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
}

static void
viewer_source_view_class_init (ViewerSourceViewClass *class)
{
//  gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (class),
//                                               "/org/gtk/exampleapp/window.ui");
//  gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (class), ExampleAppWindow, stack);
}

ViewerSourceView *viewer_source_view_new (void)
{
  return g_object_new (viewer_source_view_get_type(), NULL);
}

