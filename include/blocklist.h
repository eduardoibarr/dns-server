#ifndef BLOCKLIST_H
#define BLOCKLIST_H

extern char **blocked_domains;
extern int blocked_domains_count;

int load_blocked_domains(const char *filename);
int check_blocked_domain(const char *domain);
void free_blocked_domains();

#endif
