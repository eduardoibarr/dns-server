#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "../include/dns_server.h"
#include "../include/cache.h"
#include "../include/blocklist.h"
#include "../include/redirect.h"
#include "../include/resolver.h"

#define BLOCKLIST_FILE "data/blocked_domains.txt"
#define REDIRECTS_FILE "data/redirects.txt"

int parse_domain_name(char *buffer, ssize_t buffer_len, size_t offset, char *domain_name, size_t domain_name_size) {
    size_t name_pos = 0;

    while (offset < buffer_len) {
        uint8_t len = buffer[offset++];
        if (len == 0) {
            if (name_pos > 0 && name_pos < domain_name_size) {
                domain_name[name_pos - 1] = '\0';
            } else if (name_pos == 0 && domain_name_size > 0) {
                domain_name[0] = '\0';
            }
            return offset;
        }
        if (len & 0xC0) {
            printf("Error: Domain name compression not supported\n");
            return -1;
        }
        if (offset + len > buffer_len || name_pos + len + 1 >= domain_name_size) {
            printf("Error: Invalid or too long domain name\n");
            return -1;
        }
        memcpy(domain_name + name_pos, buffer + offset, len);
        name_pos += len;
        domain_name[name_pos++] = '.';
        offset += len;
    }
    return -1;
}

void send_dns_response(char *request_buffer, ssize_t request_len, size_t question_len, int sockfd, struct sockaddr_in *client_addr, const char *ip_string) {
    char response_buffer[BUFFER_SIZE];
    size_t response_len = 0;

    memcpy(response_buffer, request_buffer, 12);
    struct DNSHeader *dns_header = (struct DNSHeader *)response_buffer;
    dns_header->flags = htons(0x8180);
    dns_header->qd_count = htons(1);
    dns_header->an_count = htons(1);
    dns_header->ns_count = htons(0);
    dns_header->ar_count = htons(0);
    response_len += 12;

    memcpy(response_buffer + response_len, request_buffer + 12, question_len);
    response_len += question_len;

    uint16_t name_ptr = htons(0xC00C);
    memcpy(response_buffer + response_len, &name_ptr, 2);
    response_len += 2;

    uint16_t type = htons(1);
    memcpy(response_buffer + response_len, &type, 2);
    response_len += 2;

    uint16_t class = htons(1);
    memcpy(response_buffer + response_len, &class, 2);
    response_len += 2;

    uint32_t ttl = htonl(DEFAULT_TTL);
    memcpy(response_buffer + response_len, &ttl, 4);
    response_len += 4;

    uint16_t data_len = htons(4);
    memcpy(response_buffer + response_len, &data_len, 2);
    response_len += 2;

    struct in_addr ip_addr;
    if (inet_pton(AF_INET, ip_string, &ip_addr) != 1) {
        printf("Error: Invalid IP address %s\n", ip_string);
        return;
    }
    memcpy(response_buffer + response_len, &ip_addr.s_addr, 4);
    response_len += 4;

    sendto(sockfd, response_buffer, response_len, 0, (struct sockaddr *)client_addr, sizeof(*client_addr));
    printf("DNS response sent to %s with IP %s\n", inet_ntoa(client_addr->sin_addr), ip_string);
}

void handle_dns_request(char *buffer, ssize_t buffer_len, struct sockaddr_in *client_addr, int sockfd) {
    if (buffer_len < sizeof(struct DNSHeader)) {
        printf("Error: Invalid DNS request\n");
        return;
    }

    char domain_name[256];
    size_t offset = sizeof(struct DNSHeader);
    int offset_after_qname = parse_domain_name(buffer, buffer_len, offset, domain_name, sizeof(domain_name));
    if (offset_after_qname < 0) {
        printf("Error: Failed to parse domain name\n");
        return;
    }

    offset_after_qname += 4;

    size_t question_len = offset_after_qname - sizeof(struct DNSHeader);

    printf("Request for Domain: %s\n", domain_name);

    if (check_blocked_domain(domain_name)) {
        printf("Blocked domain: %s\n", domain_name);
        return;
    }

    char redirect_ip[16];
    if (check_redirect(domain_name, redirect_ip)) {
        printf("Redirecting %s to %s\n", domain_name, redirect_ip);
        send_dns_response(buffer, buffer_len, question_len, sockfd, client_addr, redirect_ip);
        return;
    }

    char cached_ip[16];
    if (search_cache(domain_name, cached_ip)) {
        printf("Cache hit for %s: %s\n", domain_name, cached_ip);
        send_dns_response(buffer, buffer_len, question_len, sockfd, client_addr, cached_ip);
        return;
    }

    printf("Resolving domain: %s\n", domain_name);
    char *resolved_ip = resolve_domain(domain_name);
    if (resolved_ip != NULL) {
        printf("Resolved %s to %s, caching result\n", domain_name, resolved_ip);
        update_cache(domain_name, resolved_ip);
        send_dns_response(buffer, buffer_len, question_len, sockfd, client_addr, resolved_ip);
        free(resolved_ip);
    } else {
        printf("Error: Failed to resolve domain %s\n", domain_name);
    }
}

void start_server() {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    printf("Loading blocklist and redirect files...\n");
    if (!load_blocked_domains(BLOCKLIST_FILE)) {
        printf("Warning: Could not load blocklist file %s\n", BLOCKLIST_FILE);
    }
    if (!load_redirects(REDIRECTS_FILE)) {
        printf("Warning: Could not load redirects file %s\n", REDIRECTS_FILE);
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Error: Socket creation failed");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error: Socket bind failed");
        close(sockfd);
        exit(1);
    }

    printf("DNS server is running on port %d\n", PORT);

    while (1) {
        ssize_t recv_len = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_len);
        if (recv_len < 0) {
            perror("recvfrom failed");
            continue;
        }
        handle_dns_request(buffer, recv_len, &client_addr, sockfd);
    }

    close(sockfd);
    free_blocked_domains();
    free_redirects();
    printf("Server shutdown\n");
}
