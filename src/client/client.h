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

#endif // CLIENT_H
