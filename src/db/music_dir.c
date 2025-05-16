#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
//Local includes
#include <db/db.h>
#include <db/music_dir.h>

db_signal db_music_dir_add(const char* path){
	if(get_db_state() == DB_OPEN){
		char* sql = db_sql_generator("INSERT INTO %s(%s) VALUES(?);", MUSIC_DIR, MUSIC_DIR_DIR);
		sqlite3_stmt* stmt = NULL;
		sqlite3_prepare_v2(get_db(), sql, -1, &stmt, 0);
		sqlite3_bind_text(stmt, 1, (char*)path, -1, SQLITE_TRANSIENT);
		int status = sqlite3_step(stmt);
		db_err();
		sqlite3_finalize(stmt);
		free(sql);
		if(status == DB_SUCCESS || status == 101 || status == 100){
			return DB_SUCCESS;
		}else if(status == 19){
			return DB_EXISTS;
		}else{
			return DB_FAILED;
		}
	}else{
		db_close_err();
		return DB_FAILED;	
	}
}

int db_music_dir_get_id(const char* dir){
	if(get_db_state() == DB_OPEN){
		char* sql = db_sql_generator("SELECT %s FROM %s WHERE %s = ?;", MUSIC_DIR_ID, MUSIC_DIR, MUSIC_DIR_DIR);
		sqlite3_stmt* stmt = NULL;
		sqlite3_prepare_v2(get_db(), sql, -1, &stmt, 0);
		sqlite3_bind_text(stmt, 1, (char*) dir, -1, SQLITE_TRANSIENT);
		int status = sqlite3_step(stmt);
		db_err();
		int res = 0;
		if(status == DB_SUCCESS || status == 101 || status == 100){
			res = sqlite3_column_int(stmt, 0);
		}else{
			res = -1;
		}
		sqlite3_finalize(stmt);
		free(sql);
		return res;
	}else{
		db_close_err();
		return DB_FAILED;
	}
}

int db_music_dir_get_all(music_dir_t* list, const int size){
	if(get_db_state() == DB_OPEN){
		char* sql = db_sql_generator("SELECT * FROM %s;", MUSIC_DIR);
		sqlite3_stmt* stmt = NULL;
		sqlite3_prepare_v2(get_db(), sql, -1, &stmt, 0);
		int index = 0;
		while(sqlite3_step(stmt) == SQLITE_ROW && index < size){
			music_dir_t dir;

			dir.id = sqlite3_column_int(stmt, 0);

			const unsigned char* path = sqlite3_column_text(stmt, 1);
			dir.dir = malloc(strlen((const char*)path) + 1);
			sprintf((char*)dir.dir, "%s", path);

			list[index] = dir;
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

db_signal db_music_dir_delete(const int id){
	if(get_db_state() == DB_OPEN){
		char* sql = db_sql_generator("DELETE FROM %s WHERE %s = ?", MUSIC_DIR, MUSIC_DIR_ID);
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
