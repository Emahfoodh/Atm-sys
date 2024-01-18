#include "header.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
}

char* readString(char* print) {
    char str[50];
    printf("%s", print);
    fflush(stdout); // Flushing the output to ensure the prompt is displayed

    do {
        if (scanf("%49s", str) == 1) {
            // Input was successful
            // Clear the input buffer to remove any remaining characters
            int c;
            while ((c = getchar()) != '\n' && c != EOF);

            return strdup(str); // Return a dynamically allocated copy of the string
        } else {
            // Input was not successful
            printf("Invalid input. Please try again: ");
            fflush(stdout);
        }
    } while (1); // Loop until a valid input is provided
}