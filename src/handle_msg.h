#ifndef HANDLE_MSG_H
#define HANDLE_MSG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#include <sys/types.h>
#include <sys/socket.h>

#include "constants.h"
#include "message.h"

// Messaging functions
int reply_PONG(int sock_fd);
int send_msg(int sock_fd, char* chan, char* msg);

// Parsing functions
ssize_t find_full_msg(char* buf, ssize_t buf_bytes, message* msg, int sock_fd);
void flush_msg(char** buf, ssize_t* buf_bytes, ssize_t pos_full_msg, message* msg, int sock_fd);
void parse_msg(char* buf, ssize_t buf_bytes, message* msg, int sock_fd);
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
