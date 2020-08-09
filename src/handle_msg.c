#include "handle_msg.h"
#include "commands.h"

void parse_msg(char* buf, char* ping, char* chan, char* usr, char* msg, int sock_fd) {

    if(sscanf(buf, "%[^:]", ping) == 1) {
        if(reply_PONG(sock_fd) == 0) {
            puts("Error: could not send PONG");
            exit(EXIT_FAILURE);
        }
    } else {
        sscanf(buf, "%*c%[^!]", usr);
        sscanf(buf, "%*[^#]%*c%[^ ]", chan);
        sscanf(buf, "%*[^#]%*[^:]%*c%[^\n]", msg);

        printf("#");
        fwrite(chan, 1, strlen(chan), stdout);
        printf("->");
        fwrite(usr, 1, strlen(usr), stdout);
        printf(" : ");
        fwrite(msg, 1, strlen(msg), stdout);
        printf("\n");

        // Only process commands if they come from my channel
        if (strcmp(chan, "olabaz") == 0 ) {
            if (msg[0] == '!') {
                parse_command(sock_fd, chan, msg, usr);
            }
        }

        
    }
}

int reply_PONG(int sock_fd) {

    /* PING :tmi.twitch.tv */

    char pong[] = "PONG :tmi.twitch.tv\n";
    int bytes_sent;
    bytes_sent = send(sock_fd, pong, strlen(pong), 0);

    return bytes_sent;
}

int send_msg(int sock_fd, char* chan, char* msg) {

    /* PRIVMSG #<channel> :This is a sample message */

    char *msg_w_header = malloc(12*sizeof(*msg_w_header) + strlen(chan) + strlen(msg));

    sprintf(msg_w_header, "PRIVMSG #%s :%s\n", chan, msg);
    
    int bytes_sent;
    bytes_sent = send(sock_fd, msg_w_header, strlen(msg_w_header), 0);

    free(msg_w_header);
    
    return bytes_sent;
}
