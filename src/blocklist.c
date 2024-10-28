#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/blocklist.h"

char **blocked_domains = NULL;
int blocked_domains_count = 0;

int load_blocked_domains(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Could not open blocklist file");
        return 0;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;
        char **temp = realloc(blocked_domains, sizeof(char *) * (blocked_domains_count + 1));
        if (temp == NULL) {
            perror("Memory allocation error");
            free_blocked_domains();
            fclose(file);
            return 0;
        }
        blocked_domains = temp;
        blocked_domains[blocked_domains_count] = strdup(line);
        blocked_domains_count++;
    }

    fclose(file);
    return 1;
}

int check_blocked_domain(const char *domain) {
    for (int i = 0; i < blocked_domains_count; i++) {
        if (strcmp(domain, blocked_domains[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

void free_blocked_domains() {
    for (int i = 0; i < blocked_domains_count; i++) {
        free(blocked_domains[i]);
    }
    free(blocked_domains);
}
