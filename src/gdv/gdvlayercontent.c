/*
 * gdvlayercontent.c
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

#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include <math.h>

#include "gdvlayercontent.h"
#include "gdvlayer.h"
#include "gdvrender.h"
#include "gdv-data-boxed.h"
#include "gdvaxis-private.h"

/**
 * SECTION:gdvlayercontent
 * @short_description: plotting a set of data
 * @title: GdvLayerContent
 *
 * #GdvLayerContent is a object-class, thats intention is to basically contain
 * all the information, that will be plotted in a single data-series of a plot.
 *
 */

/* Define Properties */
enum
{
  PROP_0,

  PROP_DATA_POINT,
  PROP_CONTENT_MATRIX,

  PROP_POINT_LINE_WIDTH,
  PROP_POINT_TYPE,

  PROP_LINE_STYLE,
  PROP_TITLE,
  PROP_SHOW_IN_LEGEND,

  PROP_FILL_BELOW,
  PROP_FILL_ABOVE,

  N_PROPERTIES
};

static GParamSpec *layer_content_properties[N_PROPERTIES] = { NULL, };

/* TODO: implement instance-member registration */
struct _GdvLayerContentPrivate
{
  GdvLayer *layer;

  GdvDataPoint *last_point;

  gdouble point_width;
  gdouble point_line_width;
  cairo_pattern_t *point_source;
  guint point_style;
  gboolean point_fill;
  cairo_pattern_t *point_fill_source;

  GdvDataPoint *layer_min;
  GdvDataPoint *layer_max;
  GdkWindow *event_win;

  gdouble line_width;
  guint line_style;
  cairo_pattern_t *line_source;

  gchar *title;
  gboolean show_in_legend;

  gboolean fill_below;
  gboolean fill_above;

  GslMatrix * content;
};

static void
gdv_layer_content_set_property (GObject      *object,
                                guint         property_id,
                                const GValue *value,
                                GParamSpec   *pspec);

static void
gdv_layer_content_get_property (GObject    *object,
                                guint       property_id,
                                GValue     *value,
                                GParamSpec *pspec);

static void
gdv_layer_content_size_allocate (GtkWidget           *widget,
                                 GtkAllocation       *allocation);

static gboolean
gdv_layer_content_on_draw (GtkWidget    *widget,
                           cairo_t      *cr);

G_DEFINE_TYPE_WITH_PRIVATE (GdvLayerContent,
                            gdv_layer_content,
                            GTK_TYPE_WIDGET)

/* define the property-setter */
static void
gdv_layer_content_set_property (GObject      *object,
                                guint         property_id,
                                const GValue *value,
                                GParamSpec   *pspec)
{
  GdvLayerContent *self = GDV_LAYER_CONTENT (object);

  switch (property_id)
  {
  case PROP_DATA_POINT:
      {
        gsize rows;
        GdvDataPoint * dp;

        if (self->priv->content == NULL)
          {
            self->priv->content = gsl_matrix_alloc(3, 1);
            g_object_notify (G_OBJECT (self), "content-matrix");
          }

        dp = g_value_get_boxed (value);
        rows = self->priv->content->size2;
        gsl_matrix_set_and_expand(self->priv->content, 0, rows, dp->x);
        gsl_matrix_set_and_expand(self->priv->content, 1, rows, dp->y);
        gsl_matrix_set_and_expand(self->priv->content, 2, rows, dp->z);
      }
    break;

//  case PROP_CONTENT_MATRIX:
//    break;

  case PROP_POINT_LINE_WIDTH:
    self->priv->point_line_width = g_value_get_double (value);
    break;

  case PROP_POINT_TYPE:
    self->priv->point_style = g_value_get_uint (value);
    break;

  case PROP_LINE_STYLE:
    self->priv->line_style = g_value_get_uint (value);
    break;

  case PROP_TITLE:
    if (self->priv->title)
      g_free (self->priv->title);

    self->priv->title = g_strdup (g_value_get_string (value));
    break;

  case PROP_SHOW_IN_LEGEND:
    self->priv->show_in_legend = g_value_get_boolean (value);
    break;

  case PROP_FILL_BELOW:
    self->priv->fill_below = g_value_get_boolean (value);
    break;

  case PROP_FILL_ABOVE:
    self->priv->fill_above = g_value_get_boolean (value);
    break;

  default:
    /* unknown property */
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }
}

