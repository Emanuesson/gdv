/*
 * epsara-data-matrix.c
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "data/primitive/epsara-data-matrix.h"

#define EPSARA_DATA_MATRIX_GET_PRIVATE(obj)\
 (G_TYPE_INSTANCE_GET_PRIVATE ((obj),\
 EPSARA_TYPE_DATA_MATRIX,\
 EpsaraDataMatrixPrivate))

/* Define Properties */
enum
{
  PROP_0,

  PROP_ROWS,
  PROP_COLUMNS,

  N_PROPERTIES
};

static GParamSpec* data_properties[N_PROPERTIES] = { NULL, };

struct _EpsaraDataMatrixPrivate
{
  gsl_matrix *data;

};

G_DEFINE_TYPE (EpsaraDataMatrix, epsara_data_matrix, G_TYPE_OBJECT);

static void
epsara_data_matrix_init (EpsaraDataMatrix* data)
{
  data->priv = EPSARA_DATA_MATRIX_GET_PRIVATE (data);

  /* predefined values */
  data->priv->data = gsl_matrix_alloc (1, 1);
}

static void
epsara_data_matrix_set_property (GObject*      object,
                                  guint         property_id,
                                  const GValue* value,
                                  GParamSpec*   pspec)
{
  EpsaraDataMatrix* self = EPSARA_DATA_MATRIX (object);

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
      /* TODO: write this function ASAP! */
      g_return_if_reached();
    }
    break;

  default:
    /* unknown property */
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }

}

