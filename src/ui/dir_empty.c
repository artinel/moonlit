#include <libadwaita-1/adwaita.h>
#include <utils.h>
#include <db/db.h>
#include <ui/main_window.h>

static void show_dir_add_dialog();
static void show_dir_add_callback(GObject* src, GAsyncResult* res, gpointer data);
static GObject* parent;

void dir_empty_init(GObject* window){
	parent = window;
	GtkBuilder* dir_builder = load_ui("/ui/dir_empty");
	GObject* dir_view = get_object(dir_builder, "dir_empty");
	GObject* btn_add = get_object(dir_builder, "add_directory");
	g_signal_connect(GTK_WIDGET(btn_add), "clicked", G_CALLBACK(show_dir_add_dialog), NULL);
	set_main_view(dir_view);
}

static void show_dir_add_dialog(){
	GtkFileDialog* dialog = gtk_file_dialog_new();
	gtk_file_dialog_select_folder(dialog, GTK_WINDOW(parent), NULL, show_dir_add_callback, NULL);
}

static void show_dir_add_callback(GObject* src, GAsyncResult* res, gpointer data){
	GFile* dir = gtk_file_dialog_select_folder_finish(GTK_FILE_DIALOG(src), res, NULL);
	if(dir != NULL){
		char* path = g_file_get_path(dir);
		int res = db_music_dir_add(path);
		if(res == DB_SUCCESS){
			show_alert_dialog("Success", "Directory added successfully", 1, parent);
		}else if(res == DB_EXISTS){
			show_alert_dialog("Fail", "Directory already exists", 0, parent);
		}else{
			show_alert_dialog("Fail", "Failed to add directory", 0, parent);
		}
	}
}
