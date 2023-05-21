#include <stdlib.h>
#include <string.h>
#include <errno.h>
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

pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;

    /**
     * Funzione che inizializza una struttura dati _queue
    */
void init_queue(_queue *q, int size) {
    q->items = (char **) malloc(size * sizeof(char *));
    if(q->items == NULL){perror("malloc");exit(errno);}
    q->front = 0;
    q->rear = -1;
    q->size = size;
    q->done = 0;
    Pthread_mutex_init(&q->q_lock, NULL);
}

    /**
     * Funzione che si occupa di svuotare la memoria occupata dlla coda
    */
void destroy_queue(_queue *q) {
    pthread_cond_destroy(&not_full);
    pthread_cond_destroy(&not_empty);
    Pthread_mutex_destroy(&q->q_lock);
    free(q->items);
    free(q);
}

    /**
     * Funzione che si occupa di inserire un nome di un file dentro la coda
     * concorrente
    */
void enqueue(_queue *q, char *item) {
    pthread_mutex_lock(&q->q_lock);
    while (q->rear == q->size - 1) {
        Pthread_cond_wait(&not_full, &q->q_lock);
    }
    q->rear++;
    q->items[q->rear] = malloc(sizeof (char) * strlen(item)+1);
    if(q->items[q->rear]  == NULL){perror("malloc");exit(errno);}
    strncpy(q->items[q->rear],item,strlen(item)+1);

    Pthread_cond_signal(&not_empty);
    Pthread_mutex_unlock(&q->q_lock);
}

    /**
     * Funzione che si occupa di prelevare un nome di file dddalla coda concorrente
    */
int dequeue(_queue *q, char * item_temp) {
    Pthread_mutex_lock(&q->q_lock);
    while (q->rear == -1) {
        if (q->done) {
            pthread_mutex_unlock(&q->q_lock);
            return 1;
        }
        Pthread_cond_wait(&not_empty, &q->q_lock);
    }
    strncpy( item_temp,q->items[q->front],strlen(q->items[q->front])+1 );
    free(q->items[q->front]);
    q->front++;
    if (q->front > q->rear) {
        q->front = 0;
        q->rear = -1;
    }
    Pthread_cond_signal(&not_full);;
    Pthread_mutex_unlock(&q->q_lock);
    return 0;
}

    /**
     * Funzione che controlla  se il flag queue-> done è settato a vero
    */
int isDone(_queue * queue){
    int bit = 0;
    Pthread_mutex_lock(&queue->q_lock);
    if(queue->done == 1){bit = 1;}
    Pthread_mutex_unlock(&queue->q_lock);
    return bit;
}

    /**
     * Funzione utilizzata per dichiarare che tutti i file sono stati inseriti nella coda
    */
void setDone(_queue * queue){
    Pthread_mutex_lock(&queue->q_lock);
    queue->done= 1;
    Pthread_mutex_unlock(&queue->q_lock);
}

    /**
     * Funzione che conttrolla se un file è una cartella oppure no
    */
int isDir(char *path){
    struct stat info;
    int r;
    SYSCALL(r,stat(path,&info), "stat" );
    return S_ISDIR(info.st_mode);
}

    /**
     *FUnzione che controlla se un file è regolare
    */
int isRegular(char *filename){
    struct stat statPath;
    int r;
    SYSCALL(r,stat(filename,&statPath), "stat" );
    if(r!=0){perror("not regular"); return 0;}
    return S_ISREG(statPath.st_mode);
}