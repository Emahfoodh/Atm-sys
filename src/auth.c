#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

void enableEcho();
void disableEcho();

void loginMenu(char a[50], char pass[50]) {
    system("clear");
    char* username = readString("\n\n\n\t\t\t\t   Bank Management System\n\t\t\t\t\t User Login:");
    strcpy(a, username);
    free(username);
    readPassword(pass);
}

void readPassword(char pass[50]) {
    disableEcho();
    char* password = readString("\n\n\n\n\n\t\t\t\tEnter the password: ");
    strcpy(pass, password);
    free(password);
    enableEcho();
}

void disableEcho() {
    struct termios oflags, nflags;
    tcgetattr(fileno(stdin), &oflags);
    nflags = oflags;
    nflags.c_lflag &= ~ECHO;
    nflags.c_lflag |= ECHONL;
    if (tcsetattr(fileno(stdin), TCSANOW, &nflags) != 0) {
        perror("tcsetattr");
        exit(1);
    }
}

void enableEcho() {
    struct termios oflags, nflags;
    tcgetattr(fileno(stdin), &oflags);
    nflags = oflags;
    nflags.c_lflag |= ECHO;
    if (tcsetattr(fileno(stdin), TCSANOW, &nflags) != 0) {
        perror("tcsetattr");
        exit(1);
    }
}

