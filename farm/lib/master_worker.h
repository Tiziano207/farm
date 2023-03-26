#ifndef MASTER_WORKER_H
#define MASTER_WORKER_H
#include "queue_lib.h"
#include "worker.h"
#include "generic.h"
#include <pthread.h>

void master_worker(int argc, char **argv, char *dir_name, int optind, int num_workers, int q_size);
#endif //MASTER_WORKER_H
