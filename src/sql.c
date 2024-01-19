#include "header.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>


#define DB_PATH "db/atm.db"

static bool sql_update(char* key_value,char* newvalue,char* table, char* columnName);
int usercallback(void *data, int rowsCount, char **rowsValues, char **coulmnNames);
int accountCallback(void* data, int rowsCount, char** rowsValues, char** columnNames);
char* get_primary_key(char* table);
const char* AccountTypeStrings[NumAccountTypes] = {
    "fixed01",
    "fixed02",
    "fixed03",
    "saving",
    "current"
};

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
bool sql_insert_user(struct User *newUser) {
    char* rcerr = NULL;
    char sql[250];
    sprintf(sql,"INSERT INTO Users (name,password) VALUES ('%s', '%s');",newUser->name,newUser->password);
    int rc = sqlite3_exec(db, sql, NULL, NULL, &rcerr);
    if (rc != SQLITE_OK) {
        if (rc == 19)
        {
            printf("User already exist\n");
            // sleep(2);
            sqlite3_free(rcerr);
            return false;  
        }
        printf("Error inserting user: %s\n", rcerr);
        sqlite3_free(rcerr);
        return false; 
    }
    *newUser = sql_select_user(newUser->name);
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
        // sqlite3_close(db);
        return user;
    }
    return user;
}

// Create new account
bool sql_create_account(struct User user, struct Account acc) {
    char user_id[21];
    sprintf(user_id, "%lu", user.id); // convert from uint64 to string

    // char date[11];
    // getCurrentDate(date);
    
    char* rcerr = NULL;
    char sql[250];
    sprintf(sql, "INSERT INTO Accounts (user_id, type, date, balance, country, phone) VALUES ('%s', '%s', '%s', %.2f, '%s', '%lld');",
        user_id,
        AccountTypeStrings[acc.type],
        acc.date,
        acc.balance,
        acc.country,
        acc.phone);

    int rc = sqlite3_exec(db, sql, NULL, NULL, &rcerr);
    if (rc != SQLITE_OK) {
        printf("Error creating account: %s\n", rcerr);
        sqlite3_free(rcerr);
        return false;
    }
    return true;
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

// Function to retrieve the primary key column name for a given table
char* get_primary_key(char* table) {
    sqlite3_stmt *stmt;
    char* primary_key_column = NULL;

    // Construct the PRAGMA statement to retrieve the primary key column name
    char pragma_sql[100];
    snprintf(pragma_sql, sizeof(pragma_sql), "PRAGMA table_info(%s);", table);

    // Prepare and execute the PRAGMA statement
    int rc = sqlite3_prepare_v2(db, pragma_sql, -1, &stmt, NULL);
    if (rc == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            // Retrieve the column name and type
            char* column_name = (char*)sqlite3_column_text(stmt, 1);
            char* column_type = (char*)sqlite3_column_text(stmt, 2);

            // Check if the column is a primary key
            if (strcmp(column_type, "INTEGER") == 0 && sqlite3_column_int(stmt, 5) > 0) {
                primary_key_column = strdup(column_name);
                break;
            }
        }
    }

    sqlite3_finalize(stmt);

    return primary_key_column;
}

static bool sql_update(char* key_value,char* newvalue,char* table, char* columnName) {
    char* rcerr = NULL;
    // char *primary_key = get_primary_key(table);
    char* primary_key = get_primary_key(table); /// make it as a function that returns the column of the primary key
    char sql[250];
    int query_length = sprintf(sql,"UPDATE %s SET %s = '%s' WHERE %s = '%s';", table, columnName, newvalue ,primary_key ,key_value);
    free(primary_key); // Free the memory allocated for primary_key
    if (query_length == -1) {
        printf("Failed to allocate memory for query.\n");
        return false;
    }
    int rc = sqlite3_exec(db, sql, NULL, NULL, &rcerr);
    if (rc != SQLITE_OK) {
        printf("Error updating: %s\n", rcerr);
        sqlite3_free(rcerr);
        // sqlite3_close(db);
        return false;
    }
    return true;
}

// static char* get_key(char* table, struct User user) {
    
// }

// Inactive the user
bool sql_delete_user(struct User user) {
    char user_id[21];
    int id_length = sprintf(user_id, "%lu", user.id); // convert from uint64 to string
    if (id_length == -1) {
        printf("Error Deleting user: Failed to allocate memory for query.\n");
        return false;
    }
    if (!sql_update(user_id, "0", "Users", "active")) {
        return false;
    }
    return true;
}

struct Account sql_select_account(char* acc_id)
{
    char* rcerr = NULL;
    char sql[250];
    sprintf(sql, "SELECT * FROM Accounts WHERE account_id = '%s';", acc_id);
    struct Account account;
    memset(&account, 0, sizeof(struct Account));

   
    int rc = sqlite3_exec(db, sql, accountCallback, &account, &rcerr);
    if (rc != SQLITE_OK)
    {
        printf("%s\n", rcerr);
        sqlite3_free(rcerr);
        // sqlite3_close(db);
        return account;
    }
    return account;
}

bool sql_update_account(uint64_t account_id, char* to_update, char* newValue) {
    if (strcmp(to_update, "country") != 0 && strcmp(to_update, "phone") != 0 ) {
        printf("Error: can only update country or phone number.");
        return false;
    }
    char account_id_str[21];
    sprintf(account_id_str, "%lu", account_id); // convert from uint64 to string
    if (!sql_update(account_id_str, newValue, "Accounts", to_update)) {
        return false;
    }
    return true;
}

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

int accountCallback(void* data, int rowsCount, char** rowsValues, char** columnNames)
{
    struct Account* account = (struct Account*)data;
    for (int i = 0; i < rowsCount; i++)
    {
        if (strcmp(columnNames[i], "account_id") == 0)
        {
            account->id = strtoull(rowsValues[i], NULL, 10);
        }
        else if (strcmp(columnNames[i], "user_id") == 0)
        {
            // Allocate memory for account->user if it's not already allocated
            if (account->user == NULL)
            {
                account->user = malloc(sizeof(struct User));
                account->user->id = 0; // Initialize the id field
            }
            // Assuming the user_id is stored as a string in the Accounts table
            account->user->id = strtoull(rowsValues[i], NULL, 10);
        }
        else if (strcmp(columnNames[i], "type") == 0)
        {
            // Assuming the type field is stored as an integer in the Accounts table
            account->type = (enum AccountType)atoi(rowsValues[i]);
        }
        else if (strcmp(columnNames[i], "balance") == 0)
        {
            account->balance = atof(rowsValues[i]);
        }
        else if (strcmp(columnNames[i], "country") == 0)
        {
            strcpy(account->country, rowsValues[i]);
        }
        else if (strcmp(columnNames[i], "phone") == 0)
        {
            account->phone = atoll(rowsValues[i]);
        }
        else if (strcmp(columnNames[i], "date") == 0)
        {
            strcpy(account->date, rowsValues[i]);
        }
    }
    return 0;
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