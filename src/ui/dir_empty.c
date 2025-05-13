#include <libadwaita-1/adwaita.h>
#include <pthread.h>
//Local includes
#include <utils.h>
#include <db/db.h>
#include <db/music_dir.h>
#include <db/music.h>
#include <ui/main_window.h>

static GObject* parent;

void dir_empty_init(GObject* window){
	parent = window;
	GtkBuilder* dir_builder = load_ui("/ui/dir_empty");
	GObject* dir_view = get_object(dir_builder, "dir_empty");
	GObject* btn_add = get_object(dir_builder, "add_directory");
	g_signal_connect(GTK_WIDGET(btn_add), "clicked", G_CALLBACK(show_dir_add_dialog), NULL);
	set_main_view(dir_view);
}


