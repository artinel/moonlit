#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <stdbool.h>
//Local includes
#include <sound.h>

static Mix_Music* music;
static bool is_playing = false;

void sound_init(){
	Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 640);
}

void sound_exit(){
	Mix_CloseAudio();
}

void sound_set(const char* music_path){
	if(music != NULL){
		Mix_FreeMusic(music);
	}
	music = Mix_LoadMUS(music_path);
}

const char* sound_get_title(){
	const char* title = Mix_GetMusicTitle(music);
	return title;
}

const char* sound_get_artist(){
	const char* tag = Mix_GetMusicArtistTag(music);
	return tag;
}

double sound_get_duration(){
	double dur = Mix_MusicDuration(music);
	return dur;
}

void sound_play(){
	Mix_PlayMusic(music, 1);
	is_playing = true;
}

void sound_pause(){
	Mix_PauseMusic();
	is_playing = false;
}

void sound_resume(){
	Mix_ResumeMusic();
	is_playing = true;
}

int sound_is_playing(){
	return is_playing;
}
