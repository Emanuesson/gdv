/*
 * viewer-main.c
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

#include <stdlib.h>
#include <gtk/gtk.h>

#include "application/gdv-app.h"

int
main (int argc, char **argv)
{
  GdvViewerApp *app;
  int status;

  app = gdv_viewer_app_new ();

  status = g_application_run (G_APPLICATION (app), argc, argv);

  /* Force disposal of the application (to help catch cleanup
   * issues at shutdown) and then (hopefully) finalize the app.
   * This trick was used in the Gnome-builder IDE and is quite handy.
   */
//  g_object_unref (primary_instance);
  g_object_run_dispose (G_OBJECT (app));
  g_clear_object (&app);

  return status;
}

