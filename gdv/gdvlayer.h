/*
 * gdvlayer.h
 * This file is part of gdv
 *
 * Copyright (C) 2013 - Emanuel Schmidt
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef GDV_LAYER_H_INCLUDED
#define GDV_LAYER_H_INCLUDED

#include <stdlib.h>
#include <gtk/gtk.h>

#include <gdv/gdvcentral.h>
#include <gdv/gdvlayercontent.h>
#include <gdv/gdvaxis.h>
#include <gdv/gdvtic.h>

G_BEGIN_DECLS

#define GDV_TYPE_LAYER\
  (gdv_layer_get_type ())
#define GDV_LAYER(obj)\
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), GDV_TYPE_LAYER, GdvLayer))
#define GDV_IS_LAYER(obj)\
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_TYPE_LAYER))
#define GDV_LAYER_CLASS(klass)\
  (G_TYPE_CHECK_CLASS_CAST ((klass), GDV_TYPE_LAYER, GdvLayerClass))
#define GDV_LAYER_IS_CLASS(klass)\
  (G_TYPE_CHECK_CLASS_TYPE ((klass), GDV_TYPE_LAYER))
#define GDV_LAYER_GET_CLASS(obj)\
  (G_TYPE_INSTANCE_GET_CLASS ((obj), GDV_TYPE_LAYER, GdvLayerClass))

typedef struct _GdvLayer GdvLayer;
typedef struct _GdvLayerClass GdvLayerClass;
typedef struct _GdvLayerPrivate GdvLayerPrivate;

struct _GdvLayer
{
  GtkOverlay parent;

  /*< private > */
  GdvLayerPrivate *priv;
};

/**
 * GdvLayerClass:
 * @parent_class: The layer class structure is derived from the common #GtkOverlayClass.
 * @evaluate_point: The method used to derive the position on screen during drawing.
 *   This signal should be implemented in order to plot #GdvLayerContent correctly.
 * @eval_inner_point: Method to evaluate if a point on screen is more central
 *   with respect to the layer than another one. This is used by #GdvTic to know where
 *   to plot the outside- and inside-length.
 */
struct _GdvLayerClass
{
  GtkOverlayClass parent_class;

  /*< public >*/
  gboolean (*evaluate_point)       (GdvLayer *layer,
                                    gdouble             x_value,
                                    gdouble             y_value,
                                    gdouble             z_value,
                                    gdouble            *pos_x,
                                    gdouble            *pos_y);

  gboolean (*eval_inner_point)     (GdvLayer      *layer,
                                    guint point_1_x, guint point_1_y,
                                    guint point_2_x, guint point_2_y);

  /*< private >*/

  /* Padding to allow adding up to 12 new virtual functions without
   * breaking ABI. */
  gpointer _gdv_reserve[12];
};

/* Public Method definitions. */
GType gdv_layer_get_type (void);

GdvLayer *gdv_layer_new (void);

gboolean gdv_layer_has_layer_content (GdvLayer *layer, GdvLayerContent *layer_content);

gboolean gdv_layer_evaluate_data_point(GdvLayer *layer,
                                       gdouble data_point_x_value,
                                       gdouble data_point_y_value,
                                       gdouble data_point_z_value,
                                       gdouble *pos_x,
                                       gdouble *pos_y);

GList *gdv_layer_get_content_list (GdvLayer *layer);

GList *gdv_layer_get_axis_list (GdvLayer *layer);

G_END_DECLS

#endif /* GDV_LAYER_H_INCLUDED */
