#include <math.h>
#include <gtk/gtk.h>
#include <gdv/gdv.h>

static void destroy( GtkWidget *widget,
                     gpointer   data )
{
    gtk_main_quit ();
}

int
main (int argc, char **argv)
{
  GtkWidget *main_window;
  GtkWidget *frame;
  gint i;

  GdvLayerContent *layer_content_sin, *layer_content_cos;
  GdvTwodLayer *layer;

  /* initializing the basic gtk+-window */
  gtk_init (&argc, &argv);

  main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_container_set_border_width (GTK_CONTAINER (main_window), 5);
  gtk_window_set_title (GTK_WINDOW (main_window), "example-1");
  frame = gtk_frame_new (NULL);

  gtk_container_add (GTK_CONTAINER (main_window), GTK_WIDGET (frame));
  g_signal_connect (main_window, "destroy", G_CALLBACK (destroy), NULL);

  /* initializing a twod-layer */
  layer = gdv_twod_layer_new ();
  layer_content_sin =
    g_object_new (gdv_layer_content_get_type (), "name", "sinus", NULL);
  layer_content_cos =
    g_object_new (gdv_layer_content_get_type (), "name", "cosinus", NULL);

  /* adding a gdv-layer to the frame & the layer_content to the layer */
  gtk_container_add (GTK_CONTAINER (frame), GTK_WIDGET (layer));
  gtk_container_add (GTK_CONTAINER (layer), GTK_WIDGET (layer_content_sin));
  gtk_container_add (GTK_CONTAINER (layer), GTK_WIDGET (layer_content_cos));

  /* filling data into the layer_content */
  for (i = 0; i < 1000; i++)
  {
    gdv_layer_content_add_data_point (
      layer_content_sin,
      (gdouble) i  / 10.0,
      50.0 + 50.0 * sin (M_PI * ((gdouble) i) / 500.0),
      0.0);
    gdv_layer_content_add_data_point (
      layer_content_cos,
      (gdouble) i  / 10.0,
      50.0 + 50.0 * cos (M_PI * ((gdouble) i) / 500.0),
      0.0);
  }

  gtk_widget_set_size_request (GTK_WIDGET (main_window), 280, 280);

  /* obligatory gtk+-functions */
  gtk_widget_show_all (GTK_WIDGET (main_window));
  gtk_main ();

  return 0;
}

