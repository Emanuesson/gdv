/*
 * gdvtic.h
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

#ifndef GDV_TIC_H_INCLUDED
#define GDV_TIC_H_INCLUDED

#include <stdlib.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GDV_TYPE_TIC\
  (gdv_tic_get_type ())
#define GDV_TIC(obj)\
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), GDV_TYPE_TIC, GdvTic))
#define GDV_IS_TIC(obj)\
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_TYPE_TIC))
#define GDV_TIC_CLASS(klass)\
  (G_TYPE_CHECK_CLASS_CAST ((klass), GDV_TYPE_TIC, GdvTicClass))
#define GDV_TIC_IS_CLASS(klass)\
  (G_TYPE_CHECK_CLASS_TYPE ((klass), GDV_TYPE_TIC))
#define GDV_TIC_GET_CLASS(obj)\
  (G_TYPE_INSTANCE_GET_CLASS ((obj), GDV_TYPE_TIC, GdvTicClass))

typedef struct _GdvTic GdvTic;
typedef struct _GdvTicClass GdvTicClass;
typedef struct _GdvTicPrivate GdvTicPrivate;

struct _GdvTic
{
  GtkBin parent;

  /*< private >*/
  GdvTicPrivate *priv;
};

/**
 * GdvTicClass:
 * @parent_class: The parent class.
 */
struct _GdvTicClass
{
  GtkBinClass parent_class;

  /*< private >*/
  /* Padding to allow adding up to 12 new virtual functions without
   * breaking ABI. */
  gpointer _gdv_reserve[12];
};

/* Public Method definitions. */
GType gdv_tic_get_type (void);

GdvTic *gdv_tic_new (const gchar *label);
void gdv_tic_label_set_markup (GdvTic *tic, gchar *markup);
const gchar *gdv_tic_get_label (GdvTic *tic);
void gdv_tic_set_label_widget (GdvTic  *tic, GtkWidget *label_widget);
void gdv_tic_set_tic_position (GdvTic  *tic, gfloat x, gfloat y);

gboolean gdv_tic_get_show_label (GdvTic *tic);

void
gdv_tic_get_space_to_tic_position (
  GdvTic              *tic,
  GtkPositionType      direction,
  int                  for_size,
  int                 *minimum,
  int                 *natural,
  gpointer             data);

gboolean
gdv_tic_in_range (
  GdvTic *tic,
  gdouble value,
  gdouble residuum);

G_END_DECLS

#endif /* GDV_TIC_H_INCLUDED */
