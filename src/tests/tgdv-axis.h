/* tgdv-axis.h
 *
 * Copyright © 2018 Emanuel Schmidt <eschmidt216@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TGDV_AXIS_H_INCLUDED
#define TGDV_AXIS_H_INCLUDED

#include <gtk/gtk.h>
#include <gdv.h>

void tgdv_axis_test_full (GdvAxis *axis);
void tgdv_axis_test_integrity (GdvAxis *axis);
void tgdv_axis_test_alloc_usage (GdvAxis *axis);
void tgdv_axis_test_tic_distance (GdvAxis *axis);
void tgdv_axis_test_unity (GdvAxis *axis);

#endif /* TGDV_AXIS_H_INCLUDED */
