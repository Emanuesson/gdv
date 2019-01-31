/*
 * gdvaxis.c
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

#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include <math.h>

/**
 * SECTION:gdvaxis
 * @short_description: an axis widget
 * @title: GdvAxis
 *
 * #GdvAxis is the base class of all single-dimensional axes. It is an
 * container for the affiliated tics and indicators. The exact appearance of
 * the axis is delegated to inherited class-definittions. However the
 * implemented geometry handling can be used for any axis, that is a basically
 * a straight line.
 *
 * # CSS nodes
 *
 * GdvAxis uses a single CSS node with name axis.
 *
 */
/* Welcome to the all-in-one design hell */

#include "gdvaxis.h"
#include "gdvtic.h"
#include "gdvmtic.h"
#include "gdv/gdvindicator.h"
#include "gdv-data-boxed.h"
#include "gdvrender.h"

/* Define Signals */
enum
{
  GET_POINT,
  GET_INNER_DIR,
  MAKE_TIC_LABEL_MARKUP,
  LAST_SIGNAL
};


/* Define Properties */
enum
{
  PROP_0,

  PROP_GDV_AXIS_DIRECTION_START,
  PROP_GDV_AXIS_DIRECTION_OUTER_SIDE,

  PROP_GDV_SCALE_MIN_VAL,
  PROP_GDV_SCALE_MAX_VAL,
  PROP_GDV_SCALE_INCREMENT_VAL,
  PROP_GDV_SCALE_AUTO_INCREMENT,
  PROP_GDV_SCALE_AUTO_LIMITS,

  PROP_GDV_AXIS_BEG_AT_SCREEN_X,
  PROP_GDV_AXIS_BEG_AT_SCREEN_Y,
  PROP_GDV_AXIS_END_AT_SCREEN_X,
  PROP_GDV_AXIS_END_AT_SCREEN_Y,

  PROP_GDV_AXIS_PIX_BEG_X,
  PROP_GDV_AXIS_PIX_BEG_Y,
  PROP_GDV_AXIS_PIX_END_X,
  PROP_GDV_AXIS_PIX_END_Y,

  PROP_GDV_TICS_BEG_VAL,
  PROP_GDV_TICS_END_VAL,
  PROP_GDV_TICS_AUTOMATIC,

  PROP_GDV_MTICS_BEG_VAL,
  PROP_GDV_MTICS_END_VAL,
  PROP_GDV_MTICS_AUTOMATIC,

  PROP_GDV_NUMBER_MTICS,

  PROP_GDV_AXIS_TITLE,
  PROP_GDV_AXIS_TITLE_WIDGET,

  PROP_GDV_AXIS_FORCE_BEG_END,

  PROP_GDV_AXIS_RESIZE_DURING_REDRAW,

  N_PROPERTIES
};

/* --- variables --- */
static guint axis_signals[LAST_SIGNAL] = { 0 };

struct _GdvAxisPrivate
{
  /* list with indicators */
  GList      *indicators;
  GHashTable *update_indicator_table;

  GSList     *ranges;

  /* list with tics */
  GList      *tics;
  GList      *mtics;

  GdkWindow *event_window;

  /* geoemtric properties of the axis */
  gdouble     direction_start;
  gdouble     direction_outer;

  /* scale-specific propterties */
  gdouble     scale_min_val;
  gdouble     scale_max_val;
  gdouble     scale_increment_val;
  gboolean    scale_auto_increment;
  gboolean    scale_auto_limits;

  /* axis-specific properties */
  gdouble          axis_beg_pix_x;
  gdouble          axis_beg_pix_y;
  gdouble          axis_end_pix_x;
  gdouble          axis_end_pix_y;

  /* tics propterties */
  gdouble           tics_beg_val;
  gdouble           tics_end_val;
  gboolean          tics_automatic;

  /* minor-tics properties */
  gdouble           mtics_beg_val;
  gdouble           mtics_end_val;
  gboolean          mtics_automatic;

  guint             no_of_mtics;

  gboolean          force_beg_end;

  gboolean          resize_during_redraw;

  gboolean          tic_labels;
  const gchar      *label_format;

  GtkWidget        *title;
};

static GParamSpec *axis_properties[N_PROPERTIES] = { NULL, };

static gboolean
gdv_axis_on_evaluate_unimplemented (GdvAxis *axis,
                                    gdouble value,
                                    gdouble *pos_x,
                                    gdouble *pos_y);
static gboolean
gdv_axis_on_evaluate_inndir_unimplemented (GdvAxis *axis,
    gdouble value,
    gdouble *pos_x,
    gdouble *pos_y);

static gboolean gdv_axis_title_widget_is_visible (GdvAxis *axis);

static gchar *
gdv_axis_make_tic_label_markup_unimplemented (
  GdvAxis *axis,
  gdouble  value);

static void
gdv_axis_size_allocate (GtkWidget     *widget,
                        GtkAllocation *allocation);
static gboolean
gdv_axis_draw (GtkWidget    *widget,
               cairo_t      *cr);

static void
gdv_axis_get_preferred_width (GtkWidget           *widget,
                              gint                *minimum_size,
                              gint                *natural_size);
static void
gdv_axis_get_preferred_height (GtkWidget           *widget,
                               gint                *minimum_size,
                               gint                *natural_size);
static void
gdv_axis_get_preferred_height_for_width (GtkWidget           *widget,
    gint                 width,
    gint                *minimum_height,
    gint                *natural_height);
static void
gdv_axis_get_preferred_width_for_height (GtkWidget           *layout,
    gint                 width,
    gint                *minimum_height,
    gint                *natural_height);

static void
gdv_axis_dispose (GObject *object);

static void
gdv_axis_finalize (GObject *object);

static void
gdv_axis_get_space_to_beg_position_unimplemented (
  GdvAxis             *axis,
  GtkPositionType      direction,
  int                  for_size,
  int                 *minimum,
  int                 *natural,
  gpointer             data);

static void
gdv_axis_get_space_to_end_position_unimplemented (
  GdvAxis             *axis,
  GtkPositionType      direction,
  int                  for_size,
  int                 *minimum,
  int                 *natural,
  gpointer             data);

G_DEFINE_TYPE_WITH_PRIVATE (GdvAxis, gdv_axis, GTK_TYPE_CONTAINER);

static void
gdv_axis_init (GdvAxis *axis)
{
  GtkStyleContext *style_context;
  GtkCssProvider *css_provider;

  GtkWidget *widget;

  widget = GTK_WIDGET (axis);

  /* initializing style-properties */
  css_provider = gtk_css_provider_new ();
  style_context = gtk_widget_get_style_context (widget);
  gtk_css_provider_load_from_resource (
    css_provider,
    "/net/gdv/libgdv/themes/default_general.css");

  gtk_style_context_add_provider (
    style_context,
    GTK_STYLE_PROVIDER (css_provider),
    GTK_STYLE_PROVIDER_PRIORITY_FALLBACK);

  gtk_widget_set_can_focus (GTK_WIDGET (axis), TRUE);
  gtk_widget_set_receives_default (GTK_WIDGET (axis), TRUE);
  gtk_widget_set_has_window (GTK_WIDGET (axis), FALSE);

  axis->priv = gdv_axis_get_instance_private (axis);

  axis->priv->direction_start = 0.0;
  axis->priv->direction_outer = 0.5 * M_PI;

  axis->priv->scale_min_val = 0.0;
  axis->priv->scale_max_val = 100.0;
  axis->priv->scale_increment_val = 100.0;
  axis->priv->scale_auto_increment = TRUE;
  axis->priv->scale_auto_limits = TRUE;

  axis->priv->axis_beg_pix_x = 0.0;
  axis->priv->axis_beg_pix_y = 0.0;
  axis->priv->axis_end_pix_x = 0.0;
  axis->priv->axis_end_pix_y = 0.0;

  axis->priv->tics_automatic = TRUE;
  axis->priv->tics_beg_val = 0.0;
  axis->priv->tics_end_val = 100.0;

  axis->priv->mtics_automatic = TRUE;
  axis->priv->mtics_beg_val = 0.0;
  axis->priv->mtics_end_val = 100.0;
  axis->priv->no_of_mtics = 4;

  axis->priv->force_beg_end = FALSE;

  /* This is a necessary evil, as the resize_queue during alloc is suppressed */
  axis->priv->resize_during_redraw = FALSE;

  axis->priv->indicators = NULL;
  axis->priv->update_indicator_table =
    g_hash_table_new_full (g_direct_hash, g_direct_equal, NULL, g_free);

  axis->priv->tics = NULL;
  axis->priv->mtics = NULL;
  axis->priv->tic_labels = TRUE;
  axis->priv->label_format = NULL;

  axis->priv->ranges = NULL;

}

