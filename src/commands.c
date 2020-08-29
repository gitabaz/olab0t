#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "message.h"
#include "commands.h"
#include "constants.h"
#include "handle_msg.h"

int parse_command(message* msg, int sock_fd) {

    char cmd[BUF_SIZE], response[BUF_SIZE];
    int bytes_sent;

    sscanf(msg->text, "%*c%s", cmd);

    if (strcmp(cmd, "hello") == 0) {
        sprintf(response, "Hi @%s!\n", msg->display_name);
    } else if (strcmp(cmd, "github") == 0) {
        sprintf(response, "@%s https://github.com/gitabaz\n", msg->display_name);
    } else if (strcmp(cmd, "olab0t") == 0) {
        sprintf(response, "@%s https://github.com/gitabaz/olab0t\n", msg->display_name);
    } else if (strcmp(cmd, "poggers") == 0) {
        sprintf(response, "PogChamp Poggers PogChamp Poggers\n");
    } else if (strcmp(cmd, "roulette") == 0) {
        int roll = 1 + rand()/((RAND_MAX + 1u)/6);
        sprintf(response, "You rolled %d", roll);
    } else {
        sprintf(response, "@%s Command not recognized!\n", msg->display_name);
    }

    bytes_sent = send_msg(sock_fd, msg->channel, response);

    return bytes_sent;
    
}
