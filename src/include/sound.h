#ifndef _SOUND_H
#define _SOUND_H

void sound_init();
void sound_exit();
void sound_set(const char* music_path);
const char* sound_get_title();
const char* sound_get_artist();
double sound_get_duration();

#endif
