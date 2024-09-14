/**
 * @file connection.h
 * @brief Defines server connection management functions.
 *
 * This header file declares functions for starting a server, accepting incoming client connections, 
 * and shutting down the server. It includes the necessary external variables and function prototypes 
 * to manage socket connections in a server application.
 */
#ifndef CONNECTION_H
#define CONNECTION_H

#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

extern int server_socket_fd; 

void start_server(const char *ip, int port);
int accept_client(int server_socket_fd, struct sockaddr_in *cli_addr);
void shutdown_server();

#endif // CONNECTION_H
