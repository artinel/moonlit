#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
//Local includes
#include <db/db.h>
#include <db/like.h>

db_signal db_like_add(const int id){
	if(get_db_state() == DB_OPEN){
		char* sql = db_sql_generator("INSERT INTO %s(%s) VALUES(?);", LIKE, LIKE_MUSIC_ID);
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

db_signal db_like_remove(const int id){
	if(get_db_state() == DB_OPEN){
		char* sql = db_sql_generator("DELETE FROM %s WHERE %s = ?", LIKE, LIKE_MUSIC_ID);
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


db_signal db_like_exists(const int id){
	if(get_db_state() == DB_OPEN){
		char* sql = db_sql_generator("SELECT COUNT(%s) FROM %s WHERE %s = ?;", LIKE_ID, LIKE, LIKE_MUSIC_ID);
		sqlite3_stmt* stmt = NULL;
		sqlite3_prepare_v2(get_db(), sql, -1, &stmt, 0);
		sqlite3_bind_int(stmt, 1, id);
		sqlite3_step(stmt);
		unsigned int res = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);
		free(sql);
		if(res > 0){
			return DB_EXISTS;
		}else{
			return DB_NOT_EXISTS;
		}
	}else{
		db_close_err();
		return DB_FAILED;
	}
}

int db_like_get_all(music_t* list, const int size){
	if(get_db_state() == DB_OPEN){
		char* sql = db_sql_generator("SELECT * FROM %s,%s WHERE %s.%s = %s.%s;", MUSIC, LIKE, MUSIC, MUSIC_ID, LIKE, LIKE_MUSIC_ID);
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
