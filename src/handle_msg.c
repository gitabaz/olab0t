#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include <ctype.h>

#include "handle_msg.h"
#include "commands.h"
#include "constants.h"

/* @badge-info=subscriber/1;
 * badges=subscriber/0;
 * client-nonce=dbd1897dc0b6924c7684818a1d6d2225;
 * color=#D2691E;
 * display-name=ghoulzylun4tic;
 * emotes=;
 * flags=;
 * id=32ba33fe-44ff-44e3-ae3d-6ec0c2fe237a;
 * mod=0;
 * reply-parent-display-name=stskyy_;
 * reply-parent-msg-body=How\sam\sI\styping\swithout\sbeing\ssub;
 * reply-parent-msg-id=635cd448-e59d-4bf3-997a-8947508607ec;
 * reply-parent-user-id=496500247;
 * reply-parent-user-login=stskyy_;
 * room-id=181224914;
 * subscriber=1;
 * tmi-sent-ts=1598472524052;
 * turbo=0;
 * user-id=557014076;
 * user-ty */


void parse_msg(char* buf, message* msg, int sock_fd) {

    char tags[501], message_header[110], message_text[501];
    
    sscanf(buf, "%s :%[^:]:%[^\r\n]", tags, message_header, message_text);
    /* printf("%s\n", tags); */
    if (strcmp(tags, PING) == 0) {
        /* puts("Found PING...sending PONG"); */
        if(reply_PONG(sock_fd) == 0) {
            puts("Error: could not send PONG");
            exit(EXIT_FAILURE);
        }
    } else {
        char delim[] = ";";
        char *token = strtok(tags, delim);
        while(token) {
            if (parse_tags(token, msg) != 0) {
                printf("---\n%s\n---\n", buf);
            }
            token = strtok(NULL, delim);
        }
        strcpy(msg->text, message_text);
        print_message(msg);
        
        // Only process commands if they come from my channel
        /* if (strcmp(chan, "olabaz") == 0 ) { */
        /*     if (msg[0] == '!') { */
        /*         parse_command(sock_fd, chan, msg, usr); */
        /*     } else if (strcmp(msg, "hello") == 0 || strcmp(msg, "hi") == 0){ */
        /*         char response[100]; */
        /*         sprintf(response, "Hi @%s!\n", usr); */
        /*         send_msg(sock_fd, chan, response); */
        /*     } */
        /* } */
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

int parse_tags(char* tags, message* msg) {
    char field[BUF_SIZE], value[BUF_SIZE];

    int res = sscanf(tags, "%[^=]=%s", field, value);
    int parse_error = 0;
    if (res == 2) {
        /* printf("Field: %s\n", field); */
        /* printf("Value: %s\n", value); */
        parse_error = parse_tag_field_value(field, value, msg);
    } else if (res == 1) {
        /* printf("Field: %s\n", field); */
        /* printf("Value: %s\n", "Using default value"); */
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
