#include<stdint.h>
#include<stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

static sqlite3* db;

struct User {
    uint64_t id;
    char name[50];
    char password[64];
    bool Active;
    struct Account **accounts;
};

enum AccountType {
    AccountTypeFixed01,
    AccountTypeFixed02,
    AccountTypeFixed03,
    AccountTypeSaving,
    AccountTypeCurrent,
};

struct Country {
    char name[50];
    char code[4];
};

struct Account {
    uint64_t id;
    struct User *user;
    enum  AccountType type;
    double balance;
    struct Country *country;
    char phone[10];
    uint64_t date; ////// fixx
};

struct Transaction {
    uint64_t id;
    struct User *sender;
    struct User *receiver;
    double amount;
    uint64_t date; ////// fixx    
};

struct AccountTransfers {
    uint64_t id;
    struct User *reciever;
    struct Account *Transfered_acc;
    uint64_t date; ////// fixx
};


// authentication functions
void loginMenu(char a[50], char pass[50]);
void registration(char a[50], char pass[50]);
void registerMenu(char a[50], char pass[50]);
const char *getPassword(struct User u);

// system function
void createNewAcc(struct User u);
void mainMenu(struct User u);
void checkAllAccounts(struct User u);
void updateAccountInfo(struct User u);

// sql
int usercallback(void *data, int rowsCount, char **rowsValues, char **coulmnNames);
char* sql_connect();
char* sql_insert_user(char* username, char* password);
struct User sql_select_user(char* username);
char* sql_delete_user(struct User user);
char* sql_create_account(struct User user,char* type, double balance, char* country, char* phone);

// file
long file_size(const char* fileName);

void printUser(struct User user);
