/*
 * gemu-glib-util.h
 * This file contains auxiliary-functions for the development of glib-apps
 *
 * Copyright (C) 2017 - Emanuel Schmidt
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
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GEMU_GLIB_UTIL_H_INCLUDED
#define GEMU_GLIB_UTIL_H_INCLUDED

#include <stdlib.h>
#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS


/* Public Functions. */

GList *gemu_glib_util_connect_to_all_signals (
  GObject *object,
  GSignalEmissionHook emission_hook,
  gpointer data,
  GDestroyNotify data_destroy);

gboolean
gemu_glib_util_show_details (
   GSignalInvocationHint *ihint,
   guint n_param_values,
   const GValue *param_values,
   gpointer data);

G_END_DECLS

#endif // GEMU_GLIB_UTIL_H_INCLUDED

