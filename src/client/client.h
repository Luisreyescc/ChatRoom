#ifndef CLIENT_H
#define CLIENT_H

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>

/** 
 * @brief Defines a new data type called Client.
 */
typedef struct {
    int socket_fd; /**< Socket file descriptor for the client connection. */
    struct sockaddr_in server_addr; /**< Server address information. */
} Client;

/**
 * @brief Creates and initializes a new client.
 */
Client *Client_create(char *ip, int port);

/**
 * @brief Connects the client socket to the server.
 */
int Client_connect(Client *client);

/**
 * @brief Sends a message to the server.
 */
int Client_send(Client *client, const char *message);

/**
 * @brief Receives a message from server.
 */
int Client_receive(Client *client, char *buffer, size_t size);

/**
 * @brief Frees the resources allocated for the client.
 */
void Client_destroy(Client *client);

#endif // CLIENT_H
