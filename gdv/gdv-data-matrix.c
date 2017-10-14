/*
 * gdv-data-matrix.c
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

#include <gsl/gsl_math.h>
#include <gsl/gsl_matrix.h>

#include "gdv/gdv-data-matrix.h"

#define GDV_DATA_MATRIX_GET_PRIVATE(obj)\
 (G_TYPE_INSTANCE_GET_PRIVATE ((obj),\
 GDV_TYPE_DATA_MATRIX,\
 GdvDataMatrixPrivate))

/* Define Properties */
enum
{
  PROP_0,

  PROP_ROWS,
  PROP_COLUMNS,

  N_PROPERTIES
};

struct _GdvDataMatrixPrivate
{
  gsl_matrix *data;

};

G_DEFINE_TYPE (GdvDataMatrix, gdv_data_matrix, G_TYPE_OBJECT);

static void
gdv_data_matrix_init (GdvDataMatrix* data)
{
  data->priv = GDV_DATA_MATRIX_GET_PRIVATE (data);

  /* predefined values */
  data->priv->data = gsl_matrix_alloc (1, 1);
}

static void
gdv_data_matrix_set_property (GObject*      object,
                              guint         property_id,
                              const GValue* value,
                              GParamSpec*   pspec)
{
  GdvDataMatrix* self = GDV_DATA_MATRIX (object);

  switch (property_id)
  {
  case PROP_ROWS:
  {
    gint new_rows;
    new_rows = g_value_get_int(value);

    if (new_rows != self->priv->data->size1)
    {
      gsl_matrix *new_matrix =
        gsl_matrix_calloc (new_rows, self->priv->data->size2);

      gint i, j;

      for (i = 0; i < GSL_MIN_INT(new_rows, self->priv->data->size1); i++)
      {
        for (j = 0; j < self->priv->data->size2; j++)
        {
          gsl_matrix_set (new_matrix, i, j,
                          gsl_matrix_get (self->priv->data, i, j));
        }
      }

      gsl_matrix_free(self->priv->data);
      self->priv->data = new_matrix;
    }
  }
  break;

  case PROP_COLUMNS:
  {
  }
  break;

  default:
    /* unknown property */
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }

}

static void
gdv_data_matrix_get_property (GObject*    object,
                              guint       property_id,
                              GValue*     value,
                              GParamSpec* pspec)
{
  GdvDataMatrix* self = GDV_DATA_MATRIX (object);

  switch (property_id)
  {
  case PROP_ROWS:
    g_value_set_int (value, (gint) self->priv->data->size1);
    break;

  case PROP_COLUMNS:
    g_value_set_int (value, (gint) self->priv->data->size2);
    break;

  default:
    /* unknown property */
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }
}

static void
gdv_data_matrix_dispose (GObject* object)
{
  G_OBJECT_CLASS (gdv_data_matrix_parent_class)->dispose (object);
}

static void
gdv_data_matrix_finalize (GObject* object)
{
  GdvDataMatrix* self = GDV_DATA_MATRIX (object);

  gsl_matrix_free(self->priv->data);

  G_OBJECT_CLASS (gdv_data_matrix_parent_class)->finalize (object);
}

static void
gdv_data_matrix_class_init (GdvDataMatrixClass* klass)
{
  GObjectClass* object_class = G_OBJECT_CLASS (klass);

  object_class->set_property = gdv_data_matrix_set_property;
  object_class->get_property = gdv_data_matrix_get_property;
  object_class->dispose = gdv_data_matrix_dispose;
  object_class->finalize = gdv_data_matrix_finalize;

  g_object_class_install_property (object_class,
                                   PROP_ROWS,
                                   g_param_spec_int ("rows",
                                       "number of rows",
                                       "Holds the number of rows of the matrix",
                                       1,
                                       G_MAXINT,
                                       1,
                                       G_PARAM_READWRITE));

  g_object_class_install_property (object_class,
                                   PROP_COLUMNS,
                                   g_param_spec_int ("columns",
                                       "number of columns",
                                       "Holds the number of columns of the matrix",
                                       1,
                                       G_MAXINT,
                                       1,
                                       G_PARAM_READWRITE));

  g_type_class_add_private (klass, sizeof (GdvDataMatrixPrivate));
}

/* Constructor-method */
GdvDataMatrix *gdv_data_matrix_new (gint rows, gint columns)
{
  GdvDataMatrix *new_matrix;

  new_matrix = GDV_DATA_MATRIX (g_object_new (gdv_data_matrix_get_type(), NULL));

  gsl_matrix_free(new_matrix->priv->data);
  new_matrix->priv->data = gsl_matrix_calloc (rows, columns);

  return new_matrix;
}


