/*
 * test_bin.h
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

#ifndef TESTBIN_H_INCLUDED
#define TESTBIN_H_INCLUDED

#include <stdlib.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define TESTBIN_TYPE                 (testbin_get_type ())
#define TESTBIN(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), TESTBIN_TYPE, Testbin))
#define IS_TESTBIN(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TESTBIN_TYPE))
#define TESTBIN_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), TESTBIN_TYPE, TestbinClass))
#define TESTBIN_IS_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), TESTBIN_TYPE))
#define TESTBIN_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), TESTBIN_TYPE, TestbinClass))

typedef struct _Testbin Testbin;
typedef struct _TestbinClass TestbinClass;
typedef struct _TestbinPrivate TestbinPrivate;

struct _Testbin
{
  GtkBin parent;

  /*< private > */
  TestbinPrivate *priv;
};

/**
 * TestbinClass:
 * @parent_class: The parent class.
 */
struct _TestbinClass
{
  GtkBinClass parent_class;

  /*< public >*/
  void (*compute_child_allocation) (Testbin *view, GtkAllocation *allocation);
};

/* Public Method definitions. */
GType testbin_get_type (void);

Testbin *testbin_new (void);

void testbin_set_background_rgba (Testbin *view,
                                      gdouble red,
                                      gdouble green,
                                      gdouble blue,
                                      gdouble alpha);

G_END_DECLS

#endif /* GDV_TEST_APP_H_INCLUDED */
