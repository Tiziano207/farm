#include "queue_lib.h"
#include "worker.h"
#include "generic.h"
#include "queue_lib.h"
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

extern sig_atomic_t queue_interrupt;
extern struct sockaddr_un sa;

void sendStop(struct sockaddr_un * psa) {
    /* figlio, client */
    int client_socket, r;
    SYSCALL(client_socket,socket(AF_UNIX, SOCK_STREAM, 0),"socket")

    while (connect(client_socket, (struct sockaddr *) psa, sizeof(*psa)) == -1) {
        if (errno == ENOENT) sleep(1);
        else exit(EXIT_FAILURE);
    }

    SYSCALL(r,write(client_socket,"STOP",5), "write");
    close(client_socket);
    return;
}

void master_worker(int argc, char **argv, char *dir_name, int optind, int num_workers, int q_size){
    _queue *queue_t = malloc(sizeof(_queue));
    init_queue(queue_t, q_size);
    pthread_t threads[num_workers];

    for (int i = 0; i < num_workers; i++) {
        Pthread_create(&threads[i], NULL, worker, (void*) queue_t);
    }

    for (int i = optind; i < argc; i++) {
        if (isRegular(argv[i])) {
            if(queue_interrupt == 1){
                break;
            }
            enqueue(queue_t, argv[i]);
        }

    }
    if (dir_name != NULL) {
        if (isDir(dir_name)) {
            explorer(dir_name,  queue_t);
        }
    }

    setDone(queue_t);


    for (int i = 0; i < num_workers; i++) {
        Pthread_join(threads[i], NULL);
    }

    sendStop(&sa);
    destroy_queue(queue_t);

    return;

}