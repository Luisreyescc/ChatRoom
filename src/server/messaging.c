/**
 * @file messaging.c
 * @brief Manages messaging on the server.
 */
#include "messaging.h"
#include "../libs/cJSON/cJSON.h"
#include <string.h>
#include <stdio.h>

/**
 * @brief Processes messages received from clients.
 *
 * This function parses the received JSON message from the client and performs actions
 * based on the message type (identify, public text, private message, status, etc.).
 *
 * @param client A pointer to the client structure that sent the message.
 * @param message A string containing the received JSON message.
 *
 * @return void
 */
void process_client_message(client_t *client, const char *message) {
    printf("Server received raw JSON from %s: %s\n", client->user_name[0] ? client->user_name : "(Unknown)", message);

    cJSON *json_msg = cJSON_Parse(message);

    if (json_msg != NULL) {
        cJSON *type = cJSON_GetObjectItemCaseSensitive(json_msg, "type");

        if (cJSON_IsString(type)) {
            if (strcmp(type->valuestring, "IDENTIFY") == 0) {
                cJSON *username = cJSON_GetObjectItemCaseSensitive(json_msg, "username");
                if (cJSON_IsString(username) && username->valuestring != NULL) {
                    if (is_username_taken(username->valuestring)) {
                        cJSON *json_response = cJSON_CreateObject();
                        cJSON_AddStringToObject(json_response, "type", "RESPONSE");
                        cJSON_AddStringToObject(json_response, "operation", "IDENTIFY");
                        cJSON_AddStringToObject(json_response, "result", "USER_ALREADY_EXISTS");
                        cJSON_AddStringToObject(json_response, "extra", username->valuestring);
                        char *response_str = cJSON_PrintUnformatted(json_response);
                        send(client->sockfd, response_str, strlen(response_str), 0);
                        free(response_str);
                        cJSON_Delete(json_response);
                        close(client->sockfd);
                        remove_client(client->id);
                        return;
                    } else {
                        strncpy(client->user_name, username->valuestring, sizeof(client->user_name) - 1);
                        client->user_name[sizeof(client->user_name) - 1] = '\0';  
                        printf("User correctly identified as %s\n", client->user_name);

                        cJSON *json_response = cJSON_CreateObject();
                        cJSON_AddStringToObject(json_response, "type", "RESPONSE");
                        cJSON_AddStringToObject(json_response, "operation", "IDENTIFY");
                        cJSON_AddStringToObject(json_response, "result", "SUCCESS");
                        cJSON_AddStringToObject(json_response, "extra", client->user_name);
                        char *response_str = cJSON_PrintUnformatted(json_response);

                        send(client->sockfd, response_str, strlen(response_str), 0);

                        free(response_str);
                        cJSON_Delete(json_response);
                    }
                }
            } else if (strcmp(type->valuestring, "PUBLIC_TEXT") == 0) {
                cJSON *text = cJSON_GetObjectItemCaseSensitive(json_msg, "text");
                if (cJSON_IsString(text)) {
                    printf("Server received from %s: %s\n", client->user_name, text->valuestring);
                    send_public_message(text->valuestring, client->user_name);
                }

            } else if (strcmp(type->valuestring, "TEXT") == 0) {
                cJSON *username = cJSON_GetObjectItemCaseSensitive(json_msg, "username");
                cJSON *text = cJSON_GetObjectItemCaseSensitive(json_msg, "text");
                if (cJSON_IsString(username) && cJSON_IsString(text)) {
                    printf("Server received private message from %s to %s: %s\n", client->user_name, username->valuestring, text->valuestring);
                    send_private_message(client, text->valuestring, client->user_name, username->valuestring);
                }

            } else if (strcmp(type->valuestring, "STATUS") == 0) {
                cJSON *status = cJSON_GetObjectItemCaseSensitive(json_msg, "status");
                if (cJSON_IsString(status)) {
                    change_user_status(client, status->valuestring);
                }

            } else if (strcmp(type->valuestring, "USERS") == 0) {
                send_user_list(client);
            } else if (strcmp(type->valuestring, "DISCONNECT") == 0) {
                printf("❌ %s is disconnecting...\n", client->user_name);

                notify_disconnected(client);
                remove_client(client->id);

                close(client->sockfd);
                return;
            }
        }
        cJSON_Delete(json_msg);
    } else {
        printf("Error parsing message from client %d\n", client->id);
    }
}

/**
 * @brief Notifies all clients when a user disconnects.
 *
 * Creates a JSON message with the name of the user who has disconnected
 * and broadcasts it to all other connected clients.
 *
 * @param client A pointer to the client who has disconnected.
 *
 * @return void
 */
void notify_disconnected(client_t *client) {
    cJSON *json_disconnected = cJSON_CreateObject();
    cJSON_AddStringToObject(json_disconnected, "type", "DISCONNECTED");
    cJSON_AddStringToObject(json_disconnected, "username", client->user_name);  // Añadir el nombre de usuario
    char *disconnected_str = cJSON_PrintUnformatted(json_disconnected);

    broadcast_message(disconnected_str, client->id);  // Excluir al cliente que se desconecta

    free(disconnected_str);
    cJSON_Delete(json_disconnected);
}

