/*
 * gdvinspector-test7.h
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

#ifndef __GDV_INSPECTOR_TEST7_H_INCLUDED
#define __GDV_INSPECTOR_TEST7_H_INCLUDED

#include <gio/gio.h>
#include <gtk/gtk.h>

#include "gdvinspector-app.h"

G_BEGIN_DECLS

/*
* Type checking and casting macros
*/
#define GDV_INSPECTOR_TYPE_TEST7            (gdv_inspector_test7_get_type ())
#define GDV_INSPECTOR_TEST7(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GDV_INSPECTOR_TYPE_TEST7, GdvInspectorTest7))
#define GDV_INSPECTOR_IS_TEST7(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_INSPECTOR_TYPE_TEST7))
#define GDV_INSPECTOR_TEST7_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GDV_INSPECTOR_TYPE_TEST7, GdvInspectorTest7Class))
#define GDV_INSPECTOR_IS_TEST7_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GDV_INSPECTOR_TYPE_TEST7))
#define GDV_INSPECTOR_TEST7_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GDV_INSPECTOR_TYPE_TEST7, GdvInspectorTest7Class))

typedef struct _GdvInspectorTest7         GdvInspectorTest7;
typedef struct _GdvInspectorTest7Class    GdvInspectorTest7Class;
typedef struct _GdvInspectorTest7Private  GdvInspectorTest7Private;

struct _GdvInspectorTest7
{
  GtkFrame parent;

  /*< private > */
  GdvInspectorTest7Private *priv;
};

struct _GdvInspectorTest7Class
{
  GtkFrameClass parent_class;

  /* Signals */
};


/* public methods */
GType gdv_inspector_test7_get_type (void);
GdvInspectorTest7 *gdv_inspector_test7_new (void);

/* not exported public methods*/

G_END_DECLS
#endif /* __GDV_INSPECTOR_TEST7_H_INCLUDED */

