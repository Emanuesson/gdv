/*
 * gdvtic.c
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

#include <gsl/gsl_math.h>

/**
 * SECTION:gdvtic
 * @title: GdvTic
 *
 * #GdvTic just defines an axis-tic, i.e. a small straight line with an widget
 *  (usually an GtkLabel with some sort of information about the specific
 *  tic-value).
 *
 * # CSS nodes
 *
 * GdvTic uses a single CSS node with name tic.
 *
 */

#include "gdvtic.h"
#include "gdv-data-boxed.h"
#include "gdvrender.h"

/* Define Properties */
enum
{
  PROP_0,

  PROP_GDV_TIC_VALUE,
  PROP_GDV_TIC_POS_X,
  PROP_GDV_TIC_POS_Y,

  PROP_GDV_TIC_AXIS_INNER_DIR_X,
  PROP_GDV_TIC_AXIS_INNER_DIR_Y,

  PROP_GDV_TIC_LABEL_XALIGN,
  PROP_GDV_TIC_LABEL_YALIGN,

  PROP_GDV_TIC_LABEL,
  PROP_GDV_TIC_LABEL_WIDGET,

  N_PROPERTIES
};

/* --- variables --- */

struct _GdvTicPrivate
{
  gdouble          tic_beg_x;
  gdouble          tic_beg_y;
  gdouble          tic_end_x;
  gdouble          tic_end_y;

  gdouble          tic_val;
  gfloat           curr_pos_in_x;
  gfloat           curr_pos_in_y;

  gdouble          axis_dir_x;
  gdouble          axis_dir_y;

  gdouble          axis_inner_dir_x;
  gdouble          axis_inner_dir_y;

  gdouble          label_xalign;
  gdouble          label_yalign;

  GtkWidget       *label;
  GtkAllocation    label_allocation;

  //  Temporary in
  gboolean         show_label;
};

static GParamSpec *tic_properties[N_PROPERTIES] = { NULL, };

static gboolean
gdv_tic_label_widget_is_visible (GdvTic *tic);
static void
gdv_tic_size_allocate (GtkWidget     *widget,
                       GtkAllocation *allocation);
static void gdv_tic_remove  (GtkContainer   *container,
                             GtkWidget      *child);
static void gdv_tic_forall        (GtkContainer   *container,
                                   gboolean	     include_internals,
                                   GtkCallback     callback,
                                   gpointer        callback_data);
static gboolean
gdv_tic_draw (GtkWidget    *widget,
              cairo_t      *cr);
static void
gdv_tic_get_preferred_width (GtkWidget           *widget,
                             gint                *minimum_size,
                             gint                *natural_size);
static void
gdv_tic_get_preferred_height (GtkWidget           *widget,
                              gint                *minimum_size,
                              gint                *natural_size);
static void
gdv_tic_get_preferred_height_for_width (GtkWidget           *widget,
                                        gint                 width,
                                        gint                *minimum_height,
                                        gint                *natural_height);
static void
gdv_tic_get_preferred_width_for_height (GtkWidget           *layout,
                                        gint                 width,
                                        gint                *minimum_height,
                                        gint                *natural_height);
static void
gdv_tic_dispose (GObject *object);
static void
gdv_tic_finalize (GObject *object);

G_DEFINE_TYPE_WITH_PRIVATE (GdvTic, gdv_tic, GTK_TYPE_BIN);

static void
gdv_tic_init (GdvTic *tic)
{
  GtkWidget *widget = GTK_WIDGET (tic);
  GtkStyleContext *style_context;
  GtkCssProvider *css_provider;

  css_provider = gtk_css_provider_new ();
  style_context = gtk_widget_get_style_context (widget);
  gtk_css_provider_load_from_resource (
    css_provider,
    "/net/gdv/libgdv/themes/default_general.css");

  gtk_style_context_add_provider (
    style_context,
    GTK_STYLE_PROVIDER (css_provider),
    GTK_STYLE_PROVIDER_PRIORITY_FALLBACK);

  tic->priv = gdv_tic_get_instance_private (tic);

  tic->priv->tic_val = 0.0;
  tic->priv->curr_pos_in_x = 0.0;
  tic->priv->curr_pos_in_y = 0.0;

  tic->priv->axis_inner_dir_x = -1.0;
  tic->priv->axis_inner_dir_y = 0.0;

  tic->priv->label_xalign = 0.0;
  tic->priv->label_yalign = -0.5;

  tic->priv->label = NULL;

//  gtk_widget_set_visible (tic->priv->label, TRUE);

  tic->priv->show_label = TRUE;

}


