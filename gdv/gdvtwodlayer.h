/*
 * gdvtwodlayer.h
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

#ifndef GDV_TWOD_LAYER_H_INCLUDED
#define GDV_TWOD_LAYER_H_INCLUDED

#include <stdlib.h>
#include <gtk/gtk.h>

#include <gdv/gdvlayer.h>
#include <gdv/gdvcentral.h>
#include <gdv/gdv-enums.h>
#include <gdv/gdvlayercontent.h>

G_BEGIN_DECLS

#define GDV_TWOD_LAYER_TYPE                 (gdv_twod_layer_get_type ())
#define GDV_TWOD_LAYER(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), GDV_TWOD_LAYER_TYPE, GdvTwodLayer))
#define GDV_TWOD_IS_LAYER(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_TWOD_LAYER_TYPE))
#define GDV_TWOD_LAYER_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), GDV_TWOD_LAYER_TYPE, GdvTwodLayerClass))
#define GDV_TWOD_LAYER_IS_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), GDV_TWOD_LAYER_TYPE))
#define GDV_TWOD_LAYER_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), GDV_TWOD_LAYER_TYPE, GdvTwodLayerClass))

typedef struct _GdvTwodLayer GdvTwodLayer;
typedef struct _GdvTwodLayerClass GdvTwodLayerClass;
typedef struct _GdvTwodLayerPrivate GdvTwodLayerPrivate;

struct _GdvTwodLayer
{
  GdvLayer parent;

  /*< private >*/
  GdvTwodLayerPrivate *priv;
};

/**
 * GdvTwodLayerClass:
 * @parent_class: The parent class.
 */
struct _GdvTwodLayerClass
{
  GdvLayerClass parent_class;

  /* Padding to allow adding up to 12 new virtual functions without
   * breaking ABI. */
  gpointer _gdv_reserve[12];
};

/* Public Method definitions. */
GType gdv_twod_layer_get_type (void);

GdvTwodLayer *gdv_twod_layer_new (void);

void gdv_twod_layer_set_xrange (GdvTwodLayer *layer, gdouble x_beg, gdouble x_end);
void gdv_twod_layer_set_yrange (GdvTwodLayer *layer, gdouble y_beg, gdouble y_end);

GdvAxis *gdv_twod_layer_get_axis (GdvTwodLayer *layer, GdvTwodAxisType axis_type);
void gdv_twod_layer_set_axis (GdvTwodLayer *layer, GdvAxis *axis, GdvTwodAxisType axis_type);
void gdv_twod_layer_unset_axis (GdvTwodLayer *layer, GdvTwodAxisType axis_type);

G_END_DECLS

#endif /* GDV_TWOD_LAYER_H_INCLUDED */
