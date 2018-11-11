/*
 * gdvlayer.c
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

/*
 * TODO:
 *  - correct the widget, to simply use GgtkContainerAdd for adding any new LayerContent, instead
 *    of the current gtk_overlay_add_overlay
 */

#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include <cairo-gobject.h>

#include "gdvlayer.h"
#include "gdvlayercontent.h"
#include "gdvaxis.h"

/**
 * SECTION:gdvlayer
 * @title: GdvLayer
 * @short_description: a layer base class
 *
 * #GdvLayer is a object-class, that contains
 * 2-dimensional plots of numerical data.
 *
 * # CSS nodes
 *
 * GdvLayer uses a single CSS node with name layer.
 */

/* Define Signals */
enum
{
  LAST_SIGNAL
};

/* Define Properties */
enum
{
  PROP_0,

  N_PROPERTIES
};

struct _GdvLayerPrivate
{
  /* elements */
  gboolean show_title;
  GtkWidget *title;
  gint title_position;

  /* axes */
  GList *layer_axes;
/*  GHashTable *update_axes_table; */

  /* data */
  GList *layer_data;

  /* TODO: this is just a feature, that may be necessary for marker-lines, etc. */
  /* marker */
  GList *layer_markers;

  /* Click-events and interaction */
  GdkWindow *event_window;
};

/* --- function declarations --- */
static void
gdv_layer_dispose (GObject *object);
static void
gdv_layer_finalize (GObject *object);
static void gdv_layer_add  (GtkContainer   *container,
                            GtkWidget      *child);
static GtkWidgetPath *
gdv_layer_get_path_for_child (GtkContainer *container,
                              GtkWidget    *child);

static gboolean gdv_layer_draw (GtkWidget    *widget,
                                cairo_t      *cr);

static void gdv_layer_get_preferred_width           (GtkWidget           *widget,
    gint                *minimum_size,
    gint                *natural_size);
static void gdv_layer_get_preferred_height          (GtkWidget           *widget,
    gint                *minimum_size,
    gint                *natural_size);
static void gdv_layer_get_preferred_height_for_width(GtkWidget           *widget,
    gint                 width,
    gint                *minimum_height,
    gint                *natural_height);
static void gdv_layer_get_preferred_width_for_height(GtkWidget           *widget,
    gint                 height,
    gint                *minimum_width,
    gint                *natural_width);

static gboolean
gdv_layer_real_has_layer_content (GdvLayer *layer,
                                  GdvLayerContent *layer_content);

static gboolean
gdv_layer_evaluate_data_point_unimplemented (GdvLayer *layer,
    gdouble             x_value,
    gdouble             y_value,
    gdouble             z_value,
    gdouble            *pos_x,
    gdouble            *pos_y);

static gboolean
gdv_layer_eval_inner_point_unimplemented (GdvLayer      *layer,
    guint point_1_x, guint point_1_y,
    guint point_2_x, guint point_2_y);

G_DEFINE_TYPE_WITH_CODE (GdvLayer,
                         gdv_layer,
                         GTK_TYPE_OVERLAY,
                         G_ADD_PRIVATE (GdvLayer));

static void
gdv_layer_class_init (GdvLayerClass *klass)
{
  GObjectClass *gobject_class;
  GtkWidgetClass *widget_class;
  GtkContainerClass *container_class;

  gobject_class = G_OBJECT_CLASS (klass);
  widget_class = GTK_WIDGET_CLASS (klass);
  container_class = GTK_CONTAINER_CLASS (klass);

  gobject_class->dispose = gdv_layer_dispose;
  gobject_class->finalize = gdv_layer_finalize;

  widget_class->draw = gdv_layer_draw;

  /* TODO: this should search for the maximum values of every child! */
  widget_class->get_preferred_width =
    gdv_layer_get_preferred_width;
  widget_class->get_preferred_height =
    gdv_layer_get_preferred_height;
  widget_class->get_preferred_height_for_width =
    gdv_layer_get_preferred_height_for_width;
  widget_class->get_preferred_width_for_height =
    gdv_layer_get_preferred_width_for_height;

  container_class->add = gdv_layer_add;
  container_class->get_path_for_child = gdv_layer_get_path_for_child;

  klass->evaluate_point = gdv_layer_evaluate_data_point_unimplemented;
  klass->eval_inner_point = gdv_layer_eval_inner_point_unimplemented;

  /* Properties */

  /* Style-Properties */

  gtk_widget_class_set_css_name (widget_class, "layer");
}

