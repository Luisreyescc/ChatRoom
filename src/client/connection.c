/**
 * @file connection.c
 * @brief Manages the connection between the client and the server.
 */
#include "connection.h"
#include <unistd.h>

int sockfd = 0;  
char user_name[32];  
volatile sig_atomic_t indicator = 0;  

/**
 * @brief Connects the client to the server.
 *
 * Establishes the connection between the client and the server using the provided IP address and port.
 *
 * @param ip The server's IP address.
 * @param port The port where the server is listening for connections.
 *
 * @return void
 */
void connect_to_server(const char *ip, int port) {
    struct sockaddr_in server_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket error");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_port = htons(port);

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect error");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Closes the client's socket connection.
 *
 * Shuts down the communication socket and cleans up resources.
 *
 * @return void
 */
void close_connection() {
    close(sockfd);
    printf("\nDisconnected from server.\n");
}

/**
 * @brief Trim the new line character.
 *
 * @return void
 */
void new_line_trim(char* arr, int length) {
    for (int i = 0; i < length; i++) {
        if (arr[i] == '\n') {
            arr[i] = '\0';
            break;
        }
    }
}
