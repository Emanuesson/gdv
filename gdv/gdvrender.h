/*
 * gdvrender.h
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

#ifndef GDV_RENDER_H_INCLUDED
#define GDV_RENDER_H_INCLUDED

#include <stdlib.h>
#include <gtk/gtk.h>

#include "gdvcentral.h"
#include <cairo/cairo.h>

G_BEGIN_DECLS


/* comment: unfortunately this is necessary to surpass some limitations of the
 *          gtk_style_context. As soon as these limitations are included in gtk
 *          these functions should be deprecated
 */
void        gdv_render_line        (GtkStyleContext     *context,
                                    cairo_t             *cr,
                                    gdouble              x0,
                                    gdouble              y0,
                                    gdouble              x1,
                                    gdouble              y1);

void        gdv_render_arc         (GtkStyleContext     *context,
                                    cairo_t             *cr,
                                    gdouble              xc,
                                    gdouble              yc,
                                    gdouble              radius,
                                    gdouble              angle1,
                                    gdouble              angle2);

void        gdv_render_data_line            (GtkStyleContext     *context,
    cairo_t             *cr,
    gdouble              x0,
    gdouble              y0,
    gdouble              x1,
    gdouble              y1);

void        gdv_render_data_point           (GtkStyleContext     *context,
    cairo_t             *cr,
    gdouble              x,
    gdouble              y);

G_END_DECLS

#endif /* GDV_RENDER_H_INCLUDED */
