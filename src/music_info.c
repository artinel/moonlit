#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <stdbool.h>
//Local includes
#include <music_info.h>

 
static Mix_Music* music;



void music_info_set(const char* music_path){
	if(music != NULL){
		Mix_FreeMusic(music);
	}
	music = Mix_LoadMUS(music_path);

}

const char* music_info_get_title(){
	const char* title = Mix_GetMusicTitle(music);
	return title;
}

const char* music_info_get_artist(){
	const char* tag = Mix_GetMusicArtistTag(music);
	return tag;
}

double music_info_get_duration(){
	double dur = Mix_MusicDuration(music);
	return dur;
}


