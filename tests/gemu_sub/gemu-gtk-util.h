/*
 * gemu-gtk-util.h
 * This file contains auxiliary-functions for the development of gtk+-apps
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

#ifndef GEMU_GTK_UTIL_H_INCLUDED
#define GEMU_GTK_UTIL_H_INCLUDED

#include <stdlib.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS


/* Public Functions. */

GList *gemu_gtk_util_signal_connect_to_widget_children (
  GtkContainer *container,
  GSignalEmissionHook emission_hook,
  gpointer data,
  GDestroyNotify data_destroy);

gboolean
gemu_gtk_util_signal_use_append_and_remove (
  GSignalInvocationHint *ihint,
  guint n_param_values,
  const GValue *param_values,
  gpointer data);

G_END_DECLS

#endif // GEMU_GTK_UTIL_H_INCLUDED

