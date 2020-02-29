/*
 * Copyright (C) 2020 - Emanuel Schmidt
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

#include "gdvcontent.h"

/**
 * SECTION:gdvcontent
 * @Title: GdvContent
 * @Short_description: An interface for drawable data
 *
 * #GdvContent is a simple interface used by GDV to represent any data that
 * should be drawn in a GdvLayer.
 */

G_DEFINE_INTERFACE (GdvContent, gdv_content, GTK_TYPE_WIDGET)

//enum {
//  PLACEHOLDER,
//  LAST_SIGNAL
//};

//static guint signals[LAST_SIGNAL] = { 0 };

#define GDK_TYPE_EMPTY_PAINTABLE (gdv_empty_content_get_type())
static
G_DECLARE_FINAL_TYPE(GdvEmptyContent, gdv_empty_content, GDV, EMPTY_CONTENT, GtkWidget)

struct _GdvEmptyContent
{
  GtkWidget parent_instance;

  int placeholder;
//  int height;
};

struct _GdvEmptyContentClass
{
  GtkWidgetClass parent_class;
};

static void
gdv_content_default_init (GdvContentInterface *iface)
{
}

static void
gdv_empty_content_content_init (GdvContentInterface *iface)
{
}

G_DEFINE_TYPE_WITH_CODE (GdvEmptyContent, gdv_empty_content, GTK_TYPE_WIDGET,
                         G_IMPLEMENT_INTERFACE (GDV_TYPE_CONTENT,
                                                gdv_empty_content_content_init))

static void
gdv_empty_content_class_init (GdvEmptyContentClass *klass)
{
}

static void
gdv_empty_content_init (GdvEmptyContent *self)
{
}