static void
gdv_tic_set_property (GObject      *object,
                      guint         property_id,
                      const GValue *value,
                      GParamSpec   *pspec)
{
  GdvTic *self;
  gchar *label_dup;

  self = GDV_TIC (object);

  switch (property_id)
  {
  case PROP_GDV_TIC_VALUE:
    self->priv->tic_val = g_value_get_double (value);
    break;

  case PROP_GDV_TIC_POS_X:
    gdv_tic_set_tic_position (self, g_value_get_float (value),
                              self->priv->curr_pos_in_y);
    break;

  case PROP_GDV_TIC_POS_Y:
    gdv_tic_set_tic_position (self, self->priv->curr_pos_in_x,
                              g_value_get_float (value));
    break;

  case PROP_GDV_TIC_AXIS_INNER_DIR_X:
    self->priv->axis_inner_dir_x = g_value_get_double (value);
    break;

  case PROP_GDV_TIC_AXIS_INNER_DIR_Y:
    self->priv->axis_inner_dir_y = g_value_get_double (value);
    break;

  case PROP_GDV_TIC_LABEL_XALIGN:
    self->priv->label_xalign = g_value_get_double (value);
    break;

  case PROP_GDV_TIC_LABEL_YALIGN:
    self->priv->label_yalign = g_value_get_double (value);
    break;

  case PROP_GDV_TIC_LABEL:
    label_dup = g_value_dup_string (value);
    gdv_tic_label_set_markup (self, label_dup);
    g_free (label_dup);
    break;

  case PROP_GDV_TIC_LABEL_WIDGET:
    gdv_tic_set_label_widget (self, g_value_get_object (value));
    break;

  default:
    /* unknown property */
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }
}


static void
gdv_tic_get_property (GObject    *object,
                      guint       property_id,
                      GValue     *value,
                      GParamSpec *pspec)
{
  GdvTic *self = GDV_TIC (object);

  switch (property_id)
  {
  case PROP_GDV_TIC_VALUE:
    g_value_set_double (value, self->priv->tic_val);
    break;

  case PROP_GDV_TIC_POS_X:
    g_value_set_float (value, self->priv->curr_pos_in_x);
    break;

  case PROP_GDV_TIC_POS_Y:
    g_value_set_float (value, self->priv->curr_pos_in_y);
    break;

  case PROP_GDV_TIC_AXIS_INNER_DIR_X:
    g_value_set_double (value, self->priv->axis_inner_dir_x);
    break;

  case PROP_GDV_TIC_AXIS_INNER_DIR_Y:
    g_value_set_double (value, self->priv->axis_inner_dir_y);
    break;

  case PROP_GDV_TIC_LABEL_XALIGN:
    g_value_set_double (value, self->priv->label_xalign);
    break;

  case PROP_GDV_TIC_LABEL_YALIGN:
    g_value_set_double (value, self->priv->label_yalign);
    break;

  case PROP_GDV_TIC_LABEL:
    g_value_set_string (value, gdv_tic_get_label (self));
    break;

  case PROP_GDV_TIC_LABEL_WIDGET:
    g_value_set_object (value,
                        self->priv->label ?
                        G_OBJECT (self->priv->label) : NULL);
    break;

  default:
    /* unknown property */
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }
}

/**
 * gdv_tic_new:
 * @label: (allow-none): the text to use as the label of the tic
 *
 * Creates a new #GdvTic, with optional label @label.
 * If @label is %NULL, the label is omitted.
 *
 * Returns: a new #GdvTic widget
 **/
GdvTic*
gdv_tic_new (const gchar *label)
{
  return g_object_new (GDV_TYPE_TIC, "label", label, NULL);
}

/**
 * gdv_tic_label_set_markup:
 * @tic: a #GdvTic
 * @markup: (allow-none): the text to use as the label of the tic
 *
 * Sets the text of the label. If @markup is %NULL,
 * the current label is removed.
 **/
void
gdv_tic_label_set_markup (GdvTic *tic,
                          gchar *markup)
{
  g_return_if_fail (GDV_IS_TIC (tic));

  if (!markup)
  {
    gdv_tic_set_label_widget (tic, NULL);
//      gtk_widget_queue_resize (GTK_WIDGET (tic));
//      if (gtk_widget_get_visible (GTK_WIDGET (tic)))
  }
  else
  {
    GtkWidget *child = gtk_label_new ("");

    gtk_label_set_markup (GTK_LABEL (child), markup);
    gtk_widget_show (child);

    gdv_tic_set_label_widget (tic, child);
  }
}

