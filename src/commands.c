#include <string.h>

#include "handle_msg.h"

int parse_command(int sock_fd, char* chan, char* msg, char* usr) {

    char cmd[51], response[100];
    int bytes_sent;

    sscanf(msg, "%*c%s", cmd );

    if (strcmp(cmd, "hello") == 0) {
        sprintf(response, "Hi @%s!\n", usr);
    } else if (strcmp(cmd, "github") == 0) {
        sprintf(response, "@%s https://github.com/gitabaz\n", usr);
    } else if (strcmp(cmd, "olab0t") == 0) {
        sprintf(response, "@%s https://github.com/gitabaz/olab0t\n", usr);
    } else {
        sprintf(response, "@%s Command not recognized!\n", usr);
    }
    bytes_sent = send_msg(sock_fd, chan, response);

    return bytes_sent;
    
}
