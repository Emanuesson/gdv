/* tgdv-shared-functions.c
 *
 * Copyright © 2018 Emanuel Schmidt <eschmidt216@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "tgdv-shared-functions.h"

#include <math.h>

#include <stdio.h>
#include <stdarg.h>
#include <gtk/gtk.h>

/* TODO: add a varg-list with blacklisted property names for the specified object */
void _st_check_all_properties (GObject * object, gboolean check_for_default, ...)
{
  guint no_of_props = 0;
  guint i;
  GParamSpec **prop_list = NULL;
  GObjectClass *tic_oclass;
  const gchar **prop_names;
  GValue **values_on_object;

  tic_oclass = G_OBJECT_GET_CLASS(object);
  g_assert_nonnull(tic_oclass);

  prop_list = g_object_class_list_properties(tic_oclass, &no_of_props);
  prop_names = g_new0(const gchar *, no_of_props);
  values_on_object = g_new0(GValue*, no_of_props);
  for (i = 0; i < no_of_props; i++)
    {
      if (!(prop_list[i]->flags & G_PARAM_READABLE))
        {
          values_on_object[i] = NULL;
          continue;
        }
      values_on_object[i] = g_new0(GValue, 1);

      prop_names[i] = prop_list[i]->name;
      g_object_getv (object, 1, &prop_names[i], values_on_object[i]);
    }

  for (i = 0; i < no_of_props; i++)
    {
      GValue * active_property = values_on_object[i];
      gpointer active_content = NULL;
      va_list ap;
      gboolean exceptional_value = FALSE;
      const gchar *exceptional_prop_name;

      if (active_property == NULL)
        continue;

      g_debug("Checking property: %s(%s)", prop_names[i], G_VALUE_TYPE_NAME(active_property));
      g_debug("\tValue contains: %s", g_strdup_value_contents(active_property));

      /* This list is used to add values that can differ from defaults */
      va_start(ap, check_for_default);
      for (exceptional_prop_name = va_arg(ap, const gchar *);
           exceptional_prop_name != NULL;
           exceptional_prop_name = va_arg(ap, const gchar *))
        {
           if (g_strcmp0(prop_names[i],exceptional_prop_name) == 0)
            {
              exceptional_value = TRUE;
              g_debug("\tDeviation from default value is excepted");
            }
        }
      va_end(ap);
      if (exceptional_value)
        continue;

      if (check_for_default)
        {
          g_debug("\tDefault Value: %s",
                  g_strdup_value_contents(g_param_spec_get_default_value(prop_list[i])));

          /* XXX: this is a nasty workaround for all empty strings */
          if (G_IS_PARAM_SPEC_STRING(prop_list[i]) &&
              g_strcmp0(g_value_get_string(g_param_spec_get_default_value(prop_list[i])),
                        NULL) == 0)
            {
              g_assert_cmpstr (g_value_get_string(active_property), <=, "");
            } else {
              /* asserts that types are identical to default values */
              g_assert(g_param_value_defaults(prop_list[i], active_property));
            }
        }

      /* asserts that all parameters are in their range */
      g_assert(!g_param_value_validate(prop_list[i], active_property));

    }

  for (i = 0; i < no_of_props; i++)
    g_free(values_on_object[i]);

  g_free(prop_names);
  g_free(values_on_object);
}

