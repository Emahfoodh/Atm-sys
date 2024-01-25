#include "header.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <float.h>
#include <ctype.h>

enum AccountType readAccountType();
char* chooseUpdateOption();
double TransectionAmount();

bool isValidUserName(char *username)
{
    for (int i = 0; i < strlen(username); i++)
    {
        if (!isalpha(username[i]) && username[i] != ' ')
        {
            return false;
        }
    }
    return true;
}

void registerUser() 
{
    struct User newUser;
    bool registRes;
    system("clear");
    while (1) {
        printf("\t\t\t===== Register =====\n");
        // Prompt and read the username
        char* username = readString("\nEnter username: ");
        strcpy(newUser.name, username);
        free(username); // Free the dynamically allocated memory
        if (!isValidUserName(newUser.name)) {
            printf("Account creation failed. Invalid username. Please enter a valid username.\n");
            continue;
        }

        // Prompt and read the password
        char password[50];
        readPassword(password);
        strcpy(newUser.password, password);
        registRes = sql_insert_user(&newUser);
        if (!registRes) {
            printf("Account creation failed. Please try again.\n");
        } else {
            break;
        }
    }
    success(newUser);
}

void createNewAcc(struct User* u)
{
    struct Account newAcc;
    bool creationResult;
    system("clear");
    do {
        printf("\t\t\t===== New Account =====\n");

        // Prompt and read the account type
        newAcc.type = readAccountType();

        newAcc.balance = readBalance("\nEnter amount to deposit: $");

        // Prompt and read the Date
        char* date = readDate();
        strcpy(newAcc.date, date);
        free(date); // Free the dynamically allocated memory

        // Prompt and read the username
        char* coun;
        do {
            coun = readString("\nEnter the country: ");
        } while (!isValidUserName(coun));

        strcpy(newAcc.country, coun);
        free(coun); // Free the dynamically allocated memory

        newAcc.phone = readPhoneNum("\nEnter the phone number:");

        creationResult = sql_create_account(*u, newAcc);

        if (!creationResult) {
            printf("Account creation failed. Please try again.\n");
        }
    } while (!creationResult);
    success(*u);
}


void updateAccountInfo(struct User* u)
{
    char input[50];
    system("clear");
    while(1) {
        printf("\t\t\t===== Update Account Info =====\n");
        printf("Enter the account ID you want to update or \\back to return: ");
        scanf("%s", input);
        if (strcmp(input, "\\back") == 0){
            mainOrExit(*u);
            return;
        }

        // Call sql_select_account to retrieve the account
        struct Account account = sql_select_account(input);

        if (account.user == NULL)
        {
            printf("Account not found.\n");
            continue;
        }
    
        if (account.user->id != u->id)
        {
            printf("The account does not belong to the user.\n");
            continue;
        }    

        char* to_update = chooseUpdateOption();
        char* newValue;
        if (to_update == "country") {
            newValue = readString("\nEnter updated country: ");
        } else {
            long long phoneNum = readPhoneNum("\nEnter updated phone number: ");
            
            // Convert the phone number to a string using sprintf
            char phoneStr[50];
            sprintf(phoneStr, "%lld", phoneNum);
            newValue = phoneStr;
        }
        
        if (!sql_update_account(account.id,to_update,newValue)) {
            printf("update account failed. Please try again.\n");
        } else {
            break;
        }
    }
    success(*u);
}

void checkAccountDetails(struct User* u) {
    char input[50];
    system("clear");
    while(1) {
        printf("\t\t\t===== Check Account Details =====\n");
        printf("Enter the account ID you want to check or \\back to return: ");
        scanf("%s", input);
        if (strcmp(input, "\\back") == 0) {
            mainOrExit(*u);
            return;
        }

        // Call sql_select_account to retrieve the account
        struct Account account = sql_select_account(input);

        if (account.user == NULL)
        {
            printf("Account not found.\n");
            continue;
        }
    
        if (account.user->id != u->id)
        {
            printf("The account does not belong to the user.\n");
            continue;
        }
        // Print account details
        printf("Account ID: %ld\n", account.id);
        printf("Country: %s\n", account.country);
        printf("Phone number: %lld\n", account.phone);
        printf("Amount deposited: $%.2lf\n", account.balance);
        printf("Account type: %s\n", AccountTypeStrings[account.type]);
        printInterestAmount(account);
    }
}


void checkAllAccounts(struct User* u) {
    sql_print_owned_account_ids(u->id);
    success(*u);
}

