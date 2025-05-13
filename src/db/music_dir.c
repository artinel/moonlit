#include <stddef.h>
#include <stdlib.h>

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
		if(status == DB_SUCCESS || status == 101){
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
