/*
 * gdv-data-vector.h
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

#ifndef GDV_DATA_VECTOR_H_INCLUDED
#define GDV_DATA_VECTOR_H_INCLUDED

#include <stdlib.h>
#include <glib.h>
#include <gio/gio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <locale.h>

G_BEGIN_DECLS

#define GDV_TYPE_DATA_VECTOR\
  (gdv_data_vector_get_type ())
#define GDV_DATA_VECTOR(obj)\
  (G_TYPE_CHECK_INSTANCE_CAST ((obj),\
  GDV_TYPE_DATA_VECTOR,\
  GdvDataVector))
#define GDV_DATA_IS_VECTOR(obj)\
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),\
  GDV_TYPE_DATA_VECTOR))
#define GDV_DATA_VECTOR_CLASS(klass)\
  (G_TYPE_CHECK_CLASS_CAST ((klass),\
  GDV_TYPE_DATA_VECTOR,\
  GdvDataVectorClass))
#define GDV_DATA_VECTOR_IS_CLASS(klass)\
  (G_TYPE_CHECK_CLASS_TYPE ((klass),\
  GDV_TYPE_DATA_VECTOR))
#define GDV_DATA_VECTOR_GET_CLASS(obj)\
  (G_TYPE_INSTANCE_GET_CLASS ((obj), GDV_TYPE_DATA_VECTOR, GdvDataVectorClass))

typedef struct _GdvDataVector GdvDataVector;
typedef struct _GdvDataVectorClass GdvDataVectorClass;
typedef struct _GdvDataVectorPrivate GdvDataVectorPrivate;

struct _GdvDataVector
{
  GObject parent;

  /*< private > */
  GdvDataVectorPrivate* priv;
};

struct _GdvDataVectorClass
{
  GObjectClass parent_class;

  /*< public >*/

};

/* Public Method definitions. */
GType gdv_data_vector_get_type (void) ;

GdvDataVector *gdv_data_vector_new (gint size);

gdouble gdv_data_vector_get (GdvDataVector *vector, gint i);
void gdv_data_vector_set (GdvDataVector *vector, gint i, gdouble x);

void gdv_data_vector_set_all (GdvDataVector *vector, gdouble x);
void gdv_data_vector_set_zero (GdvDataVector *vector);

gint gdv_data_vector_set_basis (GdvDataVector *vector, gint i);

gint gdv_data_vector_copy (GdvDataVector *dest, GdvDataVector *src);
gint gdv_data_vector_swap (GdvDataVector *v, GdvDataVector *w);

gint gdv_data_vector_swap_elements (GdvDataVector *vector, gint i, gint j);
gint gdv_data_vector_reverse (GdvDataVector *vector);

gint gdv_data_vector_add (GdvDataVector * a, const GdvDataVector * b);
gint gdv_data_vector_sub (GdvDataVector * a, const GdvDataVector * b);
gint gdv_data_vector_mul (GdvDataVector * a, const GdvDataVector * b);
gint gdv_data_vector_div (GdvDataVector * a, const GdvDataVector * b);
gint gdv_data_vector_scale (GdvDataVector * a, const gdouble x);
gint gdv_data_vector_add_constant (GdvDataVector * a, const gdouble x);

gdouble gdv_data_vector_max (const GdvDataVector * v);
gdouble gdv_data_vector_min (const GdvDataVector * v);
void gdv_data_vector_minmax (const GdvDataVector * v, gdouble * min_out, gdouble * max_out);
gint gdv_data_vector_max_index (const GdvDataVector * v);
gint gdv_data_vector_min_index (const GdvDataVector * v);
void gdv_data_vector_minmax_index (const GdvDataVector * v, gint * imin, gint * imax);

/* Public not exported methods */
/*
GdvDataVector *_gdv_data_vector_init_from_vect (gsl_vector *vector);
gdouble *_gdv_data_vector_get_array (GdvDataVector *vector);
*/

G_END_DECLS

#endif // GDV_DATA_VECTOR_H_INCLUDED