/**
 * gdv_tic_get_label:
 * @tic: a #GdvTic
 *
 * If the tic’s label widget is a #GtkLabel, returns the
 * text in the label widget. (The frame will have a #GtkLabel
 * for the label widget if a non-%NULL argument was passed
 * to gtk_frame_new().)
 *
 * Returns: (nullable): the text in the label, or %NULL if there
 *               was no label widget or the lable widget was not
 *               a #GtkLabel. This string is owned by LibGdv and
 *               must not be modified or freed.
 **/
const gchar *
gdv_tic_get_label (GdvTic *tic)
{
  GdvTicPrivate *priv;

  g_return_val_if_fail (GDV_IS_TIC (tic), NULL);

  priv = tic->priv;

  if (GTK_IS_LABEL (priv->label))
    return gtk_label_get_text (GTK_LABEL (priv->label));
  else
    return NULL;
}

/**
 * gdv_tic_set_label_widget:
 * @tic: a #GdvTic
 * @label_widget: (nullable): the #GtkWidget that is used as new label widget
 *
 * Sets the label widget for the tic. This is the widget that
 * will appear next to the tic as additional information.
 **/
void
gdv_tic_set_label_widget (GdvTic  *tic,
                          GtkWidget *label_widget)
{
  GdvTicPrivate *priv;
//  gboolean need_resize = FALSE;

  g_return_if_fail (GDV_IS_TIC (tic));
  g_return_if_fail (label_widget == NULL || GTK_IS_WIDGET (label_widget));
  g_return_if_fail (label_widget == NULL || gtk_widget_get_parent (label_widget) == NULL);

  priv = tic->priv;

  if (priv->label == label_widget)
    return;

  if (priv->label)
  {
//    need_resize = gdv_tic_label_widget_is_visible (tic);
    gtk_widget_unparent (priv->label);
  }

  priv->label = label_widget; /* unnecessary? */

  if (label_widget)
  {
    /* TODO: search for the reason, that a simple parent does not work! */
    gtk_widget_show (GTK_WIDGET (tic));
    gtk_widget_set_parent (label_widget, GTK_WIDGET (tic));
  }

  g_object_freeze_notify (G_OBJECT (tic));
  g_object_notify_by_pspec (G_OBJECT (tic), tic_properties[PROP_GDV_TIC_LABEL_WIDGET]);
  g_object_notify_by_pspec (G_OBJECT (tic),  tic_properties[PROP_GDV_TIC_LABEL]);
  g_object_thaw_notify (G_OBJECT (tic));
}

/**
 * gdv_tic_set_tic_postition:
 * @tic: a #GdvTic
 * @x: (nullable): the new x position
 * @y: (nullable): the new y position
 *
 * Sets the position for the tic. This is the position within the assigned
 * allocation of the tic, where the corresponding value should be displayed.
 **/
void gdv_tic_set_tic_position (GdvTic  *tic, gfloat x, gfloat y)
{
  GdvTicPrivate *priv;

  g_return_if_fail (GDV_IS_TIC (tic));

  priv = tic->priv;

  g_object_freeze_notify (G_OBJECT (tic));

  if (x != priv->curr_pos_in_x)
  {
    priv->curr_pos_in_x = x;
    g_object_notify_by_pspec (G_OBJECT (tic), tic_properties[PROP_GDV_TIC_POS_X]);
  }

  if (y != priv->curr_pos_in_y)
  {
    priv->curr_pos_in_y = y;
    g_object_notify_by_pspec (G_OBJECT (tic), tic_properties[PROP_GDV_TIC_POS_Y]);
  }

  g_object_thaw_notify (G_OBJECT (tic));
}

/**
 * gdv_tic_get_show_label:
 * @tic: a #GdvTic
 *
 * Returns: %TRUE if the label-widget should be shown
 **/
gboolean gdv_tic_get_show_label (GdvTic *tic)
{
  gboolean tic_label;

  g_return_val_if_fail (GDV_IS_TIC (tic), FALSE);

  gtk_widget_style_get (GTK_WIDGET (tic),
                        "show-label", &tic_label,
                        NULL);

  return tic_label;
}

static void
gdv_tic_dispose (GObject *object)
{
  G_OBJECT_CLASS (gdv_tic_parent_class)->dispose (object);
}

static void
gdv_tic_finalize (GObject *object)
{
  G_OBJECT_CLASS (gdv_tic_parent_class)->finalize (object);
}

