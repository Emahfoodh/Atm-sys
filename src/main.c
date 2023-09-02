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
    struct User user = sql_select_user("eman");
    if (user.id == 0) {
        printf("Error: no user \n");
        return EXIT_FAILURE;
    } else {
        printUser(user);
    }
    // err = sql_create_account(user,"savings", 15.5, "bah", "11111111");
    // if (err != NULL) {
    //     printf("Error: %s\n", err);
    //     free(err);
    //     return EXIT_FAILURE;
    // }
    // char* updateUser = sql_update_user(user,"Mena", "Users","name");
    // if (updateUser != NULL) {
    //     printf("Error: %s\n", updateUser);
    //     // free(err);
    //     return EXIT_FAILURE;
    // }
    // char* insertUser = sql_insert_user("Mena", "meme");
    // if (insertUser != NULL) {
    //     printf("Error: %s\n", insertUser);
    //     // free(err);
    //     return EXIT_FAILURE;
    // }
    char* dUserErr = sql_delete_user(user);
    if (dUserErr != NULL) {
        printf("Error: %s\n", dUserErr);
        free(dUserErr);
        return EXIT_FAILURE;
    }
    sqlite3_close(db);
    return EXIT_SUCCESS;
}



void printUser(struct User user) {
    printf("ID: %ld\n", user.id);
    printf("Name: %s\n", user.name);
    printf("Password: %s\n", user.password);
    printf("Active: %s\n", user.Active ? "true" : "false");
}