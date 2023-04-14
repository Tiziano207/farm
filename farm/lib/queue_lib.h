#ifndef QUEUE_LIB_H
#define QUEUE_LIB_H

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include "generic.h"

typedef struct {
    char **items;
    int front;
    int rear;
    int size;
    int done;
    pthread_mutex_t q_lock;
} _queue;

void init_queue(_queue *q, int size);
void destroy_queue(_queue *q);
void enqueue(_queue *q, char *item);
int dequeue(_queue *q, char * item_temp);
int isDone(_queue * queue);
void setDone(_queue * queue);
int isDir(char *path);
int isRegular(char *filename);


#endif //QUEUE_LIB_H
