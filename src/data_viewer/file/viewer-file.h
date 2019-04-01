/*
 * viewer-file.h
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

#ifndef __VIEWER_FILE_H__
#define __VIEWER_FILE_H__

#include <glib-object.h>

#include <gtksourceview/gtksource.h>
#include <gtksourceview/gtksourcefile.h>

G_BEGIN_DECLS

/*
 * Type declaration.
 */
#define VIEWER_TYPE_FILE viewer_file_get_type ()
G_DECLARE_DERIVABLE_TYPE (ViewerFile, viewer_file, VIEWER, FILE, GObject)

/*
 * Method definitions.
 */
ViewerFile *viewer_file_new (void);

ViewerFile *viewer_file_new_for_path (const gchar * path);

GtkSourceFile *_viewer_file_get_source_file (ViewerFile *self);// G_GNUC_INTERNAL;

void viewer_file_set_file (ViewerFile *self, GFile *file);

G_END_DECLS

#endif /* __VIEWER_FILE_H__ */
