#include "connection.h"
#include <unistd.h>

int server_socket_fd = 0;

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

int accept_client(int server_socket_fd, struct sockaddr_in *cli_addr) {
    socklen_t clilen = sizeof(*cli_addr);
    int client_socket_fd = accept(server_socket_fd, (struct sockaddr *)cli_addr, &clilen);
    if (client_socket_fd < 0) {
        perror("ERROR: accept client failed");
        return -1;
    }
    return client_socket_fd;
}

void shutdown_server() {
    if (server_socket_fd > 0) {
        close(server_socket_fd);
        printf("Server shut down.\n");
    }
}
