#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
//Local includes
#include <db/db.h>
#include <db/music.h>

db_signal db_music_add(const char* path, const int dir_id){
	if(get_db_state() == DB_OPEN){
		char* sql = db_sql_generator("INSERT INTO %s(%s,%s) VALUES(?,?);", MUSIC, MUSIC_ID_DIR, MUSIC_PATH);
		sqlite3_stmt* stmt = NULL;
		sqlite3_prepare_v2(get_db(), sql, -1, &stmt, 0);
		sqlite3_bind_int(stmt, 1, dir_id);
		sqlite3_bind_text(stmt, 2, (char*)path, -1, SQLITE_TRANSIENT);
		int status = sqlite3_step(stmt);
		db_err();
		sqlite3_finalize(stmt);
		free(sql);
		if(status == DB_SUCCESS || status == 101){
			return DB_SUCCESS;
		}else{
			return DB_FAILED;
		}
	}else{
		db_close_err();
		return DB_FAILED;	
	}
}

int db_music_get_all(music_t* list, const int size){
	if(get_db_state() == DB_OPEN){
		char* sql = db_sql_generator("SELECT * FROM %s;", MUSIC);
		sqlite3_stmt* stmt = NULL;
		sqlite3_prepare_v2(get_db(), sql, -1, &stmt, 0);
		int index = 0;
		while(sqlite3_step(stmt) == SQLITE_ROW && index < size){
			music_t music;

			music.id = sqlite3_column_int(stmt, 0);

			music.dir_id = sqlite3_column_int(stmt, 1);

			const unsigned char* path = sqlite3_column_text(stmt, 2);
			music.path = malloc(strlen((const char*)path) + 1);
			sprintf((char*)music.path, "%s", path);

			list[index] = music;
			index++;
		}
		sqlite3_finalize(stmt);
		db_err();
		return index;
	}else{
		db_close_err();
		return DB_FAILED;
	}
}


db_signal db_music_delete(const int id){
	if(get_db_state() == DB_OPEN){
		char* sql = db_sql_generator("DELETE FROM %s WHERE %s = ?", MUSIC, MUSIC_ID_DIR);
		sqlite3_stmt* stmt = NULL;
		sqlite3_prepare_v2(get_db(), sql, -1, &stmt, 0);
		sqlite3_bind_int(stmt, 1, id);
		int status = sqlite3_step(stmt);
		db_err();
		sqlite3_finalize(stmt);
		free(sql);
		if(status == DB_SUCCESS || status == 101){
			return DB_SUCCESS;
		}else{
			return DB_FAILED;
		}
	}else{
		db_close_err();
		return DB_FAILED;	
	}
}
