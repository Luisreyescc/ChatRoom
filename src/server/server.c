/**
 * @file server.c
 * @brief Implementation of server functionality.
 *
 * This file contains the implementation of functions for handling client connections,
 * sending messages, and server shutdown. It manages connected users and processes
 * client sessions in a multi-threaded environment.
 */

#include "server.h"

client_t *connected_users[MAX_CLIENTS]; /**< Array of pointers to connected clients. */
pthread_mutex_t user_list_mutex = PTHREAD_MUTEX_INITIALIZER; /**< Mutex for synchronizing access to the user list. */

_Atomic unsigned int total_active_users = 0; /**< Atomic variable for tracking the number of active users. */
int i = 10; /**< Index used in the server (initial value). */
int server_socket_fd = 0; /**< File descriptor for the server socket. */

/**
 * @brief Trims trailing newline characters from a string.
 *
 * This function removes newline characters from the end of a string up to a specified length.
 *
 * @param arr The string to be trimmed.
 * @param length The length of the string.
 */
void new_line_trim(char* arr, int length) {
    for (int i = 0; i < length; i++) { 
        if (arr[i] == '\n') {
            arr[i] = '\0';
            break;
        }
    }
}

/**
 * @brief Handles server shutdown when a termination signal is received.
 *
 * This function closes the server socket and exits the program.
 *
 * @param sig The signal number (not used in this function).
 */
void shutdown_handler(int sig) {
    (void)sig; 
    printf("\nShutting down server...\n");

    if (server_socket_fd > 0) {
        close(server_socket_fd);
    }

    exit(EXIT_SUCCESS);
}

/**
 * @brief Prints the IP address of a user.
 *
 * This function prints the IP address from a sockaddr_in structure in a human-readable format.
 *
 * @param addr The sockaddr_in structure containing the address information.
 */
void print_user_address(struct sockaddr_in addr) {
    printf("%d.%d.%d.%d",
        addr.sin_addr.s_addr & 0xff,
        (addr.sin_addr.s_addr & 0xff00) >> 8,
        (addr.sin_addr.s_addr & 0xff0000) >> 16,
        (addr.sin_addr.s_addr & 0xff000000) >> 24);
}

/**
 * @brief Adds a client to the queue of connected users.
 *
 * This function adds a client to the array of connected users while holding the user list mutex.
 *
 * @param cl Pointer to the client structure to be added.
 */
void add_user_to_queue(client_t *cl) {
    pthread_mutex_lock(&user_list_mutex);

    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (!connected_users[i]) {
            connected_users[i] = cl;
            break;
        }
    }

    pthread_mutex_unlock(&user_list_mutex);
}

/**
 * @brief Removes a client from the queue of connected users.
 *
 * This function removes a client from the array of connected users based on the given index,
 * while holding the user list mutex.
 *
 * @param id The index of the client to be removed.
 */
void remove_user_from_queue(int id) {
    pthread_mutex_lock(&user_list_mutex);

    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (connected_users[i]) {
            if (connected_users[i]->i == id) {
                connected_users[i] = NULL;
                break;
            }
        }
    }

    pthread_mutex_unlock(&user_list_mutex);
}

/**
 * @brief Sends a message to all connected clients except the sender.
 *
 * This function sends a message to all clients in the connected users array,
 * except the client with the specified index. It handles errors in message writing.
 *
 * @param s The message to be sent.
 * @param sender_i The index of the client who sent the message.
 */
void send_message(char *s, int sender_i) {
    pthread_mutex_lock(&user_list_mutex);

    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (connected_users[i]) {
            if (connected_users[i]->i != sender_i) {
                if (connected_users[i]->sockfd > 0) {
                    if (write(connected_users[i]->sockfd, s, strlen(s)) < 0) {
                        perror("ERROR: write to descriptor failed");
                        close(connected_users[i]->sockfd);
                        remove_user_from_queue(i);
                    }
                }
            }
        }
    }

    pthread_mutex_unlock(&user_list_mutex);
}

/**
 * @brief Handles communication with a client in a separate thread.
 *
 * This function manages the client session, including receiving and sending messages.
 * It handles client disconnection and updates the user list accordingly.
 *
 * @param arg Pointer to the client structure.
 * @return Pointer to the result (if any).
 */
void *client_session_handler(void *arg) {
    char message_buffer[BUFFER_SZ];
    char user_name[32];
    int disconnection_flag = 0;

    total_active_users++;
    client_t *cli = (client_t *)arg;

    if (recv(cli->sockfd, user_name, 32, 0) <= 0 || strlen(user_name) < 2 || strlen(user_name) >= 32 - 1) {
        printf("Didn't enter the user_name.\n");
        disconnection_flag = 1;
    } else {
        strcpy(cli->user_name, user_name);
        sprintf(message_buffer, "%s has joined\n", cli->user_name);
        printf("%s", message_buffer);
        send_message(message_buffer, cli->i);
    }

    bzero(message_buffer, BUFFER_SZ);

    while (1) {
        if (disconnection_flag) {
            break;
        }

        int receive = recv(cli->sockfd, message_buffer, BUFFER_SZ, 0);
        if (receive > 0) {
            if (strlen(message_buffer) > 0) {
                send_message(message_buffer, cli->i);

                new_line_trim(message_buffer, strlen(message_buffer));
                printf("%s -> %s\n", message_buffer, cli->user_name);
            }
        } else if (receive == 0 || strcmp(message_buffer, "exit") == 0) {
            sprintf(message_buffer, "%s has left\n", cli->user_name);
            printf("%s", message_buffer);
            send_message(message_buffer, cli->i);
            disconnection_flag = 1;
        } else {
            printf("ERROR: -1\n");
            disconnection_flag = 1;
        }

        bzero(message_buffer, BUFFER_SZ);
    }

    close(cli->sockfd);
    remove_user_from_queue(cli->i);
    free(cli);
    total_active_users--;
    pthread_detach(pthread_self());

    return NULL;
}
