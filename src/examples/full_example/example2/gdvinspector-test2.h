/*
* gdvinspector-test2.h
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

#ifndef __GDV_INSPECTOR_TEST2_H_INCLUDED
#define __GDV_INSPECTOR_TEST2_H_INCLUDED

#include <gio/gio.h>
#include <gtk/gtk.h>

#include "gdvinspector-app.h"

G_BEGIN_DECLS;

/*
* Type checking and casting macros
*/
#define GDV_INSPECTOR_TYPE_TEST2            (gdv_inspector_test2_get_type ())
#define GDV_INSPECTOR_TEST2(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GDV_INSPECTOR_TYPE_TEST2, GdvInspectorTest2))
#define GDV_INSPECTOR_IS_TEST2(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_INSPECTOR_TYPE_TEST2))
#define GDV_INSPECTOR_TEST2_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GDV_INSPECTOR_TYPE_TEST2, GdvInspectorTest2Class))
#define GDV_INSPECTOR_IS_TEST2_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GDV_INSPECTOR_TYPE_TEST2))
#define GDV_INSPECTOR_TEST2_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GDV_INSPECTOR_TYPE_TEST2, GdvInspectorTest2Class))

typedef struct _GdvInspectorTest2         GdvInspectorTest2;
typedef struct _GdvInspectorTest2Class    GdvInspectorTest2Class;
typedef struct _GdvInspectorTest2Private  GdvInspectorTest2Private;

struct _GdvInspectorTest2
{
  GtkFrame parent;

  /*< private > */
  GdvInspectorTest2Private *priv;
};

struct _GdvInspectorTest2Class
{
  GtkFrameClass parent_class;

  /* Signals */
};


/* public methods */
GType gdv_inspector_test2_get_type (void);
GdvInspectorTest2 *gdv_inspector_test2_new (void);

/* not exported public methods*/

G_END_DECLS;
#endif /* __GDV_INSPECTOR_TEST2_H_INCLUDED */

