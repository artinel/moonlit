#include <libadwaita-1/adwaita.h>
//local includes
#include <ui/splash_window.h>
#include <db/db.h>

#define APP_ID "com.artinel.moonlit"

__attribute__((constructor))
void constructor(){
	db_init();
}

int main(int argc, char** argv){
	AdwApplication* app = adw_application_new(APP_ID, G_APPLICATION_DEFAULT_FLAGS);
	g_signal_connect(app, "activate", G_CALLBACK(splash_window_activate), NULL);
	return g_application_run(G_APPLICATION(app), argc, argv);
}
