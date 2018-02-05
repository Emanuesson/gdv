/*
 * gdv-data-matrix.h
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

#ifndef GDV_DATA_MATRIX_H_INCLUDED
#define GDV_DATA_MATRIX_H_INCLUDED

#include <stdlib.h>
#include <glib.h>
#include <gio/gio.h>
#include <string.h>
#include <unistd.h>
#include <locale.h>

#include <gdv/gdv-data-vector.h>

G_BEGIN_DECLS

#define GDV_TYPE_DATA_MATRIX\
  (gdv_data_matrix_get_type ())
#define GDV_DATA_MATRIX(obj)\
  (G_TYPE_CHECK_INSTANCE_CAST ((obj),\
  GDV_TYPE_DATA_MATRIX,\
  GdvDataMatrix))
#define GDV_DATA_IS_MATRIX(obj)\
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),\
  GDV_TYPE_DATA_MATRIX))
#define GDV_DATA_MATRIX_CLASS(klass)\
  (G_TYPE_CHECK_CLASS_CAST ((klass),\
  GDV_TYPE_DATA_MATRIX,\
  GdvDataMatrixClass))
#define GDV_DATA_MATRIX_IS_CLASS(klass)\
  (G_TYPE_CHECK_CLASS_TYPE ((klass),\
  GDV_TYPE_DATA_MATRIX))
#define GDV_DATA_MATRIX_GET_CLASS(obj)\
  (G_TYPE_INSTANCE_GET_CLASS ((obj), GDV_TYPE_DATA_MATRIX, GdvDataMatrixClass))

typedef struct _GdvDataMatrix GdvDataMatrix;
typedef struct _GdvDataMatrixClass GdvDataMatrixClass;
typedef struct _GdvDataMatrixPrivate GdvDataMatrixPrivate;

struct _GdvDataMatrix
{
  GObject parent;

  /*< private > */
  GdvDataMatrixPrivate* priv;
};

struct _GdvDataMatrixClass
{
  GObjectClass parent_class;

  /*< public >*/

};

/* Public Method definitions. */
GType gdv_data_matrix_get_type (void) ;

GdvDataMatrix *gdv_data_matrix_new (gint rows, gint columns);

gdouble gdv_data_matrix_get (GdvDataMatrix *matrix, gint i, gint j);
void gdv_data_matrix_set (GdvDataMatrix *matrix, gint i, gint j, gdouble x);
GdvDataMatrix *gdv_data_matrix_set_and_expand (GdvDataMatrix *matrix, gint i, gint j, gdouble x);

void gdv_data_matrix_set_all (GdvDataMatrix *matrix, gdouble x);
void gdv_data_matrix_set_zero (GdvDataMatrix *matrix);
void gdv_data_matrix_set_identity (GdvDataMatrix *matrix);

gint gdv_data_matrix_copy (GdvDataMatrix *dest, GdvDataMatrix *src);
gint gdv_data_matrix_swap (GdvDataMatrix *v, GdvDataMatrix *w);

GdvDataVector *gdv_data_matrix_get_row(GdvDataMatrix * matrix, gint i);
GdvDataVector *gdv_data_matrix_get_column(GdvDataMatrix * matrix, gint j);

/*
 * gint gdv_data_matrix_swap_rows (GdvDataMatrix *matrix, gint i, gint j);
 * gint gdv_data_matrix_swap_columns (GdvDataMatrix *matrix, gint i, gint j);
 * gint gdv_data_matrix_swap_rowcol (GdvDataMatrix *matrix, gint i, gint j);
 * gint gdv_data_matrix_transpose_memcpy (GdvDataMatrix *dest, GdvDataMatrix *src);
 * gint gdv_data_matrix_transpose (GdvDataMatrix *matrix);
 */

GdvDataMatrix *gdv_data_matrix_memcpy_submatrix (
  GdvDataMatrix *matrix,
  gint min_row, gint max_row,
  gint min_col, gint max_col);

gint gdv_data_matrix_add (GdvDataMatrix * a, const GdvDataMatrix * b);
gint gdv_data_matrix_sub (GdvDataMatrix * a, const GdvDataMatrix * b);
gint gdv_data_matrix_mul_elements (
  GdvDataMatrix * a,
  const GdvDataMatrix * b);
gint gdv_data_matrix_div_elements (
  GdvDataMatrix * a,
  const GdvDataMatrix * b);
gint gdv_data_matrix_scale (GdvDataMatrix * a, const gdouble x);
gint gdv_data_matrix_add_constant (GdvDataMatrix * a, const gdouble x);

gdouble gdv_data_matrix_max (const GdvDataMatrix * v);
gdouble gdv_data_matrix_min (const GdvDataMatrix * v);
void gdv_data_matrix_minmax (
  const GdvDataMatrix * v,
  gdouble * min_out,
  gdouble * max_out);
void gdv_data_matrix_max_index (
  const GdvDataMatrix * v,
  gint *imax,
  gint *jmax);
void gdv_data_matrix_min_index (
  const GdvDataMatrix * v,
  gint *imin,
  gint *jmin);
void gdv_data_matrix_minmax_index (
  const GdvDataMatrix * v,
  gint * imin,
  gint * jmin,
  gint * imax,
  gint * jmax);

/* Public not exported methods */
/*
GdvDataMatrix *_gdv_data_matrix_init_from_matrix (gsl_matrix *matrix);
gsl_matrix *_gdv_data_matrix_get_gsl_matrix (GdvDataMatrix *data);
*/

G_END_DECLS

#endif // GDV_DATA_MATRIX_H_INCLUDED

