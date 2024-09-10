/**
 * @file client.h
 * @brief Header file for the chat client.
 *
 * This file contains the declarations for the chat client program,
 * including global variables, function prototypes, and necessary includes.
 */

#ifndef CHAT_CLIENT_H
#define CHAT_CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define LENGTH 2048 /**< Maximum length for messages */

/** Global variables */
extern volatile sig_atomic_t indicator; /**< Flag to indicate SIGINT signal reception */
extern int sockfd; /**< Socket file descriptor */
extern char user_name[32]; /**< Username */

/**
 * @brief Trims the newline character from the end of a string.
 *
 * This function modifies the input string by replacing the newline character
 * with a null terminator if it is present.
 *
 * @param arr Pointer to the string to be trimmed.
 * @param length Length of the string.
 */
void new_line_trim(char* arr, int length);

/**
 * @brief Thread function to handle sending messages.
 *
 * This function runs in a separate thread and handles sending messages from
 * the client to the server.
 *
 * @return Pointer to the result (NULL).
 */
void soft_exit();

/**
 * @brief Thread function to handle sending messages.
 *
 * This function runs in a separate thread and handles sending messages from
 * the client to the server.
 *
 * @return Pointer to the result (NULL).
 */
void* send_msg();

/**
 * @brief Thread function to handle receiving messages.
 *
 * This function runs in a separate thread and handles receiving messages
 * from the server and printing them to the console.
 *
 * @return Pointer to the result (NULL).
 */
void* recv_msg();

#endif // CHAT_CLIENT_H
