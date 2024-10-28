#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "../include/resolver.h"

char *resolve_domain(const char *domain_name) {
    struct addrinfo hints, *res;
    int ret;
    char *ip_string = malloc(16);

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4

    ret = getaddrinfo(domain_name, NULL, &hints, &res);
    if (ret != 0) {
        printf("getaddrinfo failed: %s\n", gai_strerror(ret));
        free(ip_string);
        return NULL;
    }

    struct sockaddr_in *addr = (struct sockaddr_in *)res->ai_addr;
    inet_ntop(AF_INET, &(addr->sin_addr), ip_string, 16);

    freeaddrinfo(res);
    return ip_string;
}
