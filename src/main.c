#include "header.h"
#include <sqlite3.h>
#include <stdio.h>
#include <sys/stat.h>

#define DB_PATH "atm.db"

int main() {
    char *err;
    sqlite3 *db;
    int rc = sqlite3_open(DB_PATH,&db);
    if (rc != SQLITE_OK) {
        perror(sqlite3_errmsg(db));
        return EXIT_FAILURE;
    }

    rc = sqlite3_exec(db,"create table if not exist",NULL,NULL, &err);
    if (rc != SQLITE_OK) {
        perror(err);
        return EXIT_FAILURE;
    }
    sqlite3_close(db);
    return  EXIT_SUCCESS;
}