#include<stdint.h>
#include<stdbool.h>
#include <sqlite3.h>
#include <time.h>

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
    NumAccountTypes
};

// struct Date {
//     int day;
//     int month;
//     int year;
// };

// struct Country {
//     char name[50];
//     char code[4];
// };

struct Account {
    uint64_t id;
    struct User *user;
    enum  AccountType type;
    double balance;
    // struct Country *country;
    char country[50];
    long long phone;
    char date[11];
};

struct Transaction {
    uint64_t id;
    struct User *sender;
    struct User *receiver;
    double amount;
    time_t date; 
};

struct AccountTransfers {
    uint64_t id;
    struct User *reciever;
    struct Account *Transfered_acc;
    uint64_t date; ////// fixx
};


// authentication functions
void loginMenu(char a[50], char pass[50]);
void readPassword(char pass[50]);
// void registration(char a[50], char pass[50]);
// void registerMenu(char a[50], char pass[50]);
// const char *getPassword(struct User u);

// system function
void registerUser();
void createNewAcc(struct User* u);
void mainMenu(struct User u);
// void checkAllAccounts(struct User u);
// void updateAccountInfo(struct User u);

// sql
void sql_connect();
bool sql_insert_user(struct User newUser);
struct User sql_select_user(char* username);
bool sql_create_account(struct User user, struct Account acc);
char* sql_delete_user(struct User user);

// file
long file_size(const char* fileName);

// void printUser(struct User user);

char* readString(char* print);
double readBalance(char* prompt);
long long readPhoneNum(char* prompt);
char* readDate();


void flushInputBuffer();
void getCurrentDate(char date[11]);
