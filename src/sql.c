#include "header.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>


#define DB_PATH "db/atm.db"

static char* sql_update(char* key,char* newvalue,char* table, char* columnName);

int usercallback(void *data, int rowsCount, char **rowsValues, char **columnNames) {
    struct User *user = (struct User *)data;
    for (int i = 0; i < rowsCount; i++) {
        if (strcmp(columnNames[i], "user_id") == 0) {
            user->id = strtoull(rowsValues[i], NULL, 10);
        } else if (strcmp(columnNames[i], "name") == 0) {
            strcpy(user->name, rowsValues[i]);
        } else if (strcmp(columnNames[i], "password") == 0) {
            strcpy(user->password, rowsValues[i]);
        } else if (strcmp(columnNames[i], "active") == 0) {
            user->Active = (strcmp(rowsValues[i], "1") == 0);
        }
        // else {
        //     printf("%s = %s\n", columnNames[i], rowsValues[i] ? rowsValues[i] : "NULL");
        // }
    }
    return 0;
}

// Connect to the database
void sql_connect(void) {
    if (access(DB_PATH, F_OK) == -1) {
        printf("Database file does not exist.\n");
        exit(1);
    }

    char* err = NULL;
    int rc = sqlite3_open(DB_PATH, &db);
    if (rc != SQLITE_OK) {
        err = (char*)sqlite3_errmsg(db);
        printf("Error connecting to database: %s\n", err);
        free(err);
        sqlite3_close(db);
        exit(1);
    }
}

// Create new user
bool sql_insert_user(struct User newUser) {
    char* rcerr = NULL;
    // char* err = NULL;
    char sql[250];
    sprintf(sql,"INSERT INTO Users (name,password) VALUES ('%s', '%s');",newUser.name,newUser.password);
    int rc = sqlite3_exec(db, sql, NULL, NULL, &rcerr);
    if (rc != SQLITE_OK) {
        if (rc == 19)
        {
            printf("User already exist\n");
            sleep(2);
            sqlite3_free(rcerr);
            return false;  
        }
        printf("Error inserting user: %s\n", rcerr);
        sqlite3_free(rcerr);
        return false; 
    }
    return true;
}

// for loging but needs also a password
struct User sql_select_user(char* username) { 
    char* rcerr = NULL;
    char sql[250];
    sprintf(sql,"SELECT * FROM Users WHERE name = '%s';",username);
    struct User user;
    memset(&user, 0, sizeof(struct User));
    int rc = sqlite3_exec(db, sql, usercallback, &user, &rcerr);
    if (rc != SQLITE_OK) {
        // printf("%s\n",sqlite3_errmsg(db));
        printf("%s\n",rcerr);
        sqlite3_free(rcerr);
        sqlite3_close(db);
        return user;
    }
    return user;
}

// char* get_primary_key(const char* table) {
//     char* primary_key = NULL;
//     char* query = NULL;
//     char* errorMsg = NULL;

//     // Prepare the query to retrieve table schema
//     int query_length = sprintf(query, "SELECT sql FROM sqlite_master WHERE type='table' AND name='%s'", table);
//     if (query_length == -1) {
//         fprintf(stderr, "Failed to allocate memory for query.\n");
//         return NULL;
//     }

//     // Execute the query
//     int rc = sqlite3_exec(db, query, NULL, NULL, &errorMsg);
//     if (rc != SQLITE_OK) {
//         fprintf(stderr, "Failed to execute query: %s\n", errorMsg);
//         sqlite3_free(errorMsg);
//         free(query);
//         return NULL;
//     }

//     const char* tableSchema = errorMsg;
//     const char* primaryKeyStart = strstr(tableSchema, "PRIMARY KEY");

//     if (primaryKeyStart != NULL) {
//         // Find the closing parenthesis ')' after "PRIMARY KEY"
//         const char* primaryKeyEnd = strchr(primaryKeyStart, ')');
//         if (primaryKeyEnd != NULL) {
//             // Allocate memory and copy the primary key definition
//             size_t key_length = primaryKeyEnd - primaryKeyStart + 1;
//             primary_key = malloc((key_length + 1) * sizeof(char));
//             strncpy(primary_key, primaryKeyStart, key_length);
//             primary_key[key_length] = '\0';
//         }
//     }

