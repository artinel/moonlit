#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
//local includes
#include <db/db.h>

#define DB_NAME "/.local/share/moonlit/data.db"

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
			db_state == DB_OPEN;
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
			db_state == DB_CLOSE;
			return DB_SUCCESS;
		}else{
			db_err();
			return DB_FAILED;
		}
	}
	return DB_CLOSE;
}
