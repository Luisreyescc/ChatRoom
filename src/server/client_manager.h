#ifndef CLIENT_MANAGER_H
#define CLIENT_MANAGER_H

#include <pthread.h>
#include <arpa/inet.h>

#define MAX_CLIENTS 100

typedef struct {
    struct sockaddr_in address;
    int sockfd;
    int id;
    char user_name[32];
    char status[16];
} client_t;

extern client_t *clients[MAX_CLIENTS];
extern pthread_mutex_t clients_mutex;

void add_client(client_t *client);
void remove_client(int id);
void broadcast_message(const char *message, int sender_id);

#endif // CLIENT_MANAGER_H