gdouble gdv_data_matrix_get (GdvDataMatrix *matrix, gint i, gint j)
{
  gint rows, columns;

  g_return_val_if_fail (GDV_DATA_IS_MATRIX (matrix), 0.0);

  rows = matrix->priv->data->size1;
  columns = matrix->priv->data->size2;
  g_return_val_if_fail (i < rows, 0.0);
  g_return_val_if_fail (i >= 0, 0.0);
  g_return_val_if_fail (j < columns, 0.0);
  g_return_val_if_fail (j >= 0, 0.0);

  return gsl_matrix_get (matrix->priv->data, i, j);
}

void gdv_data_matrix_set (GdvDataMatrix *matrix, gint i, gint j, gdouble x)
{
  gint rows, columns;

  g_return_if_fail (GDV_DATA_IS_MATRIX (matrix));

  rows = matrix->priv->data->size1;
  columns = matrix->priv->data->size2;
  g_return_if_fail (i < rows);
  g_return_if_fail (i >= 0);
  g_return_if_fail (j < columns);
  g_return_if_fail (j >= 0);

  gsl_matrix_set (matrix->priv->data, i, j, x);
}

/* sets or expands the matrix. If the indizes are out of range of the matrix-size, a new matrix will
 * be allocated and stored in the return pointer. If one of the indizes is negativ, the matrix will
 * be shifted by the negative value and the value at zero will be set for the new value. All newly
 * generated values are in general set to zero.
 * If both indizes are within the range of the matrix, the value at the index will be set to the new
 * value and the function will return NULL. */
/**
 * gdv_data_matrix_set_and_expand:
 * @matrix: a #GdvDataMatrix
 * @i: a given row-index
 * @j: a given column-index
 * @x: the value to be set
 *
 * Looks up the given indices. If these are in range, the position will be set to @x, otherwise
 * @matrix will be extended up to @i and @j and the value will be set.
 *
 * The intention for this function was to provide a easy and handy way to extend a given matrix
 * without the danger of memory-leaks etc.. However, it should be applied with care, as it might
 * slow down simple processes significantly.
 *
 * Returns: (nullable) (transfer full): A new copy of @matrix, or #NULL, if no copy was created.
 *
 */
GdvDataMatrix *gdv_data_matrix_set_and_expand (GdvDataMatrix *matrix, gint i, gint j, gdouble x)
{
  gint rows, columns;
  GdvDataMatrix *return_matrix;

  g_return_val_if_fail (GDV_DATA_IS_MATRIX (matrix), NULL);

  rows = matrix->priv->data->size1;
  columns = matrix->priv->data->size2;

//  g_return_if_fail (i >= 0);
//  g_return_if_fail (j >= 0);

  /* catch trivial case */
  if (i < rows &&
      i >= 0 &&
      j < columns &&
      j >= 0)
  {
    gsl_matrix_set (matrix->priv->data, i, j, x);
    return NULL;
  }
  else
  {
    gint i_tmp, j_tmp;

    /* FIXME: thats all well and good, but not speed-optimized */
    return_matrix =
      gdv_data_matrix_new (GSL_MAX_INT (i + 1, rows - GSL_MIN_INT(0, i)),
                           GSL_MAX_INT (j + 1, columns - GSL_MIN_INT(0, j)));

    for (i_tmp = 0; i_tmp < rows; i_tmp++)
    {
      for (j_tmp = 0; j_tmp < columns; j_tmp++)
      {
        gdouble local_val;
        local_val = gsl_matrix_get (matrix->priv->data, i_tmp, j_tmp);
        gsl_matrix_set (
          return_matrix->priv->data,
          i_tmp - GSL_MIN_INT (i, 0),
          j_tmp - GSL_MIN_INT (j, 0),
          local_val);
      }
    }

    gsl_matrix_set (
      return_matrix->priv->data,
      GSL_MAX_INT (0, i),
      GSL_MAX_INT (0, j),
      x);
  }

  return return_matrix;
}


void gdv_data_matrix_set_all (GdvDataMatrix *matrix, gdouble x)
{
  g_return_if_fail (GDV_DATA_IS_MATRIX (matrix));

  gsl_matrix_set_all (matrix->priv->data, x);
}

void gdv_data_matrix_set_zero (GdvDataMatrix *matrix)
{
  g_return_if_fail (GDV_DATA_IS_MATRIX (matrix));

  gsl_matrix_set_zero (matrix->priv->data);
}

void gdv_data_matrix_set_identity (GdvDataMatrix *matrix)
{
  g_return_if_fail (GDV_DATA_IS_MATRIX (matrix));

  return gsl_matrix_set_identity (matrix->priv->data);
}

