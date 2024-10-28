#ifndef DNS_SERVER_H
#define DNS_SERVER_H

#include <stdint.h>
#include <stddef.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 53
#define BUFFER_SIZE 512
#define CACHE_SIZE 100
#define DEFAULT_TTL 300

#pragma pack(push, 1)
struct DNSHeader {
    uint16_t id;
    uint16_t flags;
    uint16_t qd_count;
    uint16_t an_count;
    uint16_t ns_count;
    uint16_t ar_count;
};
#pragma pack(pop)

int parse_domain_name(char *buffer, ssize_t buffer_len, size_t offset, char *domain_name, size_t domain_name_size);
void send_dns_response(char *request_buffer, ssize_t request_len, size_t question_len, int sockfd, struct sockaddr_in *client_addr, const char *ip_string);

void handle_dns_request(char *buffer, ssize_t buffer_len, struct sockaddr_in *client_addr, int sockfd);
void start_server();

#endif
