/*
* gdvinspector-app-win.c
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

#ifndef __GDV_INSPECTOR_TEST3_H_INCLUDED
#define __GDV_INSPECTOR_TEST3_H_INCLUDED

#include <gio/gio.h>
#include <gtk/gtk.h>

#include "gdvinspector-app.h"

G_BEGIN_DECLS;

/*
* Type checking and casting macros
*/
#define GDV_INSPECTOR_TYPE_TEST3            (gdv_inspector_test3_get_type ())
#define GDV_INSPECTOR_TEST3(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GDV_INSPECTOR_TYPE_TEST3, GdvInspectorTest3))
#define GDV_INSPECTOR_IS_TEST3(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_INSPECTOR_TYPE_TEST3))
#define GDV_INSPECTOR_TEST3_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GDV_INSPECTOR_TYPE_TEST3, GdvInspectorTest3Class))
#define GDV_INSPECTOR_IS_TEST3_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GDV_INSPECTOR_TYPE_TEST3))
#define GDV_INSPECTOR_TEST3_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GDV_INSPECTOR_TYPE_TEST3, GdvInspectorTest3Class))

typedef struct _GdvInspectorTest3         GdvInspectorTest3;
typedef struct _GdvInspectorTest3Class    GdvInspectorTest3Class;
typedef struct _GdvInspectorTest3Private  GdvInspectorTest3Private;

struct _GdvInspectorTest3
{
  GtkFrame parent;

  /*< private > */
  GdvInspectorTest3Private *priv;
};

struct _GdvInspectorTest3Class
{
  GtkFrameClass parent_class;

  /* Signals */
};


/* public methods */
GType gdv_inspector_test3_get_type (void);
GdvInspectorTest3 *gdv_inspector_test3_new (void);

/* not exported public methods*/

G_END_DECLS;
#endif /* __GDV_INSPECTOR_TEST3_H_INCLUDED */

