#include <libadwaita-1/adwaita.h>
//local includes
#include <ui/main_window.h>
#include <utils.h>
#include <db/db.h>
#include <db/music_dir.h>
#include <ui/dir_empty.h>
#include <ui/home.h>

static GObject* window;
static GObject* main_view;

void main_window_activate(AdwApplication* app){
	GtkBuilder* builder = load_ui("/ui/main");
	window = get_object(builder, "window");
	main_view = get_object(builder, "main_view");
	
	gtk_window_set_application(GTK_WINDOW(window), GTK_APPLICATION(app));
	load_css(GTK_WIDGET(window), "/css/global-style");
	gtk_widget_set_visible(GTK_WIDGET(window), true);


	if(db_table_count(MUSIC_DIR) > 0){
		home_init(window);
	}else{
		dir_empty_init(window);
	}
}

void set_main_view(GObject* widget){
	adw_toolbar_view_set_content(ADW_TOOLBAR_VIEW(main_view), GTK_WIDGET(widget));
}
