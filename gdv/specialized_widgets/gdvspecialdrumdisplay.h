/*
 * gdvspecialdrumdisplay.h
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

#ifndef GDV_SPECIAL_DRUM_DISPLAY_H_INCLUDED
#define GDV_SPECIAL_DRUM_DISPLAY_H_INCLUDED

#include <stdlib.h>
#include <gtk/gtk.h>

#include "gdvonedlayer.h"

G_BEGIN_DECLS


#define GDV_TYPE_SPECIAL_DRUM_DISPLAY\
  (gdv_special_drum_display_get_type ())
#define GDV_SPECIAL_DRUM_DISPLAY(obj)\
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), GDV_TYPE_SPECIAL_DRUM_DISPLAY, GdvSpecialDrumDisplay))
#define GDV_SPECIAL_DRUM_IS_DISPLAY(obj)\
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_TYPE_SPECIAL_DRUM_DISPLAY))
#define GDV_SPECIAL_DRUM_DISPLAY_CLASS(klass)\
  (G_TYPE_CHECK_CLASS_CAST ((klass), GDV_TYPE_SPECIAL_DRUM_DISPLAY, GdvSpecialDrumDisplay))
#define GDV_SPECIAL_DRUM_DISPLAY_IS_CLASS(klass)\
  (G_TYPE_CHECK_CLASS_TYPE ((klass), GDV_TYPE_SPECIAL_DRUM_DISPLAY))
#define GDV_SPECIAL_DRUM_DISPLAY_GET_CLASS(obj)\
  (G_TYPE_INSTANCE_GET_CLASS ((obj), GDV_TYPE_SPECIAL_DRUM_DISPLAY, GdvSpecialDrumDisplay))

typedef struct _GdvSpecialDrumDisplay GdvSpecialDrumDisplay;
typedef struct _GdvSpecialDrumDisplayClass GdvSpecialDrumDisplayClass;
typedef struct _GdvSpecialDrumDisplayPrivate GdvSpecialDrumDisplayPrivate;

struct _GdvSpecialDrumDisplay
{
  GdvOnedLayer     parent_instance;

  /*< private >*/
  GdvSpecialDrumDisplayPrivate *priv;
};

struct _GdvSpecialDrumDisplayClass
{
  GdvOnedLayerClass     parent_class;

  /*< private >*/
  gpointer padding[12];
};

/* Public Method definitions. */
GType gdv_special_drum_display_get_type (void);

GdvSpecialDrumDisplay *gdv_special_drum_display_new (void);

G_END_DECLS

#endif /* GDV_SPECIAL_DRUM_DISPLAY_H_INCLUDED */
