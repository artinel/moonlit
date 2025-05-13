#ifndef _MAIN_WINDOW_H
#define _MAIN_WINDOW_H

#include <libadwaita-1/adwaita.h>
#include <db/music.h>

void main_window_activate(AdwApplication* app);
void set_main_view(GObject* widget);
void set_playing_title(const char* title, const char* path);
void set_playing_artist(const char* artist);
void set_playing_duration(const double duration);
void add_to_music_list(music_t music, GtkWidget* btn);
music_t get_music_list(int index);
void set_current_index(int index);

#endif