static void
gdv_axis_set_property (GObject      *object,
                       guint         property_id,
                       const GValue *value,
                       GParamSpec   *pspec)
{
  GdvAxis *self;
  gchar *title_dup;
  gint allocation_dimension;

  self = GDV_AXIS (object);

  switch (property_id)
  {
  case PROP_GDV_AXIS_DIRECTION_START:
    self->priv->direction_start = g_value_get_double (value);
    gtk_widget_queue_allocate (GTK_WIDGET (self));
    break;

  case PROP_GDV_AXIS_DIRECTION_OUTER_SIDE:
    self->priv->direction_outer = g_value_get_double (value);
    gtk_widget_queue_allocate (GTK_WIDGET (self));
    break;

  case PROP_GDV_SCALE_MIN_VAL:
    self->priv->scale_min_val = g_value_get_double (value);
    gtk_widget_queue_allocate (GTK_WIDGET (self));
    break;

  case PROP_GDV_SCALE_MAX_VAL:
    self->priv->scale_max_val = g_value_get_double (value);
    gtk_widget_queue_allocate (GTK_WIDGET (self));
    break;

  case PROP_GDV_SCALE_INCREMENT_VAL:
    self->priv->scale_increment_val = g_value_get_double (value);
    gtk_widget_queue_allocate (GTK_WIDGET (self));
    break;

  case PROP_GDV_SCALE_AUTO_INCREMENT:
    self->priv->scale_auto_increment = g_value_get_boolean (value);
    gtk_widget_queue_allocate (GTK_WIDGET (self));
    break;

  case PROP_GDV_SCALE_AUTO_LIMITS:
    self->priv->scale_auto_limits = g_value_get_boolean (value);
    gtk_widget_queue_allocate (GTK_WIDGET (self));
    break;

  /* TODO: connect notify for the following props */
  case PROP_GDV_AXIS_PIX_BEG_X:
    self->priv->axis_beg_pix_x = g_value_get_double (value);
    gtk_widget_queue_allocate (GTK_WIDGET (self));
    break;

  case PROP_GDV_AXIS_PIX_BEG_Y:
    self->priv->axis_beg_pix_y = g_value_get_double (value);
    gtk_widget_queue_allocate (GTK_WIDGET (self));
    break;

  case PROP_GDV_AXIS_PIX_END_X:
    self->priv->axis_end_pix_x = g_value_get_double (value);
    gtk_widget_queue_allocate (GTK_WIDGET (self));
    break;

  case PROP_GDV_AXIS_PIX_END_Y:
    self->priv->axis_end_pix_y = g_value_get_double (value);
    gtk_widget_queue_allocate (GTK_WIDGET (self));
    break;

  case PROP_GDV_AXIS_BEG_AT_SCREEN_X:
    allocation_dimension = gtk_widget_get_allocated_width (GTK_WIDGET (self));
    self->priv->axis_beg_pix_x =
      g_value_get_double (value) * ((gdouble) allocation_dimension);
    gtk_widget_queue_allocate (GTK_WIDGET (self));
    break;

  case PROP_GDV_AXIS_BEG_AT_SCREEN_Y:
    allocation_dimension = gtk_widget_get_allocated_height (GTK_WIDGET (self));
    self->priv->axis_beg_pix_y =
      g_value_get_double (value) * ((gdouble) allocation_dimension);
    gtk_widget_queue_allocate (GTK_WIDGET (self));
    break;

  case PROP_GDV_AXIS_END_AT_SCREEN_X:
    allocation_dimension = gtk_widget_get_allocated_width (GTK_WIDGET (self));
    self->priv->axis_end_pix_x =
      g_value_get_double (value) * ((gdouble) allocation_dimension);
    gtk_widget_queue_allocate (GTK_WIDGET (self));
    break;

  case PROP_GDV_AXIS_END_AT_SCREEN_Y:
    allocation_dimension = gtk_widget_get_allocated_height (GTK_WIDGET (self));
    self->priv->axis_end_pix_y =
      g_value_get_double (value) * ((gdouble) allocation_dimension);
    gtk_widget_queue_allocate (GTK_WIDGET (self));
    break;

  case PROP_GDV_TICS_BEG_VAL:
    self->priv->tics_beg_val = g_value_get_double (value);
    gtk_widget_queue_allocate (GTK_WIDGET (self));
    break;

  case PROP_GDV_TICS_END_VAL:
    self->priv->tics_end_val = g_value_get_double (value);
    gtk_widget_queue_allocate (GTK_WIDGET (self));
    break;

  case PROP_GDV_TICS_AUTOMATIC:
    self->priv->tics_automatic = g_value_get_boolean (value);
    gtk_widget_queue_allocate (GTK_WIDGET (self));
    break;

  case PROP_GDV_MTICS_BEG_VAL:
    self->priv->mtics_beg_val = g_value_get_double (value);
    gtk_widget_queue_allocate (GTK_WIDGET (self));
    break;

  case PROP_GDV_MTICS_END_VAL:
    self->priv->mtics_end_val = g_value_get_double (value);
    gtk_widget_queue_allocate (GTK_WIDGET (self));
    break;

  case PROP_GDV_MTICS_AUTOMATIC:
    self->priv->mtics_automatic = g_value_get_boolean (value);
    gtk_widget_queue_allocate (GTK_WIDGET (self));
    break;

  case PROP_GDV_NUMBER_MTICS:
    self->priv->no_of_mtics = g_value_get_uint (value);
    gtk_widget_queue_allocate (GTK_WIDGET (self));
    break;

  case PROP_GDV_AXIS_TITLE:
    title_dup = g_value_dup_string (value);
    gdv_axis_title_set_markup(self, title_dup);
    g_free (title_dup);
    gtk_widget_queue_allocate (GTK_WIDGET (self));
    break;

  case PROP_GDV_AXIS_TITLE_WIDGET:
    gdv_axis_set_title_widget (self, g_value_get_object (value));
    gtk_widget_queue_allocate (GTK_WIDGET (self));
    break;

  case PROP_GDV_AXIS_FORCE_BEG_END:
    self->priv->force_beg_end = g_value_get_boolean (value);
    gtk_widget_queue_allocate (GTK_WIDGET (self));
    break;

  case PROP_GDV_AXIS_RESIZE_DURING_REDRAW:
    self->priv->resize_during_redraw = g_value_get_boolean (value);
    break;

  default:
    /* unknown property */
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }
}


static void
gdv_axis_get_property (GObject    *object,
                       guint       property_id,
                       GValue     *value,
                       GParamSpec *pspec)
{
  GdvAxis *self = GDV_AXIS (object);
  gint allocation_dimension;

  switch (property_id)
  {
  case PROP_GDV_AXIS_DIRECTION_START:
    g_value_set_double (value, self->priv->direction_start);
    break;

  case PROP_GDV_AXIS_DIRECTION_OUTER_SIDE:
    g_value_set_double (value, self->priv->direction_outer);
    break;

  case PROP_GDV_SCALE_MIN_VAL:
    g_value_set_double (value, self->priv->scale_min_val);
    break;

  case PROP_GDV_SCALE_MAX_VAL:
    g_value_set_double (value, self->priv->scale_max_val);
    break;

  case PROP_GDV_SCALE_INCREMENT_VAL:
    g_value_set_double (value, self->priv->scale_increment_val);
    break;

  case PROP_GDV_SCALE_AUTO_INCREMENT:
    g_value_set_boolean (value, self->priv->scale_auto_increment);
    break;

  case PROP_GDV_SCALE_AUTO_LIMITS:
    g_value_set_boolean (value, self->priv->scale_auto_limits);
    break;

  case PROP_GDV_AXIS_PIX_BEG_X:
    g_value_set_double (value, self->priv->axis_beg_pix_x);
    break;

  case PROP_GDV_AXIS_PIX_BEG_Y:
    g_value_set_double (value, self->priv->axis_beg_pix_y);
    break;

  case PROP_GDV_AXIS_PIX_END_X:
    g_value_set_double (value, self->priv->axis_end_pix_x);
    break;

  case PROP_GDV_AXIS_PIX_END_Y:
    g_value_set_double (value, self->priv->axis_end_pix_y);
    break;

  case PROP_GDV_AXIS_BEG_AT_SCREEN_X:
    /* FIXME: This behaviour should be improved here! */
    allocation_dimension = gtk_widget_get_allocated_width (GTK_WIDGET (self));
    g_value_set_double (value,
      self->priv->axis_beg_pix_x / ((gdouble) allocation_dimension));
    break;

  case PROP_GDV_AXIS_BEG_AT_SCREEN_Y:
    allocation_dimension = gtk_widget_get_allocated_height (GTK_WIDGET (self));
    g_value_set_double (value,
      self->priv->axis_beg_pix_y / ((gdouble) allocation_dimension));
    break;

  case PROP_GDV_AXIS_END_AT_SCREEN_X:
    allocation_dimension = gtk_widget_get_allocated_width (GTK_WIDGET (self));
    g_value_set_double (value,
      self->priv->axis_end_pix_x / ((gdouble) allocation_dimension));
    break;

  case PROP_GDV_AXIS_END_AT_SCREEN_Y:
    allocation_dimension = gtk_widget_get_allocated_height (GTK_WIDGET (self));
    g_value_set_double (value,
      self->priv->axis_end_pix_y / ((gdouble) allocation_dimension));
    break;

  case PROP_GDV_TICS_BEG_VAL:
    g_value_set_double (value, self->priv->tics_beg_val);
    break;

  case PROP_GDV_TICS_END_VAL:
    g_value_set_double (value, self->priv->tics_end_val);
    break;

  case PROP_GDV_TICS_AUTOMATIC:
    g_value_set_boolean (value, self->priv->tics_automatic);
    break;

  case PROP_GDV_MTICS_BEG_VAL:
    g_value_set_double (value, self->priv->mtics_beg_val);
    break;

  case PROP_GDV_MTICS_END_VAL:
    g_value_set_double (value, self->priv->mtics_end_val);
    break;

  case PROP_GDV_MTICS_AUTOMATIC:
    g_value_set_boolean (value, self->priv->mtics_automatic);
    break;

  case PROP_GDV_NUMBER_MTICS:
    g_value_set_uint (value, self->priv->no_of_mtics);
    break;

  case PROP_GDV_AXIS_TITLE:
    g_value_set_string (value, gdv_axis_get_title (self));
    break;

  case PROP_GDV_AXIS_TITLE_WIDGET:
    g_value_set_object (value,
                        self->priv->title ?
                        G_OBJECT (self->priv->title) : NULL);
    break;

  case PROP_GDV_AXIS_FORCE_BEG_END:
    g_value_set_boolean (value, self->priv->force_beg_end);
    break;

  case PROP_GDV_AXIS_RESIZE_DURING_REDRAW:
    g_value_set_boolean (value, self->priv->resize_during_redraw);
    break;

  default:
    /* unknown property */
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }
}

