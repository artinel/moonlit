/**
* \file src/utils.c
* \brief Some helper functions to use in the program
* \author Mohammad shamsi <artinel@proton.me>
* \version 0.0.1
* \date 2025-05-24
* \copyright GNU Public License V3
*/


#include <libadwaita-1/adwaita.h>
#include <stdio.h>
#include <string.h>
//local includes
#include <utils.h>
#include <db/music.h>
#include <db/music_dir.h>
#include <db/like.h>
#include <ui/home.h>
#include <sound.h>
#include <ui/main_window.h>
#include <music_info.h>

#define FILENAME_BUF_SIZE 512	/**< The size of the buffer for the music file name */


static bool check_ext(const char* filename, const char* extension);
static void* thread_dir_add(void* data);
static void show_dir_add_callback(GObject* src, GAsyncResult* res, gpointer data);

/** This struct is used to pass data to the directory add thread */
struct dir_thread{
	char* path;		/**< The director we want to search in */
	char* ext;		/**< The extension we want to search for */
	int dir_id;		/**< Directory ID */
	GObject* dialog;	/**< The select directory dialog's object */
	GObject* parent;	/**< Parent of the dialog */
};


/**
 * Load a ui file into a GtkBuilder using gtk_builder_add_from resource.
 * First it initialize GtkBuilder using gtk_builder_new and then declare
 * a GError* for error checking and set it no NULL then load the ui and
 * check if GError* is NULL or not for error checking and then returns
 * the GtkBuilder
 * \param ui	The ui file we want to load
 * \return	The loaded GtkBuilder
 * */
GtkBuilder* load_ui(const char* ui){
	GtkBuilder* builder = gtk_builder_new();
	GError* err = NULL;
	gtk_builder_add_from_resource(builder, ui, &err);
	if(err != NULL){
		g_print("Error in loading (%s) : %s\n", ui, err->message);
	}
	return builder;
}

/**
 * Get an object from a loaded GtkBuilder with gtk_builder_get_object
 * \param builder	The loaded GtkBuilder
 * \param id		The object ID in the loaded ui file
 * \return		Returns the object with given ID from the given GtkBuilder
 * */
GObject* get_object(GtkBuilder* builder, const char* id){
	return gtk_builder_get_object(builder, id);
}


/**
 * Load custom css into program.
 * \param widget	The main widget(usally window)
 * \param css		The css file name we want to load
 * */
