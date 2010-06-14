using Rest;
using Json;
using Gtk;

public class FlickrFetcher {

	/* Flickr stuff */
	private const string api_key = "17c40bceda03e0d6f947b001e7c62058";
	private const string secret = "a7c16179a409418b";

	private Rest.FlickrProxy proxy;
	private Rest.FlickrProxyCall call;
	
	/* Json parser */
	private Json.Parser parser;

	/* UI elements */
	private Gtk.Dialog dialog;
	private Gtk.Entry search_entry;
	private Gtk.Button search_button;
	private Gtk.IconView iconview;

	private Gtk.ListStore list;
	
	public void on_search_button () {

		debug ("Address in signal : %p\nthis : %p", search_entry, this);
		print ("We have the tags : %s\n", this.search_entry.get_text ());

	}

	public void on_dialog_response (Gtk.Dialog dialog, int response) {

		stdout.printf ("In!\n");
	}
		
	public FlickrFetcher() {
		
		var builder = new Gtk.Builder ();

		try {
			builder.add_from_file ("flickr.ui");
		} catch (Error e) {
			stdout.printf ("Error parsing UI : %s\n", e.message);
			builder = null;
		}

		assert (builder != null);

		dialog = builder.get_object ("dialog1") as Gtk.Dialog;
		search_button = builder.get_object ("searchbutton") as Gtk.Button;
		search_entry = builder.get_object ("searchentry") as Gtk.Entry;

		if (search_entry == null) {
			debug ("Wtf ?");
		} else {
			debug ("Adresse : %p", search_entry);
		}

		search_button.grab_default ();
		search_entry.set_activates_default (true);

		builder.connect_signals (null);
	}

	public void run () {
		dialog.run ();
	}
}
		
	
public static int main (string []args)
{
	Gtk.init (ref args);

	var dial = new FlickrFetcher ();
	dial.run ();

	Gtk.main ();
	return 0;
}