/* define property-getter */
static void
gdv_layer_content_get_property (GObject    *object,
                                guint       property_id,
                                GValue     *value,
                                GParamSpec *pspec)
{
  GdvLayerContent *self = GDV_LAYER_CONTENT (object);

  switch (property_id)
  {
  case PROP_DATA_POINT:
      {
        GdvDataPoint dp;
        if (self->priv->content != NULL)
          {
            gsize last_row = self->priv->content->size2 - 1;
            dp.x = gsl_matrix_get(self->priv->content, 0, last_row);
            dp.y = gsl_matrix_get(self->priv->content, 1, last_row);
            dp.z = gsl_matrix_get(self->priv->content, 2, last_row);
          }
        else
          {
            memset(&dp, 0, sizeof(dp));
          }
        g_value_set_boxed (value, &dp);
      }
    break;

//  case PROP_CONTENT_MATRIX:
//    break;

  case PROP_POINT_LINE_WIDTH:
    g_value_set_double (value, self->priv->point_line_width);
    break;

  case PROP_POINT_TYPE:
    g_value_set_uint (value, self->priv->point_style);
    break;

  case PROP_LINE_STYLE:
    g_value_set_uint (value, self->priv->line_style);
    break;

  case PROP_TITLE:
    if (self->priv->title)
      g_value_set_string (value, self->priv->title);
    else
      g_value_set_static_string (value, "");

    break;

  case PROP_SHOW_IN_LEGEND:
    g_value_set_boolean (value, self->priv->show_in_legend);
    break;

  case PROP_FILL_BELOW:
    g_value_set_boolean (value, self->priv->fill_below);
    break;

  case PROP_FILL_ABOVE:
    g_value_set_boolean (value, self->priv->fill_above);
    break;

  default:
    /* unknown property */
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }
}

static void
gdv_layer_content_init (GdvLayerContent *content)
{
  GtkWidget *widget;
  GtkStyleContext *style_context;
  GtkCssProvider *css_provider;

  widget = GTK_WIDGET (content);

  gtk_widget_set_name (widget, "new layer");

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

  gtk_widget_set_can_focus (GTK_WIDGET (content), TRUE);
  gtk_widget_set_receives_default (GTK_WIDGET (content), TRUE);
  gtk_widget_set_has_window (GTK_WIDGET (content), FALSE);

  content->priv = gdv_layer_content_get_instance_private (content);

  content->priv->event_win = NULL;

  content->priv->point_width = 3;
  content->priv->point_line_width = 0;
  content->priv->point_style = 0;
  content->priv->point_source = cairo_pattern_create_rgba (0, 0, 0, 1);
  content->priv->point_fill = TRUE;
  content->priv->point_fill_source = NULL;

  content->priv->title = g_strdup ("new data\0");
  content->priv->show_in_legend = TRUE;

  content->priv->line_source = cairo_pattern_create_rgba (0, 0, 0, 1);
  content->priv->line_style = 1;

  content->priv->layer_min = g_new (GdvDataPoint, 1);
  content->priv->layer_max = g_new (GdvDataPoint, 1);

  content->priv->layer_max->x = NAN;
  content->priv->layer_max->y = NAN;
  content->priv->layer_max->z = NAN;

  content->priv->layer_min->x = NAN;
  content->priv->layer_min->y = NAN;
  content->priv->layer_min->z = NAN;
  content->priv->content = NULL;
}

