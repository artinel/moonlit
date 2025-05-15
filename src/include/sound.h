#ifndef _SOUND_H
#define _SOUND_H

void sound_init();
void sound_exit();
void sound_set(const char* music_path, void (*callback)(void));
const char* sound_get_title();
const char* sound_get_artist();
double sound_get_duration();
void sound_play();
void sound_pause();
void sound_resume();
int sound_is_playing();
double sound_get_position();
void sound_set_is_playing(bool status);
void sound_set_position(double pos);
void sound_set_volume(int volume);

#endif