static void
gdv_tic_class_init (GdvTicClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkContainerClass *container_class = GTK_CONTAINER_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->dispose = gdv_tic_dispose;
  object_class->finalize = gdv_tic_finalize;
  object_class->set_property = gdv_tic_set_property;
  object_class->get_property = gdv_tic_get_property;

  container_class->remove = gdv_tic_remove;
  container_class->forall = gdv_tic_forall;

  widget_class->size_allocate =
    gdv_tic_size_allocate;
  widget_class->draw =
    gdv_tic_draw;

  widget_class->get_preferred_width =
    gdv_tic_get_preferred_width;
  widget_class->get_preferred_height =
    gdv_tic_get_preferred_height;
  widget_class->get_preferred_height_for_width =
    gdv_tic_get_preferred_height_for_width;
  widget_class->get_preferred_width_for_height =
    gdv_tic_get_preferred_width_for_height;

  /* Properties */

  /**
   * GdvTic:value:
   *
   * Determines the value of the tic.
   *
   * Since: 0.1
   */
  tic_properties[PROP_GDV_TIC_VALUE] =
    g_param_spec_double ("value",
                         "the data-value at the tic",
                         "assigns the current value, that is representad by the "
                         "tic",
                         -G_MAXDOUBLE,
                         G_MAXDOUBLE,
                         0.0,
                         G_PARAM_READWRITE);

  /**
   * GdvTic:pos-x:
   *
   * Determines the horizontal position of the center of the tic.
   *
   * Since: 0.1
   */
  tic_properties[PROP_GDV_TIC_POS_X] =
    g_param_spec_float ("pos-x",
                        "the horizontal position of the tic",
                        "represents the horizontal position of the tic within "
                        "the allocated space",
                        -G_MAXFLOAT,
                        G_MAXFLOAT,
                        0.0,
                        G_PARAM_READWRITE);

  /**
   * GdvTic:pos-y:
   *
   * Determines the vertical position of the center of the tic.
   *
   * Since: 0.1
   */
  tic_properties[PROP_GDV_TIC_POS_Y] =
    g_param_spec_float ("pos-y",
                        "the vertical position of the tic",
                        "represents the vertical position of the tic within "
                        "the allocated space",
                        -G_MAXFLOAT,
                        G_MAXFLOAT,
                        0.0,
                        G_PARAM_READWRITE);

  /**
   * GdvTic:axis-inner-dir-x:
   *
   * Determines the x-component of the local vector that points inside the layer.
   *
   * Since: 0.1
   */
  tic_properties[PROP_GDV_TIC_AXIS_INNER_DIR_X] =
    g_param_spec_double ("axis-inner-dir-x",
                         "the x-component of the vector pointing inside the layer",
                         "represents the horizontal component of the vector that "
                         "points in the center of the layer-area",
                         -G_MAXDOUBLE,
                         G_MAXDOUBLE,
                         -1.0,
                         G_PARAM_READWRITE);

  /**
   * GdvTic:axis-inner-dir-y:
   *
   * Determines the y-component of the local vector that points inside the layer.
   *
   * Since: 0.1
   */
  tic_properties[PROP_GDV_TIC_AXIS_INNER_DIR_Y] =
    g_param_spec_double ("axis-inner-dir-y",
                         "the y-component of the vector pointing inside the layer",
                         "represents the vertical component of the vector that "
                         "points in the center of the layer-area",
                         -G_MAXDOUBLE,
                         G_MAXDOUBLE,
                         0.0,
                         G_PARAM_READWRITE);


  /**
   * GdvTic:label-xalign:
   *
   * Determines the alignment of the label-widget in respect to the tic-line.
   *
   * Since: 0.1
   */
  tic_properties[PROP_GDV_TIC_LABEL_XALIGN] =
    g_param_spec_double ("label-xalign",
                         P_("Label xalign"),
                         P_("The horizontal alignment of the label"),
                         -G_MAXDOUBLE, G_MAXDOUBLE,
                         0.0,
                         G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);

  /**
   * GdvTic:label-yalign:
   *
   * Determines the alignment of the label-widget in respect to the tic-line.
   *
   * Since: 0.1
   */
  tic_properties[PROP_GDV_TIC_LABEL_YALIGN] =
    g_param_spec_double ("label-yalign",
                         P_("Label yalign"),
                         P_("The vertical alignment of the label"),
                         -G_MAXDOUBLE, G_MAXDOUBLE,
                         -0.5,
                         G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);


  /**
   * GdvTic:label:
   *
   * Since: 0.1
   */
  tic_properties[PROP_GDV_TIC_LABEL] =
    g_param_spec_string ("label",
                         "holds the gtk-label associated to the tic",
                         "this contains the GtkLabel, that is associated to the "
                         "specific tic, or NULL, if the tic has no label",
                         NULL,
                         G_PARAM_READWRITE);

  /**
   * GdvTic:label-widget:
   *
   * Since: 0.1
   */
  tic_properties[PROP_GDV_TIC_LABEL_WIDGET] =
    g_param_spec_object ("label-widget",
                         "holds the gtk-label associated to the tic",
                         "this contains the GtkLabel, that is associated to the "
                         "specific tic, or NULL, if the tic has no label",
                         GTK_TYPE_WIDGET,
                         G_PARAM_READWRITE);

  g_object_class_install_properties (object_class,
                                     N_PROPERTIES,
                                     tic_properties);

  /* Style-Properties */

  /**
   * GdvTic:line-wdith:
   *
   * Sets line-width for tic-line.
   *
   * Since: 0.1
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
   * GdvTic:color:
   *
   * Sets line-color for tics.
   *
   * Since: 0.1
   */
  gtk_widget_class_install_style_property (widget_class,
      g_param_spec_boxed ("color",
                          _("line-color"),
                          _("line-color to draw"),
                          GDK_TYPE_RGBA,
                          G_PARAM_READWRITE));

  /**
   * GdvTic:tics-in-length:
   *
   * Sets the inner-length of tics.
   *
   * Since: 0.1
   */
  gtk_widget_class_install_style_property (widget_class,
      g_param_spec_double ("tics-in-length",
                           "inner length for tics",
                           "line-length to draw itcs"
                           " inside the layer",
                           -G_MAXDOUBLE,
                           G_MAXDOUBLE,
                           6.0,
                           G_PARAM_READWRITE));

  /**
   * GdvTic:tics-out-length:
   *
   * Sets the outer-length of tics.
   *
   * Since: 0.1
   */
  gtk_widget_class_install_style_property (widget_class,
      g_param_spec_double ("tics-out-length",
                           "outer length for tics",
                           "line-length to draw itcs"
                           " outside the layer",
                           -G_MAXDOUBLE,
                           G_MAXDOUBLE,
                           6.0,
                           G_PARAM_READWRITE));

  /**
   * GdvTic:tics-label:
   *
   * Sets a label at the tic.
   *
   * Since: 0.1
   */
  gtk_widget_class_install_style_property (widget_class,
      g_param_spec_boolean ("show-label",
                            "enables the tic label",
                            "determines whethter the tic-label should be drawn",
                            TRUE,
                            G_PARAM_READWRITE));

  /**
   * GdvTic:label-distance:
   *
   * Sets the outer distance from the tic-line to the label.
   *
   * Since: 0.1
   */
  gtk_widget_class_install_style_property (widget_class,
      g_param_spec_double ("label-distance",
                           "outer distance from tic to label",
                           "line-length to draw itcs"
                           " outside the layer",
                           -G_MAXDOUBLE,
                           G_MAXDOUBLE,
                           6.0,
                           G_PARAM_READWRITE));

  gtk_widget_class_set_css_name (widget_class, "tic");
}

