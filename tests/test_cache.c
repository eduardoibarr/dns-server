#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../include/cache.h"

int main() {
    const char *domain = "cachedomain.com";
    const char *ip = "192.168.1.1";

    update_cache(domain, ip);
    char result_ip[16];
    assert(search_cache(domain, result_ip) == 1);
    assert(strcmp(result_ip, ip) == 0);

    assert(search_cache("nonexistent.com", result_ip) == 0);

    printf("test_cache passed\n");
    return 0;
}