void load_css(GtkWidget *widget, char *css){
	GtkCssProvider *provider = gtk_css_provider_new();
	gtk_css_provider_load_from_resource(provider, css);
	gtk_style_context_add_provider_for_display(gtk_widget_get_display(widget),
			GTK_STYLE_PROVIDER(provider),
			GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
}

void show_alert_dialog(const char* title, const char* body, int mode, GObject* parent){
	AdwDialog* dialog = adw_alert_dialog_new(title, body);
	adw_alert_dialog_add_response(ADW_ALERT_DIALOG(dialog), "close", "Close");
	adw_alert_dialog_set_response_appearance(ADW_ALERT_DIALOG(dialog), "close", 
			(mode == 0) ? ADW_RESPONSE_DESTRUCTIVE : ADW_RESPONSE_SUGGESTED);
	adw_alert_dialog_set_default_response(ADW_ALERT_DIALOG(dialog), "close");
	adw_alert_dialog_set_close_response(ADW_ALERT_DIALOG(dialog), "close");
	adw_dialog_present(ADW_DIALOG(dialog), GTK_WIDGET(parent));
}

static bool check_ext(const char* filename, const char* extension){
	const char* dot = strrchr(filename, '.');
	if(dot != NULL && dot != filename){
		dot++;
		if(strcmp(dot, extension) == 0){
			return true;
		}else{
			return false;
		}
	}
	return false;
}

void dir_files_add_db(const char* path, const char* extension, const int dir_id){
	DIR* dir = opendir(path);
	if(dir == NULL){
		fprintf(stderr, "Failed to open directory : %s\n", path);
		return;	
	}

	struct dirent* de;
	while((de = readdir(dir)) != NULL){
		if(de->d_type == DT_DIR){

			if(strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0){
				continue;
			}
			if((strlen(path) + strlen(de->d_name)) < FILENAME_BUF_SIZE){
				char buf[FILENAME_BUF_SIZE + 1];
				snprintf(buf, sizeof(buf),"%s/%s", path, de->d_name);
				dir_files_add_db(buf, extension, dir_id);
			}else{
				fprintf(stderr, "Failed to add file(%s/%s) : File path is too long\n", path, de->d_name);
			}
		}else{

			if(check_ext(de->d_name, extension)){
				char buf[FILENAME_BUF_SIZE];
				sprintf(buf, "%s/%s", path, de->d_name);
				db_music_add(buf, dir_id);
			}
		}
	}
}


static GObject* show_music_add_load_dialog(GObject* parent){
	GtkBuilder* builder = load_ui("/ui/music_add_load");
	GObject* dialog = get_object(builder, "dialog");
	adw_dialog_present(ADW_DIALOG(dialog), GTK_WIDGET(parent));
	return dialog;
}

static void* thread_dir_add(void* data){
	struct dir_thread* dir = (struct dir_thread*) data;
	dir_files_add_db(dir->path, dir->ext, dir->dir_id);
	adw_dialog_force_close(ADW_DIALOG(dir->dialog));
	home_init(dir->parent);
	return NULL;
}


static void show_dir_add_callback(GObject* src, GAsyncResult* res, gpointer data){
	GObject* parent = (GObject*)data;
	GFile* dir = gtk_file_dialog_select_folder_finish(GTK_FILE_DIALOG(src), res, NULL);
	if(dir != NULL){
		char* path = g_file_get_path(dir);
		int res = db_music_dir_add(path);
		if(res == DB_SUCCESS){
			GObject* dialog = show_music_add_load_dialog(parent);
			int dir_id = db_music_dir_get_id(path);
			struct dir_thread* dir_s = malloc(sizeof(struct dir_thread));
			dir_s->path = path;
			dir_s->ext = "mp3";
			dir_s->dir_id = dir_id;
			dir_s->dialog = dialog;
			dir_s->parent = parent;

			pthread_t thread;
			pthread_create(&thread, NULL, thread_dir_add, (void*)dir_s);
		}else if(res == DB_EXISTS){
			show_alert_dialog("Fail", "Directory already exists", 0, parent);
		}else{
			show_alert_dialog("Fail", "Failed to add directory", 0, parent);
		}
	}
}

void show_dir_add_dialog(GObject* parent){
	GtkFileDialog* dialog = gtk_file_dialog_new();
	gtk_file_dialog_select_folder(dialog, GTK_WINDOW(parent), NULL, show_dir_add_callback, parent);
}

void parse_time(char* buffer, const double dur){
	int time = (int)dur;
	buffer[2] = ':';
	int minute = 0;
	
	while(time >= 60){
		time -= 60;
		minute++;
	}

	if(minute < 10){
		buffer[0] = '0';
		buffer[1] = minute + '0';
	}else{
		char buf[4];
		sprintf(buf, "%d", (unsigned char)minute);
		buffer[0] = buf[0];
		buffer[1] = buf[1];
	}

	if(time < 10){
		buffer[3] = '0';
		buffer[4] = time + '0';
	}else{
		char buf[4];
		sprintf(buf, "%d", (unsigned char)time);
		buffer[3] = buf[0];
		buffer[4] = buf[1];
	}

}

void playsong(GtkWidget* self, gpointer p){
	int index = GPOINTER_TO_INT(p);
	music_t music = get_music_list(index);
	sound_set((const char*)music.path, music_finish_callback);
	music_info_set((const char*) music.path);
	sound_play();
	
	for(int i = 0; i < get_list_size(); i++){
		gtk_widget_remove_css_class(GTK_WIDGET(get_btn_list(i)), "suggested-action");
	}

	set_current_index(index);
	set_playing_index(index);
	gtk_widget_add_css_class(GTK_WIDGET(get_btn_list(index)), "suggested-action");
	set_playing_title(music_info_get_title(), (const char*)music.path);
	set_playing_artist(music_info_get_artist());
	set_playing_duration(music_info_get_duration());
	if(db_like_exists(music.id) == DB_EXISTS){
		set_playing_like(true);
	}else{
		set_playing_like(false);
	}

	set_cur_playing(music);
}

void fill_listbox(GtkListBox* list_box, music_t* list, int count){

	for(int i = 0; i < count; i++){
		music_info_set((const char*)list[i].path);

		const char* title = music_info_get_title();
		const char* artist = music_info_get_artist();
		const double duration = music_info_get_duration();

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

		if(list[i].id == get_cur_playing().id){
			gtk_widget_add_css_class(GTK_WIDGET(btn), "suggested-action");
		}

		gtk_list_box_append(GTK_LIST_BOX(list_box), GTK_WIDGET(btn));
		add_to_music_list(list[i], btn);
	}

}
