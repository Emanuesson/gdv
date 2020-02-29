/*
 * gdvcontent.h
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

#ifndef GDV_CONTENT_H_INCLUDED
#define GDV_CONTENT_H_INCLUDED

#include <gtk/gtk.h>
#include <libggsl/libggsl.h>

G_BEGIN_DECLS

#define GDV_TYPE_CONTENT        (gdv_content_get_type ())

G_DECLARE_INTERFACE (GdvContent, gdv_content, GDV, CONTENT, GtkWidget)

/**
 * GdvContentInterface:
 *
 */
struct _GdvContentInterface
{
  /*< private >*/
  GTypeInterface g_iface;

  /*< public >*/
  /* draw to 0,0 with the given width and height */
//  void                  (* snapshot)                    (GdkPaintable           *paintable,
//                                                         GdkSnapshot            *snapshot,
//                                                         double                  width,
//                                                         double                  height);
};

G_END_DECLS

#endif /* GDV_CONTENT_H_INCLUDED */
