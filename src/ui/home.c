#include <libadwaita-1/adwaita.h>
//Local includes
#include <utils.h>
#include <ui/home.h>
#include <ui/main_window.h>
#include <db/music.h>
#include <sound.h>

static GObject* parent;
static GObject* home_list;

void home_init(GObject* window){
	parent = window;
	GtkBuilder* home_builder = load_ui("/ui/home");
	GObject* home_view = get_object(home_builder, "home_view");
	home_list = get_object(home_builder, "home_list");
	set_main_view(home_view);

	music_t list[1024];
	int count = db_music_get_all(list, 1024);
	clear_home_list();
	fill_listbox(GTK_LIST_BOX(home_list), list, count);

}

void clear_home_list(){
	gtk_list_box_remove_all(GTK_LIST_BOX(home_list));
}

GObject* get_home_list(){
	return home_list;
}