static gboolean
gdv_layer_content_on_draw (GtkWidget    *widget,
                           cairo_t      *cr)
{
  gboolean first_point;
  gdouble prev_pixel_x = 0.0, prev_pixel_y = 0.0;
  GtkStyleContext *context;
  GtkAllocation allocation;

  GdvLayerContent *content;
  gsize i;

  first_point = TRUE;

  g_return_val_if_fail (GDV_LAYER_IS_CONTENT (widget), FALSE);

  context = gtk_widget_get_style_context (widget);

  gtk_widget_get_allocation (widget, &allocation);

  content = GDV_LAYER_CONTENT (widget);

  {
    GList *orig_axes_list, *local_axes_list;

    orig_axes_list =
      gdv_layer_get_axis_list (GDV_LAYER (gtk_widget_get_parent (widget)));

    for (local_axes_list = orig_axes_list;
         local_axes_list; local_axes_list = local_axes_list->next)
    {
      gboolean resize_axis = _gdv_axis_get_resize_during_redraw(local_axes_list->data);

      if (resize_axis)
      {
        gtk_widget_queue_resize (GTK_WIDGET (widget));
        return FALSE;
      }
    }

    g_list_free (orig_axes_list);
  }

  if (content->priv->content == NULL)
    return TRUE;

  {
    gdouble tmp_x, tmp_y;
    gdouble tmp_x2, tmp_y2;

    gdv_layer_evaluate_data_point (GDV_LAYER (gtk_widget_get_parent (widget)),
                                   0.0,
                                   0.0,
                                   0.0,
                                   &tmp_x,
                                   &tmp_y);
    gdv_layer_evaluate_data_point (GDV_LAYER (gtk_widget_get_parent (widget)),
                                   100.0,
                                   100.0,
                                   100.0,
                                   &tmp_x2,
                                   &tmp_y2);
  }

  for (i = 0;
       i < content->priv->content->size2;
       i++)
  {
    GdvLayer *layer = GDV_LAYER (gtk_widget_get_parent (widget));
    gboolean paint_point;
    gdouble pixel_x, pixel_y;

    paint_point =
      gdv_layer_evaluate_data_point (layer,
                                     gsl_matrix_get(content->priv->content, 0, i),
                                     gsl_matrix_get(content->priv->content, 1, i),
                                     gsl_matrix_get(content->priv->content, 2, i),
                                     &pixel_x,
                                     &pixel_y);

    /* skip data-points outside the range of the layer */
    if (!paint_point)
      continue;

    if (((pixel_x != prev_pixel_x) || (pixel_y != prev_pixel_y)))
    {
      gdv_render_data_point (
        context,
        cr,
        pixel_x - (gdouble) allocation.x,
        pixel_y - (gdouble) allocation.y);

      if (!first_point)
        gdv_render_data_line (
          context,
          cr,
          prev_pixel_x - (gdouble) allocation.x,
          prev_pixel_y - (gdouble) allocation.y,
          pixel_x - (gdouble) allocation.x,
          pixel_y - (gdouble) allocation.y);
    }

    first_point = FALSE;

    prev_pixel_x = pixel_x;
    prev_pixel_y = pixel_y;
  }

  return TRUE;
}

static void
gdv_layer_content_dispose (GObject *object)
{
  G_OBJECT_CLASS (gdv_layer_content_parent_class)->dispose (object);
}

static void
gdv_layer_content_finalize (GObject *object)
{
  G_OBJECT_CLASS (gdv_layer_content_parent_class)->finalize (object);
}

