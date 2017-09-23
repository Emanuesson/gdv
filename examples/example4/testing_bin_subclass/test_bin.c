/*
 * test_bin.c
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
 * TODO
 * - implement disposer and finalizer correct concerning all private elements
 * - implement a childwidget or main_grid or whatever makes most sense and take
 *   control over reference and life-cycle to this widget
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cairo-gobject.h>

#include "test_bin.h"

/* Define Properties */
enum
{
  PROP_0,

  N_PROPERTIES
};

struct _TestbinPrivate
{
  cairo_t         *drawing_context;
  cairo_surface_t *drawing_surface;

  GtkGrid         *main_grid;
  GtkWidget       *child;

  GtkAllocation child_allocation;

  gboolean    multiplot;
  guint       vertical_spacing;
  guint       horizontal_spacing;

  GSList          *label;
  GSList          *arrows;
  GSList          *layer;
};

/* Method declaration */
static void testbin_dispose (GObject *object);
static void testbin_finalize (GObject *object);
static void testbin_set_property (GObject      *object,
                                      guint         property_id,
                                      const GValue *value,
                                      GParamSpec   *pspec);
static void testbin_get_property (GObject    *object,
                                      guint       property_id,
                                      GValue     *value,
                                      GParamSpec *pspec);

static gboolean
testbin_draw (GtkWidget *widget,
                  cairo_t   *cr);
static gboolean
configure_event_cb (GtkWidget         *widget,
                    GdkEventConfigure *event);

static void testbin_realize       (GtkWidget           *widget);
static void testbin_size_allocate (GtkWidget           *widget,
                                       GtkAllocation       *allocation);
static void testbin_add  (GtkContainer *container, GtkWidget *widget, gpointer *user_data);

static void testbin_send_configure (Testbin     *darea);
static void testbin_remove        (GtkContainer   *container,
                                       GtkWidget      *child);
static void testbin_forall        (GtkContainer   *container,
                                       gboolean	     include_internals,
                                       GtkCallback     callback,
                                       gpointer        callback_data);
static GtkWidgetPath * testbin_get_path_for_child (GtkContainer *container,
    GtkWidget    *child);

static void testbin_compute_child_allocation      (Testbin *view,
    GtkAllocation *child_allocation);
static void testbin_real_compute_child_allocation (Testbin *view,
    GtkAllocation *child_allocation);

/* GtkBuildable */
static void testbin_buildable_init                (GtkBuildableIface *iface);
static void testbin_buildable_add_child           (GtkBuildable *buildable,
    GtkBuilder   *builder,
    GObject      *child,
    const gchar  *type);

static void testbin_get_preferred_width           (GtkWidget           *widget,
    gint                *minimum_size,
    gint                *natural_size);
static void testbin_get_preferred_height          (GtkWidget           *widget,
    gint                *minimum_size,
    gint                *natural_size);
static void testbin_get_preferred_height_for_width(GtkWidget           *layout,
    gint                 width,
    gint                *minimum_height,
    gint                *natural_height);
static void testbin_get_preferred_width_for_height(GtkWidget           *layout,
    gint                 width,
    gint                *minimum_height,
    gint                *natural_height);



G_DEFINE_TYPE_WITH_CODE (Testbin, testbin, GTK_TYPE_BIN,
                         G_ADD_PRIVATE (Testbin)
                         G_IMPLEMENT_INTERFACE (GTK_TYPE_BUILDABLE,
                             testbin_buildable_init));

static void
testbin_class_init (TestbinClass *klass)
{
  GObjectClass *gobject_class;
  GtkWidgetClass *widget_class;
  GtkContainerClass *container_class;

  gobject_class = G_OBJECT_CLASS (klass);
  widget_class = GTK_WIDGET_CLASS (klass);
  container_class = GTK_CONTAINER_CLASS (klass);

  gobject_class->dispose = testbin_dispose;
  gobject_class->finalize = testbin_finalize;
  gobject_class->set_property = testbin_set_property;
  gobject_class->get_property = testbin_get_property;

  widget_class->configure_event = configure_event_cb;
  widget_class->draw = testbin_draw;
  widget_class->realize = testbin_realize;
  widget_class->size_allocate = testbin_size_allocate;
  widget_class->get_preferred_width            = testbin_get_preferred_width;
  widget_class->get_preferred_height           = testbin_get_preferred_height;
  widget_class->get_preferred_height_for_width = testbin_get_preferred_height_for_width;
  widget_class->get_preferred_width_for_height = testbin_get_preferred_width_for_height;

//  container_class->remove = testbin_remove;
//  container_class->forall = testbin_forall;

  klass->compute_child_allocation = testbin_real_compute_child_allocation;

  /* Properties */
//  view_properties[PROP_MULTIPLOT] =
//    g_param_spec_boolean ("multiplot",
//                          "multiplotting more than one layer",
//                          "activates or deactivates multiplotting",
//                          FALSE,
//                          G_PARAM_READWRITE);

//  g_object_class_install_properties (gobject_class,
//                                     N_PROPERTIES,
//                                     view_properties);

//  gtk_widget_class_install_style_property (widget_class,
//    g_param_spec_double ("custom-property",
//                         "background cairo source",
//                         "sets the view background source for plotting",
//                         -1000.0,
//                         10000.0,
//                         0.0,
//                         G_PARAM_READWRITE));

}

static void
testbin_buildable_init (GtkBuildableIface *iface)
{
//  iface->add_child = testbin_buildable_add_child;
}

static void
testbin_init (Testbin *view)
{
  GtkWidget *widget;

  widget = GTK_WIDGET (view);

  view->priv = testbin_get_instance_private (view);

  view->priv->layer = NULL;
  view->priv->multiplot = FALSE;
  view->priv->vertical_spacing = 5;
  view->priv->horizontal_spacing = 5;

  g_object_set (G_OBJECT(widget), "can-focus", TRUE, NULL);

//  gtk_container_set_reallocate_redraws (view, TRUE);

  /* Signals used to handle the backing surface */
//  g_signal_connect (GTK_WIDGET (view),"configure-event",
//                    G_CALLBACK (configure_event_cb), NULL);

  /* Ask to receive events the drawing area doesn't normally
   * subscribe to. In particular, we need to ask for the
   * button press and motion notify events that want to handle.
   */
  gtk_widget_set_events (GTK_WIDGET (view), gtk_widget_get_events (GTK_WIDGET (view)));
}

static void testbin_set_property (GObject      *object,
                                      guint         property_id,
                                      const GValue *value,
                                      GParamSpec   *pspec)
{
  Testbin *self = TESTBIN (object);

  switch (property_id)
    {
//    case PROP_MULTIPLOT:
//      self->priv->multiplot = g_value_get_boolean (value);
//      break;

    default:
      /* unknown property */
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

static void testbin_get_property (GObject    *object,
                                      guint       property_id,
                                      GValue     *value,
                                      GParamSpec *pspec)
{
  Testbin *self = TESTBIN (object);

  switch (property_id)
    {
//    case PROP_MULTIPLOT:
//      g_value_set_boolean (value, self->priv->multiplot);
//      break;

    default:
      /* unknown property */
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;

    }
}

static void
testbin_realize (GtkWidget *widget)
{
  GtkAllocation allocation;
  GdkWindow *window;
  GdkWindowAttr attributes;
  gint attributes_mask;

  g_print ("realized view\n");

  if (!gtk_widget_get_has_window (widget))
    {
      GTK_WIDGET_CLASS (testbin_parent_class)->realize (widget);
    }

  else
    {
      gtk_widget_set_realized (widget, TRUE);

      gtk_widget_get_allocation (widget, &allocation);

      attributes.window_type = GDK_WINDOW_CHILD;
      attributes.x = allocation.x;
      attributes.y = allocation.y;
      attributes.width = allocation.width;
      attributes.height = allocation.height;

      attributes.wclass = GDK_INPUT_OUTPUT;
      attributes.visual = gtk_widget_get_visual (widget);
      attributes.event_mask = gtk_widget_get_events (widget) | GDK_EXPOSURE_MASK;

      attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL;

      window = gdk_window_new (gtk_widget_get_parent_window (widget),
                               &attributes, attributes_mask);
      gtk_widget_register_window (widget, window);
      gtk_widget_set_window (widget, window);

//      gtk_style_context_set_background (gtk_widget_get_style_context (widget),
//                                        window);
    }

  testbin_send_configure (TESTBIN (widget));
}

static void
testbin_remove (GtkContainer *container,
                    GtkWidget    *child)
{
  Testbin *view = TESTBIN (container);

  GTK_CONTAINER_CLASS (testbin_parent_class)->remove (container, child);
}

static void
testbin_forall (GtkContainer *container,
                    gboolean      include_internals,
                    GtkCallback   callback,
                    gpointer      callback_data)
{
  GtkBin *bin = GTK_BIN (container);
  Testbin *view = TESTBIN (container);
  GtkWidget *child;

  child = gtk_bin_get_child (bin);

  if (child)
    (* callback) (child, callback_data);

}

static GtkWidgetPath *
testbin_get_path_for_child (GtkContainer *container,
                                GtkWidget    *child)
{
  Testbin *view = TESTBIN (container);
  GtkWidgetPath *path;

  path = GTK_CONTAINER_CLASS (testbin_parent_class)->get_path_for_child (container, child);

  return path;
}

/* Redraw the screen from the surface. Note that the ::draw
 * signal receives a ready-to-be-used cairo_t that is already
 * clipped to only draw the exposed areas of the widget
 */
static gboolean
testbin_draw (GtkWidget *widget,
                  cairo_t   *cr)
{
  Testbin *view;
  GtkAllocation allocation;
  gint x, y, width, height;
  GtkStyleContext *context;

  g_print ("drew view\n");
//  gtk_container_propagate_draw (GTK_CONTAINER (widget), gtk_bin_get_child (GTK_BIN (widget)), cr);

  g_return_val_if_fail (GTK_IS_WIDGET (widget), TRUE);

  gtk_widget_get_allocation (widget, &allocation);
  context = gtk_widget_get_style_context (widget);

  view = TESTBIN (widget);
  cairo_set_source_surface (cr, view->priv->drawing_surface, 0, 0);

  x = view->priv->child_allocation.x - allocation.x;
  y = view->priv->child_allocation.y - allocation.y;
  width = view->priv->child_allocation.width > allocation.width ?
          allocation.width : view->priv->child_allocation.width;
  height =  view->priv->child_allocation.height > allocation.height ?
            allocation.height : view->priv->child_allocation.height;

  gtk_render_background (context, cr, x, y, width, height);

//////////////////////////////

  GdkRGBA color;
  color.red = 1.0;
  color.blue = 1.0;
  color.green = 1.0;
  color.alpha = 1.0;

  gdk_cairo_set_source_rgba(cr, &color);
  cairo_arc (cr,
             10, 10,
             20,
             0, 2 * G_PI);

  cairo_stroke (cr);
  cairo_restore (cr);

//////////////////////////////

//  if (GDV_IS_LAYER (gtk_bin_get_child (GTK_BIN (widget))))
//    g_print ("is layer\n");
//  gtk_widget_queue_draw_child (widget);
//  gtk_widget_set_redraw_on_allocate (gtk_bin_get_child (GTK_BIN (widget)), TRUE);
//  gtk_widget_draw (gtk_bin_get_child (GTK_BIN (widget)), cr);
//  gtk_widget_queue_draw (gtk_bin_get_child (GTK_BIN (widget)));

//  if (gtk_container_should_propagate_draw (GTK_CONTAINER (widget), gtk_bin_get_child (widget), cr))
//    g_print ("view will propagate\n");

//////////////////////////////





  if (gtk_widget_has_visible_focus (widget))
    {
      GtkBorder border;
      GtkStateFlags state;
      guint width, height, x, y;

      state = gtk_style_context_get_state (context);

      x = 0;
      y = 0;
      width = allocation.width;
      height = allocation.height;

      gtk_style_context_get_border (context, state, &border);

      x += border.left;
      y += border.top;
      width -= border.left + border.right;
      height -= border.top + border.bottom;

      gtk_render_focus (context, cr, x, y, width, height);
    }

//  gtk_widget_queue_draw ( 
//    gtk_bin_get_child (GTK_BIN (widget)));

  return GTK_WIDGET_CLASS (testbin_parent_class)->draw (widget, cr);
}

static void
testbin_size_allocate (GtkWidget     *widget,
                           GtkAllocation *allocation)
{
  g_return_if_fail (IS_TESTBIN (widget));
  g_return_if_fail (allocation != NULL);

  g_print ("allocated view\n");

  Testbin *view = TESTBIN (widget);
  GtkBin *bin = GTK_BIN (widget);
  GtkAllocation new_allocation;
  GtkWidget *child;

  gtk_widget_set_allocation (widget, allocation);

  testbin_compute_child_allocation (view, &new_allocation);

  /* If the child allocation changed, that means that the frame is drawn
   * in a new place, so we must redraw the entire widget.
   */
  if (gtk_widget_get_realized (widget))
    {
      if (gtk_widget_get_has_window (widget))
        gdk_window_move_resize (gtk_widget_get_window (widget),
                                allocation->x, allocation->y,
                                allocation->width, allocation->height);

//      testbin_send_configure (TESTBIN (widget));
    }

//  if (gtk_widget_get_mapped (widget))
//  {
//    gdk_window_invalidate_rect (gtk_widget_get_window (widget), allocation, FALSE);
//  }

  child = gtk_bin_get_child (bin);

  if (child && gtk_widget_get_visible (child))
    gtk_widget_size_allocate (child, &new_allocation);

  view->priv->child_allocation = new_allocation;

  GTK_WIDGET_CLASS (testbin_parent_class)->size_allocate (widget, allocation);
}

static void
testbin_compute_child_allocation (Testbin      *view,
                                      GtkAllocation *child_allocation)
{
  g_return_if_fail (IS_TESTBIN (view));
  g_return_if_fail (child_allocation != NULL);

  TESTBIN_GET_CLASS (view)->compute_child_allocation (view, child_allocation);
}

static void
testbin_real_compute_child_allocation (Testbin *view,
    GtkAllocation *child_allocation)
{
  GtkWidget *widget = GTK_WIDGET (view);
  GtkAllocation allocation;

  gtk_widget_get_allocation (widget, &allocation);

  child_allocation->x = 0;
  child_allocation->y = 0;
  child_allocation->width = MAX (1, (gint) (allocation.width));
  child_allocation->height = MAX (1, (gint) (allocation.height));

  child_allocation->x += allocation.x;
  child_allocation->y += allocation.y;
}

static void
testbin_get_preferred_size (GtkWidget      *request,
                                GtkOrientation  orientation,
                                gint           *minimum_size,
                                gint           *natural_size)
{
  Testbin *view = TESTBIN (request);
  GtkWidget *widget = GTK_WIDGET (request);
  GtkWidget *child;
  GtkBin *bin = GTK_BIN (widget);
  gint child_min, child_nat;
  gint minimum, natural;

  minimum = 0;
  natural = 0;

  child = gtk_bin_get_child (bin);

  if (child && GTK_IS_WIDGET(child) && gtk_widget_get_visible (child))
    {
      if (orientation == GTK_ORIENTATION_HORIZONTAL)
        {
          gtk_widget_get_preferred_width (child,
                                          &child_min, &child_nat);
          minimum = MAX (minimum, child_min);
          natural = MAX (natural, child_nat);
        }

      else
        {
          gtk_widget_get_preferred_height (child,
                                           &child_min, &child_nat);
          minimum += child_min;
          natural += child_nat;
        }
    }

  if (minimum_size)
    *minimum_size = minimum;

  if (natural_size)
    *natural_size = natural;
}

static void
testbin_get_preferred_width (GtkWidget *widget,
                                 gint      *minimum_size,
                                 gint      *natural_size)
{
  testbin_get_preferred_size (widget, GTK_ORIENTATION_HORIZONTAL, minimum_size, natural_size);
}

static void
testbin_get_preferred_height (GtkWidget *widget,
                                  gint      *minimum_size,
                                  gint      *natural_size)
{
  testbin_get_preferred_size (widget, GTK_ORIENTATION_VERTICAL, minimum_size, natural_size);
}


static void
testbin_get_preferred_height_for_width (GtkWidget *request,
    gint       width,
    gint      *minimum_height,
    gint      *natural_height)
{
  GtkWidget *widget = GTK_WIDGET (request);
  GtkWidget *child;
  Testbin *view = TESTBIN (widget);
  GtkBin *bin = GTK_BIN (widget);
  gint child_min, child_nat;
  gint minimum, natural;

  minimum = 0;
  natural = 0;

  width -= 0;

  child = gtk_bin_get_child (bin);

  if (child && gtk_widget_get_visible (child))
    {
      gtk_widget_get_preferred_height_for_width (child,
          width, &child_min, &child_nat);
      minimum += child_min;
      natural += child_nat;
    }

  if (minimum_height)
    *minimum_height = minimum;

  if (natural_height)
    *natural_height = natural;
}

static void
testbin_get_preferred_width_for_height (GtkWidget *widget,
    gint       height,
    gint      *minimum_width,
    gint      *natural_width)
{
  GTK_WIDGET_GET_CLASS (widget)->get_preferred_width (widget, minimum_width, natural_width);
}

static void
testbin_send_configure (Testbin *darea)
{
  GtkAllocation allocation;
  GtkWidget *widget;
  GdkEvent *event = gdk_event_new (GDK_CONFIGURE);

  widget = GTK_WIDGET (darea);
  gtk_widget_get_allocation (widget, &allocation);

  event->configure.window = g_object_ref (gtk_widget_get_window (widget));
  event->configure.send_event = TRUE;
  event->configure.x = allocation.x;
  event->configure.y = allocation.y;
  event->configure.width = allocation.width;
  event->configure.height = allocation.height;

  gtk_widget_event (widget, event);
  gdk_event_free (event);
}

/**
 * SECTION:gdv2dview
 * @title: Testbin
 * @short_description: 2d-plotting widget
 *
 * #Testbin is a widget-class, that contains one or more
 * 2-dimensional plots of numerical data.
 */
static gboolean
configure_event_cb (GtkWidget         *widget,
                    GdkEventConfigure *event)
{
  Testbin *view = TESTBIN (widget);
//  GSList *static_layers = NULL;

  g_print ("configured view\n");

  if (view->priv->drawing_surface)
    cairo_surface_destroy (view->priv->drawing_surface);

//  if (view->priv->drawing_context)
//    cairo_destroy (view->priv->drawing_context);

  view->priv->drawing_surface = gdk_window_create_similar_surface (gtk_widget_get_window (widget),
                                CAIRO_CONTENT_COLOR,
                                gtk_widget_get_allocated_width (widget),
                                gtk_widget_get_allocated_height (widget));

  return TRUE;
}

static void
testbin_dispose (GObject *object)
{
  Testbin *view = TESTBIN (object);

//  if (view->priv->drawing_surface)
//    cairo_surface_destroy (view->priv->drawing_surface);


//  g_clear_object (&app->priv->settings);

  G_OBJECT_CLASS (testbin_parent_class)->dispose (object);
}

static void
testbin_finalize (GObject *object)
{
  Testbin *view = TESTBIN (object);

  G_OBJECT_CLASS (testbin_parent_class)->finalize (object);
}

Testbin *testbin_new (void)
{
//  cairo_t *cr;

  Testbin *view = g_object_new (testbin_get_type (), NULL);

  return view;
}


//void testbin_add_layer (Testbin *view, GdvLayer *layer) {
//  GtkWidget *widget_view;
//  GtkWidget *widget_layer;

//  g_return_if_fail(GDV_2D_IS_VIEW (view) && GDV_IS_LAYER (layer));

//  widget_view = GTK_WIDGET (view);
//  widget_layer = GTK_WIDGET (layer);

//  view->priv->layer = g_slist_append (view->priv->layer, layer);
//  g_object_ref (G_OBJECT (layer));

//  gtk_widget_queue_draw(widget_view);
//  gtk_widget_queue_draw(widget_layer);

//}

