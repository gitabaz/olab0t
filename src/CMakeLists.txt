set(HEADER_FILES parse_config.h handle_msg.h commands.h message.h)
set(SOURCE_FILES parse_config.c handle_msg.c commands.c message.c)

add_executable(olab0t main.c ${SOURCE_FILES} ${HEADER_FILES})
