#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "../include/dns_server.h"
#include "../include/cache.h"
#include "../include/blocklist.h"
#include "../include/redirect.h"

#define BUFFER_SIZE 512

struct sockaddr_in mock_client;
int mock_socket;

void setup_mock_environment() {
    mock_socket = socket(AF_INET, SOCK_DGRAM, 0);
    mock_client.sin_family = AF_INET;
    mock_client.sin_addr.s_addr = inet_addr("127.0.0.1");
    mock_client.sin_port = htons(5353);

    assert(load_blocked_domains("data/test_blocked_domains.txt") == 1);
    assert(load_redirects("data/test_redirects.txt") == 1);
}

void teardown_mock_environment() {
    free_blocked_domains();
    free_redirects();
    close(mock_socket);
}

ssize_t create_dns_query(const char *domain, char *buffer) {
    struct DNSHeader *dns_header = (struct DNSHeader *)buffer;
    dns_header->id = htons(0x1234);
    dns_header->flags = htons(0x0100);
    dns_header->qd_count = htons(1);
    dns_header->an_count = 0;
    dns_header->ns_count = 0;
    dns_header->ar_count = 0;

    size_t offset = sizeof(struct DNSHeader);

    const char delim[2] = ".";
    char *token;
    char domain_copy[256];
    strncpy(domain_copy, domain, sizeof(domain_copy));
    token = strtok(domain_copy, delim);

    while (token != NULL) {
        size_t len = strlen(token);
        buffer[offset++] = (unsigned char)len;
        memcpy(buffer + offset, token, len);
        offset += len;
        token = strtok(NULL, delim);
    }
    buffer[offset++] = 0;

    uint16_t qtype = htons(1);
    uint16_t qclass = htons(1);
    memcpy(buffer + offset, &qtype, 2);
    offset += 2;
    memcpy(buffer + offset, &qclass, 2);
    offset += 2;

    return offset;
}

int main() {
    setup_mock_environment();

    char buffer[BUFFER_SIZE];
    ssize_t buffer_len;

    buffer_len = create_dns_query("badexample.com", buffer);
    handle_dns_request(buffer, buffer_len, &mock_client, mock_socket);
    printf("Blocked domain test passed\n");

    buffer_len = create_dns_query("example.com", buffer);
    handle_dns_request(buffer, buffer_len, &mock_client, mock_socket);
    printf("Redirect domain test passed\n");

    update_cache("cachedomain.com", "192.168.1.1");
    buffer_len = create_dns_query("cachedomain.com", buffer);
    handle_dns_request(buffer, buffer_len, &mock_client, mock_socket);
    printf("Cache lookup test passed\n");

    buffer_len = create_dns_query("localhost", buffer);
    handle_dns_request(buffer, buffer_len, &mock_client, mock_socket);
    printf("Resolve external domain test passed\n");

    teardown_mock_environment();
    printf("test_dns_server passed\n");

    return 0;
}
