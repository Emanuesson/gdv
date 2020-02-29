/*
 * gdv-central.h
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

/*******************************************************************************/
/* This File is just a working-file, all containing info should go into source */

#ifndef GDV_CENTRAL_H_INCLUDE
#define GDV_CENTRAL_H_INCLUDE

#include <glib.h>
#include <gio/gio.h>
#include <glib/gi18n.h>

gboolean gdv_is_within_range (gdouble value, gdouble upper, gdouble lower);
gboolean gdv_is_within_range_f (gfloat value, gfloat upper, gfloat lower);

gboolean gdv_is_nearly_identical (gdouble value1, gdouble value2, gdouble difference);
gboolean gdv_is_nearly_identical_f (gfloat value1, gfloat value2, gfloat difference);

#endif  /* __GDV_CENTRAL_H_INCLUDE  */

