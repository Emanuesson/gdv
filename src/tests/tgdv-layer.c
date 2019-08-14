/* tgdv-layer.c
 *
 * Copyright © 2018 Emanuel Schmidt <eschmidt216@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "tgdv-axis.h"
#include "tgdv-layer.h"
#include "tgdv-linearaxis.h"

void tgdv_layer_test_integrity (GdvLayer *layer)
{
  GList * axis_list, * laxis_cpy;
  GList * content_list, * lcontent_cpy;

  g_assert(GDV_IS_LAYER(layer));

  axis_list = gdv_layer_get_axis_list (GDV_LAYER (layer));
  content_list = gdv_layer_get_content_list (GDV_LAYER (layer));
  lcontent_cpy = content_list;

  if (gtk_widget_get_realized (GTK_WIDGET(layer)))
    g_assert_cmpuint(g_list_length(axis_list), >=, 1);

  for (laxis_cpy = axis_list; axis_list; axis_list = axis_list->next)
  {
/*
    gdouble value;
    gfloat pos_x, pos_y;

    g_assert(GDV_IS_TIC(tics->data));
    g_object_get (tics->data,
                  "value", &value,
                  "pos-x", &pos_x,
                  "pos-y", &pos_y,
                  NULL);

    if (scale_auto_increment)
*/
    g_assert(GDV_IS_AXIS(axis_list->data));

    /* FIXME: This is just an awful way to deal with the infamous log-axis */
    if (GDV_LOG_IS_AXIS(axis_list->data))
      continue;

    if (GDV_LINEAR_IS_AXIS(axis_list->data))
      tgdv_linearaxis_test_integrity(axis_list->data);
    else if (GDV_IS_AXIS(axis_list->data))
      tgdv_axis_test_full(axis_list->data);


  }

  g_list_free(laxis_cpy);
  g_list_free(lcontent_cpy);
}

