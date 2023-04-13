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

extern volatile sig_atomic_t sigusr1_received;
extern volatile sig_atomic_t  sig_term_received;
extern struct sockaddr_un sa;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int fd_c;

int CreateSocket(){
    int fd_skt;
    //creo la socket
    fd_skt=socket(AF_UNIX,SOCK_STREAM,0);

    //provo a connettermi con il collector
    errno=0;
    while((connect(fd_skt, (struct sockaddr*)&sa, sizeof(sa)))==-1){
        if(errno>0){
            sleep(1);
        }
    }
    return fd_skt;
}


void master_worker(int argc, char **argv, char *dir_name, int optind, int num_workers, int q_size){

    _queue *queue_t = malloc(sizeof(_queue));
    if(queue_t == NULL){perror("malloc");exit(errno);}
    init_queue(queue_t, q_size);
    pthread_t threads[num_workers];
    fd_c = CreateSocket();
    for (int i = 0; i < num_workers; i++) {
        Pthread_create(&threads[i], NULL, worker, (void*) queue_t);
    }

    for (int i = optind; i < argc; i++) {
        if (isRegular(argv[i])) {
            if(sig_term_received == 1){
                printf("no more file for you\n");
                break;
            }
            enqueue(queue_t, argv[i]);
        }

    }

    if (dir_name != NULL && sig_term_received == 0) {
        if (isDir(dir_name)) {
            explorer(dir_name,  queue_t);
        }
    }
    setDone(queue_t);

    for (int i = 0; i < num_workers; i++) {
        Pthread_join(threads[i], NULL);
    }

    sendCollector("DONE'\0'", fd_c);
    Pthread_mutex_lock(&lock);
    close(fd_c);
    Pthread_mutex_unlock(&lock);

    Pthread_mutex_destroy(&lock);
    destroy_queue(queue_t);
    remove("./farm.sck");
    return;

}