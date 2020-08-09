#include "handle_msg.h"

void parse_msg(char* buf, char* ping, char* chan, char* usr, char* msg, int sock_fd) {
    /* :olabaz!olabaz@olabaz.tmi.twitch.tv PRIVMSG #olabaz :yes */
    /* :olabaz!olabaz@olabaz.tmi.twitch.tv PRIVMSG #olab0t :hello */
    /* PING :tmi.twitch.tv */

    if(sscanf(buf, "%[^:]", ping) == 1) {
        if(reply_PONG(sock_fd) == 0) {
            puts("Error: could not send PONG");
            exit(EXIT_FAILURE);
        }
    } else {
        sscanf(buf, "%*c%[^!]", usr);
        sscanf(buf, "%*[^#]%[^ ]", chan);
        sscanf(buf, "%*[^#]%*[^:]%*c%[^\n]", msg);

        fwrite(chan, 1, strlen(chan), stdout);
        printf("->");
        fwrite(usr, 1, strlen(usr), stdout);
        printf(" : ");
        fwrite(msg, 1, strlen(msg), stdout);
        printf("\n");

        /* send_msg(sock_fd, "olabaz", "hi"); */
        
    }
}

int reply_PONG(int sock_fd) {
    char pong[] = "PONG :tmi.twitch.tv\n";
    int bytes_sent;
    bytes_sent = send(sock_fd, pong, strlen(pong), 0);

    return bytes_sent;
}

int send_msg(int sock_fd, char* chan, char* msg) {

    /* PRIVMSG #<channel> :This is a sample message */

    char *msg_w_header = malloc(12*sizeof(*msg_w_header) + strlen(chan) + strlen(msg));

    sprintf(msg_w_header, "PRIVMSG #%s :%s\n", chan, msg);
    printf("%s\n",msg_w_header);
    
    int bytes_sent;
    bytes_sent = send(sock_fd, msg_w_header, strlen(msg_w_header), 0);

    free(msg_w_header);
    
    return bytes_sent;
}
