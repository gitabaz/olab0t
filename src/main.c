#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "message.h"
#include "parse_config.h"
#include "handle_msg.h"
#include "constants.h"

void authenticate(bot_config* bc, int sock_fd);
void join_channels(bot_config* bc, int sock_fd);
void request_tags(int sock_fd);

int main() {

    puts("Started olab0t...");

    char* bot_config_fn = "/home/olabaz/Data/Documents/programming/auth/twitch_oauth.json";

    bot_config* bc = bot_config_from_file(bot_config_fn);

    srand(time(NULL)); // Seed RNG for commands like !roulette

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
    request_tags(sock_fd);
    join_channels(bc, sock_fd);

    char* buf = malloc(sizeof(*buf) * 3 * BUF_SIZE);
    char* temp_buf = malloc(sizeof(*temp_buf) * 3 * BUF_SIZE);
    ssize_t bytes_recv = 0, buf_bytes = 0, full_msg_pos = 0;
    message* msg = new_message();

    while(true) {
        bytes_recv = recv(sock_fd, temp_buf, 3 * BUF_SIZE, 0);
        if (bytes_recv > 0) {
            memcpy(buf + buf_bytes, temp_buf, bytes_recv);
            buf_bytes += bytes_recv;
        }

        do {
            full_msg_pos = find_full_msg(buf, buf_bytes, msg, sock_fd);
            if (full_msg_pos > 0) {
                flush_msg(&buf, &buf_bytes, full_msg_pos, msg, sock_fd);
            }
        } while (full_msg_pos);
    }




    // Clean up
    free(buf);
    free(temp_buf);
    close(sock_fd);
    freeaddrinfo(serv_info);
    free_bc(bc);
    free_message(msg);

    return 0;
}

void authenticate(bot_config* bc, int sock_fd) {
    char pass[110], nick[110]; // 100 char strings + extra space to store prefix (`PASS `, `NICK `)
    ssize_t bytes_sent, bytes_recv;
    char buf[BUF_SIZE];

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

    bytes_recv = recv(sock_fd, buf, BUF_SIZE, 0);
    if (bytes_recv <= 0) {
        puts("Error: could not connect");
        exit(EXIT_FAILURE);
    }
    fwrite(buf, 1, bytes_recv, stdout);
}

void request_tags(int sock_fd) {
    char tags[] = "CAP REQ :twitch.tv/tags\n";
    ssize_t bytes_sent, bytes_recv;
    char buf[BUF_SIZE];
   
    bytes_sent = send(sock_fd, tags, strlen(tags), 0);
    if (bytes_sent < 0) {
        exit(EXIT_FAILURE);
    } else {
        if ((size_t) bytes_sent != strlen(tags)) {
            exit(EXIT_FAILURE);
        }
    }

    bytes_recv = recv(sock_fd, buf, BUF_SIZE, 0);

    if (bytes_recv <= 0) {
        puts("Error: could not get tags");
    } else {
        fwrite(buf, 1, bytes_recv, stdout);
    }
}

void join_channels(bot_config* bc, int sock_fd) {
    char channel[110]; // 100 char string + extra space to store prefix (`JOIN #`)
    char buf[BUF_SIZE];
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

        bytes_recv = recv(sock_fd, buf, BUF_SIZE, 0);
        if (bytes_recv <= 0) {
            printf("Error: could not connect to channel `%s`\n", bc->channel_list[i]);
        }
        fwrite(buf, 1, bytes_recv, stdout);

    }

    // Add another call to recv in case we haven't read all the
    // confirmations for joining channels
    int bytes_peek = recv(sock_fd, buf, BUF_SIZE, MSG_PEEK);
    if (bytes_peek > 0) {
        bytes_recv = recv(sock_fd, buf, BUF_SIZE, 0);
        fwrite(buf, 1, bytes_recv, stdout);
    }
}
