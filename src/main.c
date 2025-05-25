/**
* \file src/main.c
* \brief The program's main file
* \author Mohammad shamsi <artinel@proton.me>
* \version 0.0.1
* \date 2025-05-24
* \copyright GNU Public License V3
*/

#include <libadwaita-1/adwaita.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
//local includes
#include <ui/splash_window.h>
#include <db/db.h>
#include <sound.h>
#include <ui/main_window.h>

#define APP_ID "com.artinel.moonlit" /**< The application ID */


/**
 * The application constructor(Calls before the main function).
 * Mainly used for initializing libraries and frameworks like SDL and sqlite3
 * \sa db_init sound_init
 * */

__attribute__((constructor))
void constructor(){
	db_init();

	int result = 0;
	int flags = MIX_INIT_MP3;
	if(SDL_Init(SDL_INIT_AUDIO) < 0){
		fprintf(stderr, "Failed initialize SDL!!!\n");
		return;
	}

	if(flags != (result = Mix_Init(flags))){
		fprintf(stderr, "Failed to initialize mixer(%d)\n", result);
		fprintf(stderr, "Mix_Init: %s\n", Mix_GetError());
		return;
	}

	sound_init();
}


/**
 * The application destructor(Calls after the main function).
 * Mainly used for freeing and closing libraries and frameworks like SDL and sqlite3
 * \sa sound_exit SDL_Quit db_exit
 * */

__attribute__((destructor))
void destructor(){
	sound_exit();
	SDL_Quit();
	db_exit();
}

int main(int argc, char** argv){
	AdwApplication* app = adw_application_new(APP_ID, G_APPLICATION_DEFAULT_FLAGS);
	g_signal_connect(app, "activate", G_CALLBACK(splash_window_activate), NULL);
	return g_application_run(G_APPLICATION(app), argc, argv);
}