/* FIXME: Make this a class-method! */
/**
 * gdv_tic_get_space_to_tic_position:
 * @tic: a #GdvTic
 * @direction: a #GtkPositionType which indicates the direction to measure the tic
 * @for_size: a specific size-value that can be used as a base-value for measurement
 * @minimum: the place to store the minimum-space
 * @natural: the place to store the natural-space
 * @data: (nullable): any additional data for the measurement
 *
 * This function is used to measure the necessary space for a tic-instance.
 *
 **/
void
gdv_tic_get_space_to_tic_position (
  GdvTic              *tic,
  GtkPositionType      direction,
  int                  for_size,
  int                 *minimum,
  int                 *natural,
  gpointer             data)
{
  GdvTicPrivate *priv;
  gdouble   returned_space = 0.0;
  gdouble   tic_line_width;
  gdouble   tics_inner_length, tics_outer_length;
  gdouble   label_distance;
  gboolean  tic_label;
  gint label_height = 0, label_width = 0;
  gdouble   normalized_x_inner_dir, normalized_y_inner_dir, inner_dir_length;

  g_return_if_fail (GDV_IS_TIC (tic));

  priv = tic->priv;

  gtk_widget_style_get (GTK_WIDGET (tic),
                        "tics-in-length", &tics_inner_length,
                        "tics-out-length", &tics_outer_length,
                        "label-distance", &label_distance,
                        "show-label", &tic_label,
                        "line-width", &tic_line_width,
                        NULL);

  /* normalizing inner dir-vector */
  inner_dir_length =
    sqrt (tic->priv->axis_inner_dir_x * tic->priv->axis_inner_dir_x +
          tic->priv->axis_inner_dir_y * tic->priv->axis_inner_dir_y);

  if (inner_dir_length)
  {
    normalized_x_inner_dir = tic->priv->axis_inner_dir_x / inner_dir_length;
    normalized_y_inner_dir = tic->priv->axis_inner_dir_y / inner_dir_length;
  }
  else
  {
    normalized_x_inner_dir = 0.0;
    normalized_y_inner_dir = 0.0;
  }

  tics_inner_length += 0.05;
  tics_outer_length += 0.05;

  /* FIXME: make this a own function! */
  tic_label = tic_label && gdv_tic_label_widget_is_visible (tic);

  if (tic_label)
  {
    gint child_nat = 0; /* unused */

//      if (direction == GTK_POS_LEFT || direction == GTK_POS_RIGHT)
    {
      if (for_size > 0)
        gtk_widget_get_preferred_width_for_height (
          priv->label, for_size, &label_width, &child_nat);
      else
        gtk_widget_get_preferred_width (priv->label, &label_width, &child_nat);
    }
//      else
    {
      if (for_size > 0)
        gtk_widget_get_preferred_height_for_width (priv->label,
            for_size, &label_height, &child_nat);
      else
        gtk_widget_get_preferred_height (priv->label, &label_height, &child_nat);
    }
  }

  /* Calculating spaces */

  /* FIXME: These implementations suggest, that the Labels are Top-Left-aligned
   *        to the tics-base... This is really the worst of all possibilities
   */
  if (direction == GTK_POS_LEFT)
  {
    returned_space +=
      GSL_MAX (
        -1 * normalized_x_inner_dir * tics_inner_length,
        normalized_x_inner_dir *
        (tics_outer_length +
         (tic_label ? label_distance : 0.0)) -
        (normalized_x_inner_dir >= 0.0 ?
         (tic->priv->label_xalign - normalized_x_inner_dir) * (gdouble) (label_width) :
         tic->priv->label_xalign * (gdouble) (label_width))
      );

    returned_space +=
      fabs(normalized_y_inner_dir) * 0.5 * tic_line_width;
  }
  else if (direction == GTK_POS_RIGHT)
  {
    returned_space +=
      GSL_MAX (
        normalized_x_inner_dir * tics_inner_length,
        -1.0 * normalized_x_inner_dir *
        (tics_outer_length +
         (tic_label ? label_distance : 0.0)) -
        (normalized_x_inner_dir < 0.0 ?
         (tic->priv->label_xalign + normalized_x_inner_dir) * (gdouble) (label_width) :
         tic->priv->label_xalign * (gdouble) (label_width))
      );

    returned_space +=
      fabs(normalized_y_inner_dir) * 0.5 * tic_line_width;
  }
  else if (direction == GTK_POS_TOP)
  {
    returned_space +=
      GSL_MAX (
        -1 * normalized_y_inner_dir * tics_inner_length,
        normalized_y_inner_dir *
        (tics_outer_length +
         (tic_label ? label_distance : 0.0)) -
        (normalized_y_inner_dir >= 0.0 ?
         (tic->priv->label_yalign - normalized_y_inner_dir) * (gdouble) (label_height) :
         tic->priv->label_yalign * (gdouble) (label_height))
      );

    returned_space +=
      fabs(normalized_x_inner_dir) * 0.5 * tic_line_width;
  }
  else if (direction == GTK_POS_BOTTOM)
  {
    returned_space +=
      GSL_MAX (
        normalized_y_inner_dir * (tics_inner_length),
        -1 * normalized_y_inner_dir *
        (tics_outer_length +
         (tic_label ? label_distance : 0.0)) -
        (normalized_y_inner_dir <= 0.0 ?
         (tic->priv->label_yalign + normalized_y_inner_dir) * (gdouble) (label_height) :
         tic->priv->label_yalign * (gdouble) (label_height))
      );

    returned_space +=
      fabs(normalized_x_inner_dir) * 0.5 * tic_line_width;
  }

  *minimum = (gint) returned_space;
  *natural = (gint) returned_space;
}

