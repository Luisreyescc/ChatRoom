#include "connection.h"
#include "client_manager.h"
#include "messaging.h"
#include <pthread.h>
#include <unistd.h>

void *client_handler(void *arg) {
    client_t *client = (client_t *)arg;
    char buffer[2048];

    while (1) {
        int receive = recv(client->sockfd, buffer, 2048, 0);
        if (receive > 0) {
            buffer[receive] = '\0';
            process_client_message(client, buffer);
        } else if (receive == 0) {
            printf("Client %s disconnected.\n", client->user_name);
            close(client->sockfd);
            remove_client(client->id);
            break;
        } else {
            perror("ERROR: recv failed");
            break;
        }
    }

    pthread_exit(NULL);
}

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Usage: %s <ip> <port>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *ip = argv[1];
    int port = atoi(argv[2]);

    start_server(ip, port);

    while (1) {
        struct sockaddr_in cli_addr;
        int client_socket_fd = accept_client(server_socket_fd, &cli_addr);
        if (client_socket_fd != -1) {
            client_t *new_client = (client_t *)malloc(sizeof(client_t));
            new_client->address = cli_addr;
            new_client->sockfd = client_socket_fd;
            new_client->id = client_socket_fd;  

            add_client(new_client);

            pthread_t tid;
            pthread_create(&tid, NULL, client_handler, (void *)new_client);
        }
    }

    shutdown_server();
    return EXIT_SUCCESS;
}
