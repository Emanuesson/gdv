/*
 * gdvmtic.c
 * This file is part of gdv
 *
 * Copyright (C) 2013 - Emanuel Schmidt
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARMTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include <math.h>

/**
 * SECTION:gdvmtic
 * @title: GdvMtic
 *
 * #GdvMtic is a minor tic implementation. It is just present to collect some
 * style-properties typical for minor tics (e.g. abscence of tic-labels, etc.).
 *
 * # CSS nodes
 *
 * GdvAxis uses a single CSS node with name axis.
 *
 */

#include "gdvmtic.h"
#include "gdv-data-boxed.h"
#include "gdvrender.h"

/* --- variables --- */

struct _GdvMticPrivate
{
  gdouble          placeholder;
};

G_DEFINE_TYPE_WITH_PRIVATE (GdvMtic, gdv_mtic, GDV_TYPE_TIC);

static void
gdv_mtic_init (GdvMtic *mtic)
{
}

static void
gdv_mtic_class_init (GdvMticClass *klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  gtk_widget_class_set_css_name (widget_class, "mtic");
}

/**
 * gdv_mtic_new:
 *
 * Just a common constructor.
 *
 * Returns: a new #GdvMtic
 **/
GdvMtic *gdv_mtic_new (void)
{
  return g_object_new (gdv_mtic_get_type (),
                       NULL);
}
