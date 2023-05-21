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
#include "queue_lib.h"

int sendCollector(char* message, int fd_skt);
double calculateResult(long *nums, int n);
size_t read_file_contents(const char* filename, long long** array);
void *worker(void *args);

#endif //WORKER_H
