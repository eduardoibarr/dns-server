#ifndef REDIRECT_H
#define REDIRECT_H

typedef struct Redirect {
    char domain[256];
    char redirect_ip[16];
} Redirect;

int load_redirects(const char *filename);
int check_redirect(const char *domain, char *redirect_ip);
void free_redirects();

#endif
