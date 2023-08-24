#include "header.h"
#include <stdio.h>

long file_size(const char* fileName) {
    FILE *f = fopen(fileName,"r");
    if (f == NULL) {
        return -1;
    }
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fclose(f);
    return size;
}