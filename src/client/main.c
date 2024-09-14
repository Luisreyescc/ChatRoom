/**
 * @file main.c
 * @brief Entry point for the client-side application.
 *
 * This program allows a client to connect to a server, send and receive messages in parallel 
 * using threads, and disconnect gracefully when finished. It requires an IP address and port 
 * to establish the connection.
 */
#include "connection.h"
#include "messaging.h"
#include "tui.h"
#include <string.h>

/**
 * @brief Main function that starts the client application.
 *
 * The program expects two command-line arguments: the server's IP address and port number. 
 * It prompts the user for a username, establishes a connection to the server, and then 
 * creates two threads for sending and receiving messages. The connection is closed when 
 * the user terminates the session.
 *
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line argument strings.
 * @return int Returns EXIT_SUCCESS on successful execution or EXIT_FAILURE on error.
 */
int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Usage: %s <ip> <port>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *ip = argv[1];
    int port = atoi(argv[2]);

    printf("What's your name?: ");
    if (fgets(user_name, sizeof(user_name), stdin) == NULL) {
        perror("fgets error");
        return EXIT_FAILURE;
    }
    user_name[strcspn(user_name, "\n")] = '\0';  

    connect_to_server(ip, port);
    show_commands_menu();

    if (pthread_create(&send_msg_thread, NULL, (void*)send_msg, NULL) != 0) {
        perror("pthread_create send_msg error");
        close_connection();
        return EXIT_FAILURE;
    }

    if (pthread_create(&recv_msg_thread, NULL, (void*)recv_msg, NULL) != 0) {
        perror("pthread_create recv_msg error");
        close_connection();
        return EXIT_FAILURE;
    }

    pthread_join(send_msg_thread, NULL);
    pthread_join(recv_msg_thread, NULL);

    printf("\nCome back soon!\n");
    close_connection();

    return EXIT_SUCCESS;
}
