#include "tui.h"
#include <stdio.h>

#define RESET_COLOR "\033[0m"
#define BOLD "\033[1m"
#define UNDERLINE "\033[4m"
#define COLOR_RED "\033[31m"
#define COLOR_GREEN "\033[32m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_BLUE "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN "\033[36m"
#define COLOR_WHITE "\033[37m"
#define BG_BLUE "\033[44m"
#define BG_YELLOW "\033[43m"
#define BG_CYAN "\033[46m"

/**
 * @brief Displays an enhanced command menu with colors and formatting.
 *
 * This function prints the command menu with colors and styles to make it more
 * visually appealing. The menu provides users with available commands to interact
 * with the chat application.
 *
 * @return void
 */
void show_commands_menu() {
    printf("%s####################################################################%s\n", COLOR_WHITE, RESET_COLOR);
    printf("  %s                        COMMAND MENU%s                            \n", BOLD, RESET_COLOR);
    printf("%s####################################################################%s\n", COLOR_WHITE, RESET_COLOR);
    
    printf(" %s/public [message]%s   %s->%s Send a %spublic%s message to everyone\n", COLOR_CYAN, RESET_COLOR, COLOR_GREEN, RESET_COLOR, BOLD, RESET_COLOR);
    printf(" %s/status [status]%s    %s->%s Change your %sstatus%s (active, away, busy)\n", COLOR_CYAN, RESET_COLOR, COLOR_GREEN, RESET_COLOR, BOLD, RESET_COLOR);
    printf(" %s/users%s              %s->%s Show the list of %sconnected users%s\n", COLOR_CYAN, RESET_COLOR, COLOR_GREEN, RESET_COLOR, BOLD, RESET_COLOR);
    printf(" %s/private [user] [msg]%s%s->%s Send a %sprivate%s message to a user\n", COLOR_CYAN, RESET_COLOR, COLOR_GREEN, RESET_COLOR, BOLD, RESET_COLOR);
    printf(" %s/exit%s               %s->%s %sLeave%s the chat\n", COLOR_CYAN, RESET_COLOR, COLOR_GREEN, RESET_COLOR, BOLD, RESET_COLOR);
    printf(" %sAny message without a slash (%s/%s) will be public by default.%s\n", COLOR_YELLOW, COLOR_RED, COLOR_YELLOW, RESET_COLOR);
    
    printf("%s####################################################################%s\n\n", COLOR_WHITE, RESET_COLOR);
    
    printf("%sType a message!%s\n\n", COLOR_WHITE, RESET_COLOR);
}
