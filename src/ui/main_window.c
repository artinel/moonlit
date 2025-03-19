#include <libadwaita-1/adwaita.h>
//local includes
#include <ui/main_window.h>
#include <utils.h>

void main_window_activate(AdwApplication* app){
	GtkBuilder* builder = load_ui("/ui/main");
	GObject* window = get_object(builder, "window");

	gtk_window_set_application(GTK_WINDOW(window), GTK_APPLICATION(app));
	load_css(GTK_WIDGET(window), "/css/global-style");
	gtk_widget_set_visible(GTK_WIDGET(window), true);

}
