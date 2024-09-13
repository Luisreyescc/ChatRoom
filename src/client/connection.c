#include "connection.h"
#include <unistd.h>

int sockfd = 0;  
char user_name[32];  
volatile sig_atomic_t indicator = 0;  

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

    signal(SIGINT, handle_sigint);  
}

void close_connection() {
    close(sockfd);
    printf("\nDisconnected from server.\n");
}

void handle_sigint(int sig) {
    (void) sig; 
    indicator = 1;  
    close_connection();
    exit(EXIT_SUCCESS);
}

void new_line_trim(char* arr, int length) {
    for (int i = 0; i < length; i++) {
        if (arr[i] == '\n') {
            arr[i] = '\0';
            break;
        }
    }
}
