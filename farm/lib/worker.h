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

#define MAX_MESSAGE_LENGTH 2048

#define SYSCALL(r,c,e) \
    if((r=c)==-1) { perror(e);exit(errno); }

extern int t_delay;
extern struct sockaddr_un sa;

    /*
     * Funzione che si occupa di gestire la connessione con il processo
     * Collector, e di inviare i messaggi nel formato "risultato;fileName<"
     */
void run_client(struct sockaddr_un * psa, long result , char* file_name);
int sendCollector(char* message, int fd_skt);
    /*
     * Funzione che preso un array di numeri calcola il risultato
     */
double calculateResult(long *nums, int n);

    /*
     * Funzione che si occupa di leggere il contenuto dei file e riempire
     * l'array che verrà passato a calculateResult per calcolare il risultato
     */
size_t read_file_contents(const char* filename, long long** array);

    /*
     * Funzione che viene usata dal thread, Il Thread dopo aver ottenuto la lock
     * della coda concorrente, acquisirà un nome di un file attraverso la funzione
     * dequeue calcolerà il risultato e poi lo invierà al processo Collector
     */
void *worker(void *args);

#endif //WORKER_H
