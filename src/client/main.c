/**
 * @file main.c
 * @brief Main file for the chat client application.
 *
 * This file contains the main function for the chat client program,
 * handling the initialization, connection to the server, and creation
 * of threads for sending and receiving messages.
 */
#include "client.h"

/**
 * @brief Entry point of the chat client program.
 *
 * This function parses command-line arguments to obtain the server IP
 * address and port number, then initializes the client, connects to
 * the server, and creates threads for sending and receiving messages.
 *
 * @param argc The number of command-line arguments.
 * @param argv Array of command-line argument strings.
 * @return EXIT_SUCCESS on successful execution, or EXIT_FAILURE if
 *         an error occurs.
 */
int main(int argc, char **argv) {

    if (argc != 3) {
        printf("Usage: %s <ip> <port>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *ip = argv[1];
    int port = atoi(argv[2]);

    printf("What's your name?: ");
    fgets(user_name, 9, stdin);
    new_line_trim(user_name, strlen(user_name));

    if (strlen(user_name) > 8 || strlen(user_name) < 1) {
        printf("Name must be 8 characters or less and more than 1 characters.\n");
        return EXIT_FAILURE;
    }

    struct sockaddr_in server_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_port = htons(port);

    int err = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (err == -1) {
        printf("ERROR: connect\n");
        return EXIT_FAILURE;
    }

    send(sockfd, user_name, 32, 0);

    printf("##############################################\n");
    printf("#            WELCOME TO THE CHATROOM         #\n");
    printf("#  Try writing something! If there are other #\n");
    printf("#  users, they may respond to your messages, #\n");
    printf("#  and you can respond to theirs as well. If #\n");
    printf("#  a new user connects, the message 'User    #\n");
    printf("#  has joined' will appear. If a user leaves,#\n");
    printf("#  the message 'User has left' will appear.  #\n");
    printf("#  When you receive a new message, the user's#\n");
    printf("#  name and this emoji 🗣️ will be shown.    #\n");
    printf("##############################################\n");
    printf("         \\      ^__^\n");
    printf("          \\     (oo)\\_______\n");
    printf("               (__)\\         )\\/\\\n");
    printf("                 ||------w--||\n");
    printf("                 ||         ||\n");

    pthread_t send_msg_thread;
    if (pthread_create(&send_msg_thread, NULL, (void*)send_msg, NULL) != 0) {
        printf("ERROR: pthread_create for send_msg\n");
        return EXIT_FAILURE;
    }

    pthread_t recv_msg_thread;
    if (pthread_create(&recv_msg_thread, NULL, (void*)recv_msg, NULL) != 0) {
        printf("ERROR: pthread_create for recv_msg\n");
        return EXIT_FAILURE;
    }

    pthread_join(send_msg_thread, NULL);
    pthread_join(recv_msg_thread, NULL);

    printf("\nCome back soon!\n");
    close(sockfd);

    return EXIT_SUCCESS;
}
