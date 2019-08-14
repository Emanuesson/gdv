/*
 * gdvhair.h
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

#ifndef GDV_HAIR_H_INCLUDED
#define GDV_HAIR_H_INCLUDED

#include <stdlib.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

/* TODO: rename this widget into "marker" */

#define GDV_TYPE_HAIR\
  (gdv_hair_get_type ())
#define GDV_HAIR(obj)\
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), GDV_TYPE_HAIR, GdvHair))
#define GDV_IS_HAIR(obj)\
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_TYPE_HAIR))
#define GDV_HAIR_CLASS(klass)\
  (G_TYPE_CHECK_CLASS_CAST ((klass), GDV_TYPE_HAIR, GdvHairClass))
#define GDV_HAIR_IS_CLASS(klass)\
  (G_TYPE_CHECK_CLASS_TYPE ((klass), GDV_TYPE_HAIR))
#define GDV_HAIR_GET_CLASS(obj)\
  (G_TYPE_INSTANCE_GET_CLASS ((obj), GDV_TYPE_HAIR, GdvHairClass))

typedef struct _GdvHair GdvHair;
typedef struct _GdvHairClass GdvHairClass;
typedef struct _GdvHairPrivate GdvHairPrivate;

struct _GdvHair
{
  GtkContainer parent;

  /*< private >*/
  GdvHairPrivate *priv;
};

/**
 * GdvHairClass:
 * @parent_class: The hair class structure is derived from the common
 *   #GtkContainer.
 */
struct _GdvHairClass
{
  GtkContainerClass parent_class;

  /*< private >*/
  /* Padding to allow adding up to 12 new virtual functions without
   * breaking ABI. */
  gpointer _gdv_reserve[12];
};

/* Public Method definitions. */
GType gdv_hair_get_type (void);

GdvHair *gdv_hair_new (void);

G_END_DECLS

#endif /* GDV_HAIR_H_INCLUDED */
