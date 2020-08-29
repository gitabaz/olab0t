#ifndef __MESSAGE_H_
#define __MESSAGE_H_

#include <stdbool.h>
#include <time.h>

typedef struct message {
    char* badge_info;
    char* badges;
    char* bits;
    char* client_nonce;
    char color[10];
    char* custom_reward_id;
    char* display_name;
    char* emotes;
    bool emote_only;
    char* flags;
    char* id;
    bool mod;
    char* msg_id;
    char* reply_parent_display_name;
    char* reply_parent_msg_body;
    char* reply_parent_msg_id;
    long long reply_parent_user_id;
    char* reply_parent_user_login;
    char* room_id;
    char* subscriber;
    time_t tmi_sent_ts;
    bool turbo;
    long long user_id;
    char* user_type;
    char* text;
    char* channel;
} message;

message* new_message();
void free_message(message* msg);

#endif // __MESSAGE_H_
