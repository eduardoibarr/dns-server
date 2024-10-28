#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/resolver.h"

int main() {
    char *resolved_ip = resolve_domain("google.com");
    assert(resolved_ip != NULL);
    printf("Resolved google.com to %s\n", resolved_ip);
    free(resolved_ip);

    resolved_ip = resolve_domain("nonexistentdomain.example");
    assert(resolved_ip == NULL);

    printf("test_resolver passed\n");
    return 0;
}