static gboolean
gdv_tic_label_widget_is_visible (GdvTic *tic)
{
  gboolean style_show;

  gtk_widget_style_get (GTK_WIDGET (tic),
                        "show-label", &style_show,
                        NULL);

  return tic->priv->label &&
         gtk_widget_get_visible (tic->priv->label) &&
         style_show;
}

static void
gdv_tic_size_allocate (GtkWidget     *widget,
                       GtkAllocation *allocation)
{
  GdvTic *tic = GDV_TIC (widget);
  GdvTicPrivate *priv = tic->priv;

  gdouble   tic_line_width;
  gdouble   tics_inner_length, tics_outer_length, label_distance;
  gboolean  tic_label;
  gdouble   normalized_x_inner_dir, normalized_y_inner_dir, inner_dir_length;

  gtk_widget_style_get (GTK_WIDGET (tic),
                        "tics-in-length", &tics_inner_length,
                        "tics-out-length", &tics_outer_length,
                        "show-label", &tic_label,
                        "label-distance", &label_distance,
                        "line-width", &tic_line_width,
                        NULL);

  /* normalizing inner dir-vector */
  inner_dir_length =
    sqrt (tic->priv->axis_inner_dir_x * tic->priv->axis_inner_dir_x +
          tic->priv->axis_inner_dir_y * tic->priv->axis_inner_dir_y);

  if (inner_dir_length)
  {
    normalized_x_inner_dir = tic->priv->axis_inner_dir_x / inner_dir_length;
    normalized_y_inner_dir = tic->priv->axis_inner_dir_y / inner_dir_length;
  }
  else
  {
    normalized_x_inner_dir = 0.0;
    normalized_y_inner_dir = 0.0;
  }

  gtk_widget_set_allocation (widget, allocation);

  if (gdv_tic_label_widget_is_visible (tic))
  {
    gint nat_width, width, height;

    gtk_widget_get_preferred_width (priv->label, NULL, &nat_width);
    width = nat_width;
    gtk_widget_get_preferred_height_for_width (priv->label, width, &height, NULL);

    priv->label_allocation.x =
      (gint) (((gdouble) priv->curr_pos_in_x) +
              -1.0 * normalized_x_inner_dir * tics_outer_length +
              fabs(-0.5 * normalized_y_inner_dir * tic_line_width) +
              -1.0 * normalized_x_inner_dir * label_distance +
              (gdouble) (normalized_x_inner_dir > 0.0 ?
                         (priv->label_xalign - normalized_x_inner_dir) * ((gdouble) width) :
                         priv->label_xalign * ((gdouble) width)));
    priv->label_allocation.y =
      (gint) (((gdouble) priv->curr_pos_in_y) +
              -1.0 * normalized_y_inner_dir * tics_outer_length +
              fabs(-0.5 * normalized_x_inner_dir * tic_line_width) +
              -1.0 * normalized_y_inner_dir * label_distance +
              (gdouble) (normalized_y_inner_dir > 0.0 ?
                         (priv->label_yalign - normalized_y_inner_dir) * ((gfloat) height) :
                         priv->label_yalign * ((gfloat) height)));

    priv->label_allocation.width = width;
    priv->label_allocation.height = height;

    gtk_widget_size_allocate (priv->label, &priv->label_allocation);
  }
}