static void
epsara_data_matrix_get_property (GObject*    object,
                                 guint       property_id,
                                 GValue*     value,
                                 GParamSpec* pspec)
{
  EpsaraDataMatrix* self = EPSARA_DATA_MATRIX (object);

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
epsara_data_matrix_dispose (GObject* object)
{
  EpsaraDataMatrix* self = EPSARA_DATA_MATRIX (object);

  G_OBJECT_CLASS (epsara_data_matrix_parent_class)->dispose (object);
}

static void
epsara_data_matrix_finalize (GObject* object)
{
  EpsaraDataMatrix* self = EPSARA_DATA_MATRIX (object);

  gsl_matrix_free(self->priv->data);

  G_OBJECT_CLASS (epsara_data_matrix_parent_class)->finalize (object);
}

static void
epsara_data_matrix_class_init (EpsaraDataMatrixClass* klass)
{
  GObjectClass* object_class = G_OBJECT_CLASS (klass);

  object_class->set_property = epsara_data_matrix_set_property;
  object_class->get_property = epsara_data_matrix_get_property;
  object_class->dispose = epsara_data_matrix_dispose;
  object_class->finalize = epsara_data_matrix_finalize;

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

  g_type_class_add_private (klass, sizeof (EpsaraDataMatrixPrivate));
}

/* Constructor-method */
EpsaraDataMatrix *epsara_data_matrix_new (gint rows, gint columns)
{
  EpsaraDataMatrix *new_matrix;

  /* TODO: would be nice if 0-initiated matrices were available */
  g_return_val_if_fail (rows, NULL);
  g_return_val_if_fail (columns, NULL);

  new_matrix = EPSARA_DATA_MATRIX (g_object_new (epsara_data_matrix_get_type(), NULL));

  gsl_matrix_free(new_matrix->priv->data);
  new_matrix->priv->data = gsl_matrix_calloc (rows, columns);

  return new_matrix;
}


gdouble epsara_data_matrix_get (EpsaraDataMatrix *matrix, gint i, gint j)
{
  gint rows, columns;

  g_return_val_if_fail (EPSARA_DATA_IS_MATRIX (matrix), 0.0);

  rows = matrix->priv->data->size1;
  columns = matrix->priv->data->size2;
  g_return_val_if_fail (i < rows, 0.0);
  g_return_val_if_fail (i >= 0, 0.0);
  g_return_val_if_fail (j < columns, 0.0);
  g_return_val_if_fail (j >= 0, 0.0);

  return gsl_matrix_get (matrix->priv->data, i, j);
}

void epsara_data_matrix_set (EpsaraDataMatrix *matrix, gint i, gint j, gdouble x)
{
  gint rows, columns;

  g_return_if_fail (EPSARA_DATA_IS_MATRIX (matrix));

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
 * value and the function will return the old matrix. */
/* FIXME: This function require major rework! */
EpsaraDataMatrix *epsara_data_matrix_set_and_expand (EpsaraDataMatrix *matrix, gint i, gint j, gdouble x)
{
  gint rows, columns;
  EpsaraDataMatrix *return_matrix;
  gsl_matrix *new_matrix;

//  g_return_val_if_fail (i < 0, NULL);
//  g_return_val_if_fail (j < 0, NULL);

  if (matrix)
    g_return_val_if_fail (EPSARA_DATA_IS_MATRIX (matrix), NULL);
  else
    matrix = epsara_data_matrix_new (i + 1, j + 1);

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
      return matrix;
    }
  else
    {
      gint i_tmp, j_tmp;

      /* FIXME: thats all well and good, but not speed-optimized */
//      return_matrix =
//        epsara_data_matrix_new (GSL_MAX_INT (i + 1, rows - GSL_MIN_INT(0,i)),
//                                GSL_MAX_INT (j + 1, columns - GSL_MIN_INT(0,j)));

      new_matrix = gsl_matrix_calloc (GSL_MAX_INT (i + 1, rows - GSL_MIN_INT(0,i)),
                                      GSL_MAX_INT (j + 1, columns - GSL_MIN_INT(0,j)));

      for (i_tmp = 0; i_tmp < rows; i_tmp++)
        {
          for (j_tmp = 0; j_tmp < columns; j_tmp++)
            {
              gdouble local_val;
              local_val = gsl_matrix_get (matrix->priv->data, i_tmp, j_tmp);
              gsl_matrix_set (
                new_matrix,
                i_tmp - GSL_MIN_INT (i, 0),
                j_tmp - GSL_MIN_INT (j, 0),
                local_val);
            }
        }
      gsl_matrix_set (
        new_matrix,
        GSL_MAX_INT (0, i),
        GSL_MAX_INT (0, j),
        x);

      gsl_matrix_free(matrix->priv->data);
      matrix->priv->data = new_matrix;
    }

  /* TODO: Emit property::changed-signal, that size- and row-number have changed! */

//  g_object_unref (matrix);
//  matrix = NULL;

  return matrix;
}


void epsara_data_matrix_set_all (EpsaraDataMatrix *matrix, gdouble x)
{
  g_return_if_fail (EPSARA_DATA_IS_MATRIX (matrix));

  gsl_matrix_set_all (matrix->priv->data, x);
}

void epsara_data_matrix_set_zero (EpsaraDataMatrix *matrix)
{
  g_return_if_fail (EPSARA_DATA_IS_MATRIX (matrix));

  gsl_matrix_set_zero (matrix->priv->data);
}

void epsara_data_matrix_set_identity (EpsaraDataMatrix *matrix)
{
  g_return_if_fail (EPSARA_DATA_IS_MATRIX (matrix));

  return gsl_matrix_set_identity (matrix->priv->data);
}

gint epsara_data_matrix_copy (EpsaraDataMatrix *dest, EpsaraDataMatrix *src)
{
  g_return_val_if_fail (EPSARA_DATA_IS_MATRIX (src), 0);
  g_return_val_if_fail (EPSARA_DATA_IS_MATRIX (dest), 0);

  return gsl_matrix_memcpy (dest->priv->data, src->priv->data);
}

gint epsara_data_matrix_swap (EpsaraDataMatrix *v, EpsaraDataMatrix *w)
{
  g_return_val_if_fail (EPSARA_DATA_IS_MATRIX (v), 0);
  g_return_val_if_fail (EPSARA_DATA_IS_MATRIX (w), 0);

  return gsl_matrix_swap (v->priv->data, w->priv->data);
}

EpsaraDataVector *epsara_data_matrix_get_row(EpsaraDataMatrix * matrix, gint i)
{
  EpsaraDataVector *new_vector;
  gint rows, columns;
  gint j;

  g_return_val_if_fail (EPSARA_DATA_IS_MATRIX (matrix), NULL);

  rows = matrix->priv->data->size1;
  columns = matrix->priv->data->size2;
  g_return_val_if_fail (i < rows, NULL);
  g_return_val_if_fail (i >= 0, NULL);

  new_vector = epsara_data_vector_new (columns);

  for (j = 0; j < columns; j++)
  {
    epsara_data_vector_set (
      new_vector,
      j,
      gsl_matrix_get (matrix->priv->data, i, j));
  }

  return new_vector;
}

EpsaraDataVector *epsara_data_matrix_get_column(EpsaraDataMatrix * matrix, gint j)
{
  EpsaraDataVector *new_vector;
  gint rows, columns;
  gint i;

  g_return_val_if_fail (EPSARA_DATA_IS_MATRIX (matrix), NULL);

  rows = matrix->priv->data->size1;
  columns = matrix->priv->data->size2;
  g_return_val_if_fail (j < columns, NULL);
  g_return_val_if_fail (j >= 0, NULL);

  new_vector = epsara_data_vector_new (rows);

  for (i = 0; i < rows; i++)
  {
    epsara_data_vector_set (
      new_vector,
      i,
      gsl_matrix_get (matrix->priv->data, i, j));
  }

  return new_vector;
}

/*
gint epsara_data_matrix_swap_rows (EpsaraDataMatrix *matrix, gint i, gint j);
gint epsara_data_matrix_swap_columns (EpsaraDataMatrix *matrix, gint i, gint j);
gint epsara_data_matrix_swap_rowcol (EpsaraDataMatrix *matrix, gint i, gint j);
gint epsara_data_matrix_transpose_memcpy (EpsaraDataMatrix *dest, EpsaraDataMatrix *src);
gint epsara_data_matrix_transpose (EpsaraDataMatrix *matrix);

*/

EpsaraDataMatrix *epsara_data_matrix_memcpy_submatrix (
  EpsaraDataMatrix *a,
  gint min_row, gint max_row,
  gint min_col, gint max_col)
{
  EpsaraDataMatrix *return_matrix;
  gint row_i, col_i;

  g_return_val_if_fail (EPSARA_DATA_IS_MATRIX (a), NULL);
  g_return_val_if_fail (max_row > min_row, NULL);
  g_return_val_if_fail (max_col > min_col, NULL);
  g_return_val_if_fail (min_row >= 0 && min_row < a->priv->data->size1, NULL);
  g_return_val_if_fail (max_row > 0 && max_row < a->priv->data->size1, NULL);
  g_return_val_if_fail (min_col >= 0 && min_col < a->priv->data->size2, NULL);
  g_return_val_if_fail (max_col > 0 && max_col < a->priv->data->size2, NULL);

  return_matrix =
    epsara_data_matrix_new ((gint) (max_row - min_row + 1), (gint) (max_col - min_col + 1));

  for (row_i = 0; row_i < return_matrix->priv->data->size1; row_i++)
  {
    for (col_i = 0; col_i < return_matrix->priv->data->size2; col_i++)
    {
      epsara_data_matrix_set (
        return_matrix,
        row_i,
        col_i,
        gsl_matrix_get (a->priv->data, row_i + min_row, col_i + min_col));
    }
  }

  return return_matrix;
}

gint epsara_data_matrix_add (EpsaraDataMatrix * a, const EpsaraDataMatrix * b)
{
  g_return_val_if_fail (EPSARA_DATA_IS_MATRIX (b), 0);
  g_return_val_if_fail (EPSARA_DATA_IS_MATRIX (a), 0);

  return gsl_matrix_add (a->priv->data, b->priv->data);
}

gint epsara_data_matrix_sub (EpsaraDataMatrix * a, const EpsaraDataMatrix * b)
{
  g_return_val_if_fail (EPSARA_DATA_IS_MATRIX (b), 0);
  g_return_val_if_fail (EPSARA_DATA_IS_MATRIX (a), 0);

  return gsl_matrix_sub (a->priv->data, b->priv->data);
}

gint epsara_data_matrix_mul_elements (EpsaraDataMatrix * a, const EpsaraDataMatrix * b)
{
  g_return_val_if_fail (EPSARA_DATA_IS_MATRIX (b), 0);
  g_return_val_if_fail (EPSARA_DATA_IS_MATRIX (a), 0);

  return gsl_matrix_mul_elements (a->priv->data, b->priv->data);
}

gint epsara_data_matrix_div_elements (EpsaraDataMatrix * a, const EpsaraDataMatrix * b)
{
  g_return_val_if_fail (EPSARA_DATA_IS_MATRIX (b), 0);
  g_return_val_if_fail (EPSARA_DATA_IS_MATRIX (a), 0);

  return gsl_matrix_div_elements (a->priv->data, b->priv->data);
}

gint epsara_data_matrix_scale (EpsaraDataMatrix * a, const gdouble x)
{
  g_return_val_if_fail (EPSARA_DATA_IS_MATRIX (a), 0);

  return gsl_matrix_scale (a->priv->data, x);
}

gint epsara_data_matrix_add_constant (EpsaraDataMatrix * a, const gdouble x)
{
  g_return_val_if_fail (EPSARA_DATA_IS_MATRIX (a), 0);

  return gsl_matrix_add_constant (a->priv->data, x);
}

gdouble epsara_data_matrix_max (const EpsaraDataMatrix * v)
{
  g_return_val_if_fail (EPSARA_DATA_IS_MATRIX (v), 0.0);

  return gsl_matrix_max  (v->priv->data);
}

gdouble epsara_data_matrix_min (const EpsaraDataMatrix * v)
{
  g_return_val_if_fail (EPSARA_DATA_IS_MATRIX (v), 0.0);

  return gsl_matrix_min  (v->priv->data);
}

void
epsara_data_matrix_minmax (const EpsaraDataMatrix * v,
                           gdouble * min_out,
                           gdouble * max_out)
{
  g_return_if_fail (EPSARA_DATA_IS_MATRIX (v));

  gsl_matrix_minmax  (v->priv->data, min_out, max_out);
}

void epsara_data_matrix_max_index (const EpsaraDataMatrix * v, gint *imax, gint *jmax)
{
  g_return_if_fail (EPSARA_DATA_IS_MATRIX (v));

  gsl_matrix_max_index  (v->priv->data, (size_t *) imax, (size_t *) jmax);
}

void epsara_data_matrix_min_index (const EpsaraDataMatrix * v, gint *imin, gint *jmin)
{
  g_return_if_fail (EPSARA_DATA_IS_MATRIX (v));

  gsl_matrix_min_index  (v->priv->data, (size_t *) imin, (size_t *) jmin);
}

void epsara_data_matrix_minmax_index (const EpsaraDataMatrix * v, gint * imin, gint * jmin, gint * imax, gint * jmax)
{
  g_return_if_fail (EPSARA_DATA_IS_MATRIX (v));

  gsl_matrix_minmax_index  (v->priv->data, (size_t *) imin, (size_t *) jmin, (size_t *) imax, (size_t *) jmax);
}

/**
 * epsara_data_matrix_integrate_column:
 * @matrix: a #EpsaraDataMatrix
 * @peek_column: the column with margin values
 * @integrate_column: the column contianing the function values that will be integrated
 * @sort_matrix: if set to TRUE, the matrix will be sorted along the @peek_column before integration
 * @start_value: the lower margin of the integration
 * @end_value: the upper margin of the integration
 *
 * Integrates the given matrix over the @peek_column. If @sort_matrix is set to TRUE, the matrix
 * will be sorted, before starting the integration. This can be deactivated for performance reasons.
 * However beware, that if the matrix is not sorted along the @peek_column-column, the first
 * appearence of two neighboring values around the @start_value and @stop_value in the @peek_column
 * will be used for the integration.
 * Therfor make sure the matrix is sorted along the @peek_column for a correct result.
 *
 * Returns: the value of the integration
 *
 * Since: 0.1
 */
gdouble epsara_data_matrix_integrate_column (
  EpsaraDataMatrix *matrix,
  gint peek_column,
  gint integrate_column,
  gboolean sort_matrix,
  gdouble start_value, gdouble end_value)
{
  EpsaraDataMatrix *copy_relevant_matrix;
  gint index;
  gdouble return_value;
  gdouble last_peek, last_integrate;
  gdouble lower_margin, upper_margin;

  g_return_val_if_fail (EPSARA_DATA_IS_MATRIX (matrix), 0.0);

  lower_margin = GSL_MIN (start_value, end_value);
  upper_margin = GSL_MAX (start_value, end_value);
  return_value = 0.0;
  copy_relevant_matrix = NULL;
  last_peek = 0.0;
  last_integrate = 0.0;

  if (sort_matrix)
  {
    epsara_data_matrix_sort_along_column (matrix, integrate_column);
  }

  for (index = 0; index < matrix->priv->data->size1; index++)
  {
    gdouble peek_value, integrate_value;

    peek_value = epsara_data_matrix_get (matrix, index, peek_column);
    integrate_value = epsara_data_matrix_get (matrix, index, integrate_column);

    /* This is just a simple linear interpolation! */
    /* FIXME: Make the interpolation-type here optional! */
    if (lower_margin < peek_value && upper_margin > last_peek)
    {
      gdouble lower_border, upper_border;

      lower_border = GSL_MAX (lower_margin, last_peek);
      upper_border = GSL_MIN (upper_margin, peek_value);

      return_value += (
        (last_integrate - integrate_value) * (gsl_pow_2 (upper_border) - gsl_pow_2 (lower_border)) /
        (2.0 * (last_peek - peek_value))) + (upper_border - lower_border) *
        (last_integrate - last_peek * ((last_integrate - integrate_value) / (last_peek - peek_value)));
    }

    last_peek = peek_value;
    last_integrate = integrate_value;
  }

  return return_value;
}

gdouble epsara_data_matrix_get_mean_column_value_near_row (
  EpsaraDataMatrix *matrix,
  gint peek_column,
  gint return_column,
  gint *nearest_row,
  gdouble peek_value)
{
  gint upper_index, lower_index;
  gint upper_index_prev, lower_index_prev;
  gint index;
  gint starting_index;
  gdouble return_value;

  gdouble upper_value_to_peek_prev, upper_value_to_mean_prev;
  gdouble upper_value_to_peek, upper_value_to_mean;
  gdouble lower_value_to_peek_prev, lower_value_to_mean_prev;
  gdouble lower_value_to_peek, lower_value_to_mean;

  g_return_val_if_fail (EPSARA_DATA_IS_MATRIX (matrix), 0.0);

  /* making the boundary check */
  if ((*nearest_row) < 0) starting_index = matrix->priv->data->size1 - 1;
  else starting_index = GSL_MIN ((*nearest_row), matrix->priv->data->size1 - 1);

  upper_index = 0;
  lower_index = 0;
  upper_index_prev = 0;
  lower_index_prev = 0;

  upper_value_to_peek_prev = gsl_matrix_get (matrix->priv->data, starting_index, peek_column);
  upper_value_to_peek = upper_value_to_peek_prev;
  lower_value_to_peek_prev = upper_value_to_peek_prev;
  lower_value_to_peek = upper_value_to_peek_prev;

  upper_value_to_mean_prev = gsl_matrix_get (matrix->priv->data, starting_index, return_column);
  upper_value_to_mean = upper_value_to_mean_prev;
  lower_value_to_mean_prev = upper_value_to_mean_prev;
  lower_value_to_mean = upper_value_to_mean_prev;

  for (index = 0; index <= GSL_MAX (starting_index, matrix->priv->data->size1 - starting_index); index++)
  {
    /* Making boundary-checks again */
    upper_index = GSL_MIN (matrix->priv->data->size1 - 1, starting_index + index);
    lower_index = GSL_MAX (0, starting_index - index);

    /* checking the upper-values */
    if (upper_index != upper_index_prev)
    {
      upper_value_to_peek =
        gsl_matrix_get (matrix->priv->data, upper_index, peek_column);
      upper_value_to_mean =
        gsl_matrix_get (matrix->priv->data, upper_index, return_column);

      if (peek_value >= GSL_MIN (upper_value_to_peek, upper_value_to_peek_prev) &&
          peek_value <= GSL_MAX (upper_value_to_peek, upper_value_to_peek_prev) &&
          upper_value_to_mean != upper_value_to_mean_prev)
      {
        return_value =
          ((upper_value_to_mean_prev - upper_value_to_mean) /
           (upper_value_to_peek_prev - upper_value_to_peek)) *
          (peek_value - upper_value_to_peek_prev) + upper_value_to_mean_prev;
        /* FIXME: this is a littlebit nasty, as there is no exact way to use
         *        whether to  use prev or pres value */
        (*nearest_row) = upper_index_prev;
        return return_value;
      }

      upper_value_to_peek_prev = upper_value_to_peek;
      upper_value_to_mean_prev = upper_value_to_mean;
    }

    /* checking the lower-values */
    if (lower_index != lower_index_prev)
    {
//      lower_value_to_peek = epsara_data_matrix_get (matrix, lower_index, peek_column);
//      lower_value_to_mean = epsara_data_matrix_get (matrix, lower_index, return_column);
      lower_value_to_peek =
        gsl_matrix_get (matrix->priv->data, lower_index, peek_column);
      lower_value_to_mean =
        gsl_matrix_get (matrix->priv->data, lower_index, return_column);

      if (peek_value >= GSL_MIN (lower_value_to_peek, lower_value_to_peek_prev) &&
          peek_value <= GSL_MAX (lower_value_to_peek, lower_value_to_peek_prev) &&
          lower_value_to_mean != lower_value_to_mean_prev)
      {
        return_value =
          ((lower_value_to_mean_prev - lower_value_to_mean) /
           (lower_value_to_peek_prev - lower_value_to_peek)) *
          (peek_value - lower_value_to_peek_prev) + lower_value_to_mean_prev;
        (*nearest_row) = lower_index;
        return return_value;
      }

      lower_value_to_peek_prev = lower_value_to_peek;
      lower_value_to_mean_prev = lower_value_to_mean;
    }

    upper_index_prev = upper_index;
    lower_index_prev = lower_index;
  }

  /* unsuccessful return */
  return 0.0;
}

gdouble epsara_data_matrix_get_mean_column_value (
  EpsaraDataMatrix *matrix,
  gint peek_column,
  gint return_column,
  gboolean sort_matrix,
  gdouble peek_value)
{
  gint index = 0;
  gdouble return_value = 0.0;
  gdouble value_to_peek_prev, value_to_mean_prev;
  gdouble value_to_peek, value_to_mean;

  g_return_val_if_fail (EPSARA_DATA_IS_MATRIX (matrix), 0.0);

  value_to_peek_prev = 0.0;
  value_to_mean_prev = 0.0;
  value_to_peek = 0.0;
  value_to_mean = 0.0;

  if (sort_matrix)
  {
    epsara_data_matrix_sort_along_column (matrix, peek_column);
  }

/*
  for (index = 0; index < matrix->priv->data->size1; index++)
  {
    value_to_peak = epsara_data_matrix_get (matrix, index, peek_column);
    value_to_mean = epsara_data_matrix_get (matrix, index, return_column);

    if (peek_value >= GSL_MIN (value_to_peak, value_to_peak_prev) &&
        peek_value <= GSL_MAX (value_to_peak, value_to_peak_prev))
    {
      return_value =
        ((value_to_mean_prev - value_to_mean) / (value_to_peak_prev - value_to_peak)) *
        (peek_value - value_to_peak_prev) + value_to_mean_prev;
//      g_print ("REACHED: %e-%e-%e ;; %e-%e-%e\n", value_to_peak_prev, peek_value, value_to_peak, value_to_mean_prev, return_value, value_to_mean);
      return return_value;
    }

    value_to_peak_prev = value_to_peak;
    value_to_mean_prev = value_to_mean;
  }
*/

  return_value =
    epsara_data_matrix_get_mean_column_value_near_row (
  matrix,
  peek_column,
  return_column,
  &index,
  peek_value);

  return return_value;
}


/* thanks for http://www.geeksforgeeks.org/3-way-quicksort/ */
/* with some free time, I should also implement:
 * http://permalink.gmane.org/gmane.comp.java.openjdk.core-libs.devel/2628 */
static void quicksort_partition_3_way (
  EpsaraDataMatrix *matrix,
  gint column,
  gint start_index, gint end_index,
  gint *first_end, gint *second_start)
{
  gint pivot_point_lower;
  gint pivot_point_upper;
  gdouble end_value;
  gint local_column_index;
  gint local_row_index;

  *first_end = start_index - 1;
  *second_start = end_index;
  pivot_point_lower = start_index - 1;
  pivot_point_upper = end_index;
  end_value =
    matrix->priv->data->data[end_index * matrix->priv->data->tda + column];
/*
  g_print ("check system0: l:%d, r:%d, i:%d, j:%d, p:%d, q:%d, v:%e\n",
    start_index, end_index, *first_end, *second_start, pivot_point_lower, pivot_point_upper,
    end_value);
*/
  while (TRUE)
  {
   /* From left, find the first element greater than
    * or equal to end_value. This loop will definitely terminate
    * as end_value is last element
    */
    while (matrix->priv->data->data[++(*first_end) * matrix->priv->data->tda + column]
      < end_value);
/*
  g_print ("check system1: l:%d, r:%d, i:%d, j:%d, p:%d, q:%d, v:%e\n",
    start_index, end_index, *first_end, *second_start, pivot_point_lower, pivot_point_upper,
    end_value);
*/
    /* From right, find the first element smaller than or
     * equal to end_value
     */
    while (end_value <
      matrix->priv->data->data[--(*second_start) * matrix->priv->data->tda + column])
      if (*second_start == start_index)
        break;

    /* If first_end and second_start cross, then we are done */
    if (*first_end >= *second_start) break;

    /* Swap, so that smaller goes on left greater goes on right */
    for (local_row_index = 0; local_row_index < matrix->priv->data->size2; local_row_index++)
    {
      gdouble tmp_buffer;
      tmp_buffer =
        matrix->priv->data->data[
          *first_end * matrix->priv->data->tda + local_row_index];
      matrix->priv->data->data[*first_end * matrix->priv->data->tda + local_row_index] =
        matrix->priv->data->data[*second_start * matrix->priv->data->tda + local_row_index];
      matrix->priv->data->data[*second_start * matrix->priv->data->tda + local_row_index] =
        tmp_buffer;
    }

    /* Move all same left occurrence of pivot to beginning of
     * array and keep count using pivot_point_lower */
    if (matrix->priv->data->data[*first_end * matrix->priv->data->tda + column] == end_value)
    {
      pivot_point_lower++;

      for (local_row_index = 0; local_row_index < matrix->priv->data->size2; local_row_index++)
      {
        gdouble tmp_buffer;
        tmp_buffer =
          matrix->priv->data->data[
            pivot_point_lower * matrix->priv->data->tda + local_row_index];
        matrix->priv->data->data[pivot_point_lower * matrix->priv->data->tda + local_row_index] =
          matrix->priv->data->data[*first_end * matrix->priv->data->tda + local_row_index];
        matrix->priv->data->data[*first_end * matrix->priv->data->tda + local_row_index] =
          tmp_buffer;
      }
    }

    /* Move all same right occurrence of pivot to end of array
     * and keep count using pivot_point_upper */
    if (matrix->priv->data->data[*second_start * matrix->priv->data->tda + column] == end_value)
    {
      pivot_point_upper--;

      for (local_row_index = 0; local_row_index < matrix->priv->data->size2; local_row_index++)
      {
        gdouble tmp_buffer;
        tmp_buffer =
          matrix->priv->data->data[
            pivot_point_upper * matrix->priv->data->tda + local_row_index];
        matrix->priv->data->data[pivot_point_upper * matrix->priv->data->tda + local_row_index] =
          matrix->priv->data->data[*second_start * matrix->priv->data->tda + local_row_index];
        matrix->priv->data->data[*second_start * matrix->priv->data->tda + local_row_index] =
          tmp_buffer;
      }
    }
  }

  /* Move pivot element to its correct index */
  for (local_row_index = 0; local_row_index < matrix->priv->data->size2; local_row_index++)
  {
    gdouble tmp_buffer;
    tmp_buffer =
      matrix->priv->data->data[
        *first_end * matrix->priv->data->tda + local_row_index];
    matrix->priv->data->data[*first_end * matrix->priv->data->tda + local_row_index] =
      matrix->priv->data->data[end_index * matrix->priv->data->tda + local_row_index];
    matrix->priv->data->data[end_index * matrix->priv->data->tda + local_row_index] =
      tmp_buffer;
  }

  /* Move all left same occurrences from beginning
   * to adjacent to matrix->priv->data->data[first_end] */
  *second_start = *first_end - 1;
  for (
    local_column_index = start_index;
    local_column_index < pivot_point_lower;
    local_column_index++, (*second_start)--)
  {
    for (local_row_index = 0; local_row_index < matrix->priv->data->size2; local_row_index++)
    {
      gdouble tmp_buffer;
      tmp_buffer =
        matrix->priv->data->data[
          local_column_index * matrix->priv->data->tda + local_row_index];
      matrix->priv->data->data[local_column_index * matrix->priv->data->tda + local_row_index] =
        matrix->priv->data->data[*second_start * matrix->priv->data->tda + local_row_index];
      matrix->priv->data->data[*second_start * matrix->priv->data->tda + local_row_index] =
        tmp_buffer;
    }
  }

  /* Move all right same occurrences from end
   * to adjacent to arr[i] */
  *first_end = *first_end + 1;
  for (
    local_column_index = end_index - 1;
    local_column_index > pivot_point_upper;
    local_column_index--, (*first_end)++)
  {
    for (local_row_index = 0; local_row_index < matrix->priv->data->size2; local_row_index++)
    {
      gdouble tmp_buffer;
      tmp_buffer =
        matrix->priv->data->data[
          *first_end * matrix->priv->data->tda + local_row_index];
      matrix->priv->data->data[*first_end * matrix->priv->data->tda + local_row_index] =
        matrix->priv->data->data[local_column_index * matrix->priv->data->tda + local_row_index];
      matrix->priv->data->data[local_column_index * matrix->priv->data->tda + local_row_index] =
        tmp_buffer;
    }
  }
}

static void sort_along_column_quicksort_3_way (
  EpsaraDataMatrix *matrix,
  gint column, gint start, gint end)
{
  gint i,j;

  if (end <= start) return;

  /* partitionate the matrix */
  quicksort_partition_3_way (
    matrix, column,
    start, end, &i, &j);

  /* sort recursive both parts */
  sort_along_column_quicksort_3_way(
    matrix, column,
    start, j);
  sort_along_column_quicksort_3_way(
    matrix, column,
    i, end);
}

/**
 * epsara_data_matrix_sort_along_column:
 * @matrix: a #EpsaraDataMatrix
 * @column: the column with the values for the sorting
 *
 * Sorts the whole matrix along the @column.
 *
 * Since: 0.1
 */
void epsara_data_matrix_sort_along_column (
  EpsaraDataMatrix *matrix,
  gint column)
{

  g_return_if_fail (EPSARA_DATA_IS_MATRIX (matrix));

  sort_along_column_quicksort_3_way (
    matrix, column, 0, matrix->priv->data->size1 - 1
  );


}

void
epsara_data_matrix_print (EpsaraDataMatrix* matrix)
{
  gint loc_row, loc_col;

  g_return_if_fail (EPSARA_DATA_IS_MATRIX (matrix));

  /* permutting columns */
  for (loc_row = 0; loc_row < (gint)matrix->priv->data->size1; loc_row++)
  {
    /* permutting rows */
    for (loc_col = 0; loc_col < (gint)matrix->priv->data->size2; loc_col++)
    {
      g_print("%12.5e\t",
               epsara_data_matrix_get(matrix,
                                      loc_row,
                                      loc_col));
    }
    g_print ("%s", "\n");
  }

}

/**
 * epsara_data_matrix_sort_along_column:
 * @matrix: a #EpsaraDataMatrix
 * @file: the file to write the matrix to
 *
 * writes the matrix to an asci-file in tab-separated columns.
 *
 * Since: 0.1
 */
void
epsara_data_matrix_save_to_file (EpsaraDataMatrix* matrix,
                                 GFile* file)
{
  gint loc_row, loc_col;
  gchar tmp_save[26];
  FILE* output_stream;

  g_return_if_fail (EPSARA_DATA_IS_MATRIX (matrix));

  output_stream = fopen(g_file_get_path (file), "a+");

  /* permutting columns */
  for (loc_row = 0; loc_row < (gint)matrix->priv->data->size1; loc_row++)
  {
    /* permutting rows */
    for (loc_col = 0; loc_col < (gint)matrix->priv->data->size2; loc_col++)
    {
      snprintf(tmp_save,
               25,
               "%12.5e",
               epsara_data_matrix_get(matrix,
                                      loc_row,
                                      loc_col));
      fprintf(output_stream, "%s\t", tmp_save); /*writes*/

    }
    fprintf(output_stream, "%s", "\n");
  }
  fclose(output_stream);
}


/* Public not exported methods */

static void epsara_data_matrix_sort_along_row (
  EpsaraDataMatrix *matrix,
  gint row)
{
  /* TODO: write this function to! */
  g_return_if_reached();
}

EpsaraDataMatrix *_epsara_data_matrix_init_from_matrix (gsl_matrix *matrix)
{
  EpsaraDataMatrix *new_matrix;

  new_matrix = EPSARA_DATA_MATRIX (g_object_new (epsara_data_matrix_get_type(), NULL));

  gsl_matrix_free(new_matrix->priv->data);
  new_matrix->priv->data = matrix;

  return new_matrix;
}

gsl_matrix *_epsara_data_matrix_get_gsl_matrix (EpsaraDataMatrix *data)
{
  g_return_val_if_fail (EPSARA_DATA_IS_MATRIX (data), NULL);

  return data->priv->data;
}
