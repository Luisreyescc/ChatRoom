#include "messaging.h"
#include "connection.h"
#include "../libs/cJSON/cJSON.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>

pthread_t send_msg_thread;
pthread_t recv_msg_thread;

void* send_msg() {
    char buffer[2048] = {};
    char message[2048] = {};

    cJSON *json_identify = cJSON_CreateObject();
    cJSON_AddStringToObject(json_identify, "type", "IDENTIFY");
    cJSON_AddStringToObject(json_identify, "username", user_name);
    const char *json_identify_string = cJSON_PrintUnformatted(json_identify);
    send(sockfd, json_identify_string, strlen(json_identify_string), 0);
    cJSON_Delete(json_identify);

    while (1) {
        if (fgets(message, 2048, stdin) == NULL) {
            if (feof(stdin)) {
                break;
            } else {
                continue;
            }
        }

        new_line_trim(message, 2048);

        if (message[0] == '/') {
            if (strncmp(message, "/public ", 8) == 0) {
                char *public_text = message + 8;
                cJSON *json_public = cJSON_CreateObject();
                cJSON_AddStringToObject(json_public, "type", "PUBLIC_TEXT");
                cJSON_AddStringToObject(json_public, "text", public_text);
                const char *json_public_string = cJSON_PrintUnformatted(json_public);
                send(sockfd, json_public_string, strlen(json_public_string), 0);
                cJSON_Delete(json_public);

            } else if (strncmp(message, "/status ", 8) == 0) {
                char *status = message + 8;
                cJSON *json_status = cJSON_CreateObject();
                cJSON_AddStringToObject(json_status, "type", "STATUS");
                cJSON_AddStringToObject(json_status, "status", status);
                const char *json_status_string = cJSON_PrintUnformatted(json_status);
                send(sockfd, json_status_string, strlen(json_status_string), 0);
                cJSON_Delete(json_status);

            } else if (strcmp(message, "/users") == 0) {
                cJSON *json_users = cJSON_CreateObject();
                cJSON_AddStringToObject(json_users, "type", "USERS");
                const char *json_users_string = cJSON_PrintUnformatted(json_users);
                send(sockfd, json_users_string, strlen(json_users_string), 0);
                cJSON_Delete(json_users);

            } else if (strncmp(message, "/private ", 9) == 0) {
                char *msg_parts = message + 9;
                char *recipient = strtok(msg_parts, " ");
                char *private_text = strtok(NULL, "");

                if (recipient && private_text) {
                    cJSON *json_private = cJSON_CreateObject();
                    cJSON_AddStringToObject(json_private, "type", "TEXT");
                    cJSON_AddStringToObject(json_private, "username", recipient);  
                    cJSON_AddStringToObject(json_private, "text", private_text);
                    const char *json_private_string = cJSON_PrintUnformatted(json_private);
                    send(sockfd, json_private_string, strlen(json_private_string), 0);
                    cJSON_Delete(json_private);
                } else {
                    printf("Usage: /private [username] [message]\n");
                }

            } else if (strcmp(message, "/exit") == 0) {
                indicator = 1;
                close_connection();
                break;

            } else {
                printf("Unknown command. Try again.\n");
            }
        } else {
            cJSON *json_public = cJSON_CreateObject();
            cJSON_AddStringToObject(json_public, "type", "PUBLIC_TEXT");
            cJSON_AddStringToObject(json_public, "text", message);
            const char *json_public_string = cJSON_PrintUnformatted(json_public);
            send(sockfd, json_public_string, strlen(json_public_string), 0);
            cJSON_Delete(json_public);
        }

        memset(buffer, 0, 2048);  
    }

    pthread_exit(NULL);
}

void* recv_msg() {
    char buffer[2048] = {};

    while (1) {
        int received = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        if (received > 0) {
            buffer[received] = '\0';
            /* Uncomment this line to see the raw json in the clients.*/
            printf("Raw JSON received from server: %s\n", buffer);

            cJSON *json_msg = cJSON_Parse(buffer);

            if (json_msg != NULL) {
                cJSON *type = cJSON_GetObjectItemCaseSensitive(json_msg, "type");

                if (cJSON_IsString(type)) {
                    if (strcmp(type->valuestring, "PUBLIC_TEXT_FROM") == 0) {
                        cJSON *username = cJSON_GetObjectItemCaseSensitive(json_msg, "username");
                        cJSON *text = cJSON_GetObjectItemCaseSensitive(json_msg, "text");
                        if (cJSON_IsString(username) && cJSON_IsString(text)) {
                            printf("📩 [Public] %s 🗣️: %s\n", username->valuestring, text->valuestring);
                        }

                    } else if (strcmp(type->valuestring, "TEXT_FROM") == 0) {
                        cJSON *username = cJSON_GetObjectItemCaseSensitive(json_msg, "username");
                        cJSON *text = cJSON_GetObjectItemCaseSensitive(json_msg, "text");
                        if (cJSON_IsString(username) && cJSON_IsString(text)) {
                            printf("📩 [Private] %s 🗣️: %s\n", username->valuestring, text->valuestring);
                        }

                    } else if (strcmp(type->valuestring, "NEW_STATUS") == 0) {
                        cJSON *username = cJSON_GetObjectItemCaseSensitive(json_msg, "username");
                        cJSON *status = cJSON_GetObjectItemCaseSensitive(json_msg, "status");
                        if (cJSON_IsString(username) && cJSON_IsString(status)) {
                            printf("🔄 %s is now %s\n", username->valuestring, status->valuestring);
                        }

                    } else if (strcmp(type->valuestring, "USER_LIST") == 0) {
                        cJSON *users = cJSON_GetObjectItemCaseSensitive(json_msg, "users");
                        if (cJSON_IsObject(users)) {
                            printf("👥 Connected Users:\n");
                            cJSON *user;
                            cJSON_ArrayForEach(user, users) {
                                printf(" - %s: %s\n", user->string, user->valuestring);
                            }
                        }

                    } else if (strcmp(type->valuestring, "NEW_USER") == 0) {
                        cJSON *username = cJSON_GetObjectItemCaseSensitive(json_msg, "username");
                        if (cJSON_IsString(username)) {
                            printf("🎉 New user connected: %s\n", username->valuestring);  
                        }
                    }
                }

                cJSON_Delete(json_msg);  
            } else {
                printf("Error parsing received message.\n");
            }
        } else if (received == 0) {
            printf("\nConnection closed by the server.\n");
            break;
        } else {
            perror("recv error");
            break;
        }

        memset(buffer, 0, 2048);  
    }

    pthread_exit(NULL);
}

