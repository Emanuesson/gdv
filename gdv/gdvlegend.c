/*
 * gdvlegend.c
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

/**
 * SECTION:gdvlegend
 * @title: GdvLegend
 * @short_description: a Gtk+-Widget that displays a legend
 *
 * #GdvLegend is an object-class, that displays an overview over the
 * layer-content- and indicator-elements that belong to a single layer.
 *
 * # CSS nodes
 *
 * GdvLegend uses a single CSS node with name legend.
 *
 */

#include "gdvlegend.h"
#include "gdvlayer.h"
#include "gdvlegendelement.h"

/* Define Properties */
enum
{
  PROP_0,

  PROP_LAYER,
  PROP_ORIENTATION,
  PROP_ROWS,
  PROP_COLUMNS,

  N_PROPERTIES
};

static GParamSpec *legend_properties[N_PROPERTIES] = { NULL, };

struct _GdvLegendPrivate
{
  /* data */
  GdvLayer        *attached_layer;
  GtkWidget       *main_box;

  gint samplelen;

  /* base properties */
  GtkPositionType orientation;
  guint rows;
  guint columns;

  /* Click-events and interaction */
  GdkWindow *event_window;

};

static void
gdv_legend_refresh (GdvLegend *legend);

G_DEFINE_TYPE_WITH_PRIVATE (GdvLegend, gdv_legend, GTK_TYPE_FRAME);

static void
gdv_legend_init (GdvLegend *legend)
{
  GtkWidget *widget = GTK_WIDGET (legend);
  GtkWidgetPath *widget_path;
  GtkStyleContext *style_context;
  GtkCssProvider *css_provider;

  legend->priv = gdv_legend_get_instance_private (legend);

  gtk_widget_set_receives_default (GTK_WIDGET (legend), TRUE);
  gtk_widget_set_has_window (GTK_WIDGET (legend), FALSE);

  widget_path = gtk_widget_get_path (widget);
  css_provider = gtk_css_provider_new ();
  style_context = gtk_widget_get_style_context (widget);
  gtk_css_provider_load_from_resource (
    css_provider,
    "/net/gdv/libgdv/themes/default_general.css");

  gtk_style_context_add_provider (
    style_context,
    GTK_STYLE_PROVIDER (css_provider),
    GTK_STYLE_PROVIDER_PRIORITY_FALLBACK);


  /* data */
  legend->priv->attached_layer = NULL;
  legend->priv->main_box =
    g_object_new (GTK_TYPE_GRID,
                  "border-width", 5,
                  "column-spacing", 5,
                  "row-spacing", 5,
                  NULL);
  gtk_container_add (GTK_CONTAINER (legend), GTK_WIDGET (legend->priv->main_box));

  legend->priv->samplelen = 5;

  legend->priv->orientation = GTK_POS_RIGHT;
  legend->priv->rows = G_MAXUINT;
  legend->priv->columns = 1;

  legend->priv->event_window = NULL;
}

static void
gdv_legend_set_property (GObject      *object,
                         guint         property_id,
                         const GValue *value,
                         GParamSpec   *pspec)
{
  GdvLegend *self;
  cairo_pattern_t *source;

  self = GDV_LEGEND (object);

  switch (property_id)
  {
  case PROP_LAYER:
    self->priv->attached_layer = g_value_get_object (value);
    gdv_legend_refresh (self);
    break;

  case PROP_ORIENTATION:
    self->priv->orientation = g_value_get_enum (value);
    gdv_legend_refresh (self);
    break;

  case PROP_ROWS:
    self->priv->rows = g_value_get_uint (value);
    gdv_legend_refresh (self);
    break;

  case PROP_COLUMNS:
    self->priv->columns = g_value_get_uint (value);
    gdv_legend_refresh (self);
    break;

  default:
    /* unknown property */
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }
}


static void
gdv_legend_get_property (GObject    *object,
                         guint       property_id,
                         GValue     *value,
                         GParamSpec *pspec)
{
  GdvLegend *self = GDV_LEGEND (object);

  switch (property_id)
  {
  case PROP_LAYER:
    g_value_set_object (value, self->priv->attached_layer);
    break;

  case PROP_ORIENTATION:
    g_value_set_enum (value, self->priv->orientation);
    break;

  case PROP_ROWS:
    g_value_set_uint (value, self->priv->rows);
    break;

  case PROP_COLUMNS:
    g_value_set_uint (value, self->priv->columns);
    break;

  default:
    /* unknown property */
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }
}

