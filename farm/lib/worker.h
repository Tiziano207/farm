//
// Created by ubuntu on 09/03/23.
//

#ifndef WORKER_H

#define WORKER_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include "generic.h"
#include "queue_lib.h"

#define MAX_MESSAGE_LENGTH 2048

#define SYSCALL(r,c,e) \
    if((r=c)==-1) { perror(e);exit(errno); }

extern int max_execution_time;
extern struct sockaddr_un sa;
extern pthread_cond_t not_empty;
extern pthread_cond_t not_full;

void run_client(struct sockaddr_un * psa, long result , char* file_name);
double calculateResult(long *nums, int n);
size_t read_file_contents(const char* filename, long long** array);
void *worker(void *args);

#endif //WORKER_H
