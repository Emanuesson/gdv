/*
 * gdvdebug.h
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef GDV_DEBUG_H_INCLUDED
#define GDV_DEBUG_H_INCLUDED

#include <stdlib.h>
#include <gtk/gtk.h>

#include <gdv/gdvelement.h>
#include <gdv/gdvcentral.h>

//#define __GDV_DEBUG_MODE

gboolean
_gdv_debugging_init_element_frame (GdvElement *element);

//gboolean
//_gdv_debugging_set_frame_on_2d_view (Gdv2dView *view);

#endif /* GDV_DEBUGGING_H_INCLUDED */
