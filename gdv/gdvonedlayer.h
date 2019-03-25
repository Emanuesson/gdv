/*
 * gdvonddlayer.h
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

#ifndef GDV_ONED_LAYER_H_INCLUDED
#define GDV_ONED_LAYER_H_INCLUDED

#include <stdlib.h>
#include <gtk/gtk.h>

#include "gdvlayer.h"
#include "gdvaxis.h"

G_BEGIN_DECLS

#define GDV_ONED_LAYER_TYPE                 (gdv_oned_layer_get_type ())
#define GDV_ONED_LAYER(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), GDV_ONED_LAYER_TYPE, GdvOnedLayer))
#define GDV_ONED_IS_LAYER(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_ONED_LAYER_TYPE))
#define GDV_ONED_LAYER_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), GDV_ONED_LAYER_TYPE, GdvOnedLayerClass))
#define GDV_ONED_LAYER_IS_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), GDV_ONED_LAYER_TYPE))
#define GDV_ONED_LAYER_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), GDV_ONED_LAYER_TYPE, GdvOnedLayerClass))

typedef struct _GdvOnedLayer GdvOnedLayer;
typedef struct _GdvOnedLayerClass GdvOnedLayerClass;
typedef struct _GdvOnedLayerPrivate GdvOnedLayerPrivate;

struct _GdvOnedLayer
{
  GdvLayer parent;

  /*< private >*/
  GdvOnedLayerPrivate *priv;
};

struct _GdvOnedLayerClass
{
  GdvLayerClass parent_class;

  /*< private >*/
  /* Padding to allow adding up to 12 new virtual functions without
   * breaking ABI. */
  gpointer _gdv_reserve[12];
};

/* Public Method definitions. */
GType gdv_oned_layer_get_type (void);

GdvOnedLayer *gdv_oned_layer_new (void);

void gdv_oned_layer_replace_axis (GdvOnedLayer *layer, GdvAxis *axis);

G_END_DECLS

#endif /* GDV_ONED_LAYER_H_INCLUDED */
