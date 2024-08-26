#include "client.h"
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
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

/**
 * Establishes a connection to the server specified in the `Client` structure.
 *
 * This function attempts to connect the TCP socket (created and configured in 
 * `Client_create`) to the server using the address and port specified in the 
 * `Client` structure. If the connection is successful, the socket is ready for 
 * communication. If the connection fails, an error message is printed to stderr.
 *
 * @param client A pointer to a `Client` structure that contains the socket 
 *               file descriptor and the server's address information.
 * @return An integer representing the success or failure of the connection:
 *         - 0 on success.
 *         - -1 on failure, in which case an error message is printed and 
 *           `errno` is set appropriately.
 */
int Client_connect(Client *client) {
    int result = connect(client->socket_fd, (struct sockaddr *)&client->server_addr, sizeof(client->server_addr));
    if (result < 0) {
        perror("Connection failed");
    }
    return result;
}

/**
 * This function sends a message using the socket associated with the client.
 * It uses the system's `send()` function to transmit the message.
 *
 * @param client A pointer to the `Client` structure containing the socket descriptor.
 * @param message A pointer to a character string representing the message to be sent.
 * 
 * @return An integer value indicating the result of the operation.
 *         - Returns the number of bytes sent on success.
 *         - Returns -1 on error.
 */
int Client_send(Client *client, const char *message){
    return send(client->socket_fd, message, strlen(message), 0);
}


/**
 * This function receives a message from the server using the socket associated with the client.
 * It uses the system's `recv()` function to read the message.
 *
 * @param client A pointer to the `Client` structure containing the socket descriptor.
 * @param buffer A pointer to a character array where the received message will be stored.
 * @param size The maximum number of bytes to be received and stored in the buffer.
 * 
 * @return An integer value indicating the result of the operation.
 *         - Returns the number of bytes received on success.
 *         - Returns 0 if the connection has been closed by the server.
 *         - Returns -1 on error.
 */
int Client_receive(Client *client, char *buffer, size_t size){
    return recv(client->socket_fd, buffer, size, 0);
}


/**
 * This function releases the resources associated with the client.
 * It closes the socket and frees the memory allocated for the `Client` structure.
 *
 * @param client A pointer to the `Client` structure to be destroyed.
 * 
 * @note This function should be called when the client is no longer needed
 *       to avoid memory leaks and ensure proper resource cleanup.
 */
void Client_destroy(Client *client){
    if (client){
        close(client->socket_fd);
        free(client);
    }
}




