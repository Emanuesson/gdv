/*
 * gdvrender.c
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

#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include <math.h>

#include "gdvrender.h"

static void
gtk_do_render_line (GtkStyleContext *context,
                    cairo_t         *cr,
                    gdouble          x0,
                    gdouble          y0,
                    gdouble          x1,
                    gdouble          y1)
{
  GdkRGBA *color;
  gdouble axis_line_width;
  GValue value_lw = G_VALUE_INIT;
  GValue value_lc = G_VALUE_INIT;
//  GValue *value_lw = NULL;
//  GValue *value_lc = NULL;

  g_value_init (&value_lw, G_TYPE_DOUBLE);
  g_value_init (&value_lc, GDK_TYPE_RGBA);

  cairo_save (cr);

  gtk_style_context_get_style_property (context,
                                        "line-width", &value_lw);
  gtk_style_context_get_style_property (context,
                                        "color", &value_lc);

  axis_line_width = g_value_get_double (&value_lw);
  color = g_value_get_boxed (&value_lc);

  cairo_set_line_cap (cr, CAIRO_LINE_CAP_SQUARE);
  cairo_set_line_width (cr, axis_line_width);

  cairo_move_to (cr, x0 + 0.5, y0 + 0.5);
  cairo_line_to (cr, x1 + 0.5, y1 + 0.5);
  gdk_cairo_set_source_rgba(cr, color);

  cairo_stroke (cr);

  cairo_restore (cr);

  g_value_unset (&value_lw);
  g_value_unset (&value_lc);
}

static void
gtk_do_render_straight_axis (GtkStyleContext *context,
                             cairo_t         *cr,
                             gdouble          x0,
                             gdouble          y0,
                             gdouble          x1,
                             gdouble          y1)
{
  GdkRGBA *color;
  gdouble axis_line_width;
  GValue value_lw = G_VALUE_INIT;
  GValue value_lc = G_VALUE_INIT;

  g_value_init (&value_lw, G_TYPE_DOUBLE);
  g_value_init (&value_lc, GDK_TYPE_RGBA);

  cairo_save (cr);

  gtk_style_context_get_style_property (context,
                                        "line-width", &value_lw);
  gtk_style_context_get_style_property (context,
                                        "color", &value_lc);

  axis_line_width = g_value_get_double (&value_lw);
  color = g_value_get_boxed (&value_lc);

  cairo_set_line_cap (cr, CAIRO_LINE_CAP_SQUARE);
  cairo_set_line_width (cr, axis_line_width);

  cairo_move_to (cr, x0 + 0.5, y0 + 0.5);
  cairo_line_to (cr, x1 + 0.5, y1 + 0.5);
  gdk_cairo_set_source_rgba(cr, color);

  cairo_stroke (cr);

  cairo_restore (cr);

  g_value_unset (&value_lw);
  g_value_unset (&value_lc);
}

void        gdv_render_arc         (GtkStyleContext     *context,
                                    cairo_t             *cr,
                                    gdouble              xc,
                                    gdouble              yc,
                                    gdouble              radius,
                                    gdouble              angle1,
                                    gdouble              angle2)
{
  GdkRGBA *color;
  gdouble axis_line_width;
  GValue value_lw = G_VALUE_INIT;
  GValue value_lc = G_VALUE_INIT;

  g_value_init (&value_lw, G_TYPE_DOUBLE);
  g_value_init (&value_lc, GDK_TYPE_RGBA);

  gtk_style_context_get_style_property (context,
                                        "line-width", &value_lw);
  gtk_style_context_get_style_property (context,
                                        "color", &value_lc);

  cairo_save (cr);

  gtk_style_context_get_style_property (context,
                                        "line-width", &value_lw);
  gtk_style_context_get_style_property (context,
                                        "color", &value_lc);

  axis_line_width = g_value_get_double (&value_lw);
  color = g_value_get_boxed (&value_lc);

  cairo_set_line_cap (cr, CAIRO_LINE_CAP_SQUARE);
  cairo_set_line_width (cr, axis_line_width);

  cairo_move_to (cr, xc + radius, yc);
  cairo_arc (cr, xc, yc, radius, angle1, angle2);

  gdk_cairo_set_source_rgba(cr, color);

  cairo_stroke (cr);

  cairo_restore (cr);


  g_value_unset (&value_lw);
  g_value_unset (&value_lc);

}

static void
gtk_do_render_tic (GtkStyleContext *context,
                   cairo_t         *cr,
                   gdouble          lw,
                   GdkRGBA         *lc,
                   gdouble          x0,
                   gdouble          y0,
                   gdouble          x1,
                   gdouble          y1)
{
//  GdkRGBA *color;
//  gdouble axis_line_width;
  GValue value_lw = G_VALUE_INIT;
  GValue value_lc = G_VALUE_INIT;

  g_value_init (&value_lw, G_TYPE_DOUBLE);
  g_value_init (&value_lc, GDK_TYPE_RGBA);

  cairo_save (cr);

  gtk_style_context_get_style_property (context,
                                        "tics-line-width", &value_lw);
  gtk_style_context_get_style_property (context,
                                        "tics-color", &value_lc);

//  axis_line_width = g_value_get_double (&value_lw);
//  color = g_value_get_boxed (&value_lc);

  cairo_set_line_cap (cr, CAIRO_LINE_CAP_SQUARE);
  cairo_set_line_width (cr, lw);

  cairo_move_to (cr, x0 + 0.5, y0 + 0.5);
  cairo_line_to (cr, x1 + 0.5, y1 + 0.5);
  gdk_cairo_set_source_rgba(cr, lc);

  cairo_stroke (cr);

  cairo_restore (cr);

  g_value_unset (&value_lw);
  g_value_unset (&value_lc);
}

static void
gtk_do_render_mtic (GtkStyleContext *context,
                    cairo_t         *cr,
                    gdouble          lw,
                    GdkRGBA         *lc,
                    gdouble          x0,
                    gdouble          y0,
                    gdouble          x1,
                    gdouble          y1)
{
//  GdkRGBA *color;
//  gdouble axis_line_width;
  GValue value_lw = G_VALUE_INIT;
  GValue value_lc = G_VALUE_INIT;

  g_value_init (&value_lw, G_TYPE_DOUBLE);
  g_value_init (&value_lc, GDK_TYPE_RGBA);

  cairo_save (cr);

  gtk_style_context_get_style_property (context,
                                        "mtics-line-width", &value_lw);
  gtk_style_context_get_style_property (context,
                                        "mtics-color", &value_lc);

//  axis_line_width = g_value_get_double (&value_lw);
//  color = g_value_get_boxed (&value_lc);

   cairo_set_line_cap (cr, CAIRO_LINE_CAP_SQUARE);
  cairo_set_line_width (cr, lw);

  cairo_move_to (cr, x0 + 0.5, y0 + 0.5);
  cairo_line_to (cr, x1 + 0.5, y1 + 0.5);
  gdk_cairo_set_source_rgba(cr, lc);

  cairo_stroke (cr);

  cairo_restore (cr);

  g_value_unset (&value_lw);
  g_value_unset (&value_lc);
}

/**
 * gdv_render_line:
 * @context: a #GtkStyleContext
 * @cr: a cairo surface
 * @x0: X coordinate for the origin of the axis
 * @y0: Y coordinate for the origin of the axis
 * @x1: X coordinate for the end of the axis
 * @y1: Y coordinate for the end of the axis
 *
 * Renders a straight axis line from (x0, y0) to (x1, y1).
 *
 * Since: 0.1
 **/