static void
gdv_legend_dispose (GObject *object)
{
  GdvLegend *legend = GDV_LEGEND (object);

  G_OBJECT_CLASS (gdv_legend_parent_class)->dispose (object);
}

static void
gdv_legend_finalize (GObject *object)
{
  GdvLegend *view = GDV_LEGEND (object);

  G_OBJECT_CLASS (gdv_legend_parent_class)->finalize (object);
}

static void
gdv_legend_class_init (GdvLegendClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->dispose = gdv_legend_dispose;
  object_class->finalize = gdv_legend_finalize;

  object_class->set_property = gdv_legend_set_property;
  object_class->get_property = gdv_legend_get_property;

  gtk_widget_class_set_css_name (widget_class, "legend");

  /**
   * GdvLegend:layer:
   *
   * The layer, that the legend is connected to.
   *
   * Since: 0.1
   */
  legend_properties[PROP_LAYER] =
    g_param_spec_object ("layer",
                         "the connected layer",
                         "layer that is presented in the legend",
                         GDV_TYPE_LAYER,
                         G_PARAM_READWRITE);

  /**
   * GdvLegend:orientation:
   *
   * Sets the orientation of the legend.
   *
   * Since: 0.1
   */
  legend_properties[PROP_ORIENTATION] =
    g_param_spec_enum ("text-position",
                       "text-position relativ to the markers",
                       "determines if the data-title is shown on the left, "
                       "right, top or bottom to the data-examples",
                       GTK_TYPE_POSITION_TYPE,
                       GTK_POS_RIGHT,
                       G_PARAM_READWRITE);

  /**
   * GdvLegend:rows:
   *
   * Sets the number of rows, that the legend should consist of.
   *
   * Since: 0.1
   */
  legend_properties[PROP_ROWS] =
    g_param_spec_uint ("rows",
                       "number of rows",
                       "determines the number of rows for the legend",
                       1,
                       G_MAXUINT,
                       G_MAXUINT,
                       G_PARAM_READWRITE);

  /**
   * GtkLegend:columns:
   *
   * Sets the number of columns, that the legend should consist of.
   *
   * Since: 0.1
   */
  legend_properties[PROP_COLUMNS] =
    g_param_spec_uint ("columns",
                       "sets the basic orientation of the legend",
                       "determines the number of rows for the legend",
                       1,
                       G_MAXUINT,
                       1,
                       G_PARAM_READWRITE);

  g_object_class_install_properties (object_class,
                                     N_PROPERTIES,
                                     legend_properties);

}

/**
 * gdv_legend_new:
 *
 * Just a common constructor.
 *
 * Returns: a new #GdvLegend
 **/
GdvLegend *gdv_legend_new (void)
{
  return g_object_new (gdv_legend_get_type (),
                       NULL);
}

static void
gdv_legend_refresh (GdvLegend *legend)
{
  guint current_row, current_col;
  GList *contents =
    gdv_layer_get_content_list (GDV_LAYER (legend->priv->attached_layer));
  GList *contents_copy = contents;
  GList *axes =
    gdv_layer_get_axis_list (GDV_LAYER (legend->priv->attached_layer));
  GList *axes_copy = axes;
  GList *indicator_list = NULL;
  GList *indicators_copy;

  while (axes)
  {
    GList *local_indicators =
      gdv_axis_get_indicator_list (GDV_AXIS (axes->data));
    indicator_list = g_list_concat (indicator_list, local_indicators);

    axes = axes->next;
  }

  g_list_free (axes_copy);
  indicators_copy = indicator_list;

  for (current_row = 0; current_row < legend->priv->rows; current_row++)
  {
    for (current_col = 0; current_col < legend->priv->columns; current_col++)
    {
      GtkWidget *current_element = NULL;
      guint example_current_grid_row, example_current_grid_column;
      guint descriptor_current_grid_row, descriptor_current_grid_column;
      GtkWidget *current_legend_example = NULL;
      GtkWidget *current_legend_descriptor = NULL;

      gchar *current_element_name;

      /* determining existing legend elements */
      if (legend->priv->orientation == GTK_POS_RIGHT)
        example_current_grid_column = current_col * 2,
        example_current_grid_row = current_row,
        descriptor_current_grid_column = current_col * 2 + 1,
        descriptor_current_grid_row = current_row;
      else if (legend->priv->orientation == GTK_POS_LEFT)
        example_current_grid_column = current_col * 2 + 1,
        example_current_grid_row = current_row,
        descriptor_current_grid_column = current_col * 2,
        descriptor_current_grid_row = current_row;
      else if (legend->priv->orientation == GTK_POS_TOP)
        example_current_grid_column = current_col,
        example_current_grid_row = current_row * 2 + 1,
        descriptor_current_grid_column = current_col,
        descriptor_current_grid_row = current_row * 2;
      else if (legend->priv->orientation == GTK_POS_BOTTOM)
        example_current_grid_column = current_col,
        example_current_grid_row = current_row * 2,
        descriptor_current_grid_column = current_col,
        descriptor_current_grid_row = current_row * 2 + 1;

      current_legend_example =
        gtk_grid_get_child_at (GTK_GRID (legend->priv->main_box),
                               example_current_grid_column,
                               example_current_grid_row),
        current_legend_descriptor =
          gtk_grid_get_child_at (GTK_GRID (legend->priv->main_box),
                                 descriptor_current_grid_column,
                                 descriptor_current_grid_row);

      /* receiving the next element */
      /* TODO: receive "show-in-legend"-property and toggle over elements */
      if (contents)
      {
        current_element = contents->data;
        contents = contents->next;
      }
      else if (indicator_list)
      {
        current_element = indicator_list->data;
        indicator_list = indicator_list->next;
      }
      else
      {
        if (current_legend_example)
          gtk_container_remove (
            GTK_CONTAINER (legend->priv->main_box),
            GTK_WIDGET (current_legend_example));

        if (current_legend_descriptor)
          gtk_container_remove (
            GTK_CONTAINER (legend->priv->main_box),
            GTK_WIDGET (current_legend_descriptor));

        return;
      }

      /* correction/initialisation of the example-elements */
      if (current_legend_example && GDV_IS_LEGEND_ELEMENT (current_legend_example))
      {
        GtkWidget *connected_element;
        g_object_get (current_legend_example, "element", &connected_element, NULL);

        if (connected_element != current_element)
          g_object_set (current_legend_example, "element", current_element, NULL);

//        g_print ("overwrite existing element\n");
      }
      else
      {
        if (current_legend_example)
          gtk_container_remove (
            GTK_CONTAINER (legend->priv->main_box),
            GTK_WIDGET (current_legend_example));

        current_legend_example =
          g_object_new (GDV_TYPE_LEGEND_ELEMENT, "element", current_element, NULL);
        gtk_grid_attach (GTK_GRID (legend->priv->main_box),
                         current_legend_example,
                         example_current_grid_column,
                         example_current_grid_row,
                         1, 1);
      }

      /* correction/initialisation of the legend-descriptors */
      g_object_get (current_element, "title", &current_element_name, NULL);

      if (current_legend_descriptor && GTK_IS_LABEL (current_legend_descriptor))
      {
        const gchar *current_label_text =
          gtk_label_get_text (GTK_LABEL (current_legend_descriptor));

        if (g_strcmp0 (current_label_text, (const gchar *) current_element_name) != 0)
          gtk_label_set_text (GTK_LABEL (current_legend_descriptor),
                              (const gchar *) current_element_name);
      }
      else
      {
        if (current_legend_descriptor)
          gtk_container_remove (
            GTK_CONTAINER (legend->priv->main_box),
            GTK_WIDGET (current_legend_descriptor));

        current_legend_descriptor = gtk_label_new (current_element_name);
        gtk_grid_attach (GTK_GRID (legend->priv->main_box),
                         current_legend_descriptor,
                         descriptor_current_grid_column,
                         descriptor_current_grid_row,
                         1, 1);
      }

    }
  }

  g_list_free (contents_copy);
  g_list_free (indicators_copy);
}
