#ifndef MESSAGING_H
#define MESSAGING_H

#include <pthread.h>

extern pthread_t send_msg_thread;
extern pthread_t recv_msg_thread;

void* send_msg();
void* recv_msg();

#endif // MESSAGING_H
