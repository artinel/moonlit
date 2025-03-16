#include <libadwaita-1/adwaita.h>
//local includes
#include <utils.h>

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
