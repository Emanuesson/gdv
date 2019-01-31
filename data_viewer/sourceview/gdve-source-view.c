/*
 * gdve-source-view.c
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

#include <cairo-gobject.h>
//#include <dazzle.h>
#include <gtk/gtk.h>

#include "sourceview/gdve-source-view.h"
#include "sourceview/gdve-column-delimiter.h"
#include "file/viewer-file.h"
//#include "ide.h"

//struct _GdveSourceView
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

struct _GdveSourceViewClass
{
  GtkSourceViewClass parent;
};

typedef struct _GdveSourceViewPrivate GdveSourceViewPrivate;

struct _GdveSourceViewPrivate
{
  GtkWidget *xmpl;
  ViewerFile *file;
};

//static GParamSpec *properties [N_PROPERTIES];

G_DEFINE_TYPE_WITH_PRIVATE (GdveSourceView,
                            gdve_source_view,
                            GTK_SOURCE_TYPE_VIEW);

static inline void
draw_column_separator (GdveSourceView *self,
                       GtkTextViewLayer  layer,
                       cairo_t          *cr,
                       guint cursor)
{

}

static void
on_buffer_changed (GdveSourceView *view)
{
  GtkSourceBuffer *buffer;
//  GtkTextTagTable *tag_table;
//  GtkTextTag *tag;

  g_object_get (view, "buffer", &buffer, NULL);

  gtk_source_buffer_set_highlight_syntax (buffer, TRUE);

//  tag_table = gtk_text_buffer_get_tag_table (GTK_TEXT_BUFFER (buffer));
//  tag = gtk_text_tag_table_lookup (tag_table, "gtksourceview:context-classes:string");
}

/*
  GtkTextBuffer *buffer;
  GtkTextView *text_view = (GtkTextView *)self;
  GtkTextIter begin;
  GtkTextIter end;
  GtkTextMark *mark_begin;
  GtkTextMark *mark_end;
  GdkRectangle r;

  g_assert (GTK_IS_TEXT_VIEW (text_view));
  g_assert (IDE_IS_SOURCE_VIEW (self));
  g_assert (cr);
  g_assert (IDE_IS_SNIPPET (snippet));

  buffer = gtk_text_view_get_buffer (text_view);

  mark_begin = ide_snippet_get_mark_begin (snippet);
  mark_end = ide_snippet_get_mark_end (snippet);

  if (!mark_begin || !mark_end)
    return;

  gtk_text_buffer_get_iter_at_mark (buffer, &begin, mark_begin);
  gtk_text_buffer_get_iter_at_mark (buffer, &end, mark_end);

  get_rect_for_iters (text_view, &begin, &end, &r, GTK_TEXT_WINDOW_TEXT);

  gtk_text_view_window_to_buffer_coords (text_view, GTK_TEXT_WINDOW_TEXT, r.x, r.y, &r.x, &r.y);

  dzl_cairo_rounded_rectangle (cr, &r, 5, 5);

  cairo_fill (cr);
}
*/

static void
data_source_view_draw_layer (GtkTextView      *text_view,
                             GtkTextViewLayer  layer,
                             cairo_t          *cr)
{
  GdkWindow *window;
  gint width;
  GdveSourceView *self = (GdveSourceView *)text_view;
  GdveSourceViewPrivate *priv = gdve_source_view_get_instance_private (self);
  GdkRectangle r;

  g_assert (GTK_IS_TEXT_VIEW (text_view));
  g_assert (GDVE_IS_SOURCE_VIEW (self));
  g_assert (cr);

  GTK_TEXT_VIEW_CLASS (gdve_source_view_parent_class)->draw_layer (text_view,
                                                                     layer,
                                                                     cr);

  window = gtk_text_view_get_window (text_view, GTK_TEXT_WINDOW_TEXT);
  width = gdk_window_get_width (window);

  draw_column_separator (GDVE_SOURCE_VIEW(text_view), layer, cr, 9);

//  gdk_cairo_set_source_rgba (cr, &priv->snippet_area_background_rgba);
//  cairo_save (cr);
/*


  for (guint i = 0; i < priv->snippets->length; i++)
    {
      IdeSnippet *snippet = g_queue_peek_nth (priv->snippets, i);

      ide_source_view_draw_snippet_background (self,
                                               cr,
                                               snippet,
                                               width - ((priv->snippets->length - i) * 10));
    }

  cairo_restore (cr);
*/
}

