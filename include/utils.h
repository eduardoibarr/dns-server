#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

void log_error(const char *msg);
void log_info(const char *msg);

char *trim_whitespace(char *str);
void to_lowercase(char *str);

#endif
