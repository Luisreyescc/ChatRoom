/**
 * @file server.h
 * @brief Header file for the server implementation.
 *
 * This header file contains the necessary includes, defines, and function prototypes for the server.
 * It defines constants for maximum clients and buffer size, and provides a structure for client information.
 * It also declares functions for handling user connections, message sending, and server shutdown.
 */

#ifndef SERVER_H
#define SERVER_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <signal.h>

/** Maximum number of clients allowed. */
#define MAX_CLIENTS 100

/** Size of the buffer for messages. */
#define BUFFER_SZ 2048

/**
 * @struct client_t
 * @brief Structure to store information about a client.
 *
 * This structure holds information about a client including their socket file descriptor, address,
 * index, and user name.
 */
typedef struct {
    struct sockaddr_in address; /**< Client's address information. */
    int sockfd;                /**< Client's socket file descriptor. */
    int i;                     /**< Index of the client. */
    char user_name[32];        /**< User name of the client. */
} client_t;

/** Atomic variable to keep track of the total number of active users. */
extern _Atomic unsigned int total_active_users;

/** Index used for various purposes in the server. */
extern int i;

/**
 * @brief Trims trailing newline characters from a string.
 *
 * This function removes newline characters from the end of a string up to a specified length.
 *
 * @param arr The string to be trimmed.
 * @param length The length of the string.
 */
void new_line_trim(char* arr, int length);

/**
 * @brief Prints the address of a user.
 *
 * This function prints the IP address and port of a given sockaddr_in structure.
 *
 * @param addr The sockaddr_in structure containing the address information.
 */
void print_user_address(struct sockaddr_in addr);

/**
 * @brief Signal handler for server shutdown.
 *
 * This function handles the server shutdown process when a termination signal is received.
 *
 * @param sig The signal number.
 */
void shutdown_handler(int sig);

/**
 * @brief Adds a client to the queue.
 *
 * This function adds a client to the queue of active clients.
 *
 * @param cl Pointer to the client structure to be added.
 */
void add_user_to_queue(client_t *cl);

/**
 * @brief Removes a client from the queue.
 *
 * This function removes a client from the queue based on the given index.
 *
 * @param i The index of the client to be removed.
 */
void remove_user_from_queue(int i);

/**
 * @brief Sends a message to a client.
 *
 * This function sends a message to a specified client.
 *
 * @param s The message to be sent.
 * @param i The index of the client to receive the message.
 */
void send_message(char *s, int i);

/**
 * @brief Handles client sessions.
 *
 * This function runs in a separate thread to manage communication with a client.
 *
 * @param arg Pointer to the client structure.
 * @return Pointer to the result (if any).
 */
void *client_session_handler(void *arg);

#endif // SERVER_H
