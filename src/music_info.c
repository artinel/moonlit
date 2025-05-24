/**
* \file src/music_info.c
* \brief Extracting music information from MP3 file
* \author Mohammad shamsi <artinel@proton.me>
* \version 0.0.1
* \date 2025-05-24
* \copyright GNU Public License V3
*/

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <stdbool.h>
//Local includes
#include <music_info.h>


static Mix_Music* music;	/**< The music file that we want to extract the metadata from */



/**
 * Setting the music variable
 * This function first checks if music object is NULL or not, if it is not
 * NULL, then it will free the music file using Mix_FreeMusic function(SDL2)
 * and if it is NULL then it will initialize it using the given music path
 * and Mix_LoadMUS function.
 *
 * \param music_path	The path if the music that we want to load
 * \sa music
 **/
void music_info_set(const char* music_path){
	if(music != NULL){
		Mix_FreeMusic(music);
	}
	music = Mix_LoadMUS(music_path);

}

/**
 * Get the title from the music that has been set before using Mix_GetMusicTitle
 * \return	The music title
 * \sa	music
 **/
const char* music_info_get_title(){
	const char* title = Mix_GetMusicTitle(music);
	return title;
}


/**
 * Get the artist from the music that has been set before using Mix_GetMusicArtistTag
 * \return	The music artist
 * \sa	music
 **/
const char* music_info_get_artist(){
	const char* tag = Mix_GetMusicArtistTag(music);
	return tag;
}


/**
 * Get the music duration from the music that has been set before using Mix_MusicDuration
 * \return	The music duration
 * \sa	music
 **/
double music_info_get_duration(){
	double dur = Mix_MusicDuration(music);
	return dur;
}
