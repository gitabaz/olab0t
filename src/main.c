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


int main() {

    puts("Started olab0t...");

    char* bot_config_fn = "bin/twitch_oauth.json";
    const char* twitch_irc_url = "irc.chat.twitch.tv";
    const char* twitch_irc_port = "6667";

    bot_config* bc = bot_config_from_file(bot_config_fn);

    int status;
    struct addrinfo hints;
    struct addrinfo* serv_info;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC; // Either IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets

    if ((status = getaddrinfo(twitch_irc_url, twitch_irc_port, &hints, &serv_info)) != 0) {
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

    char pass[107], nick[107], channel[107]; // 101 char strings + prefix (`PASS `, `NICK `, `JOIN #`)
    int bytes_sent, bytes_recv;
    char buf[501];

    // Authenticate
    sprintf(pass, "PASS %s\n", bc->oauth_token);
    sprintf(nick, "NICK %s\n", bc->bot_username);
    bytes_sent = send(sock_fd, pass, strlen(pass), 0);
    sleep(0.1);
    bytes_sent = send(sock_fd, nick, strlen(nick), 0);

    if (bytes_sent != strlen(nick)) {
        exit(EXIT_FAILURE);
    }

    bytes_recv = recv(sock_fd, buf, 500, 0);
    if (bytes_recv == 0) {
        puts("Error: could not connect");
        exit(EXIT_FAILURE);
    }
    fwrite(buf, 1, bytes_recv, stdout);

    // TODO: make sure we are actually authenticated

    // Join channels in list
    if (bc->capacity == 0) {
        puts("Error: no channels added");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < bc->capacity; i++) {
        sprintf(channel, "JOIN #%s\n", bc->channel_list[i]);
        bytes_sent = send(sock_fd, channel, strlen(channel), 0);

        if (bytes_sent != strlen(channel)) {
            printf("Error: could not connect to channel `%s`\n", bc->channel_list[i]);
        }
        bytes_recv = recv(sock_fd, buf, 500, 0);
        sleep(0.1);
        if (bytes_recv == 0) {
            printf("Error: could not connect to channel `%s`\n", bc->channel_list[i]);
        }
        fwrite(buf, 1, bytes_recv, stdout);
    }

    char ping[6], msg[301], msg_channel[101], msg_user[101];
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
