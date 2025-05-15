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

#define FILENAME_BUF_SIZE 512


static bool check_ext(const char* filename, const char* extension);
static void* thread_dir_add(void* data);
static void show_dir_add_callback(GObject* src, GAsyncResult* res, gpointer data);

struct dir_thread{
	char* path;
	char* ext;
	int dir_id;
	GObject* dialog;
	GObject* parent;
};


GtkBuilder* load_ui(const char* ui){
	GtkBuilder* builder = gtk_builder_new();
	GError* err = NULL;
	gtk_builder_add_from_resource(builder, ui, &err);
	if(err != NULL){
		g_print("Error in loading (%s) : %s\n", ui, err->message);
	}
	return builder;
}

GObject* get_object(GtkBuilder* builder, const char* id){
	return gtk_builder_get_object(builder, id);
}

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
	sound_play();
	if(get_btn_list(get_current_index()) != NULL){
		gtk_widget_remove_css_class(GTK_WIDGET(get_btn_list(get_current_index())), "suggested-action");
	}
	set_current_index(index);
	gtk_widget_add_css_class(GTK_WIDGET(get_btn_list(index)), "suggested-action");
	set_playing_title(sound_get_title(), (const char*)music.path);
	set_playing_artist(sound_get_artist());
	set_playing_duration(sound_get_duration());
	if(db_like_exists(music.id) == DB_EXISTS){
		set_playing_like(true);
	}else{
		set_playing_like(false);
	}
}