gint gdv_data_matrix_copy (GdvDataMatrix *dest, GdvDataMatrix *src)
{
  g_return_val_if_fail (GDV_DATA_IS_MATRIX (src), 0);
  g_return_val_if_fail (GDV_DATA_IS_MATRIX (dest), 0);

  return gsl_matrix_memcpy (dest->priv->data, src->priv->data);
}

gint gdv_data_matrix_swap (GdvDataMatrix *v, GdvDataMatrix *w)
{
  g_return_val_if_fail (GDV_DATA_IS_MATRIX (v), 0);
  g_return_val_if_fail (GDV_DATA_IS_MATRIX (w), 0);

  return gsl_matrix_swap (v->priv->data, w->priv->data);
}

/**
 * gdv_data_matrix_get_row:
 * @matrix: a #GdvDataMatrix
 * @i: a given row-index
 *
 * Copies the row given under index @i.
 *
 * Returns: (nullable) (transfer full): A fresh #GdvDataVector with the row-copy.
 *
 */
GdvDataVector *gdv_data_matrix_get_row(GdvDataMatrix * matrix, gint i)
{
  GdvDataVector *new_vector;
  gint rows, columns;
  gint j;

  g_return_val_if_fail (GDV_DATA_IS_MATRIX (matrix), NULL);

  rows = matrix->priv->data->size1;
  columns = matrix->priv->data->size2;
  g_return_val_if_fail (i < rows, NULL);
  g_return_val_if_fail (i >= 0, NULL);

  new_vector = gdv_data_vector_new (columns);

  for (j = 0; j < columns; j++)
  {
    gdv_data_vector_set (
      new_vector,
      j,
      gsl_matrix_get (matrix->priv->data, i, j));
  }

  return new_vector;
}

/**
 * gdv_data_matrix_get_column:
 * @matrix: a #GdvDataMatrix
 * @j: a given column-index
 *
 * Copies the column given under index @i.
 *
 * Returns: (nullable) (transfer full): A fresh #GdvDataVector with the column-copy.
 *
 */
GdvDataVector *gdv_data_matrix_get_column(GdvDataMatrix * matrix, gint j)
{
  GdvDataVector *new_vector;
  gint rows, columns;
  gint i;

  g_return_val_if_fail (GDV_DATA_IS_MATRIX (matrix), NULL);

  rows = matrix->priv->data->size1;
  columns = matrix->priv->data->size2;
  g_return_val_if_fail (j < columns, NULL);
  g_return_val_if_fail (j >= 0, NULL);

  new_vector = gdv_data_vector_new (rows);

  for (i = 0; i < rows; i++)
  {
    gdv_data_vector_set (
      new_vector,
      i,
      gsl_matrix_get (matrix->priv->data, i, j));
  }

  return new_vector;
}

/*
gint gdv_data_matrix_swap_rows (GdvDataMatrix *matrix, gint i, gint j);
gint gdv_data_matrix_swap_columns (GdvDataMatrix *matrix, gint i, gint j);
gint gdv_data_matrix_swap_rowcol (GdvDataMatrix *matrix, gint i, gint j);
gint gdv_data_matrix_transpose_memcpy (GdvDataMatrix *dest, GdvDataMatrix *src);
gint gdv_data_matrix_transpose (GdvDataMatrix *matrix);

*/

/**
 * gdv_data_matrix_memcpy_submatrix:
 * @matrix: a #GdvDataMatrix
 * @min_row: a given lower row-index
 * @max_row: a given upper row-index
 * @min_col: a given lower column-index
 * @max_col: a given upper column-index
 *
 * Copies the whole submatrix under the given boundaries.
 *
 * Returns: (nullable) (transfer full): A fresh #GdvDataMatrix with the submatrix.
 *
 */
GdvDataMatrix *gdv_data_matrix_memcpy_submatrix (
  GdvDataMatrix *matrix,
  gint min_row, gint max_row,
  gint min_col, gint max_col)
{
  GdvDataMatrix *return_matrix;
  gint row_i, col_i;

  g_return_val_if_fail (GDV_DATA_IS_MATRIX (matrix), NULL);
  g_return_val_if_fail (max_row > min_row, NULL);
  g_return_val_if_fail (max_col > min_col, NULL);
  g_return_val_if_fail (min_row >= 0 && min_row < matrix->priv->data->size1, NULL);
  g_return_val_if_fail (max_row > 0 && max_row < matrix->priv->data->size1, NULL);
  g_return_val_if_fail (min_col >= 0 && min_col < matrix->priv->data->size2, NULL);
  g_return_val_if_fail (max_col > 0 && max_col < matrix->priv->data->size2, NULL);

  return_matrix =
    gdv_data_matrix_new ((gint) (max_row - min_row + 1), (gint) (max_col - min_col + 1));

  for (row_i = 0; row_i < return_matrix->priv->data->size1; row_i++)
  {
    for (col_i = 0; col_i < return_matrix->priv->data->size2; col_i++)
    {
      gdv_data_matrix_set (
        return_matrix,
        row_i,
        col_i,
        gsl_matrix_get (matrix->priv->data, row_i + min_row, col_i + min_col));
    }
  }

  return return_matrix;
}

