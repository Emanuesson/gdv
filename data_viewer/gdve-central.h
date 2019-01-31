/*
 * gdve-central.h
 * This file is part of gdv
 *
 * Copyright (C) 2019 - Emanuel Schmidt
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

#pragma once

/* gettext-premade-functions */
#undef _
#define _(String) (String)
#undef N_
#define N_(String) String
#undef textdomain
#define textdomain(Domain)
#undef bindtextdomain
#define bindtextdomain(Package, Directory)

