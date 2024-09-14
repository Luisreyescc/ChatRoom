/**
 * @file messaging.h
 * @brief Declares the functions and thread variables for message sending and receiving.
 *
 * This header file defines the threads and functions responsible for sending and receiving 
 * messages between the client and server in a concurrent manner.
 */

#ifndef MESSAGING_H
#define MESSAGING_H

#include <pthread.h>

extern pthread_t send_msg_thread; /**< Thread used for sending messages to the server. */
extern pthread_t recv_msg_thread; /**< Thread used for receiving messages from the server. */

/**
 * @brief Function responsible for sending messages to the server.
 *
 * This function is designed to run in a separate thread. It handles reading user input 
 * and sending it to the server through the established socket connection.
 *
 * @return void* A pointer to the result of the thread execution (usually NULL).
 */
void* send_msg();

/**
 * @brief Function responsible for receiving messages from the server.
 *
 * This function is designed to run in a separate thread. It continuously listens for 
 * incoming messages from the server and displays them to the user.
 *
 * @return void* A pointer to the result of the thread execution (usually NULL).
 */
void* recv_msg();

#endif // MESSAGING_H
