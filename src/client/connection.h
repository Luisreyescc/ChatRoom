/**
 * @file connection.h
 * @brief Defines the functions and variables for managing the client-server connection.
 *
 * This header file declares the functions and external variables used for establishing,
 * maintaining, and closing the connection between a client and a server. It also includes
 * utility functions to handle input sanitization and signals.
 */

#ifndef CONNECTION_H
#define CONNECTION_H

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <arpa/inet.h>

extern int sockfd;  
/**
 * @brief Socket file descriptor.
 *
 * This variable stores the file descriptor of the socket used to communicate with the server.
 */

extern char user_name[32];
/**
 * @brief Username of the client.
 *
 * This buffer stores the username entered by the client, which will be sent to the server.
 */

extern volatile sig_atomic_t indicator;
/**
 * @brief Signal indicator.
 *
 * This volatile variable is used to handle asynchronous signals safely. It can be used 
 * as a flag to detect specific signals during communication with the server.
 */

/**
 * @brief Establishes a connection to the server.
 *
 * This function creates a socket and connects the client to the server using the provided 
 * IP address and port number.
 *
 * @param ip The server's IP address as a string.
 * @param port The port number where the server is listening.
 *
 * @return void
 */
void connect_to_server(const char *ip, int port);

/**
 * @brief Closes the connection with the server.
 *
 * This function closes the socket and cleans up resources when the client disconnects from
 * the server.
 *
 * @return void
 */
void close_connection();

/**
 * @brief Removes the newline character from a string.
 *
 * This utility function trims the newline character `\n` from a string if it's present, 
 * replacing it with the null terminator `\0`.
 *
 * @param arr The string array to be trimmed.
 * @param length The length of the string.
 *
 * @return void
 */
void new_line_trim(char* arr, int length);

#endif // CONNECTION_H
