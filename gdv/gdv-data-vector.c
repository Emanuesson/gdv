/*
 * gdv-data-vector.c
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
#include <gsl/gsl_vector.h>

#include "gdv/gdv-data-vector.h"

#define GDV_DATA_VECTOR_GET_PRIVATE(obj)\
 (G_TYPE_INSTANCE_GET_PRIVATE ((obj),\
 GDV_TYPE_DATA_VECTOR,\
 GdvDataVectorPrivate))

/* Define Properties */
enum
{
  PROP_0,

  PROP_SIZE,

  N_PROPERTIES
};

struct _GdvDataVectorPrivate
{
  gsl_vector *data;
};

G_DEFINE_TYPE (GdvDataVector, gdv_data_vector, G_TYPE_OBJECT);

static void
gdv_data_vector_init (GdvDataVector* data)
{
  data->priv = GDV_DATA_VECTOR_GET_PRIVATE (data);

  /* predefined values */
  data->priv->data = gsl_vector_alloc (1);
}

static void
gdv_data_vector_set_property (GObject*      object,
                              guint         property_id,
                              const GValue* value,
                              GParamSpec*   pspec)
{
  GdvDataVector* self = GDV_DATA_VECTOR (object);

  switch (property_id)
  {
  case PROP_SIZE:
  {
    gint new_size;
    new_size = g_value_get_int(value);

    if (new_size != self->priv->data->size)
    {
      gsl_vector *new_vector = gsl_vector_calloc (new_size);
      gint i;

      for (i = 0; i < GSL_MIN_INT(new_size, self->priv->data->size); i++)
      {
        gsl_vector_set (new_vector, i,
                        gsl_vector_get (self->priv->data, i));
      }

      gsl_vector_free(self->priv->data);
      self->priv->data = new_vector;
    }
  }
  break;

  default:
    /* unknown property */
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }

}

static void
gdv_data_vector_get_property (GObject*    object,
                              guint       property_id,
                              GValue*     value,
                              GParamSpec* pspec)
{
  GdvDataVector* self = GDV_DATA_VECTOR (object);

  switch (property_id)
  {
  case PROP_SIZE:
    g_value_set_int (value, (gint) self->priv->data->size);
    break;

  default:
    /* unknown property */
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }
}

static void
gdv_data_vector_dispose (GObject* object)
{
  G_OBJECT_CLASS (gdv_data_vector_parent_class)->dispose (object);
}

static void
gdv_data_vector_finalize (GObject* object)
{
  GdvDataVector* self = GDV_DATA_VECTOR (object);

  gsl_vector_free(self->priv->data);

  G_OBJECT_CLASS (gdv_data_vector_parent_class)->finalize (object);
}

static void
gdv_data_vector_class_init (GdvDataVectorClass* klass)
{
  GObjectClass* object_class = G_OBJECT_CLASS (klass);

  object_class->set_property = gdv_data_vector_set_property;
  object_class->get_property = gdv_data_vector_get_property;
  object_class->dispose = gdv_data_vector_dispose;
  object_class->finalize = gdv_data_vector_finalize;

  g_object_class_install_property (object_class,
                                   PROP_SIZE,
                                   g_param_spec_int ("size",
                                       "vector size",
                                       "Holds the size of the vector",
                                       1,
                                       G_MAXINT,
                                       1,
                                       G_PARAM_READWRITE));

  g_type_class_add_private (klass, sizeof (GdvDataVectorPrivate));
}

/* Constructor-method */
GdvDataVector *gdv_data_vector_new (gint size)
{
  GdvDataVector *new_vector;

  new_vector = GDV_DATA_VECTOR (g_object_new (gdv_data_vector_get_type(), NULL));

  gsl_vector_free(new_vector->priv->data);
  new_vector->priv->data = gsl_vector_calloc (size);

  return new_vector;
}


gdouble gdv_data_vector_get (GdvDataVector *vector, gint i)
{
  gint size;

  g_return_val_if_fail (GDV_DATA_IS_VECTOR (vector), 0.0);

  size = vector->priv->data->size;
  g_return_val_if_fail (i < size, 0.0);
  g_return_val_if_fail (i >= 0, 0.0);

  return gsl_vector_get (vector->priv->data, i);
}

void gdv_data_vector_set (GdvDataVector *vector, gint i, gdouble x)
{
  gint size;

  g_return_if_fail (GDV_DATA_IS_VECTOR (vector));

  size = vector->priv->data->size;
  g_return_if_fail (i < size);
  g_return_if_fail (i >= 0);

  gsl_vector_set (vector->priv->data, i, x);
}

void gdv_data_vector_set_all (GdvDataVector *vector, gdouble x)
{
  g_return_if_fail (GDV_DATA_IS_VECTOR (vector));

  gsl_vector_set_all (vector->priv->data, x);
}

void gdv_data_vector_set_zero (GdvDataVector *vector)
{
  g_return_if_fail (GDV_DATA_IS_VECTOR (vector));

  gsl_vector_set_zero (vector->priv->data);
}

gint gdv_data_vector_set_basis (GdvDataVector *vector, gint i)
{
  gint size;

  g_return_val_if_fail (GDV_DATA_IS_VECTOR (vector), 0);

  size = vector->priv->data->size;
  g_return_val_if_fail (i < size, 0);
  g_return_val_if_fail (i >= 0, 0);

  return gsl_vector_set_basis (vector->priv->data, i);
}

