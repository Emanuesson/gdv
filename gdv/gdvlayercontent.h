/*
 * gdvlayercontent.h
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

#ifndef GDV_LAYER_CONTENT_H_INCLUDED
#define GDV_LAYER_CONTENT_H_INCLUDED

#include <stdlib.h>
#include <gtk/gtk.h>

#include <gdv/gdvcentral.h>

G_BEGIN_DECLS

#define GDV_LAYER_TYPE_CONTENT\
  (gdv_layer_content_get_type ())
#define GDV_LAYER_CONTENT(obj)\
  (G_TYPE_CHECK_INSTANCE_CAST ((obj),\
    GDV_LAYER_TYPE_CONTENT, GdvLayerContent))
#define GDV_LAYER_IS_CONTENT(obj)\
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),\
    GDV_LAYER_TYPE_CONTENT))
#define GDV_LAYER_CONTENT_CLASS(klass)\
  (G_TYPE_CHECK_CLASS_CAST ((klass),\
    GDV_LAYER_TYPE_CONTENT, GdvLayerContentClass))
#define GDV_LAYER_CONTENT_IS_CLASS(klass)\
  (G_TYPE_CHECK_CLASS_TYPE ((klass),\
    GDV_LAYER_TYPE_CONTENT))
#define GDV_LAYER_CONTENT_GET_CLASS(obj)\
  (G_TYPE_INSTANCE_GET_CLASS ((obj),\
    GDV_LAYER_TYPE_CONTENT, GdvLayerContentClass))

typedef struct _GdvLayerContent GdvLayerContent;
typedef struct _GdvLayerContentClass GdvLayerContentClass;
typedef struct _GdvLayerContentPrivate GdvLayerContentPrivate;

struct _GdvLayerContent
{
  GtkWidget parent;

  /*< private > */
  GdvLayerContentPrivate *priv;
};

/**
 * GdvLayerContentClass:
 * @parent_class: The parent-class.
 */
struct _GdvLayerContentClass
{
  GtkWidgetClass parent_class;

  /*< private >*/
  /* Padding to allow adding up to 12 new virtual functions without
   * breaking ABI. */
  gpointer _gdv_reserve[12];
};

/* Public exported Method definitions. */
GType gdv_layer_content_get_type (void);

GdvLayerContent *gdv_layer_content_new (void);

void
gdv_layer_content_add_data_point (GdvLayerContent *layer_content,
                                  gdouble          x_value,
                                  gdouble          y_value,
                                  gdouble          z_value);

gboolean
gdv_layer_content_remove_data_point_by_index (
  GdvLayerContent *layer_content,
  gint             index);

GList *
gdv_layer_content_get_content (GdvLayerContent *content);

void
gdv_layer_content_get_min_max_x (GdvLayerContent *content,
                                 gdouble *min_x,
                                 gdouble *max_x);

void
gdv_layer_content_get_min_max_y (GdvLayerContent *content,
                                 gdouble *min_y,
                                 gdouble *max_y);
void
gdv_layer_content_get_min_max_z (GdvLayerContent *content,
                                 gdouble *min_z,
                                 gdouble *max_z);

void
gdv_layer_content_reset (GdvLayerContent *layer_content);

/*
 * Suggestions for new functions:
 *
 * Sets a single value by not touching any other value-list
 * void gdv_layer_content_set_value (GdvLayerContent *content, GdvValueType *type, gdouble value);
 *
 * Adds a new point to the content -
 *   if the value-lists are not equally ling, it adds zeros to all value-lists, that are to short
 * void gdv_layer_content_add_point (GdvLayerContent *content, GtkPackType *pack, GdvPoint *point);
 */

G_END_DECLS

#endif /* GDV_LAYER_CONTENT_H_INCLUDED */
