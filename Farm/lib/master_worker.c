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
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int fd_c;

static inline int writen(long fd, void *buf, size_t size) {
    size_t left = size;
    int r;
    char *bufptr = (char*)buf;
    while(left>0) {
        if ((r=write((int)fd ,bufptr,left)) == -1) {
            if (errno == EINTR) continue;
            return -1;
        }
        if (r == 0) return 0;
        left    -= r;
        bufptr  += r;
    }
    return 1;
}

/*Funzione che mi permette di leggere da una socket (é quella presente nel DidaWiki)*/
static inline int readn(long fd, void *buf, size_t size) {
    size_t left = size;
    int r;
    char *bufptr = (char*)buf;
    while(left>0) {
        if ((r=read((int)fd ,bufptr,left)) == -1) {
            if (errno == EINTR) continue;
            return -1;
        }
        if (r == 0) {return 0;}   // EOF
        left-= r;
        bufptr+= r;
    }
    return size;
}


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

    sendCollector("DONE'\0'", fd_c);
    Pthread_mutex_lock(&lock);
    close(fd_c);
    Pthread_mutex_unlock(&lock);

    Pthread_mutex_destroy(&lock);
    destroy_queue(queue_t);
    remove("./farm.sck");
    return;

}