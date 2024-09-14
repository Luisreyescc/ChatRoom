/**
 * @file main.c
 * @brief Main server logic for handling client connections and messaging.
 *
 * This file contains the main function for starting the server, accepting client connections, 
 * and managing client communication. It also includes a client handler function to manage 
 * individual client interactions using threads.
 */
#include "connection.h"
#include "client_manager.h"
#include "messaging.h"
#include <pthread.h>
#include <unistd.h>
#include <string.h>

/**
 * @brief Handles communication with a connected client.
 *
 * This function is executed in a separate thread for each connected client. It listens for messages 
 * from the client, processes them, and handles client disconnection if necessary.
 *
 * @param arg Pointer to the client_t structure of the connected client.
 * @return void* Always returns NULL when the thread exits.
 */
void *client_handler(void *arg) {
    client_t *client = (client_t *)arg;
    char buffer[2048];

    while (1) {
        int receive = recv(client->sockfd, buffer, 2048, 0);
        if (receive > 0) {
            buffer[receive] = '\0';
            process_client_message(client, buffer);
        } else if (receive == 0) {
            printf("Client %s disconnected.\n", client->user_name);
            close(client->sockfd);
            remove_client(client->id);
            break;
        } else {
            perror("ERROR: recv failed");
            break;
        }
    }

    pthread_exit(NULL);
}

/**
 * @brief Main function that starts the server and handles client connections.
 *
 * This function initializes the server, accepts client connections in a loop, 
 * and spawns a new thread to handle each client's communication. The server runs 
 * until it is manually shut down.
 *
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line arguments (IP and port).
 * @return int Returns EXIT_SUCCESS on successful execution or EXIT_FAILURE on error.
 */
int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Usage: %s <ip> <port>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *ip = argv[1];
    int port = atoi(argv[2]);

    start_server(ip, port);

    while (1) {
        struct sockaddr_in cli_addr;
        int client_socket_fd = accept_client(server_socket_fd, &cli_addr);
        if (client_socket_fd != -1) {
            client_t *new_client = (client_t *)malloc(sizeof(client_t));
            new_client->address = cli_addr;
            new_client->sockfd = client_socket_fd;
            new_client->id = client_socket_fd;  
            strncpy(new_client->status, "ACTIVE", sizeof(new_client->status) - 1);
            new_client->status[sizeof(new_client->status) - 1] = '\0';  // Asegura que esté null-terminated
            add_client(new_client);

            pthread_t tid;
            pthread_create(&tid, NULL, client_handler, (void *)new_client);
        }
    }

    shutdown_server();
    return EXIT_SUCCESS;
}
