#include <string.h>
#include <time.h>
#include "../include/cache.h"

CacheEntry dns_cache[100];

int search_cache(const char *domain, char *ip) {
    for (int i = 0; i < 100; i++) {
        if (strcmp(dns_cache[i].domain, domain) == 0) {
            strcpy(ip, dns_cache[i].ip);
            return 1;
        }
    }
    return 0;
}

void update_cache(const char *domain, const char *ip) {
    for (int i = 0; i < 100; i++) {
        if (dns_cache[i].domain[0] == '\0') {
            strcpy(dns_cache[i].domain, domain);
            strcpy(dns_cache[i].ip, ip);
            dns_cache[i].timestamp = time(NULL);
            break;
        }
    }
}

void remove_expired_cache_entries(int expiration_time) {
    time_t now = time(NULL);
    for (int i = 0; i < 100; i++) {
        if (dns_cache[i].timestamp != 0 && difftime(now, dns_cache[i].timestamp) > expiration_time) {
            memset(&dns_cache[i], 0, sizeof(CacheEntry));
        }
    }
}