static void gdv_tic_remove  (GtkContainer   *container,
                             GtkWidget      *child)
{
  GdvTic *tic = GDV_TIC (container);
  GdvTicPrivate *priv = tic->priv;

  if (priv->label == child)
    gdv_tic_set_label_widget (tic, NULL);
  else
    GTK_CONTAINER_CLASS (gdv_tic_parent_class)->remove (container, child);
}

static void gdv_tic_forall (GtkContainer  *container,
                            gboolean       include_internals,
                            GtkCallback    callback,
                            gpointer       callback_data)
{
  GtkBin *bin = GTK_BIN (container);
  GdvTic *tic = GDV_TIC (container);
  GdvTicPrivate *priv = tic->priv;
  GtkWidget *child;

  child = gtk_bin_get_child (bin);

  if (child)
    (* callback) (child, callback_data);

  if (priv->label)
    (* callback) (priv->label, callback_data);
}

static gboolean
gdv_tic_draw (GtkWidget    *widget,
              cairo_t      *cr)
{
  GdvTic *tic = GDV_TIC (widget);
  GtkAllocation allocation;
  gdouble   tics_inner_length, tics_outer_length;
  gboolean  tic_label;

  gdouble   normalized_x_inner_dir, normalized_y_inner_dir, inner_dir_length;
  GtkStyleContext *context;

  inner_dir_length =
    sqrt (tic->priv->axis_inner_dir_x * tic->priv->axis_inner_dir_x +
          tic->priv->axis_inner_dir_y * tic->priv->axis_inner_dir_y);

  if (inner_dir_length)
  {
    normalized_x_inner_dir = tic->priv->axis_inner_dir_x / inner_dir_length;
    normalized_y_inner_dir = tic->priv->axis_inner_dir_y / inner_dir_length;
  }
  else
  {
    normalized_x_inner_dir = 0.0;
    normalized_y_inner_dir = 0.0;
  }

  gtk_widget_style_get (widget,
                        "tics-in-length", &tics_inner_length,
                        "tics-out-length", &tics_outer_length,
                        "show-label", &tic_label,
                        NULL);

  context = gtk_widget_get_style_context (widget);
  gtk_widget_get_allocation (widget, &allocation);

  gdv_render_line (
    context, cr,
    tic->priv->curr_pos_in_x + normalized_x_inner_dir * tics_inner_length -
    (gdouble) allocation.x,
    tic->priv->curr_pos_in_y + normalized_y_inner_dir * tics_inner_length -
    (gdouble) allocation.y,
    tic->priv->curr_pos_in_x - normalized_x_inner_dir * tics_outer_length -
    (gdouble) allocation.x,
    tic->priv->curr_pos_in_y - normalized_y_inner_dir * tics_outer_length -
    (gdouble) allocation.y);

  GTK_WIDGET_CLASS (gdv_tic_parent_class)->draw (widget, cr);

  return FALSE;
}

