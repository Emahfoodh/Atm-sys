#include "header.h"
#include <stdlib.h>
#include <stdio.h>

int main() {
    char* err = sql_connect();
    if (err != NULL) {
        printf("Error: %s\n", err);
        free(err);
        return EXIT_FAILURE;
    }
    char* insertUser = sql_delete_user("gg");
    if (insertUser != NULL) {
        printf("Error: %s\n", insertUser);
        free(err);
        return EXIT_FAILURE;
    }
    sqlite3_close(db);
    return EXIT_SUCCESS;
}
