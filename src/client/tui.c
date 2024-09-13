#include "tui.h"
#include <stdio.h>

void show_commands_menu() {
    printf("##############################################\n");
    printf("#          COMMAND MENU                      #\n");
    printf("# /public [message]    -> Send a public message to everyone\n");
    printf("# /status [status]     -> Change your status (ACTIVE, AWAY, BUSY)\n");
    printf("# /users               -> Show the list of connected users\n");
    printf("# /private [user] [msg]-> Send a private message to a user\n");
    printf("# /exit                -> Leave the chat\n");
    printf("# By default, any message without a slash (/) will be public.\n");
    printf("##############################################\n\n");
}
