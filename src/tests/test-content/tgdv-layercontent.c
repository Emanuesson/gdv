/* tgdv-layercontent.c
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

#include<math.h>

#include<stdio.h>
#include<gdv/gdv.h>

static void
test_layer_content_correct_default (void)
{
//  GdvLayerContent *content_under_test;

//  content_under_test
}

int main(int argc, char* argv[]) {

  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/Gdv/LayerContent/correct_default", test_layer_content_correct_default);

  return g_test_run ();
}


