#include "header.h"
#include <stdbool.h>
#include <stdio.h>

#define DB_PATH "db/atm.db"

int callback(void *data, int rowsCount, char **rowsValues, char **coulmnNames) {
    for (int i = 0; i < rowsCount; i++) {
        printf("%s = %s\n", coulmnNames[i], rowsValues[i] ? rowsValues[i] : "NULL");
    }
    printf("\n");
    return 0;
}

char* sql_connect(void) {
    char* err = NULL;
    char* rcerr;
    int rc = sqlite3_open(DB_PATH, &db);
    if (rc != SQLITE_OK) {
        err = (char*)sqlite3_errmsg(db);
        sqlite3_close(db);
        return err;
    }
    // char* sql = "INSERT INTO Users (name,password) "
    //             "VALUES ('mina', 'mina');";
    // sql = "DELETE FROM Users "
    //         "WHERE Users.name = 'mina';";
    // sql = "SELECT * "
    //     "FROM Users "
    //     "WHERE name = 'EMAN'"; // Alter to change 
    // rc = sqlite3_exec(db, sql, callback, NULL, &rcerr);
    // if (rc != SQLITE_OK) {
    //     err = (char*)sqlite3_errmsg(db);
    //     // printf("Error: %s\n", err);
    //     sqlite3_free(rcerr);
    //     sqlite3_close(db);
    //     return err;
    // }
    // printf("Record inserted\n");
    // sqlite3_close(db);
    return err;
}

char* sql_insert_user(char* username, char* password) {
    char* rcerr = NULL;
    char* err = NULL;
    char sql[250];
    sprintf(sql,"INSERT INTO Users (name,password) VALUES ('%s', '%s');",username,password);
    int rc = sqlite3_exec(db, sql, callback, NULL, &rcerr);
    if (rc != SQLITE_OK) {
        err = (char*)sqlite3_errmsg(db);
        sqlite3_free(rcerr);
        sqlite3_close(db);
        return err;
    }
    return NULL;
}

char* sql_select_user(char* username) {
    char* rcerr = NULL;
    char* err = NULL;
    char sql[250];
    sprintf(sql,"SELECT * FROM Users WHERE name = '%s';",username);
    int rc = sqlite3_exec(db, sql, callback, NULL, &rcerr);
    if (rc != SQLITE_OK) {
        err = (char*)sqlite3_errmsg(db);
        sqlite3_free(rcerr);
        sqlite3_close(db);
        return err;
    }
    return NULL;
}

char* sql_delete_user(char* username) {
    char* rcerr = NULL;
    char* err = NULL;
    char sql[250];
    sprintf(sql,"DELETE FROM Users WHERE Users.name = '%s';",username);
    int rc = sqlite3_exec(db, sql, callback, NULL, &rcerr);
    if (rc != SQLITE_OK) {
        err = (char*)sqlite3_errmsg(db);
        sqlite3_free(rcerr);
        sqlite3_close(db);
        return err;
    }
    else if (sqlite3_changes(db) == 0) {
        return "no user to delete";
    }
    return NULL;
}

static bool _check(void) {
    char* check_file = "db/check";
    long size = file_size(check_file);
    if (size == -1) {
        return false;
    }
    FILE* f = fopen(check_file,"r");
    if (f == NULL) {
        return -1;
    }
    char* f_content = malloc((size+1));
    unsigned long b_read = fread(f_content,1,size,f);
    if (b_read != (unsigned long) size) {
        free(f_content);
        fclose(f);
        return false;
    }
    f_content[size] = '\0';
    fclose(f);
    free(f_content);
    return true;
}