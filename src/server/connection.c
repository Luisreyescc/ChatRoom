/**
 * @file connection.c
 * @brief Implements server-side connection handling.
 *
 * This file contains functions to start a server, accept incoming client connections, 
 * and shut down the server. It manages socket creation, binding, and listening on a specified 
 * IP address and port.
 */
#include "connection.h"
#include <unistd.h>

int server_socket_fd = 0;

/**
 * @brief Starts the server and binds it to the specified IP and port.
 *
 * This function creates a socket, sets socket options, binds the socket to the provided IP 
 * address and port, and starts listening for incoming connections. If any step fails, the 
 * function prints an error and exits the program.
 *
 * @param ip The IP address the server should bind to (in dotted decimal format).
 * @param port The port number the server should listen on.
 *
 * @return void
 */
void start_server(const char *ip, int port) {
    struct sockaddr_in server_addr;
    int option = 1;

    server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_fd < 0) {
        perror("ERROR: socket error");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_port = htons(port);

    if (setsockopt(server_socket_fd, SOL_SOCKET, (SO_REUSEPORT | SO_REUSEADDR), (char *)&option, sizeof(option)) < 0) {
        perror("ERROR: setsockopt failed");
        exit(EXIT_FAILURE);
    }

    if (bind(server_socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("ERROR: Socket binding failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket_fd, 10) < 0) {
        perror("ERROR: Socket listening failed");
        exit(EXIT_FAILURE);
    }

    printf("Server started. Listening on %s:%d\n", ip, port);
}

/**
 * @brief Accepts an incoming client connection.
 *
 * This function accepts a new client connection from the server's listening socket. 
 * It returns the socket file descriptor for the client connection, which can then be used 
 * to communicate with the client.
 *
 * @param server_socket_fd The file descriptor for the server's listening socket.
 * @param cli_addr Pointer to a sockaddr_in structure that will hold the client's address.
 *
 * @return int The file descriptor for the accepted client socket, or -1 on error.
 */
int accept_client(int server_socket_fd, struct sockaddr_in *cli_addr) {
    socklen_t clilen = sizeof(*cli_addr);
    int client_socket_fd = accept(server_socket_fd, (struct sockaddr *)cli_addr, &clilen);
    if (client_socket_fd < 0) {
        perror("ERROR: accept client failed");
        return -1;
    }
    return client_socket_fd;
}

/**
 * @brief Shuts down the server.
 *
 * This function closes the server's listening socket, shutting down the server. 
 * It ensures the socket is properly closed and resources are released.
 *
 * @return void
 */
void shutdown_server() {
    if (server_socket_fd > 0) {
        close(server_socket_fd);
        printf("Server shut down.\n");
    }
}
