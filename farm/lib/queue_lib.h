
#ifndef QUEUE_LIB_H
#define QUEUE_LIB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include "generic.h"

#define SYSCALL(r,c,e) \
    if((r=c)==-1) { perror(e);exit(errno); }

typedef struct {
    char **items;
    int front;
    int rear;
    int size;
    int done;
    pthread_mutex_t q_lock;
} _queue;


    /*
     * Funzione che inizializza una struttura dati _queue
     */
void init_queue(_queue *q, int size);

    /*
     * Funzione che si occupa di svuotare la memoria occupata dlla coda
     */
void destroy_queue(_queue *q);

    /*
     * Funzione che si occupa di inserire un nome di un file dentro la coda
     * concorrente
     */
void enqueue(_queue *q, char *item);

    /*
     * Funzione che si occupa di prelevare un nome di file dddalla coda concorrente
     */
int dequeue(_queue *q, char * item_temp);

    /*
     * Funzione utilizzata per dichiarare che tutti i file sono stati inseriti nella coda
     */
void setDone(_queue * queue);

    /*
     * Funzione che conttrolla se un file è una cartella oppure no
     */
int isDir(char *path);

    /*
     *FUnzione che controlla se un file è regolare
     */
int isRegular(char *filename);

    /*
     * Funzione che si occupa di visitare ricorsivamente una cartella e inserire i
     * nomi dei file all'interno della coda concorrente
     */
void explorer(char *dir_name, _queue *queue);

#endif //QUEUE_LIB_H
