/*
 * gdve-source-view.h
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

#ifndef __VIEWER_SOURCE_VIEW_H__
#define __VIEWER_SOURCE_VIEW_H__

#include <glib-object.h>
#include <gtksourceview/gtksource.h>
//#include <ide.h>

G_BEGIN_DECLS

/* FIXME: look up similar examples of source-view implementation and adapt an own source-view */

/*
 * Type declaration.
 */
#define GDVE_TYPE_SOURCE_VIEW gdve_source_view_get_type ()
//G_DECLARE_DERIVABLE_TYPE (GdveSourceView, gdve_source_view, GDVE, SOURCE_VIEW, IdeSourceView)
G_DECLARE_DERIVABLE_TYPE (GdveSourceView, gdve_source_view, GDVE, SOURCE_VIEW, GtkSourceView)

/*
 * Method definitions.
 */
GdveSourceView *gdve_source_view_new (void);

G_END_DECLS

#endif /* __VIEWER_SOURCE_VIEW_H__ */