gint gdv_data_matrix_add (GdvDataMatrix * a, const GdvDataMatrix * b)
{
  g_return_val_if_fail (GDV_DATA_IS_MATRIX (b), 0);
  g_return_val_if_fail (GDV_DATA_IS_MATRIX (a), 0);

  return gsl_matrix_add (a->priv->data, b->priv->data);
}

gint gdv_data_matrix_sub (GdvDataMatrix * a, const GdvDataMatrix * b)
{
  g_return_val_if_fail (GDV_DATA_IS_MATRIX (b), 0);
  g_return_val_if_fail (GDV_DATA_IS_MATRIX (a), 0);

  return gsl_matrix_sub (a->priv->data, b->priv->data);
}

gint gdv_data_matrix_mul_elements (GdvDataMatrix * a, const GdvDataMatrix * b)
{
  g_return_val_if_fail (GDV_DATA_IS_MATRIX (b), 0);
  g_return_val_if_fail (GDV_DATA_IS_MATRIX (a), 0);

  return gsl_matrix_mul_elements (a->priv->data, b->priv->data);
}

gint gdv_data_matrix_div_elements (GdvDataMatrix * a, const GdvDataMatrix * b)
{
  g_return_val_if_fail (GDV_DATA_IS_MATRIX (b), 0);
  g_return_val_if_fail (GDV_DATA_IS_MATRIX (a), 0);

  return gsl_matrix_div_elements (a->priv->data, b->priv->data);
}

gint gdv_data_matrix_scale (GdvDataMatrix * a, const gdouble x)
{
  g_return_val_if_fail (GDV_DATA_IS_MATRIX (a), 0);

  return gsl_matrix_scale (a->priv->data, x);
}

gint gdv_data_matrix_add_constant (GdvDataMatrix * a, const gdouble x)
{
  g_return_val_if_fail (GDV_DATA_IS_MATRIX (a), 0);

  return gsl_matrix_add_constant (a->priv->data, x);
}

gdouble gdv_data_matrix_max (const GdvDataMatrix * v)
{
  g_return_val_if_fail (GDV_DATA_IS_MATRIX (v), 0.0);

  return gsl_matrix_max  (v->priv->data);
}

gdouble gdv_data_matrix_min (const GdvDataMatrix * v)
{
  g_return_val_if_fail (GDV_DATA_IS_MATRIX (v), 0.0);

  return gsl_matrix_min  (v->priv->data);
}

void
gdv_data_matrix_minmax (const GdvDataMatrix * v,
                        gdouble * min_out,
                        gdouble * max_out)
{
  g_return_if_fail (GDV_DATA_IS_MATRIX (v));

  gsl_matrix_minmax  (v->priv->data, min_out, max_out);
}

void gdv_data_matrix_max_index (const GdvDataMatrix * v, gint *imax, gint *jmax)
{
  g_return_if_fail (GDV_DATA_IS_MATRIX (v));

  gsl_matrix_max_index  (v->priv->data, (size_t *) imax, (size_t *) jmax);
}

void gdv_data_matrix_min_index (const GdvDataMatrix * v, gint *imin, gint *jmin)
{
  g_return_if_fail (GDV_DATA_IS_MATRIX (v));

  gsl_matrix_min_index  (v->priv->data, (size_t *) imin, (size_t *) jmin);
}

void gdv_data_matrix_minmax_index (const GdvDataMatrix * v, gint * imin, gint * jmin, gint * imax, gint * jmax)
{
  g_return_if_fail (GDV_DATA_IS_MATRIX (v));

  gsl_matrix_minmax_index  (v->priv->data, (size_t *) imin, (size_t *) jmin, (size_t *) imax, (size_t *) jmax);
}

GdvDataMatrix *_gdv_data_matrix_init_from_matrix (gsl_matrix *matrix)
{
  GdvDataMatrix *new_matrix;

  new_matrix = GDV_DATA_MATRIX (g_object_new (gdv_data_matrix_get_type(), NULL));

  gsl_matrix_free(new_matrix->priv->data);
  new_matrix->priv->data = matrix;

  return new_matrix;
}

gsl_matrix *_gdv_data_matrix_get_gsl_matrix (GdvDataMatrix *data)
{
  g_return_val_if_fail (GDV_DATA_IS_MATRIX (data), NULL);

  return data->priv->data;
}
