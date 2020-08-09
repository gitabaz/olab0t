#ifndef HANDLE_MSG_H
#define HANDLE_MSG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>

int reply_PONG(int sock_fd);
void parse_msg(char* buf, char* ping, char* chan, char* usr, char* msg, int sock_fd);
int send_msg(int sock_fd, char* chan, char* msg);
int parse_command(int sock_fd, char* chan, char* msg, char* usr);

#endif
