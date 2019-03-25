/*
 * gdvaxis.h
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

#ifndef GDV_LINEAR_AXIS_H_INCLUDED
#define GDV_LINEAR_AXIS_H_INCLUDED

#include <stdlib.h>
#include <gtk/gtk.h>

#include "gdvaxis.h"

G_BEGIN_DECLS

#define GDV_LINEAR_TYPE_AXIS\
  (gdv_linear_axis_get_type ())
#define GDV_LINEAR_AXIS(obj)\
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), GDV_LINEAR_TYPE_AXIS, GdvLinearAxis))
#define GDV_LINEAR_IS_AXIS(obj)\
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_LINEAR_TYPE_AXIS))
#define GDV_LINEAR_AXIS_CLASS(klass)\
  (G_TYPE_CHECK_CLASS_CAST ((klass), GDV_LINEAR_TYPE_AXIS, GdvLinearAxisClass))
#define GDV_LINEAR_AXIS_IS_CLASS(klass)\
  (G_TYPE_CHECK_CLASS_TYPE ((klass), GDV_LINEAR_TYPE_AXIS))
#define GDV_LINEAR_AXIS_GET_CLASS(obj)\
  (G_TYPE_INSTANCE_GET_CLASS ((obj), GDV_LINEAR_TYPE_AXIS, GdvLinearAxisClass))

typedef struct _GdvLinearAxis GdvLinearAxis;
typedef struct _GdvLinearAxisClass GdvLinearAxisClass;
typedef struct _GdvLinearAxisPrivate GdvLinearAxisPrivate;

struct _GdvLinearAxis
{
  GdvAxis parent;

  /*< private >*/
  GdvLinearAxisPrivate *priv;
};

/**
 * GdvLinearAxisClass:
 * @parent_class: The parent class.
 */
struct _GdvLinearAxisClass
{
  GdvAxisClass parent_class;

  /*< private >*/
  /* Padding to allow adding up to 12 new virtual functions without
   * breaking ABI. */
  gpointer _gdv_reserve[12];
};

/* Public Method definitions. */
GType gdv_linear_axis_get_type (void);

GdvLinearAxis *gdv_linear_axis_new (void);

G_END_DECLS

#endif /* GDV_LINEAR_AXIS_H_INCLUDED */
