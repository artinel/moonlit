#ifndef _DB_H
#define _DB_H

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

#endif
