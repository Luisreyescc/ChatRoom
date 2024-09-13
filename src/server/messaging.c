#include "messaging.h"
#include "../libs/cJSON/cJSON.h"
#include <string.h>
#include <stdio.h>

void process_client_message(client_t *client, const char *message) {
    printf("Server received raw JSON from %s: %s\n", client->user_name[0] ? client->user_name : "(Unknown)", message);

    cJSON *json_msg = cJSON_Parse(message);

    if (json_msg != NULL) {
        cJSON *type = cJSON_GetObjectItemCaseSensitive(json_msg, "type");

        if (cJSON_IsString(type)) {
            if (strcmp(type->valuestring, "IDENTIFY") == 0) {
                // Almacenar el nombre de usuario en la estructura del cliente
                cJSON *username = cJSON_GetObjectItemCaseSensitive(json_msg, "username");
                if (cJSON_IsString(username) && username->valuestring != NULL) {
                    strncpy(client->user_name, username->valuestring, sizeof(client->user_name) - 1);
                    client->user_name[sizeof(client->user_name) - 1] = '\0';  // Asegurar terminación nula
                    printf("User identified as %s\n", client->user_name);

                    // Enviar mensaje de respuesta al cliente
                    cJSON *json_response = cJSON_CreateObject();
                    cJSON_AddStringToObject(json_response, "type", "RESPONSE");
                    cJSON_AddStringToObject(json_response, "operation", "IDENTIFY");
                    cJSON_AddStringToObject(json_response, "result", "SUCCESS");
                    cJSON_AddStringToObject(json_response, "extra", client->user_name);
                    char *response_str = cJSON_PrintUnformatted(json_response);

                    if (write(client->sockfd, response_str, strlen(response_str)) < 0) {
                        perror("ERROR: write to descriptor failed");
                    }
                    free(response_str);
                    cJSON_Delete(json_response);

                    // Enviar notificación a los demás clientes
                    cJSON *json_new_user = cJSON_CreateObject();
                    cJSON_AddStringToObject(json_new_user, "type", "NEW_USER");
                    cJSON_AddStringToObject(json_new_user, "username", client->user_name);
                    char *new_user_str = cJSON_PrintUnformatted(json_new_user);

                    broadcast_message(new_user_str, client->id);  // Notificar a los demás clientes
                    free(new_user_str);
                    cJSON_Delete(json_new_user);
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
            }
        }
        cJSON_Delete(json_msg);
    } else {
        printf("Error parsing message from client %d\n", client->id);
    }
}



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
