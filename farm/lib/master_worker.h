#ifndef MASTER_WORKER_H
#define MASTER_WORKER_H

#include "queue_lib.h"
#include "worker.h"
#include "generic.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>

void explorer(char *dir_name, _queue *queue);
int create_socket();
void master_worker(int argc, char **argv, char *dir_name, int optind, int num_workers, int q_size);

#endif //MASTER_WORKER_H
