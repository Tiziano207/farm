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
#include <dirent.h>


extern volatile int sig_term_received;
extern struct sockaddr_un sa;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int fd_c;

    /**
     * Funzione ricorsiva che si occupa di visitare ricorseivamente le sottocartelle e aggiungere i file all'interno
     * delle cartelle alla coda
    */
void explorer(char *dir_name, _queue *queue) {
    DIR *dir;
    char updated_path[PATH_MAX];

    if ((dir = opendir(dir_name)) == NULL) {perror("opendir"); exit(EXIT_FAILURE);}

    errno = 0;
    struct dirent *file;
    while ((file = readdir(dir)) != NULL && (errno == 0)) {
        if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0) {
            continue;
        }
        strncpy(updated_path, dir_name, strlen(dir_name) + 1);
        strncat(updated_path, "/", 2);
        strncat(updated_path, file->d_name, strlen(file->d_name) + 1);
        if (isDir(updated_path)) {
            explorer(updated_path, queue);
        } else {
            if (isRegular(updated_path)) {
                if(sig_term_received == 1){break;}
                enqueue(queue, updated_path);
            }
        }
    }
    if (errno != 0) {perror("readdir");}
    else {closedir(dir);}
    return;
}

    /**
     * Funzione utilizzata per connettersi al processo Collector
     */
int create_socket(){
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

    /**
     * Funzione che si occupa di gestire il thread Poool e che viene chiamata dal main dopo
     * aver eseguito la funzione fork.
    */
void master_worker(int argc, char **argv, char *dir_name, int optind, int num_workers, int q_size){

    _queue *queue_t = malloc(sizeof(_queue));
    if(queue_t == NULL){perror("malloc");exit(errno);}
    init_queue(queue_t, q_size);
    pthread_t threads[num_workers];
    fd_c = create_socket();
    for (int i = 0; i < num_workers; i++) {
        Pthread_create(&threads[i], NULL, worker, (void*) queue_t);
    }

    for (int i = optind; i < argc; i++) {
        if (isRegular(argv[i])) {
            if(sig_term_received == 1){
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