/**
 * @brief Sends a public message to all connected clients.
 *
 * Creates a JSON message with the text and the name of the user who sent it,
 * and broadcasts it to all connected clients.
 *
 * @param text The public message text.
 * @param username The name of the user sending the message.
 *
 * @return void
 */
void send_public_message(const char *text, const char *username) {
    cJSON *json_message = cJSON_CreateObject();
    cJSON_AddStringToObject(json_message, "type", "PUBLIC_TEXT_FROM");
    cJSON_AddStringToObject(json_message, "username", username);
    cJSON_AddStringToObject(json_message, "text", text);
    char *json_message_str = cJSON_PrintUnformatted(json_message);

    broadcast_message(json_message_str, -1);  

    free(json_message_str);
    cJSON_Delete(json_message);
}

/**
 * @brief Sends a private message to a specific client.
 *
 * Creates a private message in JSON format and sends it to the intended recipient.
 * If the recipient does not exist, the sending client is notified.
 *
 * @param client A pointer to the client sending the message.
 * @param text The private message text.
 * @param from_username The name of the user sending the message.
 * @param to_username The name of the user receiving the message.
 *
 * @return void
 */
void send_private_message(client_t *client, const char *text, const char *from_username, const char *to_username) {
    client_t *recipient = find_client_by_username(to_username);

    if (recipient) {
        cJSON *json_message = cJSON_CreateObject();
        cJSON_AddStringToObject(json_message, "type", "TEXT_FROM");
        cJSON_AddStringToObject(json_message, "username", from_username);
        cJSON_AddStringToObject(json_message, "text", text);
        char *json_message_str = cJSON_PrintUnformatted(json_message);

        if (write(recipient->sockfd, json_message_str, strlen(json_message_str)) < 0) {
            perror("ERROR: write to descriptor failed");
        }

        free(json_message_str);
        cJSON_Delete(json_message);
    } else {
        cJSON *response = cJSON_CreateObject();
        cJSON_AddStringToObject(response, "type", "RESPONSE");
        cJSON_AddStringToObject(response, "operation", "TEXT");
        cJSON_AddStringToObject(response, "result", "NO_SUCH_USER");
        cJSON_AddStringToObject(response, "extra", to_username);
        char *response_str = cJSON_PrintUnformatted(response);

        if (write(client->sockfd, response_str, strlen(response_str)) < 0) {
            perror("ERROR: write to descriptor failed");
        }

        free(response_str);
        cJSON_Delete(response);
    }
}

/**
 * @brief Updates a client's status.
 *
 * Changes the status of the client and broadcasts the new status to all connected clients.
 *
 * @param client A pointer to the client whose status is being updated.
 * @param status The new status of the client.
 *
 * @return void
 */
void change_user_status(client_t *client, const char *status) {
    strncpy(client->status, status, sizeof(client->status) - 1);
    client->status[sizeof(client->status) - 1] = '\0';  

    cJSON *json_status = cJSON_CreateObject();
    cJSON_AddStringToObject(json_status, "type", "NEW_STATUS");
    cJSON_AddStringToObject(json_status, "username", client->user_name);
    cJSON_AddStringToObject(json_status, "status", status);
    char *json_status_str = cJSON_PrintUnformatted(json_status);

    broadcast_message(json_status_str, -1);  

    free(json_status_str);
    cJSON_Delete(json_status);
}

/**
 * @brief Sends the list of connected users to a client.
 *
 * Sends a list of connected users and their respective statuses
 * to the client who requested it.
 *
 * @param client A pointer to the client requesting the user list.
 *
 * @return void
 */
void send_user_list(client_t *client) {
    cJSON *json_users = cJSON_CreateObject();
    cJSON_AddStringToObject(json_users, "type", "USER_LIST");

    cJSON *users = cJSON_CreateObject();

    pthread_mutex_lock(&clients_mutex);  
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i]) {
            cJSON_AddStringToObject(users, clients[i]->user_name, clients[i]->status);
        }
    }
    pthread_mutex_unlock(&clients_mutex);

    cJSON_AddItemToObject(json_users, "users", users);

    char *json_users_str = cJSON_PrintUnformatted(json_users);
    if (write(client->sockfd, json_users_str, strlen(json_users_str)) < 0) {
        perror("ERROR: write to descriptor failed");
    }

    free(json_users_str);
    cJSON_Delete(json_users);
}

/**
 * @brief Finds a client by username.
 *
 * Searches the list of connected clients to find a client with the matching username.
 *
 * @param username The username to search for.
 * @return client_t* A pointer to the client found, or NULL if no match is found.
 */
client_t *find_client_by_username(const char *username) {
    client_t *client = NULL;
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i] && strcmp(clients[i]->user_name, username) == 0) {
            client = clients[i];
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
    return client;
}

/**
 * @brief Checks if a username is already in use.
 *
 * This function searches through the list of connected clients to verify if
 * a username is already being used by another client.
 *
 * @param username The username to check.
 * @return int Returns 1 if the username is in use, 0 otherwise.
 */
int is_username_taken(const char *username) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i] && strcmp(clients[i]->user_name, username) == 0) {
            pthread_mutex_unlock(&clients_mutex);
            return 1;  
        }
    }
    pthread_mutex_unlock(&clients_mutex);
    return 0;  
}