void        gdv_render_line        (GtkStyleContext     *context,
                                    cairo_t             *cr,
                                    gdouble              x0,
                                    gdouble              y0,
                                    gdouble              x1,
                                    gdouble              y1)
{
  g_return_if_fail (GTK_IS_STYLE_CONTEXT (context));
  g_return_if_fail (cr != NULL);

  cairo_save (cr);
  cairo_new_path (cr);

  gtk_do_render_line (context, cr, x0, y0, x1, y1);

  cairo_restore (cr);
}


/**
 * gdv_render_straight_axis:
 * @context: a #GtkStyleContext
 * @cr: a #cairo_t
 * @x0: X coordinate for the origin of the axis
 * @y0: Y coordinate for the origin of the axis
 * @x1: X coordinate for the end of the axis
 * @y1: Y coordinate for the end of the axis
 *
 * Renders a straight axis line from (x0, y0) to (x1, y1).
 *
 * Since: 0.1
 **/
void        gdv_render_straight_axis        (GtkStyleContext     *context,
    cairo_t             *cr,
    gdouble              x0,
    gdouble              y0,
    gdouble              x1,
    gdouble              y1)
{
  g_return_if_fail (GTK_IS_STYLE_CONTEXT (context));
  g_return_if_fail (cr != NULL);

  cairo_save (cr);
  cairo_new_path (cr);

  gtk_do_render_straight_axis (context, cr, x0, y0, x1, y1);

  cairo_restore (cr);
}


