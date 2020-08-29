#ifndef PARSE_CONFIG_H
#define PARSE_CONFIG_H

#include <stdbool.h>

typedef struct bot_config {
    char bot_username[101];
    char oauth_token[101];
    char** channel_list;
    int capacity;
    int size;
} bot_config;

void free_bc(bot_config* bc);
bot_config* bot_config_from_file(const char* const filename);

#endif
