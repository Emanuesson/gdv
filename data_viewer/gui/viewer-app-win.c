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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "viewer-app-win.h"

/* Define Properties */
enum
{
  PROP_0,

  N_PROPERTIES
};

struct _GdvViewerAppWindowPrivate
{
  gint placeholder;

  GdvLayerContent *content;
};

static GParamSpec *data_properties[N_PROPERTIES] = { NULL, };

G_DEFINE_TYPE_WITH_PRIVATE (GdvViewerAppWindow, gdv_viewer_app_window, GTK_TYPE_APPLICATION_WINDOW);

static void
gdv_viewer_app_window_dispose (GObject *object)
{
	G_OBJECT_CLASS (gdv_viewer_app_window_parent_class)->dispose (object);
}

static void
gdv_viewer_app_window_finalize (GObject *object)
{
	G_OBJECT_CLASS (gdv_viewer_app_window_parent_class)->finalize (object);
}

static void
gdv_viewer_app_window_class_init (GdvViewerAppWindowClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

	object_class->dispose = gdv_viewer_app_window_dispose;
  object_class->finalize = gdv_viewer_app_window_finalize;

  gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (object_class),
                                               "/net/gdv/viewerapp/ui/gui/viewer-app-win.ui");
//  gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (object_class), GdvViewerAppWindow, view);
}

void
gdv_viewer_app_window_init (GdvViewerAppWindow *window)
{
	GtkBox *content_box;
	GtkWindowGroup *window_group;
	GtkPaned *central_pane;
	GtkTreeView *central_proj_view;

  g_type_ensure (gdv_twod_layer_get_type ());

  gtk_widget_init_template (GTK_WIDGET (window));
	window->priv = gdv_viewer_app_window_get_instance_private (window);


	/* initialize window group */
//  window->priv->window_group = gtk_window_group_new ();
//  gtk_window_group_add_window (window->priv->window_group, GTK_WINDOW (window));

//  gtk_widget_show (GTK_WIDGET(window->priv->content_box));
}

GdvViewerAppWindow *
gdv_viewer_app_window_new (GdvViewerApp *app)
{
  return g_object_new (GDV_VIEWER_APP_TYPE_WINDOW, "application", app, NULL);
}