//     // Clean up
//     free(query);

//     return primary_key;
// }

static char* sql_update(char* key_value,char* newvalue,char* table, char* columnName) {
    char* rcerr = NULL;
    // char *primary_key = get_primary_key(table);
    char *primary_key = "user_id";
    char sql[250];
    int query_length = sprintf(sql,"UPDATE %s SET %s = '%s' WHERE %s = '%s';", table, columnName, newvalue ,primary_key ,key_value);
    if (query_length == -1) {
        return "Error Deleting user: Failed to allocate memory for query.\n";
    }
    int rc = sqlite3_exec(db, sql, NULL, NULL, &rcerr);
    if (rc != SQLITE_OK) {
        sqlite3_close(db);
        return rcerr; // dont forget to free when calling
    }
    return NULL;
}

// static char* get_key(char* table, struct User user) {
    
// }

// Inactive the user
char* sql_delete_user(struct User user) {
    char user_id[21];
    int id_length = sprintf(user_id, "%lu", user.id); // convert from uint64 to string
    if (id_length == -1) {
        return "Error Deleting user: Failed to allocate memory for query.\n";
    }
    char* err = sql_update(user_id, "0", "Users", "active");
    if (err != NULL) {
        return err;
    }
    return NULL;
}

// Create new account
char* sql_create_account(struct User user,char* type, double balance, char* country, char* phone) {
    char user_id[21];
    sprintf(user_id, "%lu", user.id); // convert from uint64 to string
    // printf("%s\n",user_id);
    time_t current_time = time(NULL);
    char date[11];
    strftime(date, sizeof(date), "%Y-%m-%d", localtime(&current_time));
    // printf("%s\n",date);
    
    char* rcerr = NULL;
    char sql[250];
    sprintf(sql, "INSERT INTO Accounts (user_id, type, date, balance, country, phone) VALUES ('%s', '%s', '%s', %.3f, '%s', '%s');", user_id, type, date, balance, country, phone);
    int rc = sqlite3_exec(db, sql, NULL, NULL, &rcerr);
    if (rc != SQLITE_OK) {
        sqlite3_close(db);
        return rcerr; // dont forget to free when calling
    }
    return NULL;
}

char* sql_update_account(uint64_t account_id, char* to_update) {
    if (strcmp(to_update, "country") != 0 && strcmp(to_update, "phone") != 0 ) {
        return "Error: can only update country or phone number.";
    }
    // char account_id_str[21];
    // sprintf(account_id_str, "%lu", account_id); // convert from uint64 to string
    // char* err = sql_update(user, "0", "Users", "active");
    // if (err != NULL) {
    //     return err;
    // }
    // return NULL;
}


// No deleting to the User Only it will be inactive
// char* sql_delete_user(char* username) {
//     char* rcerr = NULL;
//     char* err = NULL;
//     char sql[250];
//     sprintf(sql,"DELETE FROM Users WHERE Users.name = '%s';",username);
//     int rc = sqlite3_exec(db, sql, callback, NULL, &rcerr);
//     if (rc != SQLITE_OK) {
//         err = (char*)sqlite3_errmsg(db);
//         sqlite3_free(rcerr);
//         sqlite3_close(db);
//         return err;
//     }
//     else if (sqlite3_changes(db) == 0) {
//         return "no user to delete";
//     }
//     return NULL;
// }

// static bool _check(void) {
//     char* check_file = "db/check";
//     long size = file_size(check_file);
//     if (size == -1) {
//         return false;
//     }
//     FILE* f = fopen(check_file,"r");
//     if (f == NULL) {
//         return -1;
//     }
//     char* f_content = malloc((size+1));
//     unsigned long b_read = fread(f_content,1,size,f);
//     if (b_read != (unsigned long) size) {
//         free(f_content);
//         fclose(f);
//         return false;
//     }
//     f_content[size] = '\0';
//     fclose(f);
//     free(f_content);
//     return true;
// }