#include "client.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/**
 * Allocates memory for a `Client`, creates a TCP socket, and sets up the 
 * server address with the provided IP and port.
 *
 * @param ip The IP address of the server as a string.
 * @param port The port number on which the server is listening.
 * @return A pointer to the newly created `Client` instance, or NULL if an error occurred.
 */
Client *Client_create(char *ip, int port) {
    Client *client = (Client *)malloc(sizeof(Client));
    if (!client) {
        perror("Error allocating memory for Client");
        return NULL;
    }

    client->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client->socket_fd < 0) {
        perror("Error creating socket");
        free(client);
        return NULL;
    }

    client->server_addr.sin_family = AF_INET;
    client->server_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &client->server_addr.sin_addr.s_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        close(client->socket_fd);
        free(client);
        return NULL;
    }

    return client;
}

int main() {
    return 0;
}
