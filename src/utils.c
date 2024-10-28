#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "../include/utils.h"

void log_error(const char *msg) {
    fprintf(stderr, "Error: %s\n", msg);
}

void log_info(const char *msg) {
    printf("Info: %s\n", msg);
}

char *trim_whitespace(char *str) {
    char *end;

    while (isspace((unsigned char)*str)) str++;

    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    *(end + 1) = '\0';

    return str;
}

void to_lowercase(char *str) {
    for (; *str; str++) *str = tolower((unsigned char)*str);
}