static void
gdv_tic_measure (
  GdvTic              *tic,
  GtkOrientation       orientation,
  int                  for_size,
  int                 *minimum,
  int                 *natural,
  int                 *minimum_baseline,
  int                 *natural_baseline,
  gpointer             data)
{
  GtkWidget *widget;
  GtkAllocation allocation;

  gdouble   tic_line_width;

  widget = GTK_WIDGET (tic);

  gtk_widget_get_allocation (widget, &allocation);

  gtk_widget_style_get (GTK_WIDGET (tic),
                        "line-width", &tic_line_width,
                        NULL);

  /* adding inner-tic-line, outer-tic-line and line-width */
  if (orientation == GTK_ORIENTATION_HORIZONTAL)
  {
    gint minimum_left = 0, minimum_right = 0,
         natural_left = 0, natural_right = 0;

    gdv_tic_get_space_to_tic_position (
      tic, GTK_POS_LEFT, for_size, &minimum_left, &natural_left, NULL);
    gdv_tic_get_space_to_tic_position (
      tic, GTK_POS_RIGHT, for_size, &minimum_right, &natural_right, NULL);

    *minimum +=
      minimum_left +
      minimum_right;
    *natural +=
      natural_left +
      natural_right;
  }
  else
  {
    gint minimum_top = 0, minimum_bottom = 0,
         natural_top = 0, natural_bottom = 0;

    gdv_tic_get_space_to_tic_position (
      tic, GTK_POS_TOP, for_size, &minimum_top, &natural_top, NULL);
    gdv_tic_get_space_to_tic_position (
      tic, GTK_POS_BOTTOM, for_size, &minimum_bottom, &natural_bottom, NULL);

    *minimum +=
      minimum_top +
      minimum_bottom;
    *natural +=
      natural_top +
      natural_bottom;
  }

  *minimum += (gint) tic_line_width;
  *natural = *minimum;
}

static void
gdv_tic_get_preferred_width (GtkWidget           *widget,
                             gint                *minimum_size,
                             gint                *natural_size)
{
  gdv_tic_measure (GDV_TIC (widget),
                   GTK_ORIENTATION_HORIZONTAL,
                   -1,
                   minimum_size, natural_size,
                   NULL, NULL, NULL);
}

static void
gdv_tic_get_preferred_height (GtkWidget           *widget,
                              gint                *minimum_size,
                              gint                *natural_size)
{
  gdv_tic_measure (GDV_TIC (widget),
                   GTK_ORIENTATION_VERTICAL,
                   -1,
                   minimum_size, natural_size,
                   NULL, NULL, NULL);
}

static void
gdv_tic_get_preferred_height_for_width (GtkWidget           *widget,
                                        gint                 width,
                                        gint                *minimum_height,
                                        gint                *natural_height)
{
  gdv_tic_measure (GDV_TIC (widget),
                   GTK_ORIENTATION_VERTICAL,
                   width,
                   minimum_height, natural_height,
                   NULL, NULL, NULL);
}

static void
gdv_tic_get_preferred_width_for_height (GtkWidget           *widget,
                                        gint                 height,
                                        gint                *minimum_width,
                                        gint                *natural_width)
{
  gdv_tic_measure (GDV_TIC (widget),
                   GTK_ORIENTATION_HORIZONTAL,
                   height,
                   minimum_width, natural_width,
                   NULL, NULL, NULL);
}


