/*
 * gdv-viewer-settings-panel.h
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

#ifndef __GDV_VIEWER_SETTINGS_PANEL_H_INCLUDED
#define __GDV_VIEWER_SETTINGS_PANEL_H_INCLUDED

#include <gio/gio.h>
#include <gtk/gtk.h>

#include <gdv/gdv.h>

G_BEGIN_DECLS;

/*
 * Type declaration.
 */
#define GDV_TYPE_VIEWER_SETTINGS_PANEL (gdv_viewer_settings_panel_get_type ())
G_DECLARE_DERIVABLE_TYPE (GdvViewerSettingsPanel,
                          gdv_viewer_settings_panel,
                          GDV, VIEWER_SETTINGS_PANEL,
                          GtkBox)

/* public methods */
GdvViewerSettingsPanel *gdv_viewer_settings_panel_new (void);

/* not exported public methods*/

G_END_DECLS;
#endif // __GDV_VIEWER_SETTINGS_PANEL_H_INCLUDED
