#include <libadwaita-1/adwaita.h>
//Local includes
#include <utils.h>
#include <ui/home.h>
#include <ui/main_window.h>

static GObject* parent;

void home_init(GObject* window){
	parent = window;
	GtkBuilder* home_builder = load_ui("/ui/home");
	GObject* home_view = get_object(home_builder, "home_view");
	set_main_view(home_view);
}
