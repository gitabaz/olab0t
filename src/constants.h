#ifndef __CONSTANTS_H_
#define __CONSTANTS_H_


#define TWITCH_IRC_URL "irc.chat.twitch.tv"
#define TWITCH_IRC_PORT "6667"
#define PING "PING :tmi.twitch.tv"
#define PONG "PONG :tmi.twitch.tv\n"

#define BUF_SIZE 1024
#define LEN_PING (sizeof PING - 1)
#define LEN_PONG (sizeof PONG - 1)
#define LEN_RGB (sizeof "255:255:255")

#endif // __CONSTANTS_H_
