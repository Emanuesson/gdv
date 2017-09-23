/*
 * epsara-data-vector.c
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

#include "data/primitive/epsara-data-vector.h"

#define EPSARA_DATA_VECTOR_GET_PRIVATE(obj)\
 (G_TYPE_INSTANCE_GET_PRIVATE ((obj),\
 EPSARA_TYPE_DATA_VECTOR,\
 EpsaraDataVectorPrivate))

/* Define Properties */
enum
{
  PROP_0,

  PROP_SIZE,

  N_PROPERTIES
};

static GParamSpec* data_properties[N_PROPERTIES] = { NULL, };

struct _EpsaraDataVectorPrivate
{
  gsl_vector *data;

};

G_DEFINE_TYPE (EpsaraDataVector, epsara_data_vector, G_TYPE_OBJECT);

static void
epsara_data_vector_init (EpsaraDataVector* data)
{
  data->priv = EPSARA_DATA_VECTOR_GET_PRIVATE (data);

  /* predefined values */
  data->priv->data = gsl_vector_alloc (1);
}

static void
epsara_data_vector_set_property (GObject*      object,
                                  guint         property_id,
                                  const GValue* value,
                                  GParamSpec*   pspec)
{
  EpsaraDataVector* self = EPSARA_DATA_VECTOR (object);

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
epsara_data_vector_get_property (GObject*    object,
                                 guint       property_id,
                                 GValue*     value,
                                 GParamSpec* pspec)
{
  EpsaraDataVector* self = EPSARA_DATA_VECTOR (object);

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
epsara_data_vector_dispose (GObject* object)
{
  EpsaraDataVector* self = EPSARA_DATA_VECTOR (object);

  G_OBJECT_CLASS (epsara_data_vector_parent_class)->dispose (object);
}

static void
epsara_data_vector_finalize (GObject* object)
{
  EpsaraDataVector* self = EPSARA_DATA_VECTOR (object);

  gsl_vector_free(self->priv->data);

  G_OBJECT_CLASS (epsara_data_vector_parent_class)->finalize (object);
}

static void
epsara_data_vector_class_init (EpsaraDataVectorClass* klass)
{
  GObjectClass* object_class = G_OBJECT_CLASS (klass);

  object_class->set_property = epsara_data_vector_set_property;
  object_class->get_property = epsara_data_vector_get_property;
  object_class->dispose = epsara_data_vector_dispose;
  object_class->finalize = epsara_data_vector_finalize;

  g_object_class_install_property (object_class, 
    PROP_SIZE,
    g_param_spec_int ("size",
                      "vector size",
                      "Holds the size of the vector",
                      1,
                      G_MAXINT,
                      1,
                      G_PARAM_READWRITE));

  g_type_class_add_private (klass, sizeof (EpsaraDataVectorPrivate));
}

/* Constructor-method */
EpsaraDataVector *epsara_data_vector_new (gint size)
{
  EpsaraDataVector *new_vector;

  new_vector = EPSARA_DATA_VECTOR (g_object_new (epsara_data_vector_get_type(), NULL));

  gsl_vector_free(new_vector->priv->data);
  new_vector->priv->data = gsl_vector_calloc (size);

  return new_vector;
}


gdouble epsara_data_vector_get (EpsaraDataVector *vector, gint i)
{
  gint size;

  g_return_val_if_fail (EPSARA_DATA_IS_VECTOR (vector), 0.0);

  size = vector->priv->data->size;
  g_return_val_if_fail (i < size, 0.0);
  g_return_val_if_fail (i >= 0, 0.0);

  return gsl_vector_get (vector->priv->data, i);
}

void epsara_data_vector_set (EpsaraDataVector *vector, gint i, gdouble x)
{
  gint size;

  g_return_if_fail (EPSARA_DATA_IS_VECTOR (vector));

  size = vector->priv->data->size;
  g_return_if_fail (i < size);
  g_return_if_fail (i >= 0);

  gsl_vector_set (vector->priv->data, i, x);
}

EpsaraDataVector *epsara_data_vector_set_and_expand (EpsaraDataVector *vector, gint i, gdouble x)
{
  gint size;
  EpsaraDataVector *return_vector;

  g_return_val_if_fail (EPSARA_DATA_IS_VECTOR (vector), NULL);

  size = vector->priv->data->size;

//  g_return_if_fail (i >= 0);
//  g_return_if_fail (j >= 0);

  /* catch trivial case */
  if (i < size &&
      i >= 0)
    {
      gsl_vector_set (vector->priv->data, i, x);
      return vector;
    }
  else
    {
      gint i_tmp;

      /* FIXME: thats all well and good, but not speed-optimized */
      return_vector =
        epsara_data_vector_new (GSL_MAX_INT (i + 1, size - GSL_MIN_INT(0,i)));

      for (i_tmp = 0; i_tmp < size; i_tmp++)
        {
              gdouble local_val;
              local_val = gsl_vector_get (vector->priv->data, i_tmp);
              gsl_vector_set (
                return_vector->priv->data,
                i_tmp - GSL_MIN_INT (i, 0),
                local_val);
        }
      gsl_vector_set (
        return_vector->priv->data,
        GSL_MAX_INT (0, i),
        x);
    }

  g_object_unref (vector);

  return return_vector;

}

void epsara_data_vector_set_all (EpsaraDataVector *vector, gdouble x)
{
  g_return_if_fail (EPSARA_DATA_IS_VECTOR (vector));

  gsl_vector_set_all (vector->priv->data, x);
}

void epsara_data_vector_set_zero (EpsaraDataVector *vector)
{
  g_return_if_fail (EPSARA_DATA_IS_VECTOR (vector));

  gsl_vector_set_zero (vector->priv->data);
}

gint epsara_data_vector_set_basis (EpsaraDataVector *vector, gint i)
{
  gint size;

  g_return_val_if_fail (EPSARA_DATA_IS_VECTOR (vector), 0);

  size = vector->priv->data->size;
  g_return_val_if_fail (i < size, 0);
  g_return_val_if_fail (i >= 0, 0);

  return gsl_vector_set_basis (vector->priv->data, i);
}

gint epsara_data_vector_copy (EpsaraDataVector *dest, EpsaraDataVector *src)
{
  g_return_val_if_fail (EPSARA_DATA_IS_VECTOR (src), 0);
  g_return_val_if_fail (EPSARA_DATA_IS_VECTOR (dest), 0);

  return gsl_vector_memcpy (dest->priv->data, src->priv->data);
}

gint epsara_data_vector_swap (EpsaraDataVector *v, EpsaraDataVector *w)
{
  g_return_val_if_fail (EPSARA_DATA_IS_VECTOR (v), 0);
  g_return_val_if_fail (EPSARA_DATA_IS_VECTOR (w), 0);

  return gsl_vector_swap (v->priv->data, w->priv->data);
}

gint epsara_data_vector_swap_elements (EpsaraDataVector *vector, gint i, gint j)
{
  gint size;

  g_return_val_if_fail (EPSARA_DATA_IS_VECTOR (vector), 0);

  size = vector->priv->data->size;
  g_return_val_if_fail (i < size, 0);
  g_return_val_if_fail (i >= 0, 0);
  g_return_val_if_fail (j < size, 0);
  g_return_val_if_fail (j >= 0, 0);

  return gsl_vector_swap_elements (vector->priv->data, i, j);
}

gint epsara_data_vector_reverse (EpsaraDataVector *vector)
{
  g_return_val_if_fail (EPSARA_DATA_IS_VECTOR (vector), 0);

  return gsl_vector_reverse (vector->priv->data);
}

gint epsara_data_vector_add (EpsaraDataVector * a, const EpsaraDataVector * b)
{
  g_return_val_if_fail (EPSARA_DATA_IS_VECTOR (b), 0);
  g_return_val_if_fail (EPSARA_DATA_IS_VECTOR (a), 0);

  return gsl_vector_add (a->priv->data, b->priv->data);
}

gint epsara_data_vector_sub (EpsaraDataVector * a, const EpsaraDataVector * b)
{
  g_return_val_if_fail (EPSARA_DATA_IS_VECTOR (b), 0);
  g_return_val_if_fail (EPSARA_DATA_IS_VECTOR (a), 0);

  return gsl_vector_sub (a->priv->data, b->priv->data);
}

gint epsara_data_vector_mul (EpsaraDataVector * a, const EpsaraDataVector * b)
{
  g_return_val_if_fail (EPSARA_DATA_IS_VECTOR (b), 0);
  g_return_val_if_fail (EPSARA_DATA_IS_VECTOR (a), 0);

  return gsl_vector_mul (a->priv->data, b->priv->data);
}

gint epsara_data_vector_div (EpsaraDataVector * a, const EpsaraDataVector * b)
{
  g_return_val_if_fail (EPSARA_DATA_IS_VECTOR (b), 0);
  g_return_val_if_fail (EPSARA_DATA_IS_VECTOR (a), 0);

  return gsl_vector_div (a->priv->data, b->priv->data);
}

gint epsara_data_vector_scale (EpsaraDataVector * a, const gdouble x)
{
  g_return_val_if_fail (EPSARA_DATA_IS_VECTOR (a), 0);

  return gsl_vector_scale (a->priv->data, x);
}

gint epsara_data_vector_add_constant (EpsaraDataVector * a, const gdouble x)
{
  g_return_val_if_fail (EPSARA_DATA_IS_VECTOR (a), 0);

  return gsl_vector_add_constant (a->priv->data, x);
}

gdouble epsara_data_vector_max (const EpsaraDataVector * v)
{
  g_return_val_if_fail (EPSARA_DATA_IS_VECTOR (v), 0.0);

  return gsl_vector_max  (v->priv->data);
}

gdouble epsara_data_vector_min (const EpsaraDataVector * v)
{
  g_return_val_if_fail (EPSARA_DATA_IS_VECTOR (v), 0.0);

  return gsl_vector_min  (v->priv->data);
}

void
epsara_data_vector_minmax (const EpsaraDataVector * v,
                           gdouble * min_out,
                           gdouble * max_out)
{
  g_return_if_fail (EPSARA_DATA_IS_VECTOR (v));

  gsl_vector_minmax  (v->priv->data, min_out, max_out);
}

gint epsara_data_vector_max_index (const EpsaraDataVector * v)
{
  g_return_val_if_fail (EPSARA_DATA_IS_VECTOR (v), 0);

  return gsl_vector_max_index  (v->priv->data);
}

gint epsara_data_vector_min_index (const EpsaraDataVector * v)
{
  g_return_val_if_fail (EPSARA_DATA_IS_VECTOR (v), 0);

  return gsl_vector_min_index  (v->priv->data);
}

void epsara_data_vector_minmax_index (const EpsaraDataVector * v, gint * imin, gint * imax)
{
  g_return_if_fail (EPSARA_DATA_IS_VECTOR (v));

  gsl_vector_minmax_index  (v->priv->data, (size_t *) imin, (size_t *) imax);
}

EpsaraDataVector *_epsara_data_vector_init_from_vect (gsl_vector *vector)
{
  EpsaraDataVector *new_vector;

  EPSARA_DATA_VECTOR (g_object_new (epsara_data_vector_get_type(), NULL));

  gsl_vector_free(new_vector->priv->data);
  new_vector->priv->data = vector;

  return new_vector;
}

gdouble *_epsara_data_vector_get_array (EpsaraDataVector *vector)
{
  gsl_vector *gsl_vector;

  g_return_val_if_fail (EPSARA_DATA_IS_VECTOR (vector), NULL);

  gsl_vector = vector->priv->data;

  return gsl_vector->data;
}
