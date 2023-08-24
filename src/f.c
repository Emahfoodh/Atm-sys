#include "header.h"
#include <stdio.h>

long file_size(const char* fileName) {
    FILE *f = fopen(fileName,"r");
    if (f == NULL) {
        return -1;
    }
    fseek(f, 0, SEEK_END);
    fclose(f);
    return ftell(f);
}