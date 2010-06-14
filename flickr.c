#include <stdio.h>
#include <stdlib.h>
#include <rest-extras/flickr-proxy.h>
#include <rest/rest-proxy-call.h>
#include <json-glib/json-glib.h>
#include <gtk/gtk.h>
#include <gio/gio.h>

GtkWidget *search_button;
GtkWidget *search_entry;
RestProxy *proxy;
RestProxyCall *call;
GtkListStore *store;
GError *err;

GdkPixbuf *
gdk_pixbuf_new_for_uri (const gchar *uri)
{
	GFile *file = g_file_new_for_uri (uri);
	GFileInputStream *filestream = g_file_read (file, 0, &err);

	if (err != NULL) {
		g_debug ("Error reading file. URI was %s.\n%s\n", uri, err->message);
		return NULL;
	}

	GdkPixbuf *pix = gdk_pixbuf_new_from_stream_at_scale (G_INPUT_STREAM (filestream), 
							      200, 
							      200,
							      TRUE,
							      0,
							      &err);

	if (err != NULL) {
		g_debug ("Error creating pixbug : %s\n", err->message);
		return NULL;
	} else {
		return pix;
	}
}

void
on_destroy (GtkObject *object,
	    gpointer data)
{
	printf ("Destroying app...\n");
	gtk_main_quit ();
}

const gchar *
search_tags (const gchar *tags)
{
	// taglist = parse_entry (tags);
	g_debug ("Setting up call...");
	rest_proxy_call_set_function (call, "flickr.photos.search");
	g_debug ("Setting params...");
	rest_proxy_call_add_params (call,
				    "tags", tags,
				    "per_page", "10",
				    "format", "json",
				    "nojsoncallback", "1",
				    "extras", "description,licence,owner_name",
				    NULL);

	printf ("Launching request...\n");
	if (!rest_proxy_call_sync (call, NULL)) {
		g_error ("Couldn't make call.\n");
	}
	return (rest_proxy_call_get_payload (call));
}

void
populate_icon_view (JsonArray *arr,
		    guint index,
		    JsonNode *element,
		    gpointer user_data)
{
	GtkTreeIter iter;
	JsonObject *photo = json_node_get_object (element);

	int farmint = json_object_get_int_member (photo, "farm");
	const gchar *farm = g_strdup_printf ("%i", farmint);
	const gchar *secret = json_object_get_string_member (photo, "secret");
	const gchar *server = json_object_get_string_member (photo, "server");
	const gchar *id = json_object_get_string_member (photo, "id");
       
	printf ("farm : %s\nsecret : %s\nserver : %s\nid : %s\n", farm, secret, server, id);
	// static parts
	const static gchar *http = "http://farm";
	const static gchar *stat = ".static.flickr.com/";
	       
	gchar *uri = g_strconcat (http,
				  farm,
				  stat,
				  server,
				  "/",
				  id,
				  "_",
				  secret,
				  ".jpg",
				  NULL);
	
	GdkPixbuf *pix = gdk_pixbuf_new_for_uri (uri);
	g_free (uri);

	const gchar *title = json_object_get_string_member (photo, "title");

	gtk_list_store_append (store, &iter);
	gtk_list_store_set (store, &iter,
			    0, id,
			    1, title,
			    2, pix,
			    -1);
			    
}

void
parse_result (const gchar *jsondata)
{
	JsonParser *parser = json_parser_new ();
	printf ("Answer : %s", jsondata);

	if (!json_parser_load_from_data (parser, jsondata, -1, &err)) {
		printf ("Error parsing response: %s\n", err->message);
	}

	// let's parse.

	JsonNode *root = json_parser_get_root (parser);

	JsonObject *obj = json_node_get_object (root);
	JsonObject *photos = json_object_get_object_member (obj, "photos");
	JsonArray *photo_array = json_object_get_array_member (photos, "photo");

	json_array_foreach_element (photo_array, populate_icon_view, NULL);
	
	// cleaning up
/*	json_node_free (root);
	json_object_unref (obj);
	json_object_unref (photos);
	json_array_unref (photo_array);
	g_object_unref (parser);*/
	
}

void
on_search_button (GtkButton *button,
		  gpointer user_data)
{
	printf ("Button clicked!\n");
	const gchar *tag = gtk_entry_get_text (GTK_ENTRY (search_entry));
	printf ("Looking up %s...\n", tag);
	const gchar *answer = search_tags (tag);
	parse_result (answer);
}

void
on_search_icon (GtkEntry            *entry,
		GtkEntryIconPosition icon_pos,
		GdkEvent            *event,
		gpointer             user_data)
{
	printf ("Got click ?\n");
}

int
main (int argc, char **argv)
{
	gtk_init (&argc, &argv);
	g_thread_init (NULL);
	g_type_init ();

	err = NULL;

	gchar *api_key = "17c40bceda03e0d6f947b001e7c62058";
	gchar *secret = "a7c16179a409418b";

	proxy = flickr_proxy_new (api_key, secret);
	call = rest_proxy_new_call (proxy);

	GtkBuilder *builder = gtk_builder_new ();
	gtk_builder_add_from_file (builder, "flickr.ui", &err);

	GtkWidget *dial = GTK_WIDGET (gtk_builder_get_object (builder, "dialog1"));
	GtkWidget *iconview = GTK_WIDGET (gtk_builder_get_object (builder, "iconview1"));


	gtk_icon_view_set_pixbuf_column (GTK_ICON_VIEW (iconview), 2);
	gtk_icon_view_set_text_column (GTK_ICON_VIEW (iconview), 1);
	
	search_button = GTK_WIDGET (gtk_builder_get_object (builder, "searchbutton"));
	search_entry = GTK_WIDGET (gtk_builder_get_object (builder, "searchbox"));

	gtk_widget_grab_default (search_button);
	gtk_entry_set_activates_default (GTK_ENTRY (search_entry), TRUE);

	gtk_builder_connect_signals (builder, NULL);

	if (err != NULL) {
		printf ("Error while parsing UI : %s\n", err->message);
		return 1;
	}

	store = GTK_LIST_STORE (gtk_builder_get_object (builder, "liststore1"));

	g_object_unref (builder);
	gtk_dialog_run (GTK_DIALOG (dial));
	gtk_main ();
	return 0;

}
