/*
 * gdv-data-geometric.h
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

#ifndef GDV_DATA_POINT_H_INCLUDED
#define GDV_DATA_POINT_H_INCLUDED

#include <stdlib.h>
#include <glib.h>
#include <gio/gio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <locale.h>

G_BEGIN_DECLS

/* structs */
struct _GdvDataPoint
{
  gdouble x;
  gdouble y;
  gdouble z;
};

typedef struct _GdvDataPoint GdvDataPoint;

struct _GdvDataPointSingleExtended
{
  GdvDataPoint *point;
  GdvDataPoint *neg_range_1;
  GdvDataPoint *pos_range_1;
};

typedef struct _GdvDataPointSingleExtended GdvDataPointSingleExtended;

struct _GdvDataPointDoubleExtended
{
  GdvDataPointSingleExtended *point;
  GdvDataPoint *neg_range_2;
  GdvDataPoint *pos_range_2;
};

typedef struct _GdvDataPointDoubleExtended GdvDataPointDoubleExtended;

struct _GdvDataNPoint
{
  guint N;
  gdouble *x;
};

typedef struct _GdvDataNPoint GdvDataNPoint;

struct _GdvDataDirection
{
  GdvDataPoint origin;
  GdvDataPoint direction;
};

typedef struct _GdvDataDirection GdvDataDirection;

/* FIXME: this needs to get better - it is not a good implementation for a Mesh */
struct _GdvDataPolygon
{
  GdvDataPoint point1;
  GdvDataPoint point2;
  GdvDataPoint point3;
};

typedef struct _GdvDataPolygon GdvDataPolygon;

struct _GdvDataPixel
{
  gint x;
  gint y;
};

typedef struct _GdvDataPixel GdvDataPixel;

/* Public functions */
gboolean gdv_data_point_equal (const GdvDataPoint *point1,
                               const GdvDataPoint *point2);
gdouble gdv_data_point_metric (const GdvDataPoint *point1,
                               const GdvDataPoint *point2);
gdouble gdv_data_point_distance_to_origin    (const GdvDataPoint *point);
GType gdv_data_point_get_type (void) G_GNUC_CONST;

gboolean gdv_data_n_point_equal (const GdvDataNPoint *point1,
                                 const GdvDataNPoint *point2);
gdouble gdv_data_n_point_metric (const GdvDataNPoint *point1,
                                 const GdvDataNPoint *point2);
gdouble gdv_data_n_point_get (const GdvDataNPoint *point1,
                              const guint n);
void gdv_data_n_point_set (GdvDataNPoint *point1,
                           const guint n,
                           gdouble x);
GdvDataNPoint *gdv_data_n_point_set_and_expand (GdvDataNPoint *point1,
    const guint n,
    gdouble x);
gdouble gdv_data_n_point_distance_to_origin (const GdvDataNPoint *point);
GType gdv_data_n_point_get_type (void) G_GNUC_CONST;

gboolean gdv_data_direction_equal     (const GdvDataDirection *direction1,
                                       const GdvDataDirection *direction2);

GType gdv_data_direction_get_type (void) G_GNUC_CONST;

gboolean gdv_data_polygon_equal     (const GdvDataPolygon *polygon1,
                                     const GdvDataPolygon *polygon2);

GType gdv_data_polygon_get_type (void) G_GNUC_CONST;

gboolean gdv_data_pixel_equal     (const GdvDataPixel *pixel1,
                                   const GdvDataPixel *pixel2);

GType gdv_data_pixel_get_type (void) G_GNUC_CONST;

/* Type definitions */
#define GDV_TYPE_DATA_POINT\
  (gdv_data_point_get_type ())
#define GDV_TYPE_DATA_POINT_SINGLE_EXTENDED\
  (gdv_data_point_single_extended_get_type ())
#define GDV_TYPE_DATA_POINT_DOUBLE_EXTENDED\
  (gdv_data_point_double_extended_get_type ())
#define GDV_TYPE_DATA_N_POINT\
  (gdv_data_n_point_get_type ())
#define GDV_TYPE_DATA_DIRECTION\
  (gdv_data_direction_get_type ())
#define GDV_TYPE_DATA_POLYGON\
  (gdv_data_polygon_get_type ())
#define GDV_TYPE_DATA_PIXEL\
  (gdv_data_pixel_get_type ())

G_END_DECLS

#endif // GDV_DATA_POINT_H_INCLUDED

