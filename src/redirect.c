#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/redirect.h"

Redirect *redirects = NULL;
int redirects_count = 0;

int load_redirects(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Could not open redirects file");
        return 0;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;
        char *comma = strchr(line, ',');
        if (comma == NULL) {
            fprintf(stderr, "Invalid redirect format in file\n");
            continue;
        }

        *comma = '\0';
        char *domain = line;
        char *ip = comma + 1;

        Redirect *temp = realloc(redirects, sizeof(Redirect) * (redirects_count + 1));
        if (temp == NULL) {
            perror("Memory allocation error");
            free_redirects();
            fclose(file);
            return 0;
        }
        redirects = temp;
        strncpy(redirects[redirects_count].domain, domain, sizeof(redirects[redirects_count].domain) - 1);
        strncpy(redirects[redirects_count].redirect_ip, ip, sizeof(redirects[redirects_count].redirect_ip) - 1);
        redirects_count++;
    }

    fclose(file);
    return 1;
}

int check_redirect(const char *domain, char *redirect_ip) {
    for (int i = 0; i < redirects_count; i++) {
        if (strcmp(domain, redirects[i].domain) == 0) {
            strcpy(redirect_ip, redirects[i].redirect_ip);
            return 1;
        }
    }
    return 0;
}

void free_redirects() {
    free(redirects);
}