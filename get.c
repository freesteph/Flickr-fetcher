/* Testing if GIO hanldes HTTP uri as well */
#include <gio/gio.h>
#include <gtk/gtk.h>

int
main (int argc, char **argv)
{
	gtk_init (&argc, &argv);

	GError *err = NULL;
	GFileInputStream *stream;
	GFile *im;
	GdkPixbuf *pix;
	GtkWidget *image;
	GtkWidget *win;

	im = g_file_new_for_uri ("http://farm1.static.flickr.com/91/244965419_5e81e2b32d.jpg");
	
	stream = g_file_read (im, 0, &err);
	if (err != NULL) {
		printf ("Error loading file!\n");
		return -1;
	}
	
	pix = gdk_pixbuf_new_from_stream (G_INPUT_STREAM (stream), 0, &err);

	if (err != NULL) {
		printf ("Error loading pixbuf!\n");
		return -1;
	}

	image = gtk_image_new_from_pixbuf (pix);
	
	win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_container_add (GTK_CONTAINER (win), image);

	gtk_widget_show_all (win);
	gtk_main ();
	return 0;
}
