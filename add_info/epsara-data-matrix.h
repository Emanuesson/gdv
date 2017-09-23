/*
 * epsara-data-matrix.h
 * This file is part of the particle-scattering framework epsara
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

#ifndef EPSARA_DATA_MATRIX_H_INCLUDED
#define EPSARA_DATA_MATRIX_H_INCLUDED

#include <stdlib.h>
#include <glib.h>
#include <gio/gio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <locale.h>

#include <gsl/gsl_math.h>
#include <gsl/gsl_matrix.h>

#include <data/primitive/epsara-data-vector.h>

G_BEGIN_DECLS

#define EPSARA_TYPE_DATA_MATRIX\
  (epsara_data_matrix_get_type ())
#define EPSARA_DATA_MATRIX(obj)\
  (G_TYPE_CHECK_INSTANCE_CAST ((obj),\
  EPSARA_TYPE_DATA_MATRIX,\
  EpsaraDataMatrix))
#define EPSARA_DATA_IS_MATRIX(obj)\
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),\
  EPSARA_TYPE_DATA_MATRIX))
#define EPSARA_DATA_MATRIX_CLASS(klass)\
  (G_TYPE_CHECK_CLASS_CAST ((klass),\
  EPSARA_TYPE_DATA_MATRIX,\
  EpsaraDataMatrixClass))
#define EPSARA_DATA_MATRIX_IS_CLASS(klass)\
  (G_TYPE_CHECK_CLASS_TYPE ((klass),\
  EPSARA_TYPE_DATA_MATRIX))
#define EPSARA_DATA_MATRIX_GET_CLASS(obj)\
  (G_TYPE_INSTANCE_GET_CLASS ((obj), EPSARA_TYPE_DATA_MATRIX, EpsaraDataMatrixClass))

typedef struct _EpsaraDataMatrix EpsaraDataMatrix;
typedef struct _EpsaraDataMatrixClass EpsaraDataMatrixClass;
typedef struct _EpsaraDataMatrixPrivate EpsaraDataMatrixPrivate;

struct _EpsaraDataMatrix
{
  GObject parent;

  /*< private > */
  EpsaraDataMatrixPrivate* priv;
};

struct _EpsaraDataMatrixClass
{
  GObjectClass parent_class;

  /*< public >*/

};

/* Public Method definitions. */
GType epsara_data_matrix_get_type (void) ;

EpsaraDataMatrix *epsara_data_matrix_new (gint rows, gint columns);

gdouble epsara_data_matrix_get (EpsaraDataMatrix *matrix, gint i, gint j);
void epsara_data_matrix_set (EpsaraDataMatrix *matrix, gint i, gint j, gdouble x);
EpsaraDataMatrix *epsara_data_matrix_set_and_expand (EpsaraDataMatrix *matrix, gint i, gint j, gdouble x);

void epsara_data_matrix_set_all (EpsaraDataMatrix *matrix, gdouble x);
void epsara_data_matrix_set_zero (EpsaraDataMatrix *matrix);
void epsara_data_matrix_set_identity (EpsaraDataMatrix *matrix);

gint epsara_data_matrix_copy (EpsaraDataMatrix *dest, EpsaraDataMatrix *src);
gint epsara_data_matrix_swap (EpsaraDataMatrix *v, EpsaraDataMatrix *w);

EpsaraDataVector *epsara_data_matrix_get_row(EpsaraDataMatrix * matrix, gint i);
EpsaraDataVector *epsara_data_matrix_get_column(EpsaraDataMatrix * matrix, gint j);

/*
 * gint epsara_data_matrix_swap_rows (EpsaraDataMatrix *matrix, gint i, gint j);
 * gint epsara_data_matrix_swap_columns (EpsaraDataMatrix *matrix, gint i, gint j);
 * gint epsara_data_matrix_swap_rowcol (EpsaraDataMatrix *matrix, gint i, gint j);
 * gint epsara_data_matrix_transpose_memcpy (EpsaraDataMatrix *dest, EpsaraDataMatrix *src);
 * gint epsara_data_matrix_transpose (EpsaraDataMatrix *matrix);
 */

EpsaraDataMatrix *epsara_data_matrix_memcpy_submatrix (
  EpsaraDataMatrix *a,
  gint min_row, gint max_row,
  gint min_col, gint max_col);

gint epsara_data_matrix_add (EpsaraDataMatrix * a, const EpsaraDataMatrix * b);
gint epsara_data_matrix_sub (EpsaraDataMatrix * a, const EpsaraDataMatrix * b);
gint epsara_data_matrix_mul_elements (
  EpsaraDataMatrix * a,
  const EpsaraDataMatrix * b);
gint epsara_data_matrix_div_elements (
  EpsaraDataMatrix * a,
  const EpsaraDataMatrix * b);
gint epsara_data_matrix_scale (EpsaraDataMatrix * a, const gdouble x);
gint epsara_data_matrix_add_constant (EpsaraDataMatrix * a, const gdouble x);

gdouble epsara_data_matrix_max (const EpsaraDataMatrix * v);
gdouble epsara_data_matrix_min (const EpsaraDataMatrix * v);
void epsara_data_matrix_minmax (
  const EpsaraDataMatrix * v,
  gdouble * min_out,
  gdouble * max_out);
void epsara_data_matrix_max_index (
  const EpsaraDataMatrix * v,
  gint *imax,
  gint *jmax);
void epsara_data_matrix_min_index (
  const EpsaraDataMatrix * v,
  gint *imin,
  gint *jmin);
void epsara_data_matrix_minmax_index (
  const EpsaraDataMatrix * v,
  gint * imin,
  gint * jmin,
  gint * imax,
  gint * jmax);

gdouble epsara_data_matrix_integrate_column (
  EpsaraDataMatrix *matrix,
  gint peek_column,
  gint integrate_column,
  gboolean sort_matrix,
  gdouble start_value, gdouble end_value);

gdouble epsara_data_matrix_get_mean_column_value_near_row (
  EpsaraDataMatrix *matrix,
  gint peek_column,
  gint return_column,
  gint *nearest_row,
  gdouble peek_value);
gdouble epsara_data_matrix_get_mean_column_value (
  EpsaraDataMatrix *matrix,
  gint peek_column,
  gint return_column,
  gboolean sort_matrix,
  gdouble peek_value);

void epsara_data_matrix_sort_along_column (
  EpsaraDataMatrix *matrix,
  gint column);

//void epsara_data_matrix_sort_along_row (
//  EpsaraDataMatrix *matrix,
//  gint row);

void
epsara_data_matrix_print (EpsaraDataMatrix* matrix);

void
epsara_data_matrix_save_to_file (EpsaraDataMatrix* matrix,
                                 GFile* file);

/* Public not exported methods */
EpsaraDataMatrix *_epsara_data_matrix_init_from_matrix (gsl_matrix *matrix);
gsl_matrix *_epsara_data_matrix_get_gsl_matrix (EpsaraDataMatrix *data);

G_END_DECLS

#endif // EPSARA_DATA_MATRIX_H_INCLUDED

