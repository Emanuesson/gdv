/*
 * gdvcentral.c
 * This file is part of gdv
 *
 * Copyright (C) 2013 - Emanuel Schmidt
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include <stdlib.h>
#include <math.h>
#include <glib.h>
#include <gio/gio.h>

#include "gdvcentral.h"

gboolean gdv_is_within_range (gdouble value, gdouble upper, gdouble lower)
{
  gdouble min = fmin (upper, lower);
  gdouble max = fmax (upper, lower);

  return value <= max && value >= min ? TRUE : FALSE;
}

gboolean gdv_is_within_range_f (gfloat value, gfloat upper, gfloat lower)
{
  gfloat min = fminf (upper, lower);
  gfloat max = fmaxf (upper, lower);

  return value <= max && value >= min ? TRUE : FALSE;
}

gboolean gdv_is_nearly_identical (gdouble value1, gdouble value2, gdouble difference)
{
  gdouble diff = fabs (value1 - value2);

  return diff < difference ? TRUE : FALSE;
}

gboolean gdv_is_nearly_identical_f (gfloat value1, gfloat value2, gfloat difference)
{
  gfloat diff = fabsf (value1 - value2);

  return diff < difference ? TRUE : FALSE;
}