/**
 * gdv_render_tic:
 * @context: a #GtkStyleContext
 * @cr: a #cairo_t
 * @lw: the line-width as received from the style-property
 * @lc: the line-color as received from the style-property
 * @x0: X coordinate for the origin of the tic
 * @y0: Y coordinate for the origin of the tic
 * @x1: X coordinate for the end of the tic
 * @y1: Y coordinate for the end of the tic
 *
 * Renders a straight tic line from (x0, y0) to (x1, y1).
 *
 * Since: 0.1
 **/
void        gdv_render_tic                  (GtkStyleContext     *context,
    cairo_t             *cr,
    gdouble              lw,
    GdkRGBA             *lc,
    gdouble              x0,
    gdouble              y0,
    gdouble              x1,
    gdouble              y1)
{
  g_return_if_fail (GTK_IS_STYLE_CONTEXT (context));
  g_return_if_fail (cr != NULL);

  cairo_save (cr);
  cairo_new_path (cr);

  gtk_do_render_tic (context, cr, lw, lc, x0, y0, x1, y1);

  cairo_restore (cr);
}

/**
 * gdv_render_mtic:
 * @context: a #GtkStyleContext
 * @cr: a #cairo_t
 * @lw: the line-width as received from the style-property
 * @lc: the line-color as received from the style-property
 * @x0: X coordinate for the origin of the mtic
 * @y0: Y coordinate for the origin of the mtic
 * @x1: X coordinate for the end of the mtic
 * @y1: Y coordinate for the end of the mtic
 *
 * Renders a straight tic line from (x0, y0) to (x1, y1).
 *
 * Since: 0.1
 **/
void        gdv_render_mtic                 (GtkStyleContext     *context,
    cairo_t             *cr,
    gdouble              lw,
    GdkRGBA             *lc,
    gdouble              x0,
    gdouble              y0,
    gdouble              x1,
    gdouble              y1)
{
  g_return_if_fail (GTK_IS_STYLE_CONTEXT (context));
  g_return_if_fail (cr != NULL);

  cairo_save (cr);
  cairo_new_path (cr);

  gtk_do_render_mtic (context, cr, lw, lc, x0, y0, x1, y1);

  cairo_restore (cr);
}

