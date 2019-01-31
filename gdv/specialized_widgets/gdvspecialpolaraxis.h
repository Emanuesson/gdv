/*
 * gdvspecialpolaraxis.h
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

#ifndef GDV_SPECIAL_POLAR_AXIS_H_INCLUDED
#define GDV_SPECIAL_POLAR_AXIS_H_INCLUDED

#include <stdlib.h>
#include <gtk/gtk.h>

#include <gdv/gdv.h>
//#include <gtk/gtksocket.h>

G_BEGIN_DECLS

#define GDV_SPECIAL_POLAR_AXIS_TYPE                 (gdv_special_polar_axis_get_type ())
#define GDV_SPECIAL_POLAR_AXIS(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), GDV_SPECIAL_POLAR_AXIS_TYPE, GdvSpecialPolarAxis))
#define GDV_SPECIAL_POLAR_IS_AXIS(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_SPECIAL_POLAR_AXIS_TYPE))
#define GDV_SPECIAL_POLAR_AXIS_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), GDV_SPECIAL_POLAR_AXIS_TYPE, GdvSpecialPolarAxisClass))
#define GDV_SPECIAL_POLAR_AXIS_IS_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), GDV_SPECIAL_POLAR_AXIS_TYPE))
#define GDV_SPECIAL_POLAR_AXIS_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), GDV_SPECIAL_POLAR_AXIS_TYPE, GdvSpecialPolarAxisClass))

typedef struct _GdvSpecialPolarAxis GdvSpecialPolarAxis;
typedef struct _GdvSpecialPolarAxisClass GdvSpecialPolarAxisClass;
typedef struct _GdvSpecialPolarAxisPrivate GdvSpecialPolarAxisPrivate;

struct _GdvSpecialPolarAxis
{
  GdvLinearAxis parent;

  /*< private > */
  GdvSpecialPolarAxisPrivate *priv;
};

/**
 * GdvSpecialPolarAxisClass:
 * @parent_class: The parent class.
 */
struct _GdvSpecialPolarAxisClass
{
  GdvLinearAxisClass parent_class;

  /*< public >*/
};

/* Public Method definitions. */
GType gdv_special_polar_axis_get_type (void);

GdvSpecialPolarAxis *gdv_special_polar_axis_new (void);

G_END_DECLS

#endif /* GDV_SPECIAL_POLAR_AXIS_H_INCLUDED */