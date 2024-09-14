/**
 * @file client_manager.c
 * @brief Manages the list of clients on the server.
 */
#include "client_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

client_t *clients[MAX_CLIENTS];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * @brief Adds a client to the list of connected clients.
 *
 * Adds a client to the list of active clients on the server.
 *
 * @param client A pointer to the client to add.
 *
 * @return void
 */
void add_client(client_t *client) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (!clients[i]) {
            clients[i] = client;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

/**
 * @brief Removes a client from the list of connected clients.
 *
 * Removes a client from the list of active clients based on their ID.
 *
 * @param id The ID of the client to remove.
 *
 * @return void
 */
void remove_client(int id) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i] && clients[i]->id == id) {
            clients[i] = NULL;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

/**
 * @brief Broadcasts a message to all connected clients except the sender.
 *
 * This function sends a message to all connected clients, excluding the client
 * identified by `sender_id`. The message is sent through the socket of each client.
 * If a client socket fails during the transmission, the client is removed from the
 * list of connected clients.
 *
 * @param message A string containing the message to broadcast.
 * @param sender_id The ID of the client that sent the message (will not receive the broadcast).
 *
 * @return void
 */
void broadcast_message(const char *message, int sender_id) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i] && clients[i]->id != sender_id) {
            if (write(clients[i]->sockfd, message, strlen(message)) < 0) {
                perror("ERROR: write to descriptor failed");
                close(clients[i]->sockfd);
                remove_client(clients[i]->id);
            }
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}
