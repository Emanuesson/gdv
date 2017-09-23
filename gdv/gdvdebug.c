/*
 * gdvdebug.c
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

#include "gdvdebug.h"

static gboolean
_gdv_debugging_element_on_draw (GtkWidget    *widget,
                                cairo_t      *cr);

gboolean
_gdv_debugging_init_element_frame (GdvElement *element)
{
  g_return_val_if_fail (GDV_IS_ELEMENT (element), TRUE);

  g_signal_connect (GTK_WIDGET (element), "draw", G_CALLBACK (_gdv_debugging_element_on_draw), NULL);
}

/*
gboolean
_gdv_debugging_set_frame_on_2d_view (Gdv2dView *view)
{
  GtkAllocation __allocation;
  cairo_pattern_t *__debugging_source;
  cairo_text_extents_t __debugging_tex;
  GtkWidget *widget;
  cairo_t *cr;

  widget = GTK_WIDGET (view);

  gtk_widget_get_allocation (widget, &__allocation);
  __debugging_source = cairo_pattern_create_rgba (1.0, 1.0, 1.0, 1.0);

  cairo_move_to (cr,
                 __allocation.x,
                 __allocation.y);
  cairo_line_to (cr,
                 __allocation.x + __allocation.height,
                 __allocation.y);
  cairo_line_to (cr,
                 __allocation.x + __allocation.height,
                 __allocation.y + __allocation.width);
  cairo_line_to (cr,
                 __allocation.x,
                 __allocation.y + __allocation.width);

  cairo_set_source (cr, __debugging_source);
  cairo_stroke (cr);

  g_print ("debugging running\n");

  cairo_set_source_rgb (cr, 1.0, 1.0, 1.0);
  cairo_select_font_face (cr, "Georgia",
                          CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
  cairo_set_font_size (cr, 1.2);
  cairo_text_extents (cr, "a", &__debugging_tex);
  cairo_move_to (cr,
                 __allocation.x - 0.9 * __allocation.width - __debugging_tex.x_bearing,
                 __allocation.y - 0.1 * __allocation.height -
                 __debugging_tex.height / 2 - __debugging_tex.y_bearing);
  cairo_show_text (cr, "GDV-dbg: Element");

}
*/

static gboolean
_gdv_debugging_element_on_draw (GtkWidget    *widget,
                                cairo_t      *cr)
{
  GtkAllocation __allocation;
  cairo_pattern_t *__debugging_source;
  cairo_text_extents_t __debugging_tex;
  const double dashed1[] = {1.0, 1.0};

  gtk_widget_get_allocation (widget, &__allocation);
  __debugging_source = cairo_pattern_create_rgba (1.0, 1.0, 1.0, 1.0);

  if (__allocation.width >= 20 && __allocation.height >= 20)
    {
      cairo_set_dash(cr, dashed1, 2, 0);
      cairo_move_to (cr,
                     0,
                     0);
      cairo_line_to (cr,
                     5,
                     5);
      cairo_line_to (cr,
                     -5 + __allocation.width,
                     5);
      cairo_line_to (cr,
                     0 + __allocation.width,
                     0);

      cairo_move_to (cr,
                     -5 + __allocation.width,
                     5);
      cairo_line_to (cr,
                     -5 + __allocation.width,
                     -5 + __allocation.height);
      cairo_line_to (cr,
                     0 + __allocation.width,
                     0 + __allocation.height);

      cairo_move_to (cr,
                     -5 + __allocation.width,
                     -5 + __allocation.height);
      cairo_line_to (cr,
                     5,
                     -5 + __allocation.height);

      cairo_line_to (cr,
                     0,
                     0 + __allocation.height);

      cairo_move_to (cr,
                     5,
                     -5 + __allocation.height);
      cairo_line_to (cr,
                     5,
                     5);

      cairo_set_source (cr, __debugging_source);
      cairo_stroke (cr);

      cairo_set_source_rgb (cr, 1.0, 1.0, 1.0);
      cairo_select_font_face (cr, "Courier",
                              CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
      cairo_set_font_size (cr, 8.0);
      cairo_text_extents (cr, "a", &__debugging_tex);
      cairo_move_to (cr,
                     7 - __debugging_tex.x_bearing,
                     7 +
                     __debugging_tex.height / 2 - __debugging_tex.y_bearing);

      char str[200];
      sprintf ((char *) &str, "DBG: %s(%p)", G_OBJECT_TYPE_NAME (widget), widget);
      cairo_show_text (cr, str);

      if (__allocation.height >= 23)
        {
          char str2[200];
          sprintf ((char *) &str2, "size: %u, %u", __allocation.width, __allocation.height);
          cairo_move_to (cr,
                         7 - __debugging_tex.x_bearing,
                         7 +
                         2.5 * __debugging_tex.height - __debugging_tex.y_bearing);
          cairo_show_text (cr, str2);
        }

      if (__allocation.height >= 26)
        {
          char str3[200];
          sprintf ((char *) &str3, "pos: %u, %u", __allocation.x, __allocation.y);
          cairo_move_to (cr,
                         7 - __debugging_tex.x_bearing,
                         7 +
                         4.5 * __debugging_tex.height - __debugging_tex.y_bearing);
          cairo_show_text (cr, str3);
        }
    }
  else
    {
      gint offset = 0;

      cairo_set_dash(cr, dashed1, 4, 0);
      cairo_move_to (cr,
                 offset,
                 offset);
      cairo_line_to (cr,
                     offset,
                     __allocation.height - 2 * offset);
      cairo_line_to (cr,
                     __allocation.width - 2 * offset,
                     __allocation.height -2 * offset);
      cairo_line_to (cr,
                     __allocation.width - 2 * offset,
                     offset);
      cairo_line_to (cr,
                     offset,
                     offset);
      cairo_set_source (cr, __debugging_source);
      cairo_stroke (cr);
    }

}

