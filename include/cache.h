#ifndef CACHE_H
#define CACHE_H

#include <time.h>

typedef struct CacheEntry {
    char domain[256];
    char ip[16];
    time_t timestamp;
} CacheEntry;

void init_cache(); 
int search_cache(const char *domain, char *ip);
void update_cache(const char *domain, const char *ip);
void remove_expired_cache_entries(int expiration_time);

#endif
