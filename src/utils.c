#include <libadwaita-1/adwaita.h>
#include <stdio.h>
#include <string.h>
//local includes
#include <utils.h>

#define FILENAME_BUF_SIZE 512


static bool check_ext(const char* filename, const char* extension);

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

static bool check_ext(const char* filename, const char* extension){
	const char* dot = strrchr(filename, '.');
	if(dot != NULL && dot != filename){
		dot++;
		if(strcmp(dot, extension) == 0){
			return true;
		}else{
			return false;
		}
	}
	return false;
}

void dir_files_add_db(const char* path, const char* extension, const int dir_id){
	DIR* dir = opendir(path);
	if(dir == NULL){
		fprintf(stderr, "Failed to open directory : %s\n", path);
		return;	
	}

	struct dirent* de;
	while((de = readdir(dir)) != NULL){
		if(de->d_type == DT_DIR){
			if(strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0){
				continue;
			}
			if((strlen(path) + strlen(de->d_name)) < FILENAME_BUF_SIZE){
				char buf[FILENAME_BUF_SIZE];
				sprintf("%s/%s", path, de->d_name);
				dir_files_add_db(buf, extension, dir_id);
			}else{
				sprintf("Failed to add file(%s/%s) : File path is too long\n", path, de->d_name);
			}
		}else{
			if(check_ext(de->d_name, extension)){
				//TODO add files to database;
			}
		}
	}
}
