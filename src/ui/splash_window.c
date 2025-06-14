#include <libadwaita-1/adwaita.h>
//local includes
#include <ui/splash_window.h>
#include <ui/main_window.h>
#include <utils.h>
#include <unistd.h>

void splash_window_activate(AdwApplication* app){
	GtkBuilder* builder = load_ui("/ui/splash");
	GObject* window = get_object(builder, "window");

	gtk_window_set_application(GTK_WINDOW(window), GTK_APPLICATION(app));
	load_css(GTK_WIDGET(window), "/css/global-style");
	gtk_widget_set_visible(GTK_WIDGET(window), true);
	gtk_window_destroy(GTK_WINDOW(window));
	main_window_activate(app);
}