/* FIXME: This function is not even close to a good look */
void        gdv_render_data_point           (GtkStyleContext     *context,
    cairo_t             *cr,
    gdouble              x,
    gdouble              y)
{
  GdkRGBA *color;
  gdouble point_width;
  GValue value_lw = G_VALUE_INIT;
  GValue value_lc = G_VALUE_INIT;

  g_return_if_fail (GTK_IS_STYLE_CONTEXT (context));
  g_return_if_fail (cr != NULL);

  cairo_new_path (cr);

  g_value_init (&value_lw, G_TYPE_DOUBLE);
  g_value_init (&value_lc, GDK_TYPE_RGBA);

  cairo_save (cr);

  gtk_style_context_get_style_property (context,
                                        "point-width", &value_lw);
  gtk_style_context_get_style_property (context,
                                        "point-color", &value_lc);

  point_width = g_value_get_double (&value_lw);
  color = g_value_get_boxed (&value_lc);

  cairo_move_to (cr, x + 0.5, y + 0.5);

  gdk_cairo_set_source_rgba(cr, color);

  if (point_width)
  {
    cairo_arc (cr,
               x + 0.5, y + 0.5,
               point_width,
               0, 2 * G_PI);
    cairo_fill (cr);
  }

  cairo_restore (cr);

  g_value_unset (&value_lw);
  g_value_unset (&value_lc);
}

static void
gdv_do_render_data_line (GtkStyleContext *context,
                         cairo_t         *cr,
                         gdouble          x0,
                         gdouble          y0,
                         gdouble          x1,
                         gdouble          y1)
{
  GdkRGBA *color;
  gdouble line_width;
  gdouble prim_dash_port;
  gdouble secon_dash_port;
  gint dash_length;
  GValue value_lw = G_VALUE_INIT;
  GValue value_lc = G_VALUE_INIT;
  GValue value_prim_dp = G_VALUE_INIT;
  GValue value_sec_dp = G_VALUE_INIT;
  GValue value_dash_length = G_VALUE_INIT;

  g_value_init (&value_lw, G_TYPE_DOUBLE);
  g_value_init (&value_lc, GDK_TYPE_RGBA);
  g_value_init (&value_prim_dp, G_TYPE_DOUBLE);
  g_value_init (&value_sec_dp, G_TYPE_DOUBLE);
  g_value_init (&value_dash_length, G_TYPE_INT);

  gtk_style_context_get_style_property (context,
                                        "line-width", &value_lw);
  gtk_style_context_get_style_property (context,
                                        "line-color", &value_lc);
  gtk_style_context_get_style_property (context,
                                        "line-dash-portion", &value_prim_dp);
  gtk_style_context_get_style_property (context,
                                        "line-secondary-dash-portion", &value_sec_dp);
  gtk_style_context_get_style_property (context,
                                        "line-dash-length", &value_dash_length);

  line_width = g_value_get_double (&value_lw);
  color = g_value_get_boxed (&value_lc);
  prim_dash_port = g_value_get_double (&value_prim_dp);
  secon_dash_port = g_value_get_double (&value_sec_dp);
  dash_length = g_value_get_int (&value_dash_length);

  if (line_width)
  {
    gdouble dash_array[] =
    {
      prim_dash_port, (1.0 - prim_dash_port - secon_dash_port) / 2.0,
      secon_dash_port, (1.0 - prim_dash_port - secon_dash_port) / 2.0
    };

    cairo_move_to (cr, x0 + 0.5, y0 + 0.5);
    cairo_line_to (cr, x1 + 0.5, y1 + 0.5);
    cairo_set_line_width (cr, line_width);

//    g_print ("LC\n");
    gdk_cairo_set_source_rgba(cr, color);

    cairo_set_dash (cr, dash_array, dash_length, 0.0);

    cairo_stroke (cr);
  }

  g_value_unset (&value_lw);
  g_value_unset (&value_lc);
  g_value_unset (&value_prim_dp);
  g_value_unset (&value_sec_dp);
  g_value_unset (&value_dash_length);
}


/* FIXME: This function is not even close to a good look */
void        gdv_render_data_line            (GtkStyleContext     *context,
    cairo_t             *cr,
    gdouble              x0,
    gdouble              y0,
    gdouble              x1,
    gdouble              y1)
{
  g_return_if_fail (GTK_IS_STYLE_CONTEXT (context));
  g_return_if_fail (cr != NULL);

  cairo_save (cr);
  cairo_new_path (cr);

  gdv_do_render_data_line (context, cr, x0, y0, x1, y1);

  cairo_restore (cr);
}
