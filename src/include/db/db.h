#ifndef _DB_H
#define _DB_H

#include <sqlite3.h>

typedef enum{
	DB_SUCCESS,
	DB_FAILED,
	DB_CLOSE,
	DB_OPEN,
	DB_EXISTS,
	DB_NOT_EXISTS,
}db_signal;

void db_init();
void db_exit();
void db_err();
char* db_sql_generator(char* base, ...);
unsigned int db_table_count(const char* table);
sqlite3* get_db();
db_signal get_db_state();
void db_close_err();

#endif
