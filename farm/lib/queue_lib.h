
#ifndef QUEUE_LIB_H
#define QUEUE_LIB_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
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

void init_queue(_queue *q, int size);
void destroy_queue(_queue *q);
void enqueue(_queue *q, char *item);
int dequeue(_queue *q, char * item_temp);
void setDone(_queue * queue);

int isDir(char *path);
int isRegular(char *filename);
void explorer(char *dir_name, _queue *queue);

#endif //QUEUE_LIB_H
