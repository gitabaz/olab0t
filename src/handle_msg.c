#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <ctype.h>

#include "handle_msg.h"
#include "commands.h"
#include "constants.h"

ssize_t find_full_msg(char* buf, ssize_t buf_bytes, message* msg, int sock_fd){
    for (ssize_t i = 0; i < buf_bytes; i++) {
        if (buf[i] == '\n') {
            return i;
        }
    }
    return 0;
}

void flush_msg(char** buf, ssize_t* buf_bytes, ssize_t full_msg_pos, message* msg, int sock_fd) {
    size_t full_msg_bytes = full_msg_pos + 1;
    parse_msg(*buf, full_msg_bytes, msg, sock_fd);
    memmove(*buf, *buf + full_msg_bytes, *buf_bytes - full_msg_bytes);
    *buf_bytes -= full_msg_bytes;
}

void parse_msg(char* buf, ssize_t buf_bytes, message* msg, int sock_fd) {

    char tags[BUF_SIZE], message_header[BUF_SIZE], message_text[BUF_SIZE];
   
    if (buf_bytes >= LEN_PING && memcmp(buf, PING, LEN_PING) == 0) {
        /* puts("Found PING...sending PONG"); */
        if(reply_PONG(sock_fd) == 0) {
            puts("Error: could not send PONG");
            exit(EXIT_FAILURE);
        }
    } else {
        sscanf(buf, "%s :%[^:]:%[^\r\n]", tags, message_header, message_text);
        char delim[] = ";";
        char *token = strtok(tags, delim);
        while(token) {
            if (parse_tags(token, msg) != 0) {
                puts("--- buf ---");
                fwrite(buf, 1, buf_bytes, stdout);
                puts("---");
                puts("--- tags ---");
                printf("%s", tags);
                puts("---");
            }
            token = strtok(NULL, delim);
        }
        strcpy(msg->text, message_text);
        print_message(msg);

        sscanf(message_header, "%*[^#]#%s", msg->channel);

        // Only process commands if they come from my channel
        if (strcmp(msg->channel, "olabaz") == 0) {
            if (msg->text[0] == '!') {
                parse_command(msg, sock_fd);
            } else if (strcmp(msg->text, "hello") == 0 || strcmp(msg->text, "hi") == 0){
                char response[100];
                sprintf(response, "Hi @%s!\n", msg->display_name);
                send_msg(sock_fd, msg->channel, response);
            }
        }
    }
}

int reply_PONG(int sock_fd) {
    return send(sock_fd, PONG, LEN_PONG, 0);
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

int parse_tags(char* tags, message* msg) {
    char field[BUF_SIZE], value[BUF_SIZE];

    int res = sscanf(tags, "%[^=]=%s", field, value);
    int parse_error = 0;
    if (res == 2) {
        parse_error = parse_tag_field_value(field, value, msg);
    } else if (res == 1) {
        parse_error = parse_tag_field_value(field, "NULL", msg);
    } else {
        printf("Error parsing %s\n", tags);
        parse_error = -1;
    }
    return parse_error;
}
int parse_tag_field_value(char* field, char* value, message* msg){
    if (strcmp(field, "@badge-info") == 0){
        strcpy(msg->badge_info, value);
    } else if (strcmp(field, "badges") == 0) {
        strcpy(msg->badges, value);
    } else if (strcmp(field, "bits") == 0) {
        strcpy(msg->bits, value);
    } else if (strcmp(field, "client-nonce") == 0) {
        strcpy(msg->client_nonce, value);
    } else if (strcmp(field, "color") == 0) {
        if (strcmp(value, "NULL") == 0) {
            // Make default color red
            strcpy(msg->color, "#FF0000");
        } else {
            strcpy(msg->color, value);
        }
    } else if (strcmp(field, "custom-reward-id") == 0) {
        strcpy(msg->custom_reward_id, value);
    } else if (strcmp(field, "display-name") == 0) {
        strcpy(msg->display_name, value);
    } else if (strcmp(field, "emotes") == 0) {
        strcpy(msg->emotes, value);
    } else if (strcmp(field, "emote-only") == 0) {
        msg->emote_only = (bool) atoi(value);
    } else if (strcmp(field, "flags") == 0) {
        strcpy(msg->flags, value);
    } else if (strcmp(field, "id") == 0) {
        strcpy(msg->id, value);
    } else if (strcmp(field, "mod") == 0) {
        msg->mod = (bool) atoi(value);
    } else if (strcmp(field, "msg-id") == 0) {
        strcpy(msg->msg_id, value);
    } else if (strcmp(field, "reply-parent-display-name") == 0) {
        strcpy(msg->reply_parent_display_name, value);
    } else if (strcmp(field, "reply-parent-msg-body") == 0) {
        strcpy(msg->reply_parent_msg_body, value);
    } else if (strcmp(field, "reply-parent-msg-id") == 0) {
        strcpy(msg->reply_parent_msg_id, value);
    } else if (strcmp(field, "reply-parent-user-id") == 0) {
        msg->reply_parent_user_id = atoll(value);
    } else if (strcmp(field, "reply-parent-user-login") == 0) {
        strcpy(msg->reply_parent_user_login, value);
    } else if (strcmp(field, "room-id") == 0) {
        strcpy(msg->room_id, value);
    } else if (strcmp(field, "subscriber") == 0) {
        // Deprecated use badges instead
        strcpy(msg->subscriber, value);
    } else if (strcmp(field, "tmi-sent-ts") == 0) {
        // Twitch gives the time in milliseconds
        msg->tmi_sent_ts = (time_t) atoll(value)/1000;
    } else if (strcmp(field, "turbo") == 0) {
        // Deprecated use badges instead
        msg->turbo = (bool) atoi(value);
    } else if (strcmp(field, "user-id") == 0) {
        msg->user_id = atoll(value);
    } else if (strcmp(field, "user-type") == 0) {
        // Deprecated use badges instead
        strcpy(msg->user_type, value);
    } else {
        printf("Unknown field: %s\n", field);
        return -1;
    }
    return 0;
}

void print_color_str(char* str, char* color) {
    start_color(color);
    printf("%s", str);
    end_color();
}

void print_time(message* msg) {
    /* Use the form [hh:mm:ss] */
    struct tm* cdate = localtime(&msg->tmi_sent_ts);
    printf("[%02d:%02d:%02d] ", cdate->tm_hour, cdate->tm_min, cdate->tm_sec);
}

void print_username(message* msg) {
    print_color_str(msg->display_name, msg->color);
    printf(": ");
}

void print_message_text(message* msg) {
    printf("%s\n", msg->text);
}

void print_message(message* msg) {
    print_time(msg);
    print_username(msg);
    print_message_text(msg);
}

void start_color(char* hex_color) {
    printf("\033[38:2::%s::m", hex_to_rgb(hex_color));
}
void end_color(char* rgb_color) {
    printf("\033[m");
}

char* hex_to_rgb(char* color) {
    char red[3], green[3], blue[3];

    sprintf(red,   "%c%c", color[1], color[2]);
    sprintf(green, "%c%c", color[3], color[4]);
    sprintf(blue,  "%c%c", color[5], color[6]);

    int r = strtol(red,   NULL, 16);
    int g = strtol(green, NULL, 16);
    int b = strtol(blue,  NULL, 16);

    static char rgb[LEN_RGB];
    sprintf(rgb, "%d:%d:%d", r, g, b);
    return rgb;
}
