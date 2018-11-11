/*
 * gdvlegend.h
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

#ifndef GDV_LEGEND_H_INCLUDED
#define GDV_LEGEND_H_INCLUDED

#include <stdlib.h>
#include <gtk/gtk.h>

#include <gdv/gdvcentral.h>

G_BEGIN_DECLS

#define GDV_LEGEND_TYPE                 (gdv_legend_get_type ())
#define GDV_LEGEND(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj),\
                                         GDV_LEGEND_TYPE, GdvLegend))
#define GDV_IS_LEGEND(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj),\
                                          GDV_LEGEND_TYPE))
#define GDV_LEGEND_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass),\
                                          GDV_LEGEND_TYPE, GdvLegendClass))
#define GDV_LEGEND_IS_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass),\
                                         GDV_LEGEND_TYPE))
#define GDV_LEGEND_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj),\
                                          GDV_LEGEND_TYPE, GdvLegendClass))

typedef struct _GdvLegend GdvLegend;
typedef struct _GdvLegendClass GdvLegendClass;
typedef struct _GdvLegendPrivate GdvLegendPrivate;

struct _GdvLegend
{
  GtkFrame parent;

  /*< private >*/
  GdvLegendPrivate *priv;
};

/**
 * GdvLegendClass:
 * @parent_class: The parent class.
 */
struct _GdvLegendClass
{
  GtkFrameClass parent_class;

  /*< private >*/
  /* Padding to allow adding up to 12 new virtual functions without
   * breaking ABI. */
  gpointer _gdv_reserve[12];
};

/* Public Method definitions. */
GType gdv_legend_get_type (void);

GdvLegend *gdv_legend_new (void);

G_END_DECLS

#endif /* GDV_LEGEND_H_INCLUDED */
