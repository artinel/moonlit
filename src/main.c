#include <libadwaita-1/adwaita.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
//local includes
#include <ui/splash_window.h>
#include <db/db.h>
#include <sound.h>

#define APP_ID "com.artinel.moonlit"

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

__attribute__((destructor))
void destructor(){
	db_exit();
	sound_exit();
	SDL_Quit();
}

int main(int argc, char** argv){
	AdwApplication* app = adw_application_new(APP_ID, G_APPLICATION_DEFAULT_FLAGS);
	g_signal_connect(app, "activate", G_CALLBACK(splash_window_activate), NULL);
	return g_application_run(G_APPLICATION(app), argc, argv);
}
