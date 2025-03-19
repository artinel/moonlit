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

void db_init(){
	if(db_open() == DB_SUCCESS){
		char* sql_music_dir = db_sql_generator(
				"CREATE TABLE IF NOT EXISTS %s(%s INTEGER NOT NULL, \
				 %s TEXT NOT NULL, PRIMARY KEY(%s));", 
				MUSIC_DIR, MUSIC_DIR_ID, MUSIC_DIR_DIR, MUSIC_DIR_DIR);
		
		char* err;
		if(sqlite3_exec(db, sql_music_dir, NULL, NULL, &err) != DB_SUCCESS){
			fprintf(stderr, "Failed to initialize database : %s\n", err);
		}
		db_close();
	}
}

void db_exit(){
	if(db_state == DB_OPEN){
		if(!sqlite3_close(db) == DB_SUCCESS){
			db_err();
		}	
	}
}
