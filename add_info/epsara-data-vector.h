/*
 * epsara-data-vector.h
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

#ifndef EPSARA_DATA_VECTOR_H_INCLUDED
#define EPSARA_DATA_VECTOR_H_INCLUDED

#include <stdlib.h>
#include <glib.h>
#include <gio/gio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <locale.h>

#include <gsl/gsl_math.h>
#include <gsl/gsl_vector.h>

G_BEGIN_DECLS

#define EPSARA_TYPE_DATA_VECTOR\
  (epsara_data_vector_get_type ())
#define EPSARA_DATA_VECTOR(obj)\
  (G_TYPE_CHECK_INSTANCE_CAST ((obj),\
  EPSARA_TYPE_DATA_VECTOR,\
  EpsaraDataVector))
#define EPSARA_DATA_IS_VECTOR(obj)\
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),\
  EPSARA_TYPE_DATA_VECTOR))
#define EPSARA_DATA_VECTOR_CLASS(klass)\
  (G_TYPE_CHECK_CLASS_CAST ((klass),\
  EPSARA_TYPE_DATA_VECTOR,\
  EpsaraDataVectorClass))
#define EPSARA_DATA_VECTOR_IS_CLASS(klass)\
  (G_TYPE_CHECK_CLASS_TYPE ((klass),\
  EPSARA_TYPE_DATA_VECTOR))
#define EPSARA_DATA_VECTOR_GET_CLASS(obj)\
  (G_TYPE_INSTANCE_GET_CLASS ((obj), EPSARA_TYPE_DATA_VECTOR, EpsaraDataVectorClass))

typedef struct _EpsaraDataVector EpsaraDataVector;
typedef struct _EpsaraDataVectorClass EpsaraDataVectorClass;
typedef struct _EpsaraDataVectorPrivate EpsaraDataVectorPrivate;

struct _EpsaraDataVector
{
  GObject parent;

  /*< private > */
  EpsaraDataVectorPrivate* priv;
};

struct _EpsaraDataVectorClass
{
  GObjectClass parent_class;

  /*< public >*/

};

/* Public Method definitions. */
GType epsara_data_vector_get_type (void) ;

EpsaraDataVector *epsara_data_vector_new (gint size);

gdouble epsara_data_vector_get (EpsaraDataVector *vector, gint i);
void epsara_data_vector_set (EpsaraDataVector *vector, gint i, gdouble x);
EpsaraDataVector *epsara_data_vector_set_and_expand (EpsaraDataVector *vector, gint i, gdouble x);

void epsara_data_vector_set_all (EpsaraDataVector *vector, gdouble x);
void epsara_data_vector_set_zero (EpsaraDataVector *vector);

gint epsara_data_vector_set_basis (EpsaraDataVector *vector, gint i);

gint epsara_data_vector_copy (EpsaraDataVector *dest, EpsaraDataVector *src);
gint epsara_data_vector_swap (EpsaraDataVector *v, EpsaraDataVector *w);

gint epsara_data_vector_swap_elements (EpsaraDataVector *vector, gint i, gint j);
gint epsara_data_vector_reverse (EpsaraDataVector *vector);

gint epsara_data_vector_add (EpsaraDataVector * a, const EpsaraDataVector * b);
gint epsara_data_vector_sub (EpsaraDataVector * a, const EpsaraDataVector * b);
gint epsara_data_vector_mul (EpsaraDataVector * a, const EpsaraDataVector * b);
gint epsara_data_vector_div (EpsaraDataVector * a, const EpsaraDataVector * b);
gint epsara_data_vector_scale (EpsaraDataVector * a, const gdouble x);
gint epsara_data_vector_add_constant (EpsaraDataVector * a, const gdouble x);

gdouble epsara_data_vector_max (const EpsaraDataVector * v);
gdouble epsara_data_vector_min (const EpsaraDataVector * v);
void epsara_data_vector_minmax (const EpsaraDataVector * v, gdouble * min_out, gdouble * max_out);
gint epsara_data_vector_max_index (const EpsaraDataVector * v);
gint epsara_data_vector_min_index (const EpsaraDataVector * v);
void epsara_data_vector_minmax_index (const EpsaraDataVector * v, gint * imin, gint * imax);

/* Public not exported methods */
EpsaraDataVector *_epsara_data_vector_init_from_vect (gsl_vector *vector);
gdouble *_epsara_data_vector_get_array (EpsaraDataVector *vector);

G_END_DECLS

#endif // EPSARA_DATA_VECTOR_H_INCLUDED

