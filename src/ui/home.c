#include <libadwaita-1/adwaita.h>
//Local includes
#include <utils.h>
#include <ui/home.h>
#include <ui/main_window.h>
#include <db/music.h>
#include <sound.h>

static GObject* parent;


void home_init(GObject* window){
	parent = window;
	GtkBuilder* home_builder = load_ui("/ui/home");
	GObject* home_view = get_object(home_builder, "home_view");
	GObject* home_list = get_object(home_builder, "home_list");
	set_main_view(home_view);

	music_t list[512];
	int count = db_music_get_all(list, 512);

	for(int i = 0; i < count; i++){
		sound_set((const char*)list[i].path, NULL);

		const char* title = sound_get_title();
		const char* artist = sound_get_artist();
		const double duration = sound_get_duration();

		GtkBuilder* music_item = load_ui("/ui/music_item");
		GObject* row = get_object(music_item, "music_item");
		GObject* lbl_dur = get_object(music_item, "duration");

		if(strlen(title) > 0){
			adw_preferences_row_set_title(ADW_PREFERENCES_ROW(row), title);
		}else{
			adw_preferences_row_set_title(ADW_PREFERENCES_ROW(row), (const char*)list[i].path);
		}

		if(strlen(artist) > 0){
			adw_action_row_set_subtitle(ADW_ACTION_ROW(row), artist);
		}else{
			adw_action_row_set_subtitle(ADW_ACTION_ROW(row), "Unknown artist");
		}

		char dur[6];
		parse_time(dur, duration);
		gtk_label_set_label(GTK_LABEL(lbl_dur), dur);

		GtkWidget* btn = gtk_button_new();
		gtk_button_set_child(GTK_BUTTON(btn), GTK_WIDGET(row));
		g_signal_connect_data(GTK_BUTTON(btn), "clicked", G_CALLBACK(playsong), GINT_TO_POINTER(i), NULL, 0);

		gtk_list_box_append(GTK_LIST_BOX(home_list), GTK_WIDGET(btn));
		add_to_music_list(list[i], btn);
	}
}
