/*
 * gemu-glib-util.c
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gemu-glib-util.h"

/*
 *
 * Example:
 *  You have an App-Instance with the name my_app. If you want to see all the
 *  signals emitted on this app just add
 *
 *  gemu_glib_util_connect_to_all_signals (
 *   G_OBJECT (my_app),
 *   gemu_glib_util_show_details,
 *   NULL,
 *   NULL);
 *
 *  right before g_application_run.
 */

typedef struct {
	GSignalEmissionHook hook;
	gpointer data;
  GDestroyNotify data_destroy;
}_ReconnecData;

static guint
global_id_list[1000] = { 0 };

//static gint compare_ids (const gulong *a, const gulong *b)
//{
//  gulong comp = *a - *b;
//
//  return (gint) comp;
//}

static gboolean
tmp_info_reconnect (
  GSignalInvocationHint *ihint,
  guint n_param_values,
  const GValue *param_values,
  gpointer data)
{
  _ReconnecData *reconn = data;

  g_signal_add_emission_hook (ihint->signal_id,
    0, reconn->hook, reconn->data, reconn->data);


  return TRUE; /* FIXME: Test the effect of this! */
//  g_signal_add_emission_hook (ihint->signal_id,
//    0, tmp_info_reconnect, data, NULL);
}

/**
 * gemu_glib_util_connect_to_all_signals:
 * @object: a #GObject
 * @emission_hook: a #GSignalEmissionHook
 * @data: user-data
 * @data_destroy: a #GDestroyNotify for @data
 *
 * Connects the @emission_hook on all signals emited on the @object.
 *
 * Returns: (transfer full) (element-type gulong):
 *     a newly allocated #GList of hook ids
 */
GList *gemu_glib_util_connect_to_all_signals (
  GObject *object,
  GSignalEmissionHook emission_hook,
  gpointer data,
  GDestroyNotify data_destroy)
{
  guint *list_of_signal_ids;
  guint no_of_signal_ids;
  guint i;
  GList *return_list = NULL;
  GType parent_type = G_TYPE_FROM_INSTANCE (object);

  _ReconnecData *reconnect_struct;

  g_return_val_if_fail (G_IS_OBJECT (object), NULL);

  reconnect_struct = g_new (_ReconnecData, 1);

  reconnect_struct->hook = emission_hook;
  reconnect_struct->data = data;
  reconnect_struct->data_destroy = data_destroy;

  /* Iterating over all parent-types */
  do {
    GList *local_connector_ids = NULL;

    list_of_signal_ids =
      g_signal_list_ids (parent_type, &no_of_signal_ids);

    /* Iterating over type-specific signals */
    for (i = 0; i < no_of_signal_ids; i++)
    {
      /* This is necessary, as otherwise signals emission would be doubled.
       * An alternative is a comparison and search within the return_list;
       * however this might break some external function-invocations that do not
       * rely on the same returrn_list
       */
      if (!global_id_list[list_of_signal_ids[i]])
      {
        GSignalQuery signal_info;
        gulong *conn_id = g_malloc(sizeof (gulong));
        gulong *reconn_id = g_malloc(sizeof (gulong));

        g_signal_query (list_of_signal_ids[i], &signal_info);

        /* Any checks for G_SIGNALO_NO_HOOKS-flag are itentionally excluded */
        if (!(signal_info.signal_flags & G_SIGNAL_NO_HOOKS))
        {
          *conn_id =
            g_signal_add_emission_hook (list_of_signal_ids[i],
              0,
              emission_hook,
              data,
              data_destroy);
           /* This is necessary, as the emission-hook gets deleted after invoke */
          *reconn_id =
            g_signal_add_emission_hook (list_of_signal_ids[i],
              0,
              tmp_info_reconnect,
              reconnect_struct,
              data_destroy);
        }
        else
          g_print ("SIGNAL %s has flag G_SIGNAL_NO_HOOKS\n",
                   g_signal_name(list_of_signal_ids[i]));

        global_id_list[list_of_signal_ids[i]] = 1;

        /* TODO: seprarate connector and reconnector-id */
        local_connector_ids = g_list_append (local_connector_ids, conn_id);
        local_connector_ids = g_list_append (local_connector_ids, reconn_id);
      }
    }

    return_list = g_list_concat (return_list, local_connector_ids);
  }
  while (parent_type == g_type_parent(parent_type));

  return return_list;
}



/**
 * gemu_glib_util_show_details:
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
gemu_glib_util_show_details (
  GSignalInvocationHint *ihint,
  guint n_param_values,
  const GValue *param_values,
  gpointer data)
{
  GObject *object_on_emit = g_value_get_object (param_values);

  g_print ("%s::%s::%s (%p) emitted\n",
    g_type_name (G_TYPE_FROM_INSTANCE(object_on_emit)),
    g_signal_name (ihint->signal_id),
    g_quark_to_string(ihint->detail),
    object_on_emit);

  return TRUE;
}
