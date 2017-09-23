/*
 * gdvindicator.h
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

#ifndef GDV_INDICATOR_H_INCLUDED
#define GDV_INDICATOR_H_INCLUDED

#include <stdlib.h>
#include <gtk/gtk.h>

#include <gdv/gdvcentral.h>

G_BEGIN_DECLS

#define GDV_TYPE_INDICATOR\
  (gdv_indicator_get_type ())
#define GDV_INDICATOR(obj)\
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), GDV_TYPE_INDICATOR, GdvIndicator))
#define GDV_IS_INDICATOR(obj)\
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_TYPE_INDICATOR))
#define GDV_INDICATOR_CLASS(klass)\
  (G_TYPE_CHECK_CLASS_CAST ((klass), GDV_TYPE_INDICATOR, GdvIndicatorClass))
#define GDV_INDICATOR_IS_CLASS(klass)\
  (G_TYPE_CHECK_CLASS_TYPE ((klass), GDV_TYPE_INDICATOR))
#define GDV_INDICATOR_GET_CLASS(obj)\
  (G_TYPE_INSTANCE_GET_CLASS ((obj), GDV_TYPE_INDICATOR, GdvIndicatorClass))

typedef struct _GdvIndicator GdvIndicator;
typedef struct _GdvIndicatorClass GdvIndicatorClass;
typedef struct _GdvIndicatorPrivate GdvIndicatorPrivate;

struct _GdvIndicator
{
  GtkContainer parent;

  /*< private >*/
  GdvIndicatorPrivate *priv;
};

/**
 * GdvIndicatorClass:
 * @parent_class: The indicator class structure is derived from the common
 *   #GtkContainer.
 */
struct _GdvIndicatorClass
{
  GtkContainerClass parent_class;

  /* Padding to allow adding up to 12 new virtual functions without
   * breaking ABI. */
  gpointer _gdv_reserve[12];
};

/* Public Method definitions. */
GType gdv_indicator_get_type (void);

GdvIndicator *gdv_indicator_new (void);

G_END_DECLS

#endif /* GDV_INDICATOR_H_INCLUDED */
