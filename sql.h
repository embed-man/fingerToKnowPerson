#ifndef __SQL_H__
#define __SQL_H__

#include <sqlite3.h>

extern sqlite3 *sqlite3_init(const char *path);
extern void sqlite3_add_user(sqlite3 *db, const unsigned char *name, unsigned short id, const unsigned char spec[]);

#endif