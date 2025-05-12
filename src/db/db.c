#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
//local includes
#include <db/db.h>
#include <db/music_dir.h>

#define DB_NAME "/.local/share/moonlit/data.db"
#define SQL_BUFFER_SIZE 1024

static sqlite3* db;
static db_signal db_state = DB_CLOSE;

static void db_err(){
	const char* err = sqlite3_errmsg(db);
	int code = sqlite3_errcode(db);
	if(err != NULL && code != 0){
		fprintf(stderr, "Database error(%d) : %s\n", code, err);
	}
}

static db_signal db_open(){
	if(db_state == DB_CLOSE){
		char* dir = malloc(256);
		sprintf(dir, "%s%s", getenv("HOME"), DB_NAME);
		if(sqlite3_open(dir, &db) == DB_SUCCESS){
			db_state = DB_OPEN;
			free(dir);
			return DB_SUCCESS;
		}else{
			db_err();
			free(dir);
			return DB_FAILED;
		}
	}
	return DB_OPEN;
}

static db_signal db_close(){
	if(db_state == DB_OPEN){
		if(sqlite3_close(db) == DB_SUCCESS){
			db_state = DB_CLOSE;
			return DB_SUCCESS;
		}else{
			db_err();
			return DB_FAILED;
		}
	}
	return DB_CLOSE;
}

static char* db_sql_generator(char* base, ...){
	va_list data;
	va_start(data, base);
	char* sql = malloc(SQL_BUFFER_SIZE);
	vsprintf(sql, base, data);
	va_end(data);
	return sql;
}

static void db_init_exec(const char* sql){
	char* err;
	if(sqlite3_exec(db, sql, NULL, NULL, &err) != DB_SUCCESS){
		fprintf(stderr, "Failed to initialize database : %s\n", err);
	}
}

static void db_close_err(){
	fprintf(stderr, "Error : Database is closed!!!\n");
}

void db_init(){
	if(db_open() == DB_SUCCESS){
		char* sql_music_dir = db_sql_generator(
				"CREATE TABLE IF NOT EXISTS %s(%s INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT , \
				 %s TEXT NOT NULL UNIQUE);", 
				MUSIC_DIR, MUSIC_DIR_ID, MUSIC_DIR_DIR, MUSIC_DIR_ID);
	
		db_init_exec(sql_music_dir);	

		free(sql_music_dir);
	}
}

void db_exit(){
	if(db_state == DB_OPEN){
		if(!sqlite3_close(db) == DB_SUCCESS){
			db_err();
		}	
	}
}

unsigned int db_table_count(const char* table){
	if(db_state == DB_OPEN){
		char* sql = db_sql_generator("SELECT COUNT(*) FROM %s;", table);
		sqlite3_stmt* stmt = NULL;
		sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
		sqlite3_step(stmt);
		unsigned int res = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);
		free(sql);
		return res;
	}else{
		db_close_err();
		return DB_FAILED;
	}
}

db_signal db_music_dir_add(const char* path){
	if(db_state == DB_OPEN){
		char* sql = db_sql_generator("INSERT INTO %s(%s) VALUES(?);", MUSIC_DIR, MUSIC_DIR_DIR);
		sqlite3_stmt* stmt = NULL;
		sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
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
