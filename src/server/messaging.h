#ifndef MESSAGING_H
#define MESSAGING_H

#include "client_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 

void process_client_message(client_t *client, const char *message);
void send_public_message(const char *text, const char *username);
void process_client_message(client_t *client, const char *message);
void send_public_message(const char *text, const char *username);
void send_private_message(client_t *client, const char *text, const char *from_username, const char *to_username);
void change_user_status(client_t *client, const char *status);
void send_user_list(client_t *client);
client_t *find_client_by_username(const char *username);

#endif // MESSAGING_H