static void
gdv_layer_content_class_init (GdvLayerContentClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  if (sizeof(gdouble) != 8)
    g_warning ("sizeof(gdouble) != 64bit; this may lead to unwarned "
               "complications especially in the usage of time-series");

  object_class->set_property = gdv_layer_content_set_property;
  object_class->get_property = gdv_layer_content_get_property;

  object_class->dispose = gdv_layer_content_dispose;
  object_class->finalize = gdv_layer_content_finalize;

  widget_class->size_allocate = gdv_layer_content_size_allocate;
  widget_class->draw = gdv_layer_content_on_draw;

  gtk_widget_class_set_css_name (widget_class, "layercontent");

  /**
   * GdvLayerContent:data-point:
   *
   * Gives the data-point that was added last to the layer-content or makes it
   * possible to add a new data-point.
   */
  layer_content_properties[PROP_DATA_POINT] =
    g_param_spec_boxed ("data-point",
                        "data-point",
                        "property, to access the containing data points",
                        GDV_TYPE_DATA_POINT,
                        G_PARAM_READWRITE);

  /**
   * GdvLayerContent:content-matrix:
   *
   * Gives the data-point that was added last to the layer-content or makes it
   * possible to add a new data-point.
   */
  layer_content_properties[PROP_CONTENT_MATRIX] =
    g_param_spec_boxed ("content-matrix",
                        "Content Matrix",
                        "property, to access the content matrix directly. The "
                        "returned object may loose it's validity. E.g. if the "
                        "layer coontent gets reset.",
                        GSL_TYPE_MATRIX,
                        G_PARAM_READWRITE);

  /**
   * GdvLayerContent:point-line-width:
   *
   * The line-width to plot a point.
   */
  layer_content_properties[PROP_POINT_LINE_WIDTH] =
    g_param_spec_double ("point-line-width",
                         "line-width of the point-symbol",
                         "line-width of the point-symbol",
                         0,
                         G_MAXDOUBLE,
                         2,
                         G_PARAM_READWRITE);

  /**
   * GdvLayerContent:point-type:
   *
   * The style-type to plot a point.
   */
  layer_content_properties[PROP_POINT_TYPE] =
    g_param_spec_uint ("point-type",
                       "point-symbol type",
                       "symbol-type that is used for the points",
                       0,
                       50,
                       0,
                       G_PARAM_READWRITE);

  /**
   * GdvLayerContent:line-style:
   *
   * The style-type to plot a line.
   */
  layer_content_properties[PROP_LINE_STYLE] =
    g_param_spec_uint ("line-style",
                       "line-style type",
                       "style-type that is used for the plotting",
                       0,
                       50,
                       0,
                       G_PARAM_READWRITE);

  /**
   * GdvLayerContent:title:
   *
   * The title of the layer-content.
   */
  layer_content_properties[PROP_TITLE] =
    g_param_spec_string ("title",
                         "content-title",
                         "the title that will be shown in the legend",
                         "new content",
                         G_PARAM_READWRITE);

  /**
   * GdvLayerContent:show-in-legend:
   *
   * Determines if the layer-content should be listed in the legend.
   */
  layer_content_properties[PROP_SHOW_IN_LEGEND] =
    g_param_spec_boolean ("show-in-legend",
                          "content shown in legend",
                          "determines if the content-title is shown or hidden "
                          "in the legend",
                          TRUE,
                          G_PARAM_READWRITE);

  /**
   * GdvLayerContent:fill-below:
   *
   * Determines if the space below the data should be filled.
   */
  layer_content_properties[PROP_FILL_BELOW] =
    g_param_spec_boolean ("fill-below",
                          "fill the space below the content",
                          "determines if the space below the data should be "
                          "filled",
                          FALSE,
                          G_PARAM_READWRITE);

  /**
   * GdvLayerContent:fill-above:
   *
   * Determines if the space above the data should be filled.
   */
  layer_content_properties[PROP_FILL_ABOVE] =
    g_param_spec_boolean ("fill-above",
                          "fill the space above the content",
                          "determines if the space above the data should be "
                          "filled",
                          FALSE,
                          G_PARAM_READWRITE);

  g_object_class_install_properties (object_class,
                                     N_PROPERTIES,
                                     layer_content_properties);

  /**
   * GdvLayerContent:point-color:
   *
   * Sets color for a data-point.
   */
  gtk_widget_class_install_style_property (widget_class,
      g_param_spec_boxed ("point-color",
                          _("Color of a data-point"),
                          _("Sets the color to draw a data-point"),
                          GDK_TYPE_RGBA,
                          G_PARAM_READWRITE));

  /**
   * GdvLayerContent:point-width:
   *
   * Sets line-width for drawing a data-point.
   */
  gtk_widget_class_install_style_property (widget_class,
      g_param_spec_double ("point-width",
                           _("Width of the data-point"),
                           _("Sets the line-width to draw a data-point"),
                           0.0,
                           G_MAXDOUBLE,
                           1.0,
                           G_PARAM_READWRITE));

  /**
   * GdvLayerContent:line-color:
   *
   * Sets color for drawing the line.
   */
  gtk_widget_class_install_style_property (widget_class,
      g_param_spec_boxed ("line-color",
                          _("Color of a data-line"),
                          _("Sets the color to draw a data-line"),
                          GDK_TYPE_RGBA,
                          G_PARAM_READWRITE));

  /**
   * GdvLayerContent:line-width:
   *
   * Sets line-width for drawing the line.
   */
  gtk_widget_class_install_style_property (widget_class,
      g_param_spec_double ("line-width",
                           _("Width of the data-line"),
                           _("Sets the line-width to draw a data-line"),
                           0.0,
                           G_MAXDOUBLE,
                           1.0,
                           G_PARAM_READWRITE));

  /**
   * GdvLayerContent:line-dash-portion:
   *
   * Sets dash-interval for drawing the line.
   */
  gtk_widget_class_install_style_property (widget_class,
      g_param_spec_double ("line-dash-portion",
                           _("primary line-dashing space"),
                           _("Sets the primary portion of the line that will be "
                             "printed"),
                           0.0,
                           1.0,
                           1.0,
                           G_PARAM_READWRITE));

  /**
   * GdvLayerContent:line-secondary-dash-portion:
   *
   * Sets a second dash-interval for drawing the line.
   */
  gtk_widget_class_install_style_property (widget_class,
      g_param_spec_double ("line-secondary-dash-portion",
                           _("secondary line-dashing space"),
                           _("Sets the secondary portion of the line that will be "
                             "printed"),
                           0.0,
                           1.0,
                           0.0,
                           G_PARAM_READWRITE));

  /**
   * GdvLayerContent:line-dash-length:
   *
   * Sets a dash-length for drawing the line.
   */
  gtk_widget_class_install_style_property (widget_class,
      g_param_spec_int ("line-dash-length",
                        _("line-dashing length of intervall"),
                        _("Sets the length for one dash-interval"),
                        0,
                        G_MAXINT,
                        0,
                        G_PARAM_READWRITE));

}

