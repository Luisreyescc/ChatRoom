#ifndef CONNECTION_H
#define CONNECTION_H

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <arpa/inet.h>

extern int sockfd;  
extern char user_name[32];
extern volatile sig_atomic_t indicator;  

void connect_to_server(const char *ip, int port);
void close_connection();
void handle_sigint(int sig);
void new_line_trim(char* arr, int length);

#endif // CONNECTION_H
