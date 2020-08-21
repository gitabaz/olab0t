#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "parse_config.h"
#include "handle_msg.h"

/* Started olab0t... */
/* :tmi.twitch.tv 001 olab0t :Welcome, GLHF! */
/* :tmi.twitch.tv 002 olab0t :Your host is tmi.twitch.tv */
/* :tmi.twitch.tv 003 olab0t :This server is rather new */
/* :tmi.twitch.tv 004 olab0t :- */
/* :tmi.twitch.tv 375 olab0t :- */
/* :tmi.twitch.tv 372 olab0t :You are in a maze of twisty passages, all alike. */
/* :tmi.twitch.tv 376 olab0t :> */
/* :olab0t!olab0t@olab0t.tmi.twitch.tv JOIN #olabaz */
/* :olab0t.tmi.twitch.tv 353 olab0t = #olabaz :olab0t */
/* :olab0t.tmi.twitch.tv 366 olab0t #olabaz :End of /NAMES list */
/* :olab0t!olab0t@olab0t.tmi.twitch.tv JOIN #olab0t */
/* :olab0t.tmi.twitch.tv 353 olab0t = #olab0t :olab0t */
/* :olab0t.tmi.twitch.tv 366 olab0t #olab0t :End of /NAMES list */
/* :olabaz!olabaz@olabaz.tmi.twitch.tv PRIVMSG #olabaz :yes */
/* :olabaz!olabaz@olabaz.tmi.twitch.tv PRIVMSG #olab0t :hello */
/* :olabaz!olabaz@olabaz.tmi.twitch.tv PRIVMSG #olab0t :hi : */

#define TWITCH_IRC_URL "irc.chat.twitch.tv"
#define TWITCH_IRC_PORT "6667"

void authenticate(bot_config* bc, int sock_fd);
void join_channels(bot_config* bc, int sock_fd);

int main() {

    puts("Started olab0t...");

    char* bot_config_fn = "bin/twitch_oauth.json";

    bot_config* bc = bot_config_from_file(bot_config_fn);

    int status;
    struct addrinfo hints;
    struct addrinfo* serv_info;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC; // Either IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets

    if ((status = getaddrinfo(TWITCH_IRC_URL, TWITCH_IRC_PORT, &hints, &serv_info)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(EXIT_FAILURE);
    }

    int sock_fd = socket(serv_info->ai_family, serv_info->ai_socktype, serv_info->ai_protocol);
    if (sock_fd < 0) {
        puts("Error: could not get socket descriptor");
        exit(EXIT_FAILURE);
    }

    int sock_conn = connect(sock_fd, serv_info->ai_addr, serv_info->ai_addrlen);
    if (sock_conn != 0) {
        puts("Error: could not connect to socket descriptor");
        exit(EXIT_FAILURE);
    }

    // TODO: make sure we are actually authenticated
    authenticate(bc, sock_fd);

    join_channels(bc, sock_fd);

    char buf[501];
    char ping[6], msg[301], msg_channel[101], msg_user[101];
    ssize_t bytes_recv;
    while(true) {
        bytes_recv = recv(sock_fd, buf, 500, 0);
        if (bytes_recv > 0) {
            parse_msg(buf, ping, msg_channel, msg_user, msg, sock_fd);
        }
        /* memset(buf, '\0', 500); // Clear the buffer */
    }

    // Clean up
    close(sock_fd);
    freeaddrinfo(serv_info);
    freebc(bc);

    return 0;
}

void authenticate(bot_config* bc, int sock_fd) {
    char pass[110], nick[110]; // 100 char strings + extra space to store prefix (`PASS `, `NICK `)
    ssize_t bytes_sent, bytes_recv;
    char buf[501];

    // In parse config, should we just read in the strings into the proper
    // format so we don't have to sprintf them here?
    sprintf(pass, "PASS %s\n", bc->oauth_token);
    sprintf(nick, "NICK %s\n", bc->bot_username);

    bytes_sent = send(sock_fd, pass, strlen(pass), 0);
    if (bytes_sent < 0) {
        exit(EXIT_FAILURE);
    } else {
        if ((size_t) bytes_sent != strlen(pass)) {
            exit(EXIT_FAILURE);
        }
    }

    bytes_sent = send(sock_fd, nick, strlen(nick), 0);
    if (bytes_sent < 0) {
        exit(EXIT_FAILURE);
    } else {
        if ((size_t) bytes_sent != strlen(nick)) {
            exit(EXIT_FAILURE);
        }
    }

    bytes_recv = recv(sock_fd, buf, 500, 0);
    if (bytes_recv <= 0) {
        puts("Error: could not connect");
        exit(EXIT_FAILURE);
    }
    fwrite(buf, 1, bytes_recv, stdout);
}

void join_channels(bot_config* bc, int sock_fd) {
    char channel[107]; // 100 char string + extra space to store prefix (`JOIN #`)
    char buf[501];
    ssize_t bytes_sent, bytes_recv;

    if (bc->capacity == 0) {
        puts("Error: no channels added");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < bc->capacity; i++) {
        sprintf(channel, "JOIN #%s\n", bc->channel_list[i]);
        bytes_sent = send(sock_fd, channel, strlen(channel), 0);

        if(bytes_sent < 0){
            exit(EXIT_FAILURE);
        } else {
            if ((size_t) bytes_sent != strlen(channel)) {
                printf("Error: could not connect to channel `%s`\n", bc->channel_list[i]);
            }
        }

        bytes_recv = recv(sock_fd, buf, 500, 0);
        if (bytes_recv <= 0) {
            printf("Error: could not connect to channel `%s`\n", bc->channel_list[i]);
        }
        fwrite(buf, 1, bytes_recv, stdout);
    }

}