void makeTransaction(struct User* u) {
    char input[50];
    system("clear");
    while(1) {
        printf("\t\t\t===== Make Transaction =====\n");
        printf("Enter the account ID for the transaction or \\back to return: ");
        scanf("%s", input);
        if (strcmp(input, "\\back") == 0) {
            mainOrExit(*u);
            return;
        }

        // Call sql_select_account to retrieve the account
        struct Account account = sql_select_account(input);

        if (account.user == NULL)
        {
            printf("Account not found.\n");
            continue;
        }
    
        if (account.user->id != u->id)
        {
            printf("The account does not belong to the user.\n");
            continue;
        }

        // Check if the account type disallows transactions
        if (account.type == AccountTypeFixed01 || account.type == AccountTypeFixed02 || account.type == AccountTypeFixed03)
        {
            printf("Transactions are only allowed for accounts of type saving and current.\n");
            continue;
        }

        double amount = TransectionAmount();
        if (account.balance + amount < 0) {
            printf("Transaction failed. Insufficient funds.\n");
            continue;
        }
        double newValue = account.balance + amount;
        char newValue_str[20];
        sprintf(newValue_str, "%.2lf", newValue);

        if (!sql_update_account(account.id,"balance",newValue_str)) {
            printf("Transaction failed. Please try again.\n");
        } else {
            break;
        }
    }
    success(*u);
}

void removeAccount(struct User* u) {
    char input[50];
    system("clear");
    while(1) {
        printf("\t\t\t===== Remove Account =====\n");
        printf("Enter the account ID you want to remove or \\back to return: ");
        scanf("%s", input);
        if (strcmp(input, "\\back") == 0) {
            mainOrExit(*u);
            return;
        }

        // Call sql_select_account to retrieve the account
        struct Account account = sql_select_account(input);

        if (account.user == NULL)
        {
            printf("Account not found.\n");
            continue;
        }
    
        if (account.user->id != u->id)
        {
            printf("The account does not belong to the user.\n");
            continue;
        }

        if (!sql_remove_account(input)) {
            printf("Remove account failed. Please try again.\n");
        } else {
            break;
        }
    }
    success(*u);
}

void transferOwnership(struct User* u) {
    char input[50];
    system("clear");
    while(1) {
        printf("\t\t\t===== Transfer Ownership =====\n");
        printf("Enter the account ID you want to transfer or \\back to return: ");
        scanf("%s", input);
        if (strcmp(input, "\\back") == 0) {
            mainOrExit(*u);
            return;
        }

        // Call sql_select_account to retrieve the account
        struct Account account = sql_select_account(input);

        if (account.user == NULL)
        {
            printf("Account not found.\n");
            continue;
        }
    
        if (account.user->id != u->id)
        {
            printf("The account does not belong to the user.\n");
            continue;
        }
        
        char* new_owner_id = readString("Enter the id of the new owner: ");
        printf("%s",new_owner_id);

        // Call sql_select_account to retrieve the account
        struct Account newOwnerAcc = sql_select_account(new_owner_id);

        if (newOwnerAcc.user == NULL)
        {
            printf("Account not found.\n");
            continue;
        }
        if (!sql_update_account(account.id,"user_id",new_owner_id)) {
            printf("Transfer account failed.\n");
        } else {
            break;
        }
    }
    success(*u);
}

void success(struct User u)
{
    printf("\n✔ Success!\n\n");
    mainOrExit(u);
}

void mainOrExit(struct User u) {
    int option;
    while (1) {
        printf("Enter 1 to go to the main menu and 0 to exit!\n");
        scanf("%d", &option);
        flushInputBuffer();
        
        if (option == 1) {
            mainMenu(u);
            break; // Exit the loop after mainMenu() is executed
        }
        else if (option == 0) {
            exit(1);
        }
        else {
            printf("Insert a valid operation!\n");
        }
    }
}

char* readDate() {
    struct Date {
        int day;
        int month;
        int year;
    };
    
    struct Date date;
    
    while (1) {
        printf("Enter date (dd/mm/yyyy): ");
        if (scanf("%d/%d/%d", &date.day, &date.month, &date.year) == 3) {
            // Check if the input values are within valid ranges
            if (date.day >= 1 && date.day <= 31 &&
                date.month >= 1 && date.month <= 12 &&
                date.year >= 1900 && date.year <= 2100) {
                break;  // Valid input, break out of the loop
            } else {
                printf("Invalid date. Please try again.\n");
            }
        } else {
            printf("Invalid input format. Please use dd/mm/yyyy format.\n");
            flushInputBuffer();
        }
    }
    
    // Convert the date to the desired format "%Y/%m/%d"
    char formattedDate[11];
    sprintf(formattedDate, "%04d/%02d/%02d", date.year, date.month, date.day);
    
    return strdup(formattedDate);
}


void getCurrentDate(char date[11]) {
    time_t current_time = time(NULL);
    strftime(date, 11, "%Y/%m/%d", localtime(&current_time));
}

double readBalance(char* prompt) {
    double value;
    int result;

    while (1) {
        printf("%s", prompt);
        result = scanf("%lf", &value);
        flushInputBuffer();

        // Check if the input was successfully converted to a double
        if (result == 1) {
            // Check if the value is a positive number with at most two decimal places
            if (value <= 0) {
                printf("Invalid input. Please enter a valid positive number.\n");
            } else if (value > 9999999) {
                printf("Amount is too large.\n");
            // } else if (hasMoreThanTwoDecimalPlaces(value)) {
            //     printf("Invalid balance. Please enter a valid number with at most two decimal places.\n");
            } else {
                break;  // Valid input, break out of the loop
            }
        } else {
            printf("Invalid input. Please enter a valid number.\n");
        }
    }

    return value;
}