static void
gdv_axis_dispose (GObject *object)
{
  /* This is an old relict. Probably everything here will be done by the 
   * GtkContainer-Class
   */

  G_OBJECT_CLASS (gdv_axis_parent_class)->dispose (object);
}

static void
gdv_axis_finalize (GObject *object)
{
  GdvAxis *axis = GDV_AXIS (object);

  /* finalizing the indicator-table */
  if (axis->priv->update_indicator_table)
  {
    g_hash_table_unref (axis->priv->update_indicator_table);
    axis->priv->update_indicator_table = NULL;
  }

  G_OBJECT_CLASS (gdv_axis_parent_class)->finalize (object);
}

static void update_indicator (
  GdvIndicator *indicator,
  gpointer property_value,
  GdvAxis *axis)
{
  if (GDV_IS_AXIS (axis))
    gtk_widget_queue_draw (GTK_WIDGET (axis));
}

static void gdv_axis_add (GtkContainer *container_axis,
                          GtkWidget    *widget)
{
  GdvAxis *axis;

  axis = GDV_AXIS (container_axis);

  if (GDV_IS_INDICATOR (widget))
  {
    GdvIndicator *indicator;
    gulong *update_indicator_id;

    indicator = GDV_INDICATOR (widget);

    g_object_ref (indicator);

    axis->priv->indicators =
      g_list_append (axis->priv->indicators, indicator);

    update_indicator_id = g_new (gulong, 1);
    *update_indicator_id =
      g_signal_connect (indicator, "notify::value",
                        G_CALLBACK (update_indicator), axis);
    g_hash_table_insert (axis->priv->update_indicator_table,
                         GDV_INDICATOR (widget),
                         update_indicator_id);

    gtk_widget_set_visible (GTK_WIDGET (indicator), TRUE);

    gtk_widget_set_parent (widget, GTK_WIDGET (container_axis));
  }
  else if (GDV_IS_MTIC (widget))
  {
    GdvMtic *mtic;

    mtic = GDV_MTIC (widget);

    g_object_ref (mtic);

    axis->priv->mtics =
      g_list_append (axis->priv->mtics, mtic);

    gtk_widget_show (GTK_WIDGET (mtic));

    gtk_widget_set_parent (widget, GTK_WIDGET (container_axis));
  }
  else if (GDV_IS_TIC (widget))
  {
    GdvTic *tic;

    tic = GDV_TIC (widget);

    g_object_ref (tic);

    axis->priv->tics =
      g_list_append (axis->priv->tics, tic);

    gtk_widget_show (GTK_WIDGET (tic));

    gtk_widget_set_parent (widget, GTK_WIDGET (axis));
  }
  else
  {
    g_warning ("Attempting to add a widget with type %s to a %s, "
               "but as a GdvLayer subclass a %s can only contain a widget of subclass "
               "GdvAxis or GdvContent",
               g_type_name (G_OBJECT_TYPE (widget)),
               g_type_name (G_OBJECT_TYPE (container_axis)),
               g_type_name (G_OBJECT_TYPE (container_axis)));
  }

/*
  if (GDV_IS_MTIC (widget))
  {
    gdouble value;
    g_object_get (widget, "value", &value, NULL);
    g_print ("ADDED MTIC WITH %e\n", value);
  }
  else if (GDV_IS_TIC (widget))
  {
    gdouble value;
    g_object_get (widget, "value", &value, NULL);
    g_print ("ADDED TIC WITH %e\n", value);
  }
*/

  gtk_widget_show_all (widget);

//  if (gtk_widget_get_visible (GTK_WIDGET (axis)))
  axis->priv->resize_during_redraw = TRUE;
}

static void gdv_axis_remove (GtkContainer *container_axis,
                             GtkWidget    *widget)
{
  GdvAxis *axis;
  GdvAxisPrivate *priv;

  g_return_if_fail (GDV_IS_AXIS (container_axis));

  axis = GDV_AXIS (container_axis);
  priv = axis->priv;

  if (priv->title == widget)
    gdv_axis_set_title_widget (GDV_AXIS (container_axis), NULL);
  else if (GDV_IS_INDICATOR (widget))
  {
    GdvIndicator *indicator;
    gulong *update_indicator_id;

    indicator = GDV_INDICATOR (widget);

    axis->priv->indicators =
      g_list_remove (axis->priv->indicators, indicator);

    update_indicator_id =
      g_hash_table_lookup (axis->priv->update_indicator_table,
                           GDV_INDICATOR (widget));
    g_signal_handler_disconnect (widget, *update_indicator_id);
    g_hash_table_remove (axis->priv->update_indicator_table,
                         GDV_INDICATOR (widget));

    g_object_unref (indicator);
  }
  else if (GDV_IS_MTIC (widget))
  {
    GdvMtic *mtic;
    mtic = GDV_MTIC (widget);

    axis->priv->mtics =
      g_list_remove (axis->priv->mtics, mtic);

    g_object_unref (mtic);
  }
  else if (GDV_IS_TIC (widget))
  {
    GdvTic *tic;
    tic = GDV_TIC (widget);

    axis->priv->tics =
      g_list_remove (axis->priv->tics, tic);

    g_object_unref (tic);
  }

/*
  if (GDV_IS_MTIC (widget))
  {
    gdouble value;
    g_object_get (widget, "value", &value, NULL);
    g_print ("REMOVED MTIC WITH %e\n", value);
  }
  else if (GDV_IS_TIC (widget))
  {
    gdouble value;
    g_object_get (widget, "value", &value, NULL);
    g_print ("REMOVED TIC WITH %e\n", value);
  }
*/

//  if (gtk_widget_get_visible (GTK_WIDGET (axis)))
  axis->priv->resize_during_redraw = TRUE;
}

static void gdv_axis_forall (GtkContainer *container,
                             gboolean      include_internals,
                             GtkCallback   callback,
                             gpointer      callback_data)
{
  GdvAxis *axis = GDV_AXIS (container);
  GdvAxisPrivate *priv = axis->priv;
  GList *indicators, *tics, *mtics;
  GdvIndicator *indicator;
  GdvTic *tic;
  GdvMtic *mtic;

  indicators = priv->indicators;
  tics = priv->tics;
  mtics = priv->mtics;

  while (tics)
  {
    tic = tics->data;
    tics = tics->next;

    (* callback) (GTK_WIDGET (tic), callback_data);
  }

  while (mtics)
  {
    mtic = mtics->data;
    mtics = mtics->next;

    (* callback) (GTK_WIDGET (mtic), callback_data);
  }

  while (indicators)
  {
    indicator = indicators->data;
    indicators = indicators->next;

    (* callback) (GTK_WIDGET (indicator), callback_data);
  }

  if (priv->title)
    (* callback) (GTK_WIDGET (priv->title), callback_data);
}

static GtkWidgetPath *
gdv_axis_get_path_for_child (GtkContainer *container,
                             GtkWidget    *child)
{
  GtkWidgetPath *path, *sibling_path;
  GList *list, *children;

  GdvAxis *axis = GDV_AXIS (container);
  GtkWidget *widget = GTK_WIDGET (container);

  path = NULL;

  {
    GtkWidget *parent;

    parent = gtk_widget_get_parent (widget);

    if (parent)
      path = gtk_container_get_path_for_child (GTK_CONTAINER (parent), widget);
    else
    {
      /* Widget is either toplevel or unparented, treat both
       * as toplevels style wise, since there are situations
       * where style properties might be retrieved on that
       * situation.
       */
      GtkWidget *attach_widget = NULL;
      GtkWidgetPath *result;

      if (GTK_IS_WINDOW (widget))
        attach_widget = gtk_window_get_attached_to (GTK_WINDOW (widget));

      if (attach_widget != NULL)
        result = gtk_widget_path_copy (gtk_widget_get_path (attach_widget));
      else
        result = gtk_widget_path_new ();

      gtk_widget_path_append_for_widget (result, widget);

      path = result;
    }
  }

  if (gtk_widget_get_visible (child))
  {
    gint position = -1;
    GList *pos_link = NULL;
    gint offset = 0;
    gboolean found = FALSE;

    sibling_path = gtk_widget_path_new ();

    /* get_children works in visible order */
    children = axis->priv->indicators;

    for (list = children; list; list = list->next)
    {
      if (!gtk_widget_get_visible (list->data))
        continue;

      if (list->data == child)
        found = TRUE;

      if (!found)
        offset++;

      gtk_widget_path_append_for_widget (sibling_path, list->data);
    }

    pos_link = g_list_find (axis->priv->indicators, child);

    if (pos_link)
      position = g_list_position (axis->priv->indicators, pos_link);

    children = axis->priv->tics;

    for (list = children; list; list = list->next)
    {
      if (!gtk_widget_get_visible (list->data))
        continue;

      if (list->data == child)
        found = TRUE;

      if (!found)
        offset++;

      gtk_widget_path_append_for_widget (sibling_path, list->data);
    }

    pos_link = g_list_find (axis->priv->tics, child);

    if (pos_link)
    {
      position = g_list_position (axis->priv->tics, pos_link);
      position += g_list_length (axis->priv->indicators);
    }

    children = axis->priv->mtics;

    for (list = children; list; list = list->next)
    {
      if (!gtk_widget_get_visible (list->data))
        continue;

      gtk_widget_path_append_for_widget (sibling_path, list->data);
    }

    pos_link = g_list_find (axis->priv->mtics, child);

    if (pos_link)
    {
      position = g_list_position (axis->priv->mtics, pos_link);
      position += g_list_length (axis->priv->indicators);
      position += g_list_length (axis->priv->tics);
    }

    if (position >= 0)
      gtk_widget_path_append_with_siblings (path, sibling_path, position);
    else
      gtk_widget_path_append_for_widget (path, child);

    gtk_widget_path_unref (sibling_path);
  }
  else
    gtk_widget_path_append_for_widget (path, child);

  return path;
}

static void
gdv_axis_class_init (GdvAxisClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
  GtkContainerClass *container_class = GTK_CONTAINER_CLASS (klass);

  object_class->dispose = gdv_axis_dispose;
  object_class->finalize = gdv_axis_finalize;
  object_class->set_property = gdv_axis_set_property;
  object_class->get_property = gdv_axis_get_property;

  container_class->add = gdv_axis_add;
  container_class->remove = gdv_axis_remove;
  container_class->forall = gdv_axis_forall;
  container_class->get_path_for_child = gdv_axis_get_path_for_child;

  /* FIXME: The allocated space for the indicator is atm the full axis-space;
   *        this should be optimized
   */
  widget_class->size_allocate =
    gdv_axis_size_allocate;

  widget_class->draw =
    gdv_axis_draw;

  widget_class->get_preferred_width =
    gdv_axis_get_preferred_width;
  widget_class->get_preferred_height =
    gdv_axis_get_preferred_height;
  widget_class->get_preferred_height_for_width =
    gdv_axis_get_preferred_height_for_width;
  widget_class->get_preferred_width_for_height =
    gdv_axis_get_preferred_width_for_height;

  klass->get_point =
    gdv_axis_on_evaluate_unimplemented;
  klass->get_inner_dir =
    gdv_axis_on_evaluate_inndir_unimplemented;
  klass->make_tic_label_markup =
    gdv_axis_make_tic_label_markup_unimplemented;

  klass->get_space_to_beg_position =
    gdv_axis_get_space_to_beg_position_unimplemented;
  klass->get_space_to_end_position  =
    gdv_axis_get_space_to_end_position_unimplemented;

  gtk_widget_class_set_css_name (widget_class, "axis");

  /* Properties */

  /**
   * GdvAxis:axis-direction-start:
   *
   * The angle of orientation of the start point of the axis.
   *
   */
  axis_properties[PROP_GDV_AXIS_DIRECTION_START] =
    g_param_spec_double ("axis-orientation",
                         "angle of beginning of the axis on screen",
                         "angle between the beginning of the axis and the "
                         "vertical on screen",
                         -G_MAXDOUBLE,
                         G_MAXDOUBLE,
                         0.0,
                         G_PARAM_READWRITE);

  /**
   * GdvAxis:axis-direction-outside:
   *
   * The angle of orientation of the outside of the axis relative to the top.
   *
   */
  axis_properties[PROP_GDV_AXIS_DIRECTION_OUTER_SIDE] =
    g_param_spec_double ("axis-direction-outside",
                         "axis of direction of the outside",
                         "direction of the axis-outside relative to the "
                         "upside of the screen",
                         -G_MAXDOUBLE,
                         G_MAXDOUBLE,
                         0.5 * M_PI,
                         G_PARAM_READWRITE);

  /**
   * GdvAxis:scale-beg-val:
   *
   * Determines the minimum value of the axis.
   *
   */
  axis_properties[PROP_GDV_SCALE_MIN_VAL] =
    g_param_spec_double ("scale-beg-val",
                         "minimum displayed data value",
                         "the data value, where the axis"
                         "should begin to show data",
                         -G_MAXDOUBLE,
                         G_MAXDOUBLE,
                         0.0,
                         G_PARAM_READWRITE);

  /**
   * GdvAxis:scale-end-val:
   *
   * Determines the maximum value of the axis.
   *
   */
  axis_properties[PROP_GDV_SCALE_MAX_VAL] =
    g_param_spec_double ("scale-end-val",
                         "maximum displayed data value",
                         "the data value, where the axis"
                         "should end to show data",
                         -G_MAXDOUBLE,
                         G_MAXDOUBLE,
                         100.0,
                         G_PARAM_READWRITE);

  /**
   * GdvAxis:scale-increment-val:
   *
   * Determines the increment value of the axis.
   *
   */
  axis_properties[PROP_GDV_SCALE_INCREMENT_VAL] =
    g_param_spec_double ("scale-increment-val",
                         "increment of axis in data-values",
                         "defines the increment of the axis, type-independent",
                         -G_MAXDOUBLE,
                         G_MAXDOUBLE,
                         100.0,
                         G_PARAM_READWRITE);



  /**
   * GdvAxis:scale-auto-increment:
   *
   * Determines whether the increment-value should be determined automatically.
   *
   */
  axis_properties[PROP_GDV_SCALE_AUTO_INCREMENT] =
    g_param_spec_boolean ("scale-auto-increment",
                          "automatic increment-value",
                          "determines, if the increment-value should "
                          "be resetted automatically",
                          TRUE,
                          G_PARAM_READWRITE);

  /**
   * GdvAxis:scale-limits-automatic:
   *
   * Determines whether if the scale-borders should be rounded automatically up
   * to the next better value.
   *
   * An annotation about the behaviour of GdvAxis:scale-limits-automatic:
   * The property does not implie, that there should be absolutely no changes
   * to the values of GdvAxis:tics-beg-val and GdvAxis:tics-end-val. It just
   * means, that the GdvAxis:scale-beg-val and GdvAxis:scale-end-val should
   * kept constant!
   */
  axis_properties[PROP_GDV_SCALE_AUTO_LIMITS] =
    g_param_spec_boolean ("scale-limits-automatic",
                          "automatic scale limits",
                          "determines, if the scale-borders should "
                          "be setted automatically",
                          TRUE,
                          G_PARAM_READWRITE);

  /**
   * GdvAxis:axis-beg-pix-x:
   *
   * Determines the x-value origin point of the axis.
   *
   */
  axis_properties[PROP_GDV_AXIS_PIX_BEG_X] =
    g_param_spec_double ("axis-beg-pix-x",
                         "beginning of the axis in pixel x-value",
                         "x-value of the absolute axis-start",
                         0.0,
                         G_MAXDOUBLE,
                         0.0,
                         G_PARAM_READWRITE);

  /**
   * GdvAxis:axis-beg-pix-y:
   *
   * Determines the y-value origin point of the axis.
   *
   */
  axis_properties[PROP_GDV_AXIS_PIX_BEG_Y] =
    g_param_spec_double ("axis-beg-pix-y",
                         "beginning of the axis in pixel y-value",
                         "y-value of the absolute axis-start",
                         0.0,
                         G_MAXDOUBLE,
                         0.0,
                         G_PARAM_READWRITE);

  /**
   * GdvAxis:axis-end-pix-x:
   *
   * Determines the x-value end point of the axis.
   *
   */
  axis_properties[PROP_GDV_AXIS_PIX_END_X] =
    g_param_spec_double ("axis-end-pix-x",
                         "end of the axis in pixel x-value",
                         "x-value of the absolute axis-end",
                         0.0,
                         G_MAXDOUBLE,
                         0.0,
                         G_PARAM_READWRITE);

  /**
   * GdvAxis:axis-end-pix-y:
   *
   * Determines the y-value end point of the axis.
   *
   */
  axis_properties[PROP_GDV_AXIS_PIX_END_Y] =
    g_param_spec_double ("axis-end-pix-y",
                         "end of the axis in pixel y-value",
                         "y-value of the absolute axis-end",
                         0.0,
                         G_MAXDOUBLE,
                         0.0,
                         G_PARAM_READWRITE);

  /**
   * GdvAxis:axis-beg-at-screen-x:
   *
   * Determines the x-value origin point of the axis relative to the layer-area.
   *
   * See the GdvAxis:axis-beg-pix-x property
   *
   */
  axis_properties[PROP_GDV_AXIS_BEG_AT_SCREEN_X] =
    g_param_spec_double ("axis-beg-at-screen-x",
                         "horizontal beginning of the axis",
                         "horizontal amount of the layer that determines the "
                         "starting of the axis",
                         -G_MAXDOUBLE,
                         G_MAXDOUBLE,
                         0.0,
                         G_PARAM_READWRITE);

  /**
   * GdvAxis:axis-beg-at-screen-y:
   *
   * Determines the x-value origin point of the axis relative to the layer-area.
   *
   * See the GdvAxis:axis-beg-pix-y property
   *
   */
  axis_properties[PROP_GDV_AXIS_BEG_AT_SCREEN_Y] =
    g_param_spec_double ("axis-beg-at-screen-y",
                         "vertical beginning of the axis",
                         "vertical amount of the layer that determines the "
                         "starting of the axis",
                         -G_MAXDOUBLE,
                         G_MAXDOUBLE,
                         0.0,
                         G_PARAM_READWRITE);

  /**
   * GdvAxis:axis-end-at-screen-x:
   *
   * Determines the x-value end point of the axis relative to the layer-area.
   *
   * See the GdvAxis:axis-end-pix-x property
   *
   */
  axis_properties[PROP_GDV_AXIS_END_AT_SCREEN_X] =
    g_param_spec_double ("axis-end-at-screen-x",
                         "horizontal ending of the axis",
                         "horizontal amount of the layer that determines the "
                         "end of the axis",
                         -G_MAXDOUBLE,
                         G_MAXDOUBLE,
                         0.0,
                         G_PARAM_READWRITE);

  /**
   * GdvAxis:axis-end-at-screen-y:
   *
   * Determines the x-value end point of the axis relative to the layer-area.
   *
   * See the GdvAxis:axis-end-pix-y property
   *
   */
  axis_properties[PROP_GDV_AXIS_END_AT_SCREEN_Y] =
    g_param_spec_double ("axis-end-at-screen-y",
                         "vertical ending of the axis",
                         "vertical amount of the layer that determines the "
                         "end of the axis",
                         -G_MAXDOUBLE,
                         G_MAXDOUBLE,
                         0.0,
                         G_PARAM_READWRITE);

  /**
   * GdvAxis:tics-beg-val:
   *
   * Determines the beginning value of tics-settings.
   *
   */
  axis_properties[PROP_GDV_TICS_BEG_VAL] =
    g_param_spec_double ("tics-beg-val",
                         "minimum data value for tics",
                         "the data value, where the tics"
                         "should begin to be drawn",
                         -G_MAXDOUBLE,
                         G_MAXDOUBLE,
                         0.0,
                         G_PARAM_READWRITE);

  /**
   * GdvAxis:tics-end-val:
   *
   * Determines the end value of tics-settings.
   *
   */
  axis_properties[PROP_GDV_TICS_END_VAL] =
    g_param_spec_double ("tics-end-val",
                         "maximum data value for tics",
                         "the data value, where the tics"
                         "should end to be drawn",
                         -G_MAXDOUBLE,
                         G_MAXDOUBLE,
                         100.0,
                         G_PARAM_READWRITE);

  /**
   * GdvAxis:tics-automatic:
   *
   * Determines whether tics should be setted automatically.
   *
   */
  axis_properties[PROP_GDV_TICS_AUTOMATIC] =
    g_param_spec_boolean ("tics-automatic",
                          "automatic tics-setting",
                          "determines, if tic-values should "
                          "be determined automatically",
                          TRUE,
                          G_PARAM_READWRITE);

  /**
   * GdvAxis:mtics-beg-val:
   *
   * Determines the beginning value of mtics-settings.
   *
   */
  axis_properties[PROP_GDV_MTICS_BEG_VAL] =
    g_param_spec_double ("mtics-beg-val",
                         "minimum data value for mtics",
                         "the data value, where the mtics"
                         "should begin to be drawn",
                         -G_MAXDOUBLE,
                         G_MAXDOUBLE,
                         0.0,
                         G_PARAM_READWRITE);

  /**
   * GdvAxis:mtics-automatic:
   *
   * Determines whether mtics should be setted automatically.
   *
   */
  axis_properties[PROP_GDV_MTICS_AUTOMATIC] =
    g_param_spec_boolean ("mtics-automatic",
                          "automatic mtics-setting",
                          "determines, if number of mtics and mtics-beg-val "
                          "and mtics-end-val should be setted automatically",
                          TRUE,
                          G_PARAM_READWRITE);

  /**
   * GdvAxis:mtics-number:
   *
   * Sets the number of mtics per scale-interval.
   *
   */
  axis_properties[PROP_GDV_NUMBER_MTICS] =
    g_param_spec_uint ("mtics",
                       "number of mtics in scale interval",
                       "sets the absolute number of mtics"
                       " per scale interval",
                       0,
                       G_MAXUINT,
                       4,
                       G_PARAM_READWRITE);

  /**
   * GdvAxis:mtics-end-val:
   *
   * Determines the end value of mtics-settings.
   *
   */
  axis_properties[PROP_GDV_MTICS_END_VAL] =
    g_param_spec_double ("mtics-end-val",
                         "maximum data value for mtics",
                         "the data value, where the mtics"
                         "should end to be drawn",
                         -G_MAXDOUBLE,
                         G_MAXDOUBLE,
                         100.0,
                         G_PARAM_READWRITE);

  /**
   * GdvAxis:title:
   *
   * The title-string of the axis.
   *
   */
  axis_properties[PROP_GDV_AXIS_TITLE] =
    g_param_spec_string ("title",
                         "holds the gtk-label associated as title of the axis",
                         "this contains the GtkLabel, that is associated as title "
                         "to the axis, or NULL, if the axis has no title",
                         NULL,
                         G_PARAM_READWRITE);

  /**
   * GdvAxis:title-widget:
   *
   * The title widget of the axis.
   *
   */
  axis_properties[PROP_GDV_AXIS_TITLE_WIDGET] =
    g_param_spec_object ("title-widget",
                         "holds the gtk-label associated as title to the axis",
                         "this contains the GtkLabel, that is associated as title "
                         "to the axis, or NULL, if the tic has no label",
                         GTK_TYPE_WIDGET,
                         G_PARAM_READWRITE);

  /* FIXME: If this property is active, the axis should check, if there is
   *  some misleading property (e.g. a axis-beg-at-screen-x == axis-end-at-screen-y).
   *  This leads atm. to a warning and crash.
   */
  /**
   * GdvAxis:force-beg-end:
   *
   * This property forces the axis to use the manually assigned beg- and end-
   * properties. This property should be used with care because it may lead
   * to a bad geometry-handling. However, in some cases one wants to bind the
   * axis-geomety to the geometry of a layer or underlying other widget.
   *
   * This might be usefull e.g. when painting an axis above a picture or live-stream.
   *
   */
  axis_properties[PROP_GDV_AXIS_FORCE_BEG_END] =
    g_param_spec_boolean ("force-beg-end",
                          "forces the use of manually assigned beg- and end- properties",
                          "this property forces the axis to use the manually assigned "
                          "beg- and end-properties",
                          FALSE,
                          G_PARAM_READWRITE);

  /* FIXME: Seriously rethink this concept */
  /*
   * GdvAxis:use-relative-values:
   *
   * Determines if the axis should rely on the relative or absolute properties
   * regarding the begin and end of the .
   *
   */
  /*  axis_properties[PROP_GDV_AXIS_USE_RELATIVE_PROP] =
      g_param_spec_boolean ("use-relative-values",
                            "forces the use of manually assigned beg- and end- properties",
                            "this property forces the axis to use the manually assigned "
                            "beg- and end-properties",
                            FALSE,
                            G_PARAM_READWRITE);
  */
  axis_properties[PROP_GDV_AXIS_RESIZE_DURING_REDRAW] =
    g_param_spec_boolean ("resize-during-redraw",
                          "*placeholder*",
                          "*placeholder*",
                          FALSE,
                          G_PARAM_PRIVATE | G_PARAM_READABLE);

  g_object_class_install_properties (object_class,
                                     N_PROPERTIES,
                                     axis_properties);

  /* Style-Properties */

  /**
   * GdvAxis:scale-min-diff-pix:
   *
   * Determines the minimum distance between two tics in pixels.
   *
   */
  gtk_widget_class_install_style_property (widget_class,
      g_param_spec_int ("scale-min-diff-pix",
                        "minimum distance between tics",
                        "determines the minimum distance "
                        "between two tics in pixels",
                        -G_MAXINT,
                        G_MAXINT,
                        40,
                        G_PARAM_READWRITE));

  /* FIXME: Make this property working */
  /**
   * GdvAxis:scale-max-diff-pix:
   *
   * Determines the maximum distance between two tics in pixels.
   *
   */
  gtk_widget_class_install_style_property (widget_class,
      g_param_spec_int ("scale-max-diff-pix",
                        "maximum distance between tics",
                        "determines the maximum distance "
                        "between two tics in pixels",
                        -G_MAXINT,
                        G_MAXINT,
                        300,
                        G_PARAM_READWRITE));

  /**
   * GdvAxis:line-wdith:
   *
   * Sets the line-width for axis-line.
   *
   */
  gtk_widget_class_install_style_property (widget_class,
      g_param_spec_double ("line-width",
                           "line-width of the main-line",
                           "line-width to draw the main line",
                           0.0,
                           G_MAXDOUBLE,
                           1.0,
                           G_PARAM_READWRITE));

  /**
   * GdvAxis:color:
   *
   * Sets the line-color for axis.
   *
   */
  gtk_widget_class_install_style_property (widget_class,
      g_param_spec_boxed ("color",
                          _("line-color"),
                          _("line-color to draw"),
                          GDK_TYPE_RGBA,
                          G_PARAM_READWRITE));

  /**
   * GdvAxis:show-title:
   *
   * Determines if the axis-title will be plotted or not.
   *
   */
  gtk_widget_class_install_style_property (widget_class,
      g_param_spec_boolean ("show-title",
                            "enables the axis title",
                            "determines whethter the axis-title should be drawn",
                            TRUE,
                            G_PARAM_READWRITE));

  /**
   * GdvAxis::get-point:
   * @axis: the axis which received the signal
   * @value: the point-value on the axis
   * @pos_x: the place to store the x-position
   * @pos_y: the place to store the y-position
   *
   * Signal to obtain the position of a value on screen for the axis.
   *
   * Returns: %TRUE if the point lies in the range of the axis.
   */
  axis_signals[GET_POINT] =
    g_signal_new ("get-point",
                  G_OBJECT_CLASS_TYPE (object_class),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (GdvAxisClass, get_point),
                  NULL, NULL,
                  NULL,
                  G_TYPE_BOOLEAN, 3,
                  G_TYPE_DOUBLE, G_TYPE_POINTER, G_TYPE_POINTER);



  /**
   * GdvAxis::get-inner-dir:
   * @axis: the axis which received the signal
   * @value: the point-value on the axis
   * @pos_x: the place to store the x-component of the direction
   * @pos_y: the place to store the y-component of the direction
   *
   * Signal to get the innder-dir of a value on the axis.
   *
   * Returns: %TRUE if the point lies in the range of the axis.
   */
  axis_signals[GET_INNER_DIR] =
    g_signal_new ("get-inner-dir",
                  G_OBJECT_CLASS_TYPE (object_class),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (GdvAxisClass, get_inner_dir),
                  NULL, NULL,
                  NULL,
                  G_TYPE_BOOLEAN, 3,
                  G_TYPE_DOUBLE, G_TYPE_POINTER, G_TYPE_POINTER);

  /**
   * GdvAxis::make-tic-label-markup:
   * @axis: the axis which received the signal
   * @value: the value that will be used for dermining the tic-label
   *
   * Signal to obtain a pango markup-text for a given value that is used to get
   * the tic-label over the axis.
   *
   */
  axis_signals[MAKE_TIC_LABEL_MARKUP] =
    g_signal_new ("make-tic-label-markup",
                  G_OBJECT_CLASS_TYPE (object_class),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (GdvAxisClass, make_tic_label_markup),
                  NULL, NULL,
                  NULL,
                  G_TYPE_STRING, 1,
                  G_TYPE_DOUBLE);

}

/**
 * gdv_axis_new:
 *
 * Just a common constructor.
 *
 * Returns: a new #GdvAxis
 **/
GdvAxis *gdv_axis_new (void)
{
  return g_object_new (gdv_axis_get_type (),
                       NULL);
}

gint _axis_compare_tic_value (gconstpointer a, gconstpointer b)
{
  GdvTic *tic = GDV_TIC (a);
  const gdouble *value = b;

  gdouble local_value = 0.0;

  g_object_get (tic, "value", &local_value, NULL);

  if ((gdouble) local_value == (gdouble) *value)
    return 0;
  else if (local_value > *value)
    return 1;
  else
    return -1;
}

/* FIXME: This function is useless due to floating-point precision problems! */
/**
 * gdv_axis_get_tic_for_value:
 * @axis: a #GdvAxis
 * @value: a given tic-value
 *
 * Returns a #GdvTic assigned to the axis with given value.
 *
 * This function should be used carefully. It is usual difficult to use due to
 * problems with the precision of Tic-valus and rounding-issues. Most of the
 * time it will be a better solution to use gdv_axis_get_tic_list() and compare
 * the tic-values to a specific precision-criteria.
 *
 * Returns: (nullable) (transfer none): The #GdvTic with the given Tic-value
 *
 */
GdvTic *gdv_axis_get_tic_for_value (GdvAxis *axis, gdouble value)
{
  GList *found_tic = NULL;

  g_return_val_if_fail (GDV_IS_AXIS (axis), NULL);

  found_tic =
    g_list_find_custom (axis->priv->tics,
                        (const gdouble *) &value,
                        _axis_compare_tic_value);

  if (!found_tic)
    found_tic =
      g_list_find_custom (axis->priv->mtics,
                          (const gdouble *) &value,
                          _axis_compare_tic_value);

  return (found_tic ? found_tic->data : NULL);
}

/**
 * gdv_axis_get_tic_list:
 * @axis: a #GdvAxis
 *
 * Lists the tics used by @axis.
 *
 * Returns: (transfer container) (element-type GdvTic):
 *     a newly allocated #GList of tics
 */
GList *gdv_axis_get_tic_list (GdvAxis *axis)
{
  GList *tic_list_copy = NULL;

  g_return_val_if_fail (GDV_IS_AXIS (axis), NULL);

  tic_list_copy = g_list_copy(axis->priv->tics);

  return tic_list_copy;
}

/**
 * gdv_axis_get_mtic_list:
 * @axis: a #GdvAxis
 *
 * Lists the minor tics used by @axis.
 *
 * Returns: (transfer container) (element-type GdvMtic):
 *     a newly allocated #GList of mtics
 */
GList *gdv_axis_get_mtic_list (GdvAxis *axis)
{
  GList *mtic_list_copy = NULL;

  g_return_val_if_fail (GDV_IS_AXIS (axis), NULL);

  mtic_list_copy = g_list_copy(axis->priv->mtics);

  return mtic_list_copy;
}

/**
 * gdv_axis_get_indicator_list:
 * @axis: a #GdvAxis
 *
 * Lists the indicators used by @axis.
 *
 * Returns: (transfer container) (element-type GdvIndicator):
 *     a newly allocated #GList of indicators
 */
GList *gdv_axis_get_indicator_list (GdvAxis *axis)
{
  GList *indicator_list_copy = NULL;

  g_return_val_if_fail (GDV_IS_AXIS (axis), NULL);

  indicator_list_copy = g_list_copy(axis->priv->indicators);

  return indicator_list_copy;
}

/**
 * gdv_axis_title_set_markup:
 * @axis: a #GdvAxis
 * @markup: (allow-none): the text to use as the title of the axis
 *
 * Sets the text of the title. If @markup is %NULL,
 * the current title is removed.
 **/
void gdv_axis_title_set_markup (GdvAxis *axis, gchar *markup)
{
  g_return_if_fail (GDV_IS_AXIS (axis));

  if (!markup)
    gdv_axis_set_title_widget (axis, NULL);
  else
  {
    GtkWidget *child = gtk_label_new ("");

    gtk_label_set_markup (GTK_LABEL (child), markup);
    gtk_widget_show (child);

    gdv_axis_set_title_widget (axis, child);
  }
}

/**
 * gdv_axis_get_title:
 * @axis: a #GdvAxis
 *
 * If the axis’ title widget is a #GtkLabel, returns the
 * text in the label widget. (The frame will have a #GtkLabel
 * for the title widget if a non-%NULL argument was passed
 * to gtk_frame_new().)
 *
 * Returns: (nullable): the text in the title, or %NULL if there
 *               was no title widget or the lable widget was not
 *               a #GtkLabel. This string is owned by GTK+ and
 *               must not be modified or freed.
 **/
const gchar *gdv_axis_get_title (GdvAxis *axis)
{
  GdvAxisPrivate *priv;

  g_return_val_if_fail (GDV_IS_AXIS (axis), NULL);

  priv = axis->priv;

  if (GTK_IS_LABEL (priv->title))
    return gtk_label_get_text (GTK_LABEL (priv->title));
  else
    return NULL;
}

/**
 * gdv_axis_set_title_widget:
 * @axis: a #GdvAxis
 * @title_widget: the #GtkWidget that should be used as new title or %NULL.
 *
 * Sets the title widget for the axis. This is the widget that
 * will appear next to the axis as additional information.
 **/
void gdv_axis_set_title_widget (GdvAxis  *axis, GtkWidget *title_widget)
{
  GdvAxisPrivate *priv;
//  gboolean need_resize = FALSE;

  g_return_if_fail (GDV_IS_AXIS (axis));
  g_return_if_fail (title_widget == NULL || GTK_IS_WIDGET (title_widget));
  g_return_if_fail (title_widget == NULL || gtk_widget_get_parent (title_widget) == NULL);

  priv = axis->priv;

  if (priv->title == title_widget)
    return;

  if (priv->title)
  {
//    need_resize = gdv_axis_title_widget_is_visible (axis);
    gtk_widget_unparent (priv->title);
  }

  priv->title = title_widget;

  if (title_widget)
  {
    gtk_widget_show (GTK_WIDGET (axis));
    gtk_widget_set_parent (title_widget, GTK_WIDGET (axis));
  }

  g_object_freeze_notify (G_OBJECT (axis));
  g_object_notify_by_pspec (G_OBJECT (axis), axis_properties[PROP_GDV_AXIS_TITLE_WIDGET]);
  g_object_notify_by_pspec (G_OBJECT (axis),  axis_properties[PROP_GDV_AXIS_TITLE]);
  g_object_thaw_notify (G_OBJECT (axis));
}

/**
 * gdv_axis_get_show_title:
 * @axis: a #GdvAxis
 *
 * Returns: %TRUE if the title should be shown
 **/
gboolean gdv_axis_get_show_title (GdvAxis *axis)
{
  gboolean axis_title;

  g_return_val_if_fail (GDV_IS_TIC (axis), FALSE);

  gtk_widget_style_get (GTK_WIDGET (axis),
                        "show-title", &axis_title,
                        NULL);

  return axis_title;
}

/**
 * gdv_axis_get_borders:
 * @axis: a #GdvAxis
 * @max_val: the maximum scale-value of the axis
 * @min_val: the minimum scale-value of the axis
 * @max_pix_x: the maximum x-value of the axis
 * @min_pix_x: the minimum x-value of the axis
 * @max_pix_y: the maximum y-value of the axis
 * @min_pix_y: the minimum y-value of the axis
 *
 **/
void
gdv_axis_get_borders (GdvAxis *axis,
                      gdouble *max_val, gdouble *min_val,
                      guint *max_pix_x, guint *min_pix_x,
                      guint *max_pix_y, guint *min_pix_y)
{
  g_return_if_fail (GDV_IS_AXIS (axis));

  *max_val = axis->priv->scale_max_val;
  *min_val = axis->priv->scale_min_val;
  *max_pix_x = (guint) axis->priv->axis_end_pix_x;
  *min_pix_x = (guint) axis->priv->axis_beg_pix_x;
  *max_pix_y = (guint) axis->priv->axis_end_pix_y;
  *min_pix_y = (guint) axis->priv->axis_beg_pix_y;
}

static void
gdv_axis_get_space_to_beg_position_unimplemented (
  GdvAxis             *axis,
  GtkPositionType      direction,
  int                  for_size,
  int                 *minimum,
  int                 *natural,
  gpointer             data)
{
  g_return_if_fail (GDV_IS_AXIS (axis));

  g_warning ("GdvAxis::get-space-to-beg-position not implemented for '%s'",
             g_type_name (G_TYPE_FROM_INSTANCE (axis)));

  *minimum = 0;
  *natural = 0;

}

static void
gdv_axis_get_space_to_end_position_unimplemented (
  GdvAxis             *axis,
  GtkPositionType      direction,
  int                  for_size,
  int                 *minimum,
  int                 *natural,
  gpointer             data)
{
  g_return_if_fail (GDV_IS_AXIS (axis));

  g_warning ("GdvAxis::get-space-to-end-position not implemented for '%s'",
             g_type_name (G_TYPE_FROM_INSTANCE (axis)));

  *minimum = 0;
  *natural = 0;
}

static void
gdv_axis_measure (
  GdvAxis             *axis,
  GtkOrientation       orientation,
  int                  for_size,
  int                 *minimum,
  int                 *natural,
  int                 *minimum_baseline,
  int                 *natural_baseline,
  gpointer             data)
{
  GList *tics_list;
  gint glob_tic_min_start = 0, glob_tic_nat_start = 0,
       glob_tic_min_stop = 0, glob_tic_nat_stop = 0;
  gint scale_min_diff, projected_min_diff = 0;
  gdouble axis_line_width;
  gdouble axis_direction = axis->priv->direction_start;
  gboolean axis_title_on;
  gint title_height = 0, title_width = 0;
  gint title_height_nat = 0, title_width_nat = 0;
  gdouble outside_dir;

  *minimum = 0;
  *natural = 0;

  gtk_widget_style_get (GTK_WIDGET (axis),
                        "scale-min-diff-pix", &scale_min_diff,
                        "line-width", &axis_line_width,
                        NULL);

  if (orientation == GTK_ORIENTATION_HORIZONTAL)
    projected_min_diff =
      (gint) (fabs(sin (axis_direction)) * ((gdouble) scale_min_diff + 0.5) +
              fabs(cos (axis_direction)) * axis_line_width);
  else
    projected_min_diff =
      (gint) ((fabs(cos (axis_direction)) * ((gdouble) scale_min_diff + 0.5))
              + fabs(sin (axis_direction)) * axis_line_width);

  /* FIXME: this is not really not an elegant solution here */
  for (
    tics_list = axis->priv->tics;
    tics_list;
    tics_list = tics_list->next)
  {
    GdvTic *local_tic = tics_list->data;
    gint loc_tic_min_start, loc_tic_nat_start,
         loc_tic_min_stop, loc_tic_nat_stop;
    gint loc_tic_min_size = 0, loc_tic_nat_size = 0;

    if (orientation == GTK_ORIENTATION_HORIZONTAL)
    {
      gdv_tic_get_space_to_tic_position (
        local_tic, GTK_POS_LEFT, for_size,
        &loc_tic_min_start, &loc_tic_nat_start, data);
      gdv_tic_get_space_to_tic_position (
        local_tic, GTK_POS_RIGHT, for_size,
        &loc_tic_min_stop, &loc_tic_nat_stop, data);

      /* this is more for the record and not obligatoric */
      if (for_size > 0)
        gtk_widget_get_preferred_width_for_height (
          GTK_WIDGET (local_tic), for_size, &loc_tic_min_size, &loc_tic_nat_size);
      else
        gtk_widget_get_preferred_width (
          GTK_WIDGET (local_tic), &loc_tic_min_size, &loc_tic_nat_size);
    }
    else
    {
      gdv_tic_get_space_to_tic_position (
        local_tic, GTK_POS_TOP, for_size,
        &loc_tic_min_start, &loc_tic_nat_start, data);
      gdv_tic_get_space_to_tic_position (
        local_tic, GTK_POS_BOTTOM, for_size,
        &loc_tic_min_stop, &loc_tic_nat_stop, data);

      /* this is more fore the record and not obligatoric */
      if (for_size > 0)
        gtk_widget_get_preferred_height_for_width (
          GTK_WIDGET (local_tic), for_size, &loc_tic_min_size, &loc_tic_nat_size);
      else
        gtk_widget_get_preferred_height (
          GTK_WIDGET (local_tic), &loc_tic_min_size, &loc_tic_nat_size);
    }

    glob_tic_min_start =
      (glob_tic_min_start < loc_tic_min_start ?
       loc_tic_min_start : glob_tic_min_start);
    glob_tic_min_stop =
      glob_tic_min_stop < loc_tic_min_stop ?
      loc_tic_min_stop : glob_tic_min_stop;
    glob_tic_nat_start =
      glob_tic_nat_start < loc_tic_nat_start ?
      loc_tic_nat_start : glob_tic_nat_start;
    glob_tic_nat_stop =
      glob_tic_nat_stop < loc_tic_nat_stop ?
      loc_tic_nat_stop : glob_tic_nat_stop;
  }

  /* Getting the size of the minor tics */
  /*  for (
      tics_list = axis->priv->mtics;
      tics_list;
      tics_list = tics_list->next)
    {
      GdvTic *local_tic = tics_list->data;
      gint loc_tic_min_start, loc_tic_nat_start,
           loc_tic_min_stop, loc_tic_nat_stop;
      gint loc_tic_min_size = 0, loc_tic_nat_size = 0;

      if (orientation == GTK_ORIENTATION_HORIZONTAL)
      {
        gdv_tic_get_space_to_tic_position (
          local_tic, GTK_POS_LEFT, for_size,
          &loc_tic_min_start, &loc_tic_nat_start, data);
        gdv_tic_get_space_to_tic_position (
          local_tic, GTK_POS_RIGHT, for_size,
          &loc_tic_nat_start, &loc_tic_nat_stop, data);
  */
  /* this is more fore the record and not obligatoric */
  /*      if (for_size > 0)
          gtk_widget_get_preferred_width_for_height (
            GTK_WIDGET (local_tic), for_size, &loc_tic_min_size, &loc_tic_nat_size);
        else
          gtk_widget_get_preferred_width (
            GTK_WIDGET (local_tic), &loc_tic_min_size, &loc_tic_nat_size);
      }
      else
      {
        gdv_tic_get_space_to_tic_position (
          local_tic, GTK_POS_TOP, for_size,
          &loc_tic_min_start, &loc_tic_nat_start, data);
        gdv_tic_get_space_to_tic_position (
          local_tic, GTK_POS_BOTTOM, for_size,
          &loc_tic_nat_start, &loc_tic_nat_stop, data);
  */
  /* this is more fore the record and not obligatoric */
  /*      if (for_size > 0)
          gtk_widget_get_preferred_height_for_width (
            GTK_WIDGET (local_tic), for_size, &loc_tic_min_size, &loc_tic_nat_size);
        else
          gtk_widget_get_preferred_height (
            GTK_WIDGET (local_tic), &loc_tic_min_size, &loc_tic_nat_size);
      }

      glob_tic_min_start =
        glob_tic_min_start < loc_tic_min_start ?
          loc_tic_min_start : glob_tic_min_start;
      glob_tic_min_stop =
        glob_tic_min_stop < loc_tic_min_stop ?
          loc_tic_min_stop : glob_tic_min_stop;
      glob_tic_nat_start =
        glob_tic_nat_start < loc_tic_nat_start ?
          loc_tic_nat_start : glob_tic_nat_start;
      glob_tic_nat_stop =
        glob_tic_nat_stop < loc_tic_nat_stop ?
          loc_tic_nat_stop : glob_tic_nat_stop;
    }
  */
  *minimum =
    glob_tic_min_start + glob_tic_min_stop + projected_min_diff;
  *natural =
    glob_tic_nat_start + glob_tic_nat_stop + projected_min_diff;

  /* Measuring the title */
  g_object_get (axis, "axis-direction-outside", &outside_dir, NULL);

  axis_title_on = gdv_axis_title_widget_is_visible (axis);

  if (axis_title_on)
  {
    if (orientation == GTK_ORIENTATION_VERTICAL)
    {
      if (for_size > 0)
        gtk_widget_get_preferred_height_for_width (
          axis->priv->title, for_size, &title_height, &title_height_nat);
      else
        gtk_widget_get_preferred_height (
          axis->priv->title, &title_height, &title_height_nat);

      *minimum += (gint) (fabs(cos(outside_dir)) * (gdouble) title_height);
      *natural += (gint) (fabs(cos(outside_dir)) * (gdouble) title_height_nat);

      *minimum =
        (gint) fmax(
          (gdouble) * minimum,
          fabs(sin(outside_dir)) * (gdouble) title_height);
      *natural =
        (gint) fmax(
          (gdouble) * natural,
          fabs(sin(outside_dir)) * (gdouble) title_height_nat);
    }
    else
    {
      if (for_size > 0)
        gtk_widget_get_preferred_width_for_height (
          axis->priv->title, for_size, &title_width, &title_width_nat);
      else
        gtk_widget_get_preferred_width (
          axis->priv->title, &title_width, &title_width_nat);

      *minimum += (gint) (fabs(sin(outside_dir)) * (gdouble) title_width);
      *natural += (gint) (fabs(sin(outside_dir)) * (gdouble) title_width_nat);

      *minimum =
        (gint) fmax(
          (gdouble) * minimum,
          fabs(cos(outside_dir)) * (gdouble) title_width);
      *natural =
        (gint) fmax(
          (gdouble) * natural,
          fabs(cos(outside_dir)) * (gdouble) title_width_nat);
    }
  }
}

static gboolean gdv_axis_title_widget_is_visible (GdvAxis *axis)
{
  gboolean style_show;

  gtk_widget_style_get (GTK_WIDGET (axis),
                        "show-title", &style_show,
                        NULL);

  style_show = TRUE;

  return axis->priv->title &&
         gtk_widget_get_visible (axis->priv->title) &&
         style_show;
}

static void
gdv_axis_size_allocate (GtkWidget     *widget,
                        GtkAllocation *allocation)
{
  GdvAxis *axis;
  GList *local_indicator_list, *local_tic_list, *local_mtic_list;

  axis = GDV_AXIS (widget);

  /* reallocating axis-indicator */
  local_indicator_list = axis->priv->indicators;

  while (local_indicator_list)
  {
    GdvIndicator *local_indicator;
    GtkAllocation indicator_allocation;
    gint indicator_width, indicator_height,
         indicator_min_width, indicator_min_height;

    local_indicator = local_indicator_list->data;

    gtk_widget_get_preferred_width (
      GTK_WIDGET (local_indicator),
      &indicator_min_width,
      &indicator_width);
    gtk_widget_get_preferred_height (
      GTK_WIDGET (local_indicator),
      &indicator_min_height,
      &indicator_height);

    indicator_allocation.x = allocation->x;
    indicator_allocation.y = allocation->y;
    indicator_allocation.width  = MAX (1, allocation->width);
    indicator_allocation.height = MAX (1, allocation->height);

    if (local_indicator &&
        gtk_widget_get_visible (GTK_WIDGET(local_indicator)))
      gtk_widget_size_allocate (GTK_WIDGET (local_indicator), &indicator_allocation);

    local_indicator_list = local_indicator_list->next;
  }

  /* reallocating axis-tics */
  local_tic_list = axis->priv->tics;
  local_mtic_list = axis->priv->mtics;

  while (local_tic_list || local_mtic_list)
  {
    GdvIndicator *local_tic;
    GtkAllocation tic_allocation;
    gdouble local_value;
    gdouble actual_pos_x, actual_pos_y;
    gdouble inner_dir_x, inner_dir_y;
    gdouble tic_label_halign, tic_label_valign;
    gint tmp_min, tmp_nat;

    if (local_tic_list)
    {
      local_tic = local_tic_list->data;
      local_tic_list = local_tic_list->next;
    }
    else
    {
      local_tic = local_mtic_list->data;
      local_mtic_list = local_mtic_list->next;
    }

    g_object_get (local_tic, "value", &local_value, NULL);

    GDV_AXIS_GET_CLASS (axis)->get_point (
      axis,
      local_value,
      &actual_pos_x,
      &actual_pos_y);
    GDV_AXIS_GET_CLASS (axis)->get_inner_dir (
      axis,
      local_value,
      &inner_dir_x,
      &inner_dir_y);

    tic_label_halign = -0.5 * fabs(inner_dir_y);
    tic_label_valign = -0.5 * fabs(inner_dir_x);

    actual_pos_x += (gdouble) allocation->x;
    actual_pos_y += (gdouble) allocation->y;

    g_object_set (local_tic,
                  "label-xalign", tic_label_halign,
                  "label-yalign", tic_label_valign,
                  "axis-inner-dir-x", inner_dir_x,
                  "axis-inner-dir-y", inner_dir_y,
                  "pos-x", actual_pos_x,
                  "pos-y", actual_pos_y,
                  NULL);

    gdv_tic_get_space_to_tic_position (
      GDV_TIC (local_tic), GTK_POS_TOP, -1,
      &tmp_min, &tmp_nat, NULL);
    tic_allocation.y = (gint) actual_pos_y - tmp_min;
    gtk_widget_get_preferred_height (GTK_WIDGET (local_tic), &tmp_min, &tmp_nat);
    tic_allocation.height = tmp_min;
    gdv_tic_get_space_to_tic_position (
      GDV_TIC (local_tic), GTK_POS_LEFT, -1,
      &tmp_min, &tmp_nat, NULL);
    tic_allocation.x = (gint) actual_pos_x - tmp_min;
    gtk_widget_get_preferred_width (GTK_WIDGET (local_tic), &tmp_min, &tmp_nat);
    tic_allocation.width = tmp_min;

    /* This is necessary because of round-down errors of the actual_pos */
    tic_allocation.height += 1;
    tic_allocation.width += 1;

    if (gtk_widget_get_visible(GTK_WIDGET (local_tic)))
      gtk_widget_size_allocate (
        GTK_WIDGET (local_tic), &tic_allocation);
  }
}

static gboolean
gdv_axis_draw (GtkWidget    *widget,
               cairo_t      *cr)
{
  guint     beg_x,
            beg_y,
            end_x,
            end_y;
  gdouble   axis_line_width = 0.0;
//  gdouble   scale_beg_val, scale_end_val;
  GtkStyleContext *context;

  GdvAxis *axis = GDV_AXIS (widget);

  context = gtk_widget_get_style_context (widget);

  gtk_widget_style_get (widget,
                        "line-width", &axis_line_width,
                        NULL);

  beg_x = (guint) axis->priv->axis_beg_pix_x;
  beg_y = (guint) axis->priv->axis_beg_pix_y;
  end_x = (guint) axis->priv->axis_end_pix_x;
  end_y = (guint) axis->priv->axis_end_pix_y;

//  scale_beg_val = axis->priv->scale_min_val;
//  scale_end_val = axis->priv->scale_max_val;

  if (axis_line_width && cr != NULL &&
      (!axis->priv->resize_during_redraw || axis->priv->force_beg_end))
  {
    /* plotting axis-line */
    gdv_render_line (
      context, cr,
      (gdouble) beg_x,
      (gdouble) beg_y,
      (gdouble) end_x,
      (gdouble) end_y);
  }

  if (axis->priv->resize_during_redraw && !axis->priv->force_beg_end)
  {
    gtk_widget_queue_resize (GTK_WIDGET (widget));
    axis->priv->resize_during_redraw = FALSE;
    return TRUE;
  }
  else
  {
    GTK_WIDGET_CLASS (gdv_axis_parent_class)->draw (widget, cr);
  }

  return FALSE;
}

static void
gdv_axis_get_preferred_width (GtkWidget           *widget,
                              gint                *minimum_size,
                              gint                *natural_size)
{
  gdv_axis_measure (GDV_AXIS (widget),
                    GTK_ORIENTATION_HORIZONTAL,
                    -1,
                    minimum_size, natural_size,
                    NULL, NULL, NULL);
}

static void
gdv_axis_get_preferred_height (GtkWidget           *widget,
                               gint                *minimum_size,
                               gint                *natural_size)
{
  gdv_axis_measure (GDV_AXIS (widget),
                    GTK_ORIENTATION_VERTICAL,
                    -1,
                    minimum_size, natural_size,
                    NULL, NULL, NULL);
}

static void
gdv_axis_get_preferred_height_for_width (GtkWidget           *widget,
    gint                 width,
    gint                *minimum_height,
    gint                *natural_height)
{
  gdv_axis_measure (GDV_AXIS (widget),
                    GTK_ORIENTATION_VERTICAL,
                    width,
                    minimum_height, natural_height,
                    NULL, NULL, NULL);
}


static void
gdv_axis_get_preferred_width_for_height (GtkWidget           *widget,
    gint                 height,
    gint                *minimum_width,
    gint                *natural_width)
{
  gdv_axis_measure (GDV_AXIS (widget),
                    GTK_ORIENTATION_HORIZONTAL,
                    height,
                    minimum_width, natural_width,
                    NULL, NULL, NULL);
}

/**
 * gdv_axis_queue_value_interval:
 * @axis: a #GdvAxis
 * @value: an new value
 *
 * The function compares the #value against the scale-begin and -end. If the
 * value is outside the #axis range, the #axis scale-beg-val or scale-end-val
 * will be adjusted to the value.
 *
 * This function is in particular usefull to update an axis for new values.
 *
 * Returns: %TRUE if the point is oudside the range of the layer-axes and %FALSE
 *      otherwise
 **/
gboolean gdv_axis_queue_value_interval (GdvAxis *axis, gdouble value)
{
  if (!axis)
    return FALSE;

  if (value < axis->priv->scale_min_val)
  {
    axis->priv->scale_min_val = value;
    g_object_notify (G_OBJECT (axis), "scale-beg-val");
    return TRUE;
  }

  if (value > axis->priv->scale_max_val)
  {
    axis->priv->scale_max_val = value;
    g_object_notify (G_OBJECT (axis), "scale-end-val");
    return TRUE;
  }

  return FALSE;
}

static gboolean
gdv_axis_on_evaluate_unimplemented (GdvAxis *axis, gdouble value, gdouble *pos_x, gdouble *pos_y)
{
  g_warning ("GdvAxis::get-point not implemented for '%s'",
             g_type_name (G_TYPE_FROM_INSTANCE (axis)));

  return FALSE;
}

static gboolean
gdv_axis_on_evaluate_inndir_unimplemented (GdvAxis *axis,
    gdouble value,
    gdouble *pos_x,
    gdouble *pos_y)
{
  g_warning ("GdvAxis::get-inner-dir not implemented for '%s'",
             g_type_name (G_TYPE_FROM_INSTANCE (axis)));

  return FALSE;
}

static gchar *
gdv_axis_make_tic_label_markup_unimplemented (
  GdvAxis *axis,
  gdouble  value)
{
  g_warning ("GdvAxis::make-tic-label not implemented for '%s'",
             g_type_name (G_TYPE_FROM_INSTANCE (axis)));

  return NULL;
}

