/*
 * gdv-data-geometric.c
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

#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include <math.h>
#include <stdio.h>

#include "gdv/gdv-data-boxed.h"

#define SQUARE(x) ((x)*(x))

/* Point element */
/**
 * gdv_data_point_equal:
 * @point1: a #GdvDataPoint
 * @point2: a #GdvDataPoint
 *
 * Checks if the two given points are equal.
 *
 * Returns: %TRUE if the points are equal.
 *
 * Since: 0.10
 */
gboolean
gdv_data_point_equal (const GdvDataPoint *point1,
                      const GdvDataPoint *point2)
{
  return point1->x == point2->x
         && point1->y == point2->y
         && point1->z == point2->z;
}

static GdvDataPoint *
gdv_data_point_copy (const GdvDataPoint *point)
{
  GdvDataPoint *result = g_new (GdvDataPoint, 1);

  *result = *point;

  return result;
}

gdouble
gdv_data_point_metric (const GdvDataPoint *point1,
                       const GdvDataPoint *point2)
{
  return sqrt (
    SQUARE (point1->x - point2->x) +
    SQUARE (point1->y - point2->y) +
    SQUARE (point1->z - point2->z)
              );
}

gdouble
gdv_data_point_distance_to_origin (const GdvDataPoint *point)
{
  return sqrt (
    SQUARE (point->x) +
    SQUARE (point->y) +
    SQUARE (point->z)
              );
}

G_DEFINE_BOXED_TYPE (GdvDataPoint, gdv_data_point,
                     gdv_data_point_copy,
                     g_free)

/**
 * gdv_data_point_single_extended_equal:
 * @point1: a #GdvDataPointSingleExtended
 * @point2: a #GdvDataPointSingleExtended
 *
 * Checks if the two given points are equal.
 *
 * Returns: %TRUE if the points are equal.
 *
 * Since: 0.10
 */
gboolean
gdv_data_point_single_extended_equal (const GdvDataPointSingleExtended *point1,
                                      const GdvDataPointSingleExtended *point2)
{
  return gdv_data_point_equal (point1->point, point2->point)
         && gdv_data_point_equal (point1->neg_range_1, point2->neg_range_1)
         && gdv_data_point_equal (point1->pos_range_1, point2->pos_range_1);
}

static GdvDataPointSingleExtended *
gdv_data_point_single_extended_copy (const GdvDataPointSingleExtended *point)
{
  GdvDataPointSingleExtended *result = g_new (GdvDataPointSingleExtended, 1);

  result->point = gdv_data_point_copy (point->point);
  result->neg_range_1 = gdv_data_point_copy (point->neg_range_1);
  result->pos_range_1 = gdv_data_point_copy (point->pos_range_1);

  return result;
}

G_DEFINE_BOXED_TYPE (GdvDataPointSingleExtended, gdv_data_point_single_extended,
                     gdv_data_point_single_extended_copy,
                     g_free)

/**
 * gdv_data_point_double_extended_equal:
 * @point1: a #GdvDataPointDoubleExtended
 * @point2: a #GdvDataPointDoubleExtended
 *
 * Checks if the two given points are equal.
 *
 * Returns: %TRUE if the points are equal.
 *
 * Since: 0.10
 */
gboolean
gdv_data_point_double_extended_equal (const GdvDataPointDoubleExtended *point1,
                                      const GdvDataPointDoubleExtended *point2)
{
  return gdv_data_point_single_extended_equal (point1->point, point2->point)
         && gdv_data_point_equal (point1->neg_range_2, point2->neg_range_2)
         && gdv_data_point_equal (point1->pos_range_2, point2->pos_range_2);
}

static GdvDataPointDoubleExtended *
gdv_data_point_double_extended_copy (const GdvDataPointDoubleExtended *point)
{
  GdvDataPointDoubleExtended *result = g_new (GdvDataPointDoubleExtended, 1);

  result->point = gdv_data_point_single_extended_copy (point->point);
  result->neg_range_2 = gdv_data_point_copy (point->neg_range_2);
  result->pos_range_2 = gdv_data_point_copy (point->pos_range_2);

  return result;
}

G_DEFINE_BOXED_TYPE (GdvDataPointDoubleExtended, gdv_data_point_double_extended,
                     gdv_data_point_double_extended_copy,
                     g_free)

/* n-dimensional point element */
/**
 * gdv_data_n_point_equal:
 * @point1: a #GdvDataNPoint
 * @point2: a #GdvDataNPoint
 *
 * Checks if the two given n-dim. points are equal.
 *
 * Returns: %TRUE if the points are equal.
 *
 * Since: 0.10
 */
gboolean
gdv_data_n_point_equal(const GdvDataNPoint *point1,
                       const GdvDataNPoint *point2)
{
  gint i;

  if ((point1->N != point2->N) ||
      !point1->x ||
      !point2->x)
    return FALSE;

  for (i = 0; i < point1->N; i++)
    {
      if (point1->x [i] != point2->x [i])
        return FALSE;
    }

  return TRUE;
}

static GdvDataNPoint *
gdv_data_n_point_copy (const GdvDataNPoint *point)
{
  GdvDataNPoint *result = g_new (GdvDataNPoint, 1);

  result->x = g_new (gdouble, point->N);

  *result->x = *point->x;
  result->N = point->N;

  return result;
}

