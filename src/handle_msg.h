#ifndef HANDLE_MSG_H
#define HANDLE_MSG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#include <sys/types.h>
#include <sys/socket.h>

typedef struct message {
    char badge_info[110];
    char badges[110];
    char bits[110];
    char client_nonce[110];
    char color[10];
    char custom_reward_id[110];
    char display_name[110];
    char emotes[110];
    int emote_only;
    char flags[110];
    char id[110];
    bool mod;
    char reply_parent_display_name[110];
    char reply_parent_msg_body[110];
    char reply_parent_msg_id[110];
    long long reply_parent_user_id;
    char reply_parent_user_login[110];
    char room_id[110];
    char subscriber[110];
    time_t tmi_sent_ts;
    bool turbo;
    long long user_id;
    char user_type[110];
    char text[501];
} message;

// Messaging functions
int reply_PONG(int sock_fd);
int send_msg(int sock_fd, char* chan, char* msg);

// Parsing functions
void parse_msg(char* buf, message* msg, int sock_fd);
int parse_tags(char* tags, message* msg);
int parse_command(int sock_fd, char* chan, char* msg, char* usr);
int parse_tag_field_value(char* field, char* value, message* msg);

// Print functions
void print_color_str(char* str, char* color);
void print_time(message* msg);
void print_username(message* msg);
void print_message_text(message* msg);
void print_message(message* msg);

// Coloring
void start_color(char* color);
void end_color();
char* hex_to_rgb(char* color);

#endif
