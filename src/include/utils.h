#ifndef _UTILS_H
#define _UTILS_H

#include <libadwaita-1/adwaita.h>

GtkBuilder* load_ui(const char* ui);
GObject* get_object(GtkBuilder* builder, const char* id);
void load_css(GtkWidget *widget, char *css);
void show_alert_dialog(const char* title, const char* body, int mode, GObject* parent);
void dir_add_db(const char* path, const char* extension, const int dir_id);

#endif