static void
gdv_layer_init (GdvLayer *layer)
{
  GtkWidget *widget = GTK_WIDGET (layer);

  GtkCssProvider *css_provider;
  GtkStyleContext *style_context;

  layer->priv = gdv_layer_get_instance_private (layer);

  gtk_widget_set_can_focus (GTK_WIDGET (layer), TRUE);
  gtk_widget_set_receives_default (GTK_WIDGET (layer), TRUE);
  gtk_widget_set_has_window (GTK_WIDGET (layer), FALSE);

  layer->priv->show_title = FALSE;
  layer->priv->title_position = GTK_POS_TOP;
  layer->priv->title = NULL;

  layer->priv->event_window = NULL;

  layer->priv->layer_data = NULL;
  layer->priv->layer_axes = NULL;

/*  layer->priv->update_axes_table =
    g_hash_table_new_full (g_direct_hash, g_direct_equal, NULL, g_free);
*/
  css_provider = gtk_css_provider_new ();
  style_context = gtk_widget_get_style_context (widget);
  gtk_css_provider_load_from_resource (
    css_provider,
    "/net/gdv/libgdv/themes/default_general.css");

  gtk_style_context_add_provider (
    style_context,
    GTK_STYLE_PROVIDER (css_provider),
    GTK_STYLE_PROVIDER_PRIORITY_FALLBACK);
}

static void gdv_layer_add (GtkContainer   *container,
                           GtkWidget      *child)
{
  if (GDV_IS_AXIS (child) || GDV_LAYER_IS_CONTENT (child))
  {
    gtk_overlay_add_overlay (GTK_OVERLAY (container), child);

    /* FIXME: shure this is a good idea? */
    if (gtk_widget_get_visible (GTK_WIDGET (container)))
      gtk_widget_show (GTK_WIDGET (child));
/*
    if (GDV_LAYER_IS_CONTENT (child))
    {
      GdvLayerContent *content;
      GdvLayer *layer;
      gulong *update_axis_id;

      content = GDV_LAYER_CONTENT (child);
      layer = GDV_LAYER (container);

      update_axis_id = g_new (gulong, 1);
      *update_axis_id =
        g_signal_connect (indicator, "notify::data-point",
                          G_CALLBACK (update_axes), layer);
      g_hash_table_insert (axis->priv->update_axes_table,
                           GDV_INDICATOR (widget),
                           update_axis_id);

    }*/
  }
  else
    GTK_CONTAINER_CLASS (gdv_layer_parent_class)->add (container, child);
}

static GtkWidgetPath *
gdv_layer_get_path_for_child (GtkContainer *container,
                              GtkWidget    *child)
{
  GtkWidgetPath *path, *sibling_path;
  GList *list, *data_list, *axes_list;

  GtkWidget *widget = GTK_WIDGET (container);
  GdvLayer *layer = GDV_LAYER (container);

  path = NULL;

  /* FIXME: if we need these functions more frequent, I should use private header */
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

    /* first use the layer-content */
    data_list = gdv_layer_get_content_list (layer);

    for (list = data_list; list; list = list->next)
    {
      if (!gtk_widget_get_visible (list->data))
        continue;

      if (list->data == child)
        found = TRUE;

      if (!found)
        offset++;

      gtk_widget_path_append_for_widget (sibling_path, list->data);
    }

    pos_link = g_list_find (data_list, child);

    if (pos_link)
    {
      position = offset;
    }

    /* then use the layer-axes */
    axes_list = gdv_layer_get_axis_list(layer);

    for (list = axes_list; list; list = list->next)
    {
      if (!gtk_widget_get_visible (list->data))
        continue;

      if (list->data == child)
        found = TRUE;

      if (!found)
        offset++;

      gtk_widget_path_append_for_widget (sibling_path, list->data);
    }

    pos_link = g_list_find (axes_list, child);

    if (pos_link)
    {
      position = offset;
    }

    g_list_free (axes_list);
    g_list_free (data_list);

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

/* FIXME: lower reference of children */
static void
gdv_layer_dispose (GObject *object)
{

  G_OBJECT_CLASS (gdv_layer_parent_class)->dispose (object);
}

static void
gdv_layer_finalize (GObject *object)
{
  G_OBJECT_CLASS (gdv_layer_parent_class)->finalize (object);
}

/**
 * gdv_layer_new:
 *
 * Just a common constructor.
 *
 * Returns: a new #GdvLayer
 **/
GdvLayer *gdv_layer_new (void)
{
  return g_object_new (gdv_layer_get_type (),
                       NULL);
}

static gboolean
gdv_layer_draw (GtkWidget *widget,
                cairo_t   *cr)
{
  GtkAllocation allocation;
  GtkStyleContext *style_context;

  gtk_widget_get_allocation (widget, &allocation);
  style_context = gtk_widget_get_style_context (widget);

  gtk_render_background (style_context, cr, 0, 0,
                         allocation.width, allocation.height);

  return GTK_WIDGET_CLASS (gdv_layer_parent_class)->draw (widget, cr);
}

static void
gdv_layer_measure (
  GdvLayer            *layer,
  GtkOrientation       orientation,
  int                  for_size,
  int                 *minimum,
  int                 *natural,
  int                 *minimum_baseline,
  int                 *natural_baseline,
  gpointer             data)
{
  GList *child_list, *child_list_copy;
  int global_content_minimum = 0,
      global_content_natural = 0;
  guint border_width;

  *minimum = 0;
  *natural = 0;

  border_width = gtk_container_get_border_width (GTK_CONTAINER (layer));

  child_list = gtk_container_get_children(GTK_CONTAINER (layer));
  child_list_copy = child_list;

  while (child_list_copy)
  {
    int data_min = 0,
        data_nat = 0;

    if (child_list_copy->data &&
        gtk_widget_get_visible (child_list_copy->data))
    {
      if (orientation == GTK_ORIENTATION_HORIZONTAL)
      {
        if (for_size > 0)
        {
          gtk_widget_get_preferred_width_for_height (
            child_list_copy->data,
            for_size,
            &data_min,
            &data_nat);
        }
        else
        {
          gtk_widget_get_preferred_width (
            child_list_copy->data,
            &data_min,
            &data_nat);
        }
      }
      else
      {
        if (for_size > 0)
        {
          gtk_widget_get_preferred_height_for_width (
            child_list_copy->data,
            for_size,
            &data_min,
            &data_nat);
        }
        else
        {
          gtk_widget_get_preferred_height (
            child_list_copy->data,
            &data_min,
            &data_nat);
        } /* orientation == GTK_ORIENTATION_VERTICAL */
      }
    }

    global_content_minimum += data_min;
    global_content_natural += data_nat;

    child_list_copy = child_list_copy->next;
  }

  g_list_free (child_list);

  *minimum += global_content_minimum;
  *natural += global_content_natural;

  *minimum += 2 * (int) border_width;
  *natural += 2 * (int) border_width;
}

static void
gdv_layer_get_preferred_width (
  GtkWidget           *widget,
  gint                *minimum,
  gint                *natural)
{
  gdv_layer_measure (GDV_LAYER (widget),
                     GTK_ORIENTATION_HORIZONTAL,
                     -1,
                     minimum, natural,
                     NULL, NULL, NULL);
}

static void gdv_layer_get_preferred_width_for_height(GtkWidget           *widget,
    gint                 height,
    gint                *minimum,
    gint                *natural)
{
  gdv_layer_measure (GDV_LAYER (widget),
                     GTK_ORIENTATION_HORIZONTAL,
                     height,
                     minimum, natural,
                     NULL, NULL, NULL);
}

static void gdv_layer_get_preferred_height          (GtkWidget           *widget,
    gint                *minimum,
    gint                *natural)
{
  gdv_layer_measure (GDV_LAYER (widget),
                     GTK_ORIENTATION_VERTICAL,
                     -1,
                     minimum, natural,
                     NULL, NULL, NULL);
}

static void gdv_layer_get_preferred_height_for_width(GtkWidget           *widget,
    gint                 width,
    gint                *minimum,
    gint                *natural)
{
  gdv_layer_measure (GDV_LAYER (widget),
                     GTK_ORIENTATION_VERTICAL,
                     width,
                     minimum, natural,
                     NULL, NULL, NULL);
}

static gboolean
gdv_layer_real_has_layer_content (GdvLayer *layer,
                                  GdvLayerContent *layer_content)
{
  if (g_list_find (layer->priv->layer_data, layer_content))
    return TRUE;
  else
    return FALSE;
}

/**
 * gdv_layer_has_layer_content:
 * @layer: a #GdvLayer
 * @layer_content: a #GdvLayerContent
 *
 * This is a convenient function to determine if a #GdvLayerContent-instance
 * is affiliated to a layer.
 **/
gboolean
gdv_layer_has_layer_content (GdvLayer *layer, GdvLayerContent *layer_content)
{
  g_return_val_if_fail (GDV_IS_LAYER (layer), FALSE);
  g_return_val_if_fail (GDV_LAYER_IS_CONTENT (layer_content), FALSE);

  return gdv_layer_real_has_layer_content(layer, layer_content);
}

/**
 * gdv_layer_evaluate_data_point:
 * @layer: a #GdvLayer
 * @data_point_x_value: the x coordinate of the point
 * @data_point_y_value: the y coordinate of the point
 * @data_point_z_value: the z coordinate of the point
 * @pos_x: the place to store the x-position on screen
 * @pos_y: the place to store the y-position on screen
 *
 * This function calculates the position of a data-point within the allocated
 * space of a layer-instance in pixel units. It simply applies the
 * #GdvLayer::evaluate_point method.
 *
 * Returns: %TRUE if the point is within the range of the layer-axes and %FALSE
 *      otherwise
 **/
gboolean gdv_layer_evaluate_data_point(GdvLayer *layer,
                                       gdouble data_point_x_value,
                                       gdouble data_point_y_value,
                                       gdouble data_point_z_value,
                                       gdouble *pos_x,
                                       gdouble *pos_y)
{
  if (GDV_LAYER_GET_CLASS (layer)->evaluate_point != NULL)
    return GDV_LAYER_GET_CLASS (layer)->evaluate_point (
             layer,
             data_point_x_value,
             data_point_y_value,
             data_point_z_value,
             pos_x,
             pos_y);

  else
    return FALSE;
}

static gboolean
gdv_layer_evaluate_data_point_unimplemented (GdvLayer *layer,
    gdouble             x_value,
    gdouble             y_value,
    gdouble             z_value,
    gdouble            *pos_x,
    gdouble            *pos_y)
{
  g_warning ("GdvLayerClass::evaluate_point must be set for %s",
             g_type_name (G_OBJECT_TYPE(layer)));
  return FALSE;
}

static gboolean
gdv_layer_eval_inner_point_unimplemented (GdvLayer      *layer,
    guint point_1_x, guint point_1_y,
    guint point_2_x, guint point_2_y)
{
  g_warning ("GdvLayerClass::eval_inner_point not implemented for '%s'",
             g_type_name (G_TYPE_FROM_INSTANCE (layer)));
  return TRUE;
}

/* TODO: I dont like this solution; elements should be filtered, not reallocated */
/**
 * gdv_layer_get_content_list:
 * @layer: a #GdvLayer
 *
 * Lists #GdvLayerContent instances that are used by the @layer.
 *
 * Returns: (element-type Gdv.LayerContent) (transfer container):
 *     a newly allocated #GList of contents
 */
GList *gdv_layer_get_content_list (GdvLayer *layer)
{
  GList *children, *list_copy;
  GList *return_list = NULL;

  g_return_val_if_fail (GDV_IS_LAYER (layer), NULL);

  children = gtk_container_get_children (GTK_CONTAINER (layer));
  list_copy = children;

  while (list_copy)
  {
    if (GDV_LAYER_IS_CONTENT (list_copy->data))
      return_list = g_list_append (return_list, list_copy->data);

    list_copy = list_copy->next;
  }

  g_list_free (children);

  return return_list;
}

/* TODO: see gdv_layer_get_content_list */
/**
 * gdv_layer_get_axis_list:
 * @layer: a #GdvLayer
 *
 * Lists #GdvAxis instances that are used by the @layer.
 *
 * Returns: (transfer container) (element-type GdvAxis):
 *     a newly allocated #GList of axes
 */
GList *gdv_layer_get_axis_list (GdvLayer *layer)
{
  GList *children, *list_copy;
  GList *return_list = NULL;

  g_return_val_if_fail (GDV_IS_LAYER (layer), NULL);

  children = gtk_container_get_children (GTK_CONTAINER (layer));
  list_copy = children;

  while (list_copy)
  {
    if (GDV_IS_AXIS (list_copy->data))
      return_list = g_list_append (return_list, list_copy->data);

    list_copy = list_copy->next;
  }

  g_list_free (children);

  return return_list;
}