gdouble
gdv_data_n_point_metric (const GdvDataNPoint *point1,
                         const GdvDataNPoint *point2)
{
  gint i;
  gdouble metric = 0.0;

  g_return_val_if_fail (point1->x, 0.0);
  g_return_val_if_fail (point2->x, 0.0);

  g_return_val_if_fail ((point1->N != point2->N) ||
                        !point1->x ||
                        !point2->x, 0.0);

  for (i = 0; i < point1->N; i++)
    {
      metric += SQUARE (point1->x [i] - point2->x [i]);
    }

  return metric;
}

gdouble
gdv_data_n_point_get (const GdvDataNPoint *point1,
                      const guint          n)
{
  g_return_val_if_fail (n < point1->N, 0.0);
  g_return_val_if_fail (point1->x, 0.0);

  return point1->x [n];
}

void
gdv_data_n_point_set (GdvDataNPoint *point1,
                      const guint    n,
                      gdouble        x)
{
  g_return_if_fail (n < point1->N);
  g_return_if_fail (point1->x);

  point1->x [n] = x;
}

GdvDataNPoint *
gdv_data_n_point_set_and_expand (GdvDataNPoint *point1,
                                 const guint    n,
                                 gdouble        x)
{
  GdvDataNPoint *result;

  if (n <= point1->N)
    {
      gdv_data_n_point_set (point1, n, x);
      return point1;
    }

  result = g_new (GdvDataNPoint, 1);
  result->x = g_new (gdouble, n);
  *result->x = *point1->x;
  result->N = n;
  result->x [n] = x;

  g_free (point1);
  return result;
}

gdouble
gdv_data_n_point_distance_to_origin (const GdvDataNPoint *point)
{
  gint i;
  gdouble return_val = 0.0;

  g_return_val_if_fail (point->x, 0.0);

  for (i = 0; i < point->N; i++)
    {
      return_val += SQUARE (point->x [i]);
    }

  return return_val;
}

G_DEFINE_BOXED_TYPE (GdvDataNPoint, gdv_data_n_point,
                     gdv_data_n_point_copy,
                     g_free)

/* Directional element */
/**
 * gdv_data_direction_equal:
 * @direction1: a #GdvDataDirection
 * @direction2: a #GdvDataDirection
 *
 * Checks if the two given directions are equal.
 *
 * Returns: %TRUE if the directions are equal.
 *
 * Since: 0.10
 */
gboolean
gdv_data_direction_equal (const GdvDataDirection *direction1,
                          const GdvDataDirection *direction2)
{
  return direction1->origin.x == direction2->origin.x
         && direction1->origin.y == direction2->origin.y
         && direction1->origin.z == direction2->origin.z
         && direction1->direction.x == direction2->direction.x
         && direction1->direction.y == direction2->direction.y
         && direction1->direction.z == direction2->direction.z;
}

static GdvDataDirection *
gdv_data_direction_copy (const GdvDataDirection *direction)
{
  GdvDataDirection *result = g_new (GdvDataDirection, 1);

  result->origin.x = direction->origin.x;
  result->origin.y = direction->origin.y;
  result->origin.z = direction->origin.z;
  result->direction.x = direction->direction.x;
  result->direction.y = direction->direction.y;
  result->direction.z = direction->direction.z;

  return result;
}

G_DEFINE_BOXED_TYPE (GdvDataDirection, gdv_data_direction,
                     gdv_data_direction_copy,
                     g_free)

/* Polygons */
gboolean
gdv_data_polygon_equal (const GdvDataPolygon *polygon1,
                        const GdvDataPolygon *polygon2)
{
  return polygon1->point1.x == polygon2->point1.x
         && polygon1->point1.y == polygon2->point1.y
         && polygon1->point1.z == polygon2->point1.z
         && polygon1->point2.x == polygon2->point2.x
         && polygon1->point2.y == polygon2->point2.y
         && polygon1->point2.z == polygon2->point2.z
         && polygon1->point3.x == polygon2->point3.x
         && polygon1->point3.y == polygon2->point3.y
         && polygon1->point3.z == polygon2->point3.z;
}

static GdvDataPolygon *
gdv_data_polygon_copy (const GdvDataPolygon *polygon)
{
  GdvDataPolygon *result = g_new (GdvDataPolygon, 1);

  result->point1.x = polygon->point1.x;
  result->point1.y = polygon->point1.y;
  result->point1.z = polygon->point1.z;
  result->point2.x = polygon->point2.x;
  result->point2.y = polygon->point2.y;
  result->point2.z = polygon->point2.z;
  result->point3.x = polygon->point3.x;
  result->point3.y = polygon->point3.y;
  result->point3.z = polygon->point3.z;

  return result;
}

G_DEFINE_BOXED_TYPE (GdvDataPolygon, gdv_data_polygon,
                     gdv_data_polygon_copy,
                     g_free)

/* Pixel */
gboolean
gdv_data_pixel_equal (const GdvDataPixel *pixel1,
                      const GdvDataPixel *pixel2)
{
  return pixel1->x == pixel2->x
         && pixel1->y == pixel2->y;
}

static GdvDataPixel *
gdv_data_pixel_copy (const GdvDataPixel *pixel)
{
  GdvDataPixel *result = g_new (GdvDataPixel, 1);

  result->x = pixel->x;
  result->y = pixel->y;

  return result;
}

G_DEFINE_BOXED_TYPE (GdvDataPixel, gdv_data_pixel,
                     gdv_data_pixel_copy,
                     g_free)
