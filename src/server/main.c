#include "server.h"

/**
 * @brief Main entry point of the server program.
 *
 * This function initializes the server, sets up the socket, and listens for incoming client connections.
 * It handles incoming connections by creating a new thread for each client session.
 *
 * @param argc The number of command-line arguments.
 * @param argv The command-line arguments.
 * @return EXIT_SUCCESS if the server starts successfully, EXIT_FAILURE otherwise.
 */
int main(int argc, char **argv) {
    int server_socket_fd = 0;
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *ip = "127.0.0.1";
    int port = atoi(argv[1]);
    int option = 1;
    struct sockaddr_in serv_addr;
    struct sockaddr_in cli_addr;
    pthread_t tid;

    server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(ip);
    serv_addr.sin_port = htons(port);

    signal(SIGINT, shutdown_handler);

    signal(SIGPIPE, SIG_IGN);

    if (setsockopt(server_socket_fd, SOL_SOCKET, (SO_REUSEPORT | SO_REUSEADDR), (char *)&option, sizeof(option)) < 0) {
        perror("ERROR: setsockopt failed");
        return EXIT_FAILURE;
    }

    if (bind(server_socket_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR: Socket binding failed");
        return EXIT_FAILURE;
    }

    if (listen(server_socket_fd, 10) < 0) {
        perror("ERROR: Socket listening failed");
        return EXIT_FAILURE;
    }

    printf("\033[34mWelcome!\033[0m\n");

    while (1) {
        socklen_t clilen = sizeof(cli_addr);
        int connfd = accept(server_socket_fd, (struct sockaddr*)&cli_addr, &clilen);

        if ((total_active_users + 1) == MAX_CLIENTS) {
            printf("Max clients reached. Rejected: ");
            print_user_address(cli_addr);
            printf(":%d\n", cli_addr.sin_port);
            close(connfd);
            continue;
        }

        client_t *cli = (client_t *)malloc(sizeof(client_t));
        cli->address = cli_addr;
        cli->sockfd = connfd;
        cli->i = i++;

        add_user_to_queue(cli);
        pthread_create(&tid, NULL, &client_session_handler, (void*)cli);

        sleep(1);
    }

    return EXIT_SUCCESS;
}
