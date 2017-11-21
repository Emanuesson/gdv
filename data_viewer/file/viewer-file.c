/*
 * viewer-file.c
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

#include <gtk/gtk.h>

#include "file/viewer-file.h"

//struct _ViewerFile
//{
//  GtkFile parent;
//};

//static void viewer_file_gfile_interface_init (GFileIface *iface);

struct _ViewerFileClass
{
  GObjectClass parent;
};

typedef struct _ViewerFilePrivate ViewerFilePrivate;

struct _ViewerFilePrivate
{
  GtkWidget *xmpl;

//  IdeObject          parent_instance;

  gchar             *content_type;
  GFile             *file;
//  IdeFileSettings   *file_settings;
//  GtkSourceLanguage *language;
  gchar             *path;
  GtkSourceFile     *source_file;
  GtkSourceBuffer   *buffer;
  guint              temporary_id;
};

enum {
  PROP_0,
  PROP_FILE,
//  PROP_IS_TEMPORARY,
//  PROP_LANGUAGE,
//  PROP_PATH,
//  PROP_TEMPORARY_ID,
  LAST_PROP
};

static GParamSpec *properties [LAST_PROP];

G_DEFINE_TYPE_WITH_CODE (ViewerFile, viewer_file, G_TYPE_OBJECT,
                         //G_IMPLEMENT_INTERFACE (G_TYPE_FILE,
                         //                       viewer_file_gfile_interface_init)
                         G_ADD_PRIVATE (ViewerFile));

//G_DEFINE_TYPE_WITH_PRIVATE (ViewerFile, viewer_file, G_TYPE_OBJECT);

//static void viewer_file_gfile_interface_init (GFileIface *iface)
//{
//}

static void
viewer_file_init (ViewerFile *view)
{
  ViewerFilePrivate *priv;

  priv = viewer_file_get_instance_private (view);
//  priv = viewer_file_get_private (view);


  priv->source_file = NULL;
}

static GFile *
viewer_file_get_file (ViewerFile *self)
{
  ViewerFilePrivate *priv;

  g_return_val_if_fail (VIEWER_IS_FILE (self), NULL);

  priv = viewer_file_get_instance_private (self);

  return priv->file;
}

static void
_set_file (ViewerFile *self,
                      GFile   *file)
{
  ViewerFilePrivate *priv;

  g_return_if_fail (VIEWER_IS_FILE (self));
  g_return_if_fail (G_IS_FILE (file));

  priv = viewer_file_get_instance_private (self);

  if (file != priv->file)
    {
      if (g_set_object (&priv->file, file))
        g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_FILE]);
    }
}


static void
viewer_file_get_property (GObject    *object,
                          guint       prop_id,
                          GValue     *value,
                          GParamSpec *pspec)
{
  ViewerFile *self = (ViewerFile *)object;

  switch (prop_id)
    {
    case PROP_FILE:
      g_value_set_object (value, viewer_file_get_file (self));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
viewer_file_set_property (GObject      *object,
                          guint         prop_id,
                          const GValue *value,
                          GParamSpec   *pspec)
{
  ViewerFile *self = (ViewerFile *)object;

  switch (prop_id)
    {
    case PROP_FILE:
      _set_file (self, g_value_get_object (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
viewer_file_class_init (ViewerFileClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->get_property = viewer_file_get_property;
  object_class->set_property = viewer_file_set_property;

  properties [PROP_FILE] =
    g_param_spec_object ("file",
                         "File",
                         "The path to the underlying file.",
                         G_TYPE_FILE,
                         (G_PARAM_READWRITE |
                          G_PARAM_CONSTRUCT_ONLY |
                          G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, LAST_PROP, properties);
}

ViewerFile *viewer_file_new (void)
{
  return g_object_new (viewer_file_get_type(), NULL);
}

ViewerFile *viewer_file_new_for_path (const gchar * path)
{
  g_autoptr(GFile) file = NULL;
  ViewerFile *ret;

  g_return_val_if_fail (path != NULL, NULL);

  file = g_file_new_for_path (path);
  ret = g_object_new (VIEWER_TYPE_FILE,
//                      "context", context,
                      "file", file,
                      NULL);

  return ret;
}

GtkSourceFile *_viewer_file_get_source_file (ViewerFile *self)
{
  ViewerFilePrivate *priv;

  g_return_val_if_fail (VIEWER_IS_FILE (self), NULL);

  priv = viewer_file_get_instance_private (self);

  if (g_once_init_enter (&priv->source_file))
    {
      GtkSourceFile *source_file;

      source_file = gtk_source_file_new ();
      gtk_source_file_set_location (source_file, priv->file);

      g_once_init_leave (&priv->source_file, source_file);
    }

  return priv->source_file;
}

void viewer_file_set_file (ViewerFile *self, GFile *file)
{
  g_return_if_fail (VIEWER_IS_FILE (self));

  _set_file (self, file);
}
