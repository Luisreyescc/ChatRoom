#include "client.h"
// Global variables
volatile sig_atomic_t indicator = 0;  
int sockfd = 0;
char user_name[32];

pthread_t send_msg_thread;
pthread_t recv_msg_thread;

/**
 * @brief Trims the newline character from the end of a string.
 *
 * This function modifies the input string by replacing the newline character
 * with a null terminator if it is present.
 *
 * @param arr Pointer to the string to be trimmed.
 * @param length Length of the string.
 */
void new_line_trim(char* arr, int length) {
    for (int i = 0; i < length; i++) {
        if (arr[i] == '\n') {
            arr[i] = '\0';
            break;
        }
    }
}

/**
 * @brief Thread function to handle sending messages.
 *
 * This function runs in a separate thread and handles sending messages from
 * the client to the server. It reads input from stdin and sends it to the server.
 *
 * @return Pointer to the result (NULL).
 */
void* send_msg() {
    char message[LENGTH] = {};
    char buffer[LENGTH + 32] = {};

    while (!indicator) {
        if (fgets(message, LENGTH, stdin) == NULL) {
            if (feof(stdin)) {
                break;  
            } else {
                continue;  
            }
        }

        new_line_trim(message, LENGTH);

        if (strcmp(message, "exit") == 0) {
            break;
        } else {
            sprintf(buffer, "%s 🗣️: %s\n", user_name, message);
            if (send(sockfd, buffer, strlen(buffer), 0) == -1) {
                perror("send error");
                break;  
            }
        }

        bzero(message, LENGTH);
        bzero(buffer, LENGTH + 32);
    }

    pthread_exit(NULL);  
}

/**
 * @brief Thread function to handle receiving messages.
 *
 * This function runs in a separate thread and handles receiving messages from
 * the server and printing them to the console.
 *
 * @return Pointer to the result (NULL).
 */
void* recv_msg() {
    char message[LENGTH] = {};
    while (!indicator) {
        int receive = recv(sockfd, message, LENGTH, 0);
        if (receive > 0) {
            printf("%s", message);
            memset(message, 0, sizeof(message));  
        } else if (receive == 0) {
            printf("\nConnection closed by the server or client.\n");
            break;  
        } else {
            if (indicator) break;  
            perror("recv error");
            break;
        }
    }
    pthread_exit(NULL);  
}

