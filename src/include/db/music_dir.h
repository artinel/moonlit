#ifndef _MUSIC_DIR_H
#define _MUSIC_DIR_H

#define MUSIC_DIR "music_dir"
#define MUSIC_DIR_ID "id"
#define MUSIC_DIR_DIR "dir"

typedef struct music_dir{
	int id;
	const unsigned char* dir;
}music_dir_t;

db_signal db_music_dir_add(const char* path);
int db_music_dir_get_id(const char* dir);

#endif
