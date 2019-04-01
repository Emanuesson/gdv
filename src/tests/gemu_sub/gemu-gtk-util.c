/*
 * gemu-gtk-util.c
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gemu-gtk-util.h"
#include "gemu-glib-util.h"

#include <gdv/gdv.h>

/*
 *
 * Example:
 *  You have an App-Window with the name my_window. If you want to see all the
 *  signals emitted on this window and all its children just add
 *
 *  gemu_gtk_util_signal_connect_to_widget_children (
 *   GTK_CONTAINER (my_window),
 *   gemu_glib_util_show_details,
 *   NULL,
 *   NULL);
 *
 *  right during the instance-initialization.
 */

/*
static void
add_emission_hook_to_children (GtkWidget *child, gpointer data)
{
  GList **pt_to_handler_id = data;
  GList  *local_list = NULL;

  g_print ("CHILD %s (%p)\n",
    g_type_name (G_TYPE_FROM_INSTANCE(child)),
    child);

  if (GTK_IS_CONTAINER (child))
    local_list = 
      gemu_gtk_util_signal_connect_to_widget_children (
        GTK_CONTAINER (child), gemu_glib_util_show_details, NULL, NULL);
  else
    local_list =
      gemu_glib_util_connect_to_all_signals (
        G_OBJECT (child), gemu_glib_util_show_details, NULL, NULL);

  *pt_to_handler_id = g_list_concat (*pt_to_handler_id, local_list);
}
*/

/**
 * gemu_gtk_util_signal_connect_to_widget_children:
 * @container: a #GtkContainer
 * @emission_hook: a #GSignalEmissionHook
 * @data: user-data
 * @data_destroy: a #GDestroyNotify for @data
 *
 * Adds an emission-hook on all signals emited on the @widget and its
 * children.
 *
 * Returns: (transfer full) (element-type gulong):
 *     a newly allocated #GList of all added handler ids
 */
GList *gemu_gtk_util_signal_connect_to_widget_children (
  GtkContainer *container,
  GSignalEmissionHook emission_hook,
  gpointer data,
  GDestroyNotify data_destroy)
{
  GList *return_list = NULL;
  GList *children, *children_start;

  g_return_val_if_fail (GTK_IS_CONTAINER (container), NULL);

  return_list =
    gemu_glib_util_connect_to_all_signals (
      G_OBJECT (container), emission_hook, data, data_destroy);

//  gtk_container_forall (container, add_emission_hook_to_children, &return_list);

  children = gtk_container_get_children (container);
  children_start = children;

  while (children)
  {
    GList *local_list = NULL;

    if (GTK_IS_CONTAINER (children->data))
    {
  local_list =
    gemu_gtk_util_signal_connect_to_widget_children (
      GTK_CONTAINER (children->data), emission_hook, data, data_destroy);

    }
    else
    {
  local_list =
    gemu_glib_util_connect_to_all_signals (
      G_OBJECT (children->data), emission_hook, data, data_destroy);
    }

    return_list = g_list_concat (return_list, local_list);

    children = children->next;
  }
  g_list_free (children_start);

  return return_list;
}


/**
 * gemu_gtk_util_signal_use_append_and_remove:
 * @ihint: Signal invocation hint, see #GSignalInvocationHint .
 * @n_param_values: the number of parameters to the function, including the 
 *    instance on which the signal was emitted.
 * @param_values: the instance on which the signal was emitted, followed by the 
 *    parameters of the emission.
 * @data: user data associated with the hook.
 *
 * This is just an example emission-hook to show the functionality.
 *
 */
gboolean
gemu_gtk_util_signal_use_append_and_remove (
  GSignalInvocationHint *ihint,
  guint n_param_values,
  const GValue *param_values,
  gpointer data)
{
  GObject *object_on_emit = g_value_get_object (param_values);

  g_print ("%s::%s::%s (%p) emitted",
    g_type_name (G_TYPE_FROM_INSTANCE(object_on_emit)),
    g_signal_name (ihint->signal_id),
    g_quark_to_string(ihint->detail),
    object_on_emit);

  if (ihint->signal_id == g_signal_lookup ("add", GTK_TYPE_CONTAINER))
  {
    /* I really don't like pointer-arrithmetics. But I have no glue, how to get
     * to the other array-elements in a more solid way.
     */
    GtkWidget *appended_widget = g_value_get_object (param_values + 1);
//    GList *handler_id_list = NULL;

    g_print (" ;add object: %s (%p)\n",
      g_type_name (G_TYPE_FROM_INSTANCE(appended_widget)),
      appended_widget);

    if (GTK_IS_CONTAINER (appended_widget))
    {
//      handler_id_list =
        gemu_gtk_util_signal_connect_to_widget_children (
          GTK_CONTAINER (appended_widget),
          gemu_gtk_util_signal_use_append_and_remove,
          NULL, NULL);
    }
    else
    {
//      handler_id_list =
        gemu_glib_util_connect_to_all_signals (
          G_OBJECT (appended_widget),
          gemu_glib_util_show_details,
          NULL, NULL);
    }

  }
  else if (ihint->signal_id == g_signal_lookup ("remove", GTK_TYPE_CONTAINER))
  {
    GtkWidget *removed_widget = g_value_get_object (param_values + 1);

    g_print (" ;remove object: %s (%p)\n",
      g_type_name (G_TYPE_FROM_INSTANCE(removed_widget)),
      removed_widget);

  }
  else
    g_print ("\n");

  return TRUE;
}


