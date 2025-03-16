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

void load_css(GtkWidget *widget, char *css){
	GtkCssProvider *provider = gtk_css_provider_new();
	gtk_css_provider_load_from_resource(provider, css);
	gtk_style_context_add_provider_for_display(gtk_widget_get_display(widget),
			GTK_STYLE_PROVIDER(provider),
			GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
}