/**
 * gdv_layer_content_new:
 *
 * Just a common constructor.
 *
 * Returns: a new #GdvLayerContent
 **/
GdvLayerContent *gdv_layer_content_new (void)
{
  return g_object_new (gdv_layer_content_get_type (),
                       NULL);
}

static void
gdv_layer_content_size_allocate (GtkWidget           *widget,
                                 GtkAllocation       *allocation)
{
  g_return_if_fail (allocation != NULL);

  gtk_widget_set_allocation (widget, allocation);
}

/**
 * gdv_layer_content_add_data_point:
 * @layer_content: a #GdvLayerContent
 * @x_value: the x value of the new data-point
 * @y_value: the y value of the new data-point
 * @z_value: the z value of the new data-point
 *
 * Just add a new cartesian point to the #layer_content.
 **/
void
gdv_layer_content_add_data_point (GdvLayerContent *layer_content,
                                  gdouble          x_value,
                                  gdouble          y_value,
                                  gdouble          z_value)
{
  gsize rows;

  g_return_if_fail (GDV_LAYER_IS_CONTENT (layer_content));

  layer_content->priv->layer_max->x =
    fmax (layer_content->priv->layer_max->x, x_value);
  layer_content->priv->layer_max->y =
    fmax (layer_content->priv->layer_max->y, y_value);
  layer_content->priv->layer_max->z =
    fmax (layer_content->priv->layer_max->z, z_value);

  layer_content->priv->layer_min->x =
    fmin (layer_content->priv->layer_min->x, x_value);
  layer_content->priv->layer_min->y =
    fmin (layer_content->priv->layer_min->y, y_value);
  layer_content->priv->layer_min->z =
    fmin (layer_content->priv->layer_min->z, z_value);

  if (layer_content->priv->content == NULL)
    layer_content->priv->content = gsl_matrix_calloc(3, 1);

  rows = layer_content->priv->content->size2;
  gsl_matrix_set_and_expand(layer_content->priv->content, 0, rows, x_value);
  gsl_matrix_set_and_expand(layer_content->priv->content, 1, rows, y_value);
  gsl_matrix_set_and_expand(layer_content->priv->content, 2, rows, z_value);

  g_object_notify (G_OBJECT (layer_content), "data-point");
}

/* FIXME: Update the minimum and maximum values! */
/*
 * gdv_layer_content_remove_data_point_by_index:
 * @layer_content: a #GdvLayerContent
 * @index: The index of the point that should be removed
 *
 * Removes the data-point from #layer_content that is stored under the #index
 * position (counted from the one added last to the one added first).
 *
 * A negative #index removes the point that was added first to the layer.
 *
 * Returns: %TRUE if the data-point could be removed and %FALSE otherwise.
 */
/*gboolean
gdv_layer_content_remove_data_point_by_index (
  GdvLayerContent *layer_content,
  gint             index)
{
  gpointer data_point = NULL;

  g_return_val_if_fail (GDV_LAYER_IS_CONTENT (layer_content), FALSE);

  if (layer_content->priv->data_values && index >= 0)
    data_point = g_list_nth_data (layer_content->priv->data_values, index);

  else if (layer_content->priv->data_values)
  {
    GList *list_last;

    list_last = g_list_last (layer_content->priv->data_values);
    data_point = list_last->data;
  }

  if (data_point)
  {
    layer_content->priv->data_values =
      g_list_remove (layer_content->priv->data_values, data_point);

    g_object_notify (G_OBJECT (layer_content), "data-point");

    return TRUE;
  }

  else
    return FALSE;
}*/

