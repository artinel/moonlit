#ifndef _MUSIC_H
#define _MUSIC_H

#include <db/db.h>

#define MUSIC "music"
#define MUSIC_ID "id"
#define MUSIC_ID_DIR "dir_id"
#define MUSIC_PATH "path"

typedef struct music{
	int id;
	int dir_id;
	const unsigned char* path;
}music_t;

db_signal db_music_add(const char* path, const int dir_id);
int db_music_get_all(music_t* list, const int size);

#endif
