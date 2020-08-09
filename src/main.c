#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "parse_config.h"

int main() {

    puts("Started olab0t...");

    char* bot_config_fn = "../twitch_oauth.json";
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
    printf("sock fd: %d\n", sock_fd);
    int sock_conn = connect(sock_fd, serv_info->ai_addr, serv_info->ai_addrlen);
    printf("sock conn: %d\n", sock_conn);

    char pass[107], nick[107], channel[107];

    sprintf(pass, "PASS %s\n", bc->oauth_token);
    sprintf(nick, "NICK %s\n", bc->bot_username);
    sprintf(channel, "JOIN #%s\n", bc->channel_list[0]);

    int bytes_sent, bytes_rcvd;
    char buf[501];
    bytes_sent = send(sock_fd, pass, strlen(pass), 0);
    sleep(1);
    bytes_sent = send(sock_fd, nick, strlen(nick), 0);

    printf("receiving...\n");
    bytes_rcvd = recv(sock_fd, buf, 500, 0);
    /* printf("bytes rcvd: %d\n", bytes_rcvd); */
    fwrite(buf, 1, bytes_rcvd, stdout);

    puts("joining channel 1...");
    bytes_sent = send(sock_fd, channel, strlen(channel), 0);
    bytes_rcvd = recv(sock_fd, buf, 500, 0);
    fwrite(buf, 1, bytes_rcvd, stdout);

    /* bytes_sent = send(sock_fd, nick, strlen(nick), 0); */
    /* printf("bytes sent: %d\n", bytes_sent); */
    puts("joining channel 2...");
    sprintf(channel, "JOIN #%s\n", bc->channel_list[1]);
    bytes_sent = send(sock_fd, channel, strlen(channel), 0);
    bytes_rcvd = recv(sock_fd, buf, 500, 0);
    fwrite(buf, 1, bytes_rcvd, stdout);

    while(true) {
        bytes_rcvd = recv(sock_fd, buf, 500, 0);
        if (bytes_rcvd > 0) {
            fwrite(buf, 1, bytes_rcvd, stdout);
        }
    }

    /* char* msg = "Hello twitch"; */
    /* int len = strlen(msg); */
    /* int bytes_sent = send(sock_fd, msg, len, 0); */
    /* printf("bytes sent: %d\n", bytes_sent); */

    /* char buf[101]; */
    /* int bytes_rcvd = recv(sock_fd, buf, strlen(buf), 0); */
    /* printf("bytes rcvd: %d\n", bytes_rcvd); */

    // Clean up
    close(sock_fd);
    freeaddrinfo(serv_info);
    freebc(bc);

    return 0;
}