gint gdv_data_vector_copy (GdvDataVector *dest, GdvDataVector *src)
{
  g_return_val_if_fail (GDV_DATA_IS_VECTOR (src), 0);
  g_return_val_if_fail (GDV_DATA_IS_VECTOR (dest), 0);

  return gsl_vector_memcpy (dest->priv->data, src->priv->data);
}

gint gdv_data_vector_swap (GdvDataVector *v, GdvDataVector *w)
{
  g_return_val_if_fail (GDV_DATA_IS_VECTOR (v), 0);
  g_return_val_if_fail (GDV_DATA_IS_VECTOR (w), 0);

  return gsl_vector_swap (v->priv->data, w->priv->data);
}

gint gdv_data_vector_swap_elements (GdvDataVector *vector, gint i, gint j)
{
  gint size;

  g_return_val_if_fail (GDV_DATA_IS_VECTOR (vector), 0);

  size = vector->priv->data->size;
  g_return_val_if_fail (i < size, 0);
  g_return_val_if_fail (i >= 0, 0);
  g_return_val_if_fail (j < size, 0);
  g_return_val_if_fail (j >= 0, 0);

  return gsl_vector_swap_elements (vector->priv->data, i, j);
}

gint gdv_data_vector_reverse (GdvDataVector *vector)
{
  g_return_val_if_fail (GDV_DATA_IS_VECTOR (vector), 0);

  return gsl_vector_reverse (vector->priv->data);
}

gint gdv_data_vector_add (GdvDataVector * a, const GdvDataVector * b)
{
  g_return_val_if_fail (GDV_DATA_IS_VECTOR (b), 0);
  g_return_val_if_fail (GDV_DATA_IS_VECTOR (a), 0);

  return gsl_vector_add (a->priv->data, b->priv->data);
}

gint gdv_data_vector_sub (GdvDataVector * a, const GdvDataVector * b)
{
  g_return_val_if_fail (GDV_DATA_IS_VECTOR (b), 0);
  g_return_val_if_fail (GDV_DATA_IS_VECTOR (a), 0);

  return gsl_vector_sub (a->priv->data, b->priv->data);
}

gint gdv_data_vector_mul (GdvDataVector * a, const GdvDataVector * b)
{
  g_return_val_if_fail (GDV_DATA_IS_VECTOR (b), 0);
  g_return_val_if_fail (GDV_DATA_IS_VECTOR (a), 0);

  return gsl_vector_mul (a->priv->data, b->priv->data);
}

gint gdv_data_vector_div (GdvDataVector * a, const GdvDataVector * b)
{
  g_return_val_if_fail (GDV_DATA_IS_VECTOR (b), 0);
  g_return_val_if_fail (GDV_DATA_IS_VECTOR (a), 0);

  return gsl_vector_div (a->priv->data, b->priv->data);
}

gint gdv_data_vector_scale (GdvDataVector * a, const gdouble x)
{
  g_return_val_if_fail (GDV_DATA_IS_VECTOR (a), 0);

  return gsl_vector_scale (a->priv->data, x);
}

gint gdv_data_vector_add_constant (GdvDataVector * a, const gdouble x)
{
  g_return_val_if_fail (GDV_DATA_IS_VECTOR (a), 0);

  return gsl_vector_add_constant (a->priv->data, x);
}

gdouble gdv_data_vector_max (const GdvDataVector * v)
{
  g_return_val_if_fail (GDV_DATA_IS_VECTOR (v), 0.0);

  return gsl_vector_max  (v->priv->data);
}

gdouble gdv_data_vector_min (const GdvDataVector * v)
{
  g_return_val_if_fail (GDV_DATA_IS_VECTOR (v), 0.0);

  return gsl_vector_min  (v->priv->data);
}

void
gdv_data_vector_minmax (const GdvDataVector * v,
                        gdouble * min_out,
                        gdouble * max_out)
{
  g_return_if_fail (GDV_DATA_IS_VECTOR (v));

  gsl_vector_minmax  (v->priv->data, min_out, max_out);
}

gint gdv_data_vector_max_index (const GdvDataVector * v)
{
  g_return_val_if_fail (GDV_DATA_IS_VECTOR (v), 0);

  return gsl_vector_max_index  (v->priv->data);
}

gint gdv_data_vector_min_index (const GdvDataVector * v)
{
  g_return_val_if_fail (GDV_DATA_IS_VECTOR (v), 0);

  return gsl_vector_min_index  (v->priv->data);
}

void gdv_data_vector_minmax_index (const GdvDataVector * v, gint * imin, gint * imax)
{
  size_t local_min, local_max;

  g_return_if_fail (GDV_DATA_IS_VECTOR (v));

  gsl_vector_minmax_index  (v->priv->data, &local_min, &local_max);

  *imin = local_min;
  *imax = local_max;
}

GdvDataVector *_gdv_data_vector_init_from_vect (gsl_vector *vector)
{
  GdvDataVector *new_vector;

  GDV_DATA_VECTOR (g_object_new (gdv_data_vector_get_type(), NULL));

  gsl_vector_free(new_vector->priv->data);
  new_vector->priv->data = vector;

  return new_vector;
}

gdouble *_gdv_data_vector_get_array (GdvDataVector *vector)
{
  gsl_vector *gsl_vector;

  g_return_val_if_fail (GDV_DATA_IS_VECTOR (vector), NULL);

  gsl_vector = vector->priv->data;

  return gsl_vector->data;
}