/* FIXME: determine (element-type GdvDataPoint) */
/**
 * gdv_layer_content_get_content:
 * @content: a #GdvLayerContent
 *
 * Returns: (transfer full): The copy of the content.
 *
 **/
GslMatrix *
gdv_layer_content_get_content (GdvLayerContent *content)
{
  g_return_val_if_fail (GDV_LAYER_IS_CONTENT (content), NULL);

  return content->priv->content;
}

/**
 * gdv_layer_content_set_content:
 * @content: a #GdvLayerContent
 * @matrix: a #GslMatrix
 **/
void
gdv_layer_content_set_content (GdvLayerContent *content, GslMatrix *matrix)
{
  g_return_if_fail(GDV_LAYER_IS_CONTENT(content));

  if (content->priv->content != NULL) {
    gsl_matrix_free(content->priv->content);
  }

  content->priv->content = matrix;
}

/**
 * gdv_layer_content_get_min_max_x:
 * @content: a #GdvLayerContent
 * @min_x: The place to store the minimum x-value
 * @max_x: The place to store the maximum x-value
 *
 * Convenient function to get the minimum and maximum value of the layer-content.
 *
 * This function should not be used for now, as it will give wrong results
 * after data-points were removed from the #content.
 *
 **/
void
gdv_layer_content_get_min_max_x (GdvLayerContent *content,
                                 gdouble *min_x,
                                 gdouble *max_x)
{
  g_return_if_fail (GDV_LAYER_IS_CONTENT (content));

  *min_x = content->priv->layer_min->x;
  *max_x = content->priv->layer_max->x;
}

/**
 * gdv_layer_content_get_min_max_y:
 * @content: a #GdvLayerContent
 * @min_y: The place to store the minimum y-value
 * @max_y: The place to store the maximum y-value
 *
 * Convenient function to get the minimum and maximum value of the layer-content.
 *
 * This function should not be used for now, as it will give wrong results
 * after data-points were removed from the #content.
 *
 **/
void
gdv_layer_content_get_min_max_y (GdvLayerContent *content,
                                 gdouble *min_y,
                                 gdouble *max_y)
{
  g_return_if_fail (GDV_LAYER_IS_CONTENT (content));

  *min_y = content->priv->layer_min->y;
  *max_y = content->priv->layer_max->y;
}

/**
 * gdv_layer_content_get_min_max_z:
 * @content: a #GdvLayerContent
 * @min_z: The place to store the minimum z-value
 * @max_z: The place to store the maximum z-value
 *
 * Convenient function to get the minimum and maximum value of the layer-content.
 *
 * This function should not be used for now, as it will give wrong results
 * after data-points were removed from the #content.
 *
 **/
void
gdv_layer_content_get_min_max_z (GdvLayerContent *content,
                                 gdouble *min_z,
                                 gdouble *max_z)
{
  g_return_if_fail (GDV_LAYER_IS_CONTENT (content));

  *min_z = content->priv->layer_min->z;
  *max_z = content->priv->layer_max->z;
}

/**
 * gdv_layer_content_reset:
 * @layer_content: a #GdvLayerContent
 *
 * Resets the #layer_content.
 *
 **/
void
gdv_layer_content_reset (GdvLayerContent *layer_content)
{
  g_return_if_fail (GDV_LAYER_IS_CONTENT (layer_content));

  if (layer_content->priv->content == NULL)
    g_object_unref(layer_content->priv->content);

  layer_content->priv->content = NULL;

  g_object_notify (G_OBJECT (layer_content), "content-matrix");

  layer_content->priv->layer_max->x = -G_MAXDOUBLE;
  layer_content->priv->layer_max->y = -G_MAXDOUBLE;
  layer_content->priv->layer_max->z = -G_MAXDOUBLE;

  layer_content->priv->layer_min->x = G_MAXDOUBLE;
  layer_content->priv->layer_min->y = G_MAXDOUBLE;
  layer_content->priv->layer_min->z = G_MAXDOUBLE;
}

