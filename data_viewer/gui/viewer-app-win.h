/*
 * viewer-app-win.c
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

#ifndef __GDV_VIEWER_APP_WINDOW_H_INCLUDED
#define __GDV_VIEWER_APP_WINDOW_H_INCLUDED

#include <gio/gio.h>
#include <gtk/gtk.h>

#include <gdv/gdv.h>

#include <application/viewer-app.h>

G_BEGIN_DECLS;

/*
 * Type checking and casting macros
 */
#define GDV_VIEWER_APP_TYPE_WINDOW                 (gdv_viewer_app_window_get_type ())
#define GDV_VIEWER_APP_WINDOW(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), GDV_VIEWER_APP_TYPE_WINDOW, GdvViewerAppWindow))
#define GDV_VIEWER_APP_IS_WINDOW(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_VIEWER_APP_TYPE_WINDOW))
#define GDV_VIEWER_APP_WINDOW_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), GDV_VIEWER_APP_TYPE_WINDOW, GdvViewerAppWindowClass))
#define GDV_VIEWER_APP_IS_WINDOW_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), GDV_VIEWER_APP_TYPE_WINDOW))
#define GDV_VIEWER_APP_WINDOW_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), GDV_VIEWER_APP_TYPE_WINDOW, GdvViewerAppWindowClass))

typedef struct _GdvViewerAppWindow         GdvViewerAppWindow;
typedef struct _GdvViewerAppWindowClass    GdvViewerAppWindowClass;
typedef struct _GdvViewerAppWindowPrivate    GdvViewerAppWindowPrivate;

struct _GdvViewerAppWindow
{
    GtkApplicationWindow parent;

    /*< private > */
    GdvViewerAppWindowPrivate *priv;
};

struct _GdvViewerAppWindowClass
{
    GtkApplicationWindowClass parent_class;

    /* Signals */
};


/* public methods */
GType                   gdv_viewer_app_window_get_type     (void);

GdvViewerAppWindow *gdv_viewer_app_window_new (GdvViewerApp *app);

/* not exported public methods*/

G_END_DECLS;
#endif // __GDV_VIEWER_APP_WINDOW_H_INCLUDED
