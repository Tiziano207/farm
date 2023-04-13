#ifndef MASTER_WORKER_H
#define MASTER_WORKER_H

#include "queue_lib.h"
#include "worker.h"
#include "generic.h"
#include <pthread.h>

    /*
    * Funzione che si occupa di madare il messaggio di "STOP" al processo Collector
    */
void sendStop(struct sockaddr_un * psa);

    /*
     * Funzione che si occupa di gestire il thread Poool e che viene chiamata dal main dopo
     * aver eseguito la funzione fork.
     */
void master_worker(int argc, char **argv, char *dir_name, int optind, int num_workers, int q_size);

#endif //MASTER_WORKER_H