/*
static void
data_source_view_draw_column_separator (GtkSourceView *view,
                                        cairo_t       *cr)
{
  GdkRectangle clip;
  gdouble x;

  GtkTextView *text_view = GTK_TEXT_VIEW (view);

  #ifdef ENABLE_PROFILE
  static GTimer *timer = NULL;

  if (timer == NULL)
  {
  timer = g_timer_new ();
  }

  g_timer_start (timer);
  #endif

  g_return_if_fail (view->priv->right_margin_line_color != NULL);

  if (!gdk_cairo_get_clip_rectangle (cr, &clip))
  {
  return;
  }

  if (view->priv->cached_right_margin_pos < 0)
  {
  view->priv->cached_right_margin_pos =
  calculate_real_tab_width (view,
  view->priv->right_margin_pos,
  '_');
  }

  x = view->priv->cached_right_margin_pos + gtk_text_view_get_left_margin (text_view);

  cairo_save (cr);
  cairo_set_line_width (cr, 1.0);

  if (x + 1 >= clip.x && x <= clip.x + clip.width)
  {
  cairo_move_to (cr, x + 0.5, clip.y);
  cairo_line_to (cr, x + 0.5, clip.y + clip.height);

  gdk_cairo_set_source_rgba (cr, view->priv->right_margin_line_color);
  cairo_stroke (cr);
  }

  *//* Only draw the overlay when the style scheme explicitly sets it. */
/*  if (view->priv->right_margin_overlay_color != NULL && clip.x + clip.width > x + 1)
  {
    *//* Draw the rectangle next to the line (x+1). */
/*    cairo_rectangle (cr,
    x + 1, clip.y,
    clip.x + clip.width - (x + 1), clip.height);

    gdk_cairo_set_source_rgba (cr, view->priv->right_margin_overlay_color);
    cairo_fill (cr);
  }

  cairo_restore (cr);
*/
/*
PROFILE ({
g_timer_stop (timer);
g_print ("    gtk_source_view_paint_right_margin time: "
"%g (sec * 1000)\n",
g_timer_elapsed (timer, NULL) * 1000);
});
 */
//}

static void
gdve_source_view_init (GdveSourceView *view)
{
  GdveSourceViewPrivate *priv;
  //IdeObject *a_single_object = g_object_new(IDE_TYPE_OBJECT, NULL);

  priv = gdve_source_view_get_instance_private (view);

  priv->file = NULL;
  g_signal_connect(view, "notify::buffer", (GCallback) on_buffer_changed, NULL);

//  g_object_unref (a_single_object);

//  gtk_source_view_set_show_grid_lines (GTK_SOURCE_VIEW (view), TRUE);
  g_object_set (view, "background-pattern", GTK_SOURCE_BACKGROUND_PATTERN_TYPE_GRID, NULL);

  {
  //  gtk_widget_init_template (GTK_WIDGET (view));
  //  GtkTextBuffer *new_buffer = gtk_text_buffer_new (NULL);
    GtkSourceBuffer *new_buffer = gtk_source_buffer_new (NULL);
  //  GtkTextTag *new_tag = gtk_text_tag_new ("New Tag");
    GtkTextIter start, end;
  //  GdkRGBA rgba;
//    GtkTextTag *tag;
    GtkSourceSpaceDrawer * spaces =
      gtk_source_view_get_space_drawer (GTK_SOURCE_VIEW(view));
    gtk_source_space_drawer_set_enable_matrix(spaces, TRUE);
    gtk_source_space_drawer_set_types_for_locations(spaces,
                                                    GTK_SOURCE_SPACE_LOCATION_ALL,
                                                    GTK_SOURCE_SPACE_TYPE_ALL);

    gtk_text_buffer_set_text (GTK_TEXT_BUFFER (new_buffer),
      "Hello my world\n"
      "Hello my other world\tsafrasfa\tfsafsa\n", -1);

    gtk_source_view_set_show_right_margin (GTK_SOURCE_VIEW(view), TRUE);
    gtk_source_view_set_right_margin_position (GTK_SOURCE_VIEW(view), 10);

/*
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
*/
    gtk_text_view_set_buffer (GTK_TEXT_VIEW (view), GTK_TEXT_BUFFER (new_buffer));
  }
}
/*
static void
gdve_source_view_get_property (GObject    *object,
                                 guint       prop_id,
                                 GValue     *value,
                                 GParamSpec *pspec)
{
  GdveSourceView *self = (GdveSourceView *)object;
  GdveSourceViewPrivate *priv;

  priv = gdve_source_view_get_instance_private (self);

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
gdve_source_view_set_property (GObject      *object,
                                 guint         prop_id,
                                 const GValue *value,
                                 GParamSpec   *pspec)
{
  GdveSourceView *self = (GdveSourceView *)object;
  GdveSourceViewPrivate *priv;

  priv = gdve_source_view_get_instance_private (self);

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
gdve_source_view_class_init (GdveSourceViewClass *klass)
{
  GtkTextViewClass *text_view_class = GTK_TEXT_VIEW_CLASS(klass);

  text_view_class->draw_layer = data_source_view_draw_layer;

//  GObjectClass *object_class;

//  object_class = G_OBJECT_CLASS (klass);

//  gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (class),
//                                               "/org/gtk/exampleapp/window.ui");
//  gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (class), ExampleAppWindow, stack);
//  object_class->get_property = gdve_source_view_get_property;
//  object_class->set_property = gdve_source_view_set_property;

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

GdveSourceView *gdve_source_view_new (void)
{
  return g_object_new (gdve_source_view_get_type(), NULL);
}

