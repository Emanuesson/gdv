/*
 * gdvaxis.h
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

#ifndef GDV_AXIS_H_INCLUDED
#define GDV_AXIS_H_INCLUDED

#include <stdlib.h>
#include <gtk/gtk.h>

#include <gdv/gdvcentral.h>
#include <gdv/gdvtic.h>
#include <gdv/gdvindicator.h>

G_BEGIN_DECLS

#define GDV_TYPE_AXIS\
  (gdv_axis_get_type ())
#define GDV_AXIS(obj)\
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), GDV_TYPE_AXIS, GdvAxis))
#define GDV_IS_AXIS(obj)\
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_TYPE_AXIS))
#define GDV_AXIS_CLASS(klass)\
  (G_TYPE_CHECK_CLASS_CAST ((klass), GDV_TYPE_AXIS, GdvAxisClass))
#define GDV_AXIS_IS_CLASS(klass)\
  (G_TYPE_CHECK_CLASS_TYPE ((klass), GDV_TYPE_AXIS))
#define GDV_AXIS_GET_CLASS(obj)\
  (G_TYPE_INSTANCE_GET_CLASS ((obj), GDV_TYPE_AXIS, GdvAxisClass))

typedef struct _GdvAxis GdvAxis;
typedef struct _GdvAxisClass GdvAxisClass;
typedef struct _GdvAxisPrivate GdvAxisPrivate;

struct _GdvAxis
{
  GtkContainer parent;

  /*< private >*/
  GdvAxisPrivate *priv;
};

/**
 * GdvAxisClass:
 * @parent_class: The parent class.
 * @get_point: The signal used to derive the screen position of a value on the
 *   axis. It is essential for calculating the point-position of any axes.
 * @get_inner_dir: The signal used to derive the inner direction of a value on
 *   the axis. It is essential for calculating the tic-appearance on the axis.
 * @make_tic_label_markup: Signal that is used to create a tic-label for any
 *   given function value.
 */
struct _GdvAxisClass
{
  GtkContainerClass parent_class;

  /*< public >*/
  gboolean (*get_point)       (GdvAxis            *axis,
                               gdouble             value,
                               gdouble            *pos_x,
                               gdouble            *pos_y);

  gboolean (*get_inner_dir)   (GdvAxis            *axis,
                               gdouble             value,
                               gdouble            *pos_x,
                               gdouble            *pos_y);

  gchar *(*make_tic_label_markup)       (GdvAxis            *axis,
                                         gdouble             value);

  /*< private >*/

  /* virtual private methods */
  void (*get_space_to_beg_position) (
    GdvAxis             *axis,
    GtkPositionType      direction,
    int                  for_size,
    int                 *minimum,
    int                 *natural,
    gpointer             data);

  void (*get_space_to_end_position) (
    GdvAxis             *axis,
    GtkPositionType      direction,
    int                  for_size,
    int                 *minimum,
    int                 *natural,
    gpointer             data);

  /* Padding to allow adding up to 12 new virtual functions without
   * breaking ABI. */
  gpointer _gdv_reserve[12];
};

/* Public Method definitions. */
GType gdv_axis_get_type (void);

GdvAxis *gdv_axis_new (void);

GdvTic *gdv_axis_get_tic_for_value (GdvAxis *axis, gdouble value);

GList *gdv_axis_get_tic_list (GdvAxis *axis);
GList *gdv_axis_get_mtic_list (GdvAxis *axis);
GList *gdv_axis_get_indicator_list (GdvAxis *axis);

void gdv_axis_title_set_markup (GdvAxis *axis, gchar *markup);
const gchar *gdv_axis_get_title (GdvAxis *axis);
void gdv_axis_set_title_widget (GdvAxis  *axis, GtkWidget *title_widget);
gboolean gdv_axis_get_show_title (GdvAxis *axis);

void
gdv_axis_get_borders (GdvAxis *axis,
                      gdouble *max_val, gdouble *min_val,
                      guint *max_pix_x, guint *min_pix_x,
                      guint *max_pix_y, guint *min_pix_y);

gboolean gdv_axis_queue_value_interval (GdvAxis *axis, gdouble value);

G_END_DECLS

#endif /* GDV_AXIS_H_INCLUDED */
