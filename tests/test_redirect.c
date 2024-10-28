#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../include/redirect.h"

int main() {
    assert(load_redirects("./data/test_redirects.txt") == 1);

    char redirect_ip[16];
    assert(check_redirect("example.com", redirect_ip) == 1);
    assert(strcmp(redirect_ip, "127.0.0.1") == 0);

    assert(check_redirect("test.com", redirect_ip) == 1);
    assert(strcmp(redirect_ip, "192.168.0.1") == 0);

    assert(check_redirect("nonexistent.com", redirect_ip) == 0);

    free_redirects();

    printf("test_redirect passed\n");
    return 0;
}
