#include "header.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <float.h>

void success(struct User u)
{
    int option;
    printf("\n✔ Success!\n\n");
invalid:
    flushInputBuffer();
    printf("Enter 1 to go to the main menu and 0 to exit!\n");
    scanf("%d", &option);
    system("clear");
    if (option == 1)
    {
        mainMenu(u);
    }
    else if (option == 0)
    {
        exit(1);
    }
    else
    {
        printf("Insert a valid operation!\n");
        goto invalid;
    }
}

void registerUser() 
{
    struct User newUser;
    do {
    system("clear");
    // Prompt and read the username
    char* username = readString("\n\n\n\n\n\t\t\t\tEnter username: ");
    strcpy(newUser.name, username);
    free(username); // Free the dynamically allocated memory

    // Prompt and read the password
    char password[50];
    readPassword(password);
    strcpy(newUser.password, password);
    
    } while (!sql_insert_user(newUser));
    printf("User registered successfully!\n");
    sleep(2);
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


void createNewAcc(struct User* u)
{
    struct Account newAcc;
    bool creationResult;
    system("clear");
    do {
        printf("\t\t\t===== New record =====\n");

        // Prompt and read the account type
        newAcc.type = readAccountType();
        printf("Selected account type: %d\n", newAcc.type);

        newAcc.balance = readBalance("\nEnter amount to deposit: $");
        printf("Amount deposited: $%.2f\n", newAcc.balance);

        // Prompt and read the Date
        char* date = readDate();
        strcpy(newAcc.date, date);
        free(date); // Free the dynamically allocated memory

        // Prompt and read the username
        char* coun = readString("\nEnter the country: ");
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
            } else if (value > DBL_MAX) {
                printf("Deposit amount is too large.\n");
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

long long readPhoneNum(char* prompt) {
    long long phoneNumber;

    while (1) {
        printf("%s ", prompt);
        if (scanf("%lld", &phoneNumber) == 1) {
            if (phoneNumber >= 1000000LL && phoneNumber <= 999999999999999LL) {
                return phoneNumber;
            } else {
                printf("Invalid input. Phone number must be between 7 and 15 digits.\n");
            }
        } else {
            printf("Invalid input. Please enter a valid phone number.\n");
            flushInputBuffer();  // Clear the input buffer
        }
    }
    return phoneNumber;
}
