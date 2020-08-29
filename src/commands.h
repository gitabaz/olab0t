#ifndef COMMANDS_H
#define COMMANDS_H

#include "message.h"

int parse_command(message* msg, int sock_fd);

#endif
