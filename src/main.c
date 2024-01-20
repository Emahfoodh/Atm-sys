#include "header.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void mainMenu(struct User u)
{
    int option;
    system("clear");
    printf("\n\n\t\t======= ATM =======\n\n");
    printf("\n\t\t-->> Feel free to choose one of the options below <<--\n");
    printf("\n\t\t[1]- Create a new account\n");
    printf("\n\t\t[2]- Update account information\n");
    printf("\n\t\t[3]- Check accounts\n");
    printf("\n\t\t[4]- Check list of owned account\n");
    printf("\n\t\t[5]- Make Transaction\n");
    printf("\n\t\t[6]- Remove existing account\n");
    printf("\n\t\t[7]- Transfer ownership\n");
    printf("\n\t\t[8]- Exit\n");
    scanf("%d", &option);
    flushInputBuffer();

    switch (option)
    {
    case 1:
        createNewAcc(&u);
        break;
    case 2:
        // student TODO : add your **Update account information** function
        updateAccountInfo(&u);
        // here
        break;
    case 3:
        // student TODO : add your **Check the details of existing accounts** function
        checkAccountDetails(&u);
        // here
        break;
    case 4:
        checkAllAccounts(&u);
        break;
    case 5:
        // student TODO : add your **Make transaction** function
        makeTransaction(&u);
        break;
    case 6:
        // student TODO : add your **Remove existing account** function
        removeAccount(&u);
        break;
    case 7:
        // student TODO : add your **Transfer owner** function
        transferOwnership(&u);
        break;
    case 8:
        exit(1);
        break;
    default:
        printf("Invalid operation!\n");
    }
};

void startMenu(struct User *u)
{
    int r = 0;
    int option;
    system("clear");
    printf("\n\n\t\t======= ATM =======\n");
    printf("\n\t\t-->> Feel free to login / register :\n");
    printf("\n\t\t[1]- login\n");
    printf("\n\t\t[2]- register\n");
    printf("\n\t\t[3]- exit\n");
    while (!r)
    {
        scanf("%d", &option);
        flushInputBuffer();
        switch (option)
        {
        case 1:
            loginMenu(u);
            struct User user_selected = sql_select_user(u->name);
            if (strcmp(u->password,user_selected.password) == 0)
            {
                (*u) = user_selected;
                printf("\n\nPassword Match!\n");
            }
            else
            {
                printf("\nWrong password!! or User Name\n");
                exit(1);
            }
            r = 1;
            break;
        case 2:
            // student TODO : add your **Registration** function
            registerUser();
            r = 1;
            break;
        case 3:
            system("clear");
            exit(1);
            break;
        default:
            printf("Insert a valid operation!\n");
        }
    }
};

int main() {
    struct User u;
    sql_connect();
    startMenu(&u);
    mainMenu(u);

    sqlite3_close(db);
    return EXIT_SUCCESS;
}

// This function is used to flush the input buffer
void flushInputBuffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {} // Keep reading characters until a newline or end-of-file
}