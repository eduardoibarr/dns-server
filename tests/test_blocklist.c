#include <assert.h>
#include <stdio.h>
#include "../include/blocklist.h"

int main() {
    assert(load_blocked_domains("./data/test_blocked_domains.txt") == 1);

    assert(check_blocked_domain("badexample.com") == 1);
    assert(check_blocked_domain("malware.com") == 1);
    assert(check_blocked_domain("safe.com") == 0);

    free_blocked_domains();

    printf("test_blocklist passed\n");
    return 0;
}
