/**
* \file src/sound.c
* \brief Play, pause and similar operations for music
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
#include <sound.h>

static Mix_Music* music;			/**< The curren playing music */
static volatile bool is_playing = false;	/**< State that if music is playing or not(paused) */

/**
 * Initialize the sound library using Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 640);
 * */
void sound_init(){
	Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 640);
}


/**
 * Free the current playing music and close the library
 * */
void sound_exit(){
	Mix_FreeMusic(music);
	Mix_CloseAudio();
}


/**
 * Set the current playing music. This function first checks that if music is NULL
 * or not. If it is NULL, then it will initialize it using Mix_LoadMUS and set the
 * end of music callback using Mix HookMusicFinished. If the music is not NULL,
 * then it will free it using Mix_FreeMusic then it will initialize it again.
 *
 * \param music_path	The path of the music that we want to set
 * \param callback	The callback function that will be called when the music is over
 * */
void sound_set(const char* music_path, void (*callback)(void)){
	if(music != NULL){
		Mix_FreeMusic(music);
	}
	music = Mix_LoadMUS(music_path);
	Mix_HookMusicFinished(callback);
}


/**
 * Play the current sound using Mix_PlayMusic and set the is_playing state to true
 * \sa	is_playing
 * */
void sound_play(){
	Mix_PlayMusic(music, 1);
	is_playing = true;
}


/**
 * Pause the currently playing song using Mix_PauseMusic and set the is_playing state to false
 * \sa	is_playing
 * */
void sound_pause(){
	Mix_PauseMusic();
	is_playing = false;
}

/**
 * Resume the currently paused song using Mix_ResumeMusic and set the is_playing state to true
 * \sa	is_playing
 * */
void sound_resume(){
	Mix_ResumeMusic();
	is_playing = true;
}


/**
 * Returns the state of the song
 * \return	The state of the song(is_playing)
 * \sa		is_playing
 * */
int sound_is_playing(){
	return is_playing;
}


/**
 * Returns the current position in playing music using Mix_GetMusicPosition
 * \return	The current position in playing music
 * */
double sound_get_position(){
	double pos = Mix_GetMusicPosition(music);
	return pos;
}


/**
 * Set the is_playing state of the song
 * \param status	The state of the song that we want to set
 * \sa	is_playing
 * */
void sound_set_is_playing(bool status){
	is_playing = status;
}

/**
 * Set the current position in the playing song using Mix_SetMusicPosition.
 * It first checks if the music is playing or not(using is_playing state) and
 * if it was playing then it will change the position.
 * \param pos	The position we want to set
 * \sa	is_playing
 * */
void sound_set_position(double pos){
	if(is_playing == true){
		Mix_SetMusicPosition(pos);
	}
}


/**
 * Set the volume of the music player library using Mix_VolumeMusic
 * \param volume	The volume we want to set
 * */
void sound_set_volume(int volume){
	Mix_VolumeMusic(volume);
}
