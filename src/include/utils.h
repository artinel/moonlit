#ifndef _UTILS_H
#define _UTILS_H

#include <libadwaita-1/adwaita.h>

GtkBuilder* load_ui(const char* ui);
GObject* get_object(GtkBuilder* builder, const char* id);

#endif