char* readString(char* print) 
{
    char str[50];

    while (1) {
        printf("%s", print);
        if (scanf("%49s", str) == 1) {
            // Input was successful
            // Clear the input buffer to remove any remaining characters
            return strdup(str); // Return a dynamically allocated copy of the string
        } else {
            // Input was not successful
            printf("Invalid input. Please try again: ");
            flushInputBuffer();  // Clear the input buffer
        }
    } // Loop until a valid input is provided
}

int readInteger(char* prompt) {
    int value;
    int result;

    while (1) {
        printf("%s", prompt);
        result = scanf("%d", &value);
        flushInputBuffer();

        // Check if the input was successfully converted to an integer
        if (result == 1) {
            // Check if the value is a positive number
            if (value <= 0) {
                printf("Invalid input. Please enter a valid positive number.\n");
            } else {
                break;  // Valid input, break out of the loop
            }
        } else {
            printf("Invalid input. Please enter a valid integer.\n");
        }
    }

    return value;
}

long long readPhoneNum(char* prompt) {
    long long phoneNumber;

    while (1) {
        printf("%s ", prompt);
        if (scanf("%lld", &phoneNumber) == 1) {
            if (phoneNumber >= 1000000LL && phoneNumber <= 9999999999LL) {
                return phoneNumber;
            } else {
                printf("Invalid input. Phone number must be between 7 and 10 digits.\n");
            }
        } else {
            printf("Invalid input. Please enter a valid phone number.\n");
            flushInputBuffer();  // Clear the input buffer
        }
    }
    return phoneNumber;
}

enum AccountType readAccountType() {
    int choice;

    printf("Select Account Type:\n");
    printf("1. Fixed01\n");
    printf("2. Fixed02\n");
    printf("3. Fixed03\n");
    printf("4. Saving\n");
    printf("5. Current\n");

    printf("Enter your choice: ");

    while (1) {
        scanf("%d", &choice);
        flushInputBuffer();

        switch (choice) {
            case 1:
                return AccountTypeFixed01;
            case 2:
                return AccountTypeFixed02;
            case 3:
                return AccountTypeFixed03;
            case 4:
                return AccountTypeSaving;
            case 5:
                return AccountTypeCurrent;
            default:
                printf("Invalid account type. Please enter a valid choice.\n");
        }
    }
}

char* chooseUpdateOption() {
    int choice;

    printf("Select an option:\n");
    printf("1. Country\n");
    printf("2. Phone Number\n");

    printf("Enter your choice: ");

    while (1) {
        scanf("%d", &choice);
        flushInputBuffer();

        switch (choice) {
            case 1:
                return "country";
            case 2:
                return "phone";
            default:
                printf("Invalid option. Please enter a valid choice.\n");
        }
    }
}


double TransectionAmount() {
    int choice;

    printf("Select an option:\n");
    printf("1. Deposit\n");
    printf("2. Withdraw\n");

    printf("Enter your choice: ");

    while (1) {
        scanf("%d", &choice);
        flushInputBuffer();

        if (choice == 1 || choice == 2) {
            double amount = readBalance(choice == 1 ? "Enter the amount to deposit: " : "Enter the amount to withdraw: ");
            return choice == 1 ? amount : -amount;
        } else {
            printf("Invalid option. Please enter a valid choice.\n");
        }
    }
}

void printInterestAmount(struct Account acc) {
    double interestAmount = 0.0;

    if (acc.type == AccountTypeSaving)
    {
        // Calculate monthly interest
        interestAmount = (acc.balance * 0.07 / 12);
        printf("You will gain $%.2lf of interest on day 10 of every month.\n", interestAmount);
    }
    else if (acc.type == AccountTypeFixed01)
    {
        // Calculate interest for one year from account creation date
        int oneYearFromDeposit = atoi(acc.date) + 1;
        interestAmount = (acc.balance * 0.04);
        printf("You will gain $%.2lf interest on %02d/%02d/%04d (one year from account creation).\n", interestAmount, acc.date[3], acc.date[4], oneYearFromDeposit);
    }
    else if (acc.type == AccountTypeFixed02)
    {
        // Calculate interest for two years from account creation date
        int twoYearsFromDeposit = atoi(acc.date) + 2;
        interestAmount = (acc.balance * 0.05 * 2);
        printf("You will gain $%.2lf interest on %02d/%02d/%04d (two years from account creation).\n", interestAmount, acc.date[3], acc.date[4], twoYearsFromDeposit);
    }
    else if (acc.type == AccountTypeFixed03)
    {
        // Calculate interest for three years from account creation date
        int threeYearsFromDeposit = atoi(acc.date) + 3;
        interestAmount = (acc.balance * 0.08 * 3);
        printf("You will gain $%.2lf interest on %02d/%02d/%04d (three years from account creation).\n", interestAmount, acc.date[3], acc.date[4], threeYearsFromDeposit);
    }
    else
    {
        printf("You will not get interests because the account is of type current.\n");
    }
}