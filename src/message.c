#include "message.h"
#include "constants.h"
#include <stdlib.h>

message* new_message() {
    message* msg = malloc(sizeof(*msg));

    msg->badge_info = malloc(sizeof(*msg->badge_info) * BUF_SIZE);
    msg->badges = malloc(sizeof(*msg->badges) * BUF_SIZE);
    msg->bits = malloc(sizeof(*msg->bits) * BUF_SIZE);
    msg->client_nonce = malloc(sizeof(*msg->client_nonce) * BUF_SIZE);
    msg->custom_reward_id = malloc(sizeof(*msg->custom_reward_id) * BUF_SIZE);
    msg->display_name = malloc(sizeof(*msg->display_name) * BUF_SIZE);
    msg->emotes = malloc(sizeof(*msg->emotes) * BUF_SIZE);
    msg->flags = malloc(sizeof(*msg->flags) * BUF_SIZE);
    msg->id = malloc(sizeof(*msg->id) * BUF_SIZE);
    msg->msg_id = malloc(sizeof(*msg->msg_id) * BUF_SIZE);
    msg->reply_parent_display_name = malloc(sizeof(*msg->reply_parent_display_name) * BUF_SIZE);
    msg->reply_parent_msg_body = malloc(sizeof(*msg->reply_parent_msg_body) * BUF_SIZE);
    msg->reply_parent_msg_id = malloc(sizeof(*msg->reply_parent_msg_id) * BUF_SIZE);
    msg->reply_parent_user_login = malloc(sizeof(*msg->reply_parent_user_login) * BUF_SIZE);
    msg->room_id = malloc(sizeof(*msg->id) * BUF_SIZE);
    msg->subscriber = malloc(sizeof(*msg->subscriber) * BUF_SIZE);;
    msg->user_type = malloc(sizeof(*msg->user_type) * BUF_SIZE);
    msg->text = malloc(sizeof(*msg->text) * BUF_SIZE);

    return msg;
}

void free_message(message* msg) {
    free(msg->badge_info);
    free(msg->badges);
    free(msg->bits);
    free(msg->client_nonce);
    free(msg->custom_reward_id);
    free(msg->display_name);
    free(msg->emotes);
    free(msg->flags);
    free(msg->id);
    free(msg->msg_id);
    free(msg->reply_parent_display_name);
    free(msg->reply_parent_msg_body);
    free(msg->reply_parent_msg_id);
    free(msg->reply_parent_user_login);
    free(msg->room_id);
    free(msg->subscriber);
    free(msg->user_type);
    free(msg->text);
    free(msg);
}
