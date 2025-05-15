#ifndef _FAVORITE_H
#define _FAVORITE_H

#include <db/music.h>

#define LIKE "fav"
#define LIKE_ID "id"
#define LIKE_MUSIC_ID "music_id"


db_signal db_like_add(const int id);
db_signal db_like_exists(const int id);
db_signal db_like_remove(const int id);
int db_like_get_all(music_t* list, const int size);

#endif
