#ifndef _MUSIC_INFO_H
#define _MUSIC_INFO_H



void music_info_set(const char* music_path);
const char* music_info_get_title();
const char* music_info_get_artist();
double music_info_get_duration();

#endif
