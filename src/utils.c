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

void show_alert_dialog(const char* title, const char* body, int mode, GObject* parent){
	AdwDialog* dialog = adw_alert_dialog_new(title, body);
	adw_alert_dialog_add_response(ADW_ALERT_DIALOG(dialog), "close", "Close");
	adw_alert_dialog_set_response_appearance(ADW_ALERT_DIALOG(dialog), "close", 
			(mode == 0) ? ADW_RESPONSE_DESTRUCTIVE : ADW_RESPONSE_SUGGESTED);
	adw_alert_dialog_set_default_response(ADW_ALERT_DIALOG(dialog), "close");
	adw_alert_dialog_set_close_response(ADW_ALERT_DIALOG(dialog), "close");
	adw_dialog_present(ADW_DIALOG(dialog), GTK_WIDGET(parent));
}
