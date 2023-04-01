#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <dirent.h>
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

void init_queue(_queue *q, int size) {
    q->items = (char **) malloc(size * sizeof(char *));
    q->front = 0;
    q->rear = -1;
    q->size = size;
    q->done = 0;
    Pthread_mutex_init(&q->q_lock, NULL);
}
void destroy_queue(_queue *q) {
    pthread_cond_destroy(&not_full);
    pthread_cond_destroy(&not_empty);
    Pthread_mutex_destroy(&q->q_lock);
    free(q->items);
    free(q);
}
void enqueue(_queue *q, char *item) {
    pthread_mutex_lock(&q->q_lock);
    while (q->rear == q->size - 1) {
        Pthread_cond_wait(&not_full, &q->q_lock);
    }
    q->rear++;
    q->items[q->rear] = malloc(sizeof (char) * strlen(item)+1);
    strncpy(q->items[q->rear],item,strlen(item)+1);

    Pthread_cond_signal(&not_empty);
    Pthread_mutex_unlock(&q->q_lock);
}
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
int isDone(_queue * queue){
    int bit = 0;
    Pthread_mutex_lock(&queue->q_lock);
    if(queue->done == 1){bit = 1;}
    Pthread_mutex_unlock(&queue->q_lock);
    return bit;
}

void setDone(_queue * queue){
    Pthread_mutex_lock(&queue->q_lock);
    queue->done= 1;
    Pthread_mutex_unlock(&queue->q_lock);
}
int isDir(char *path){
    struct stat info;
    int r;
    SYSCALL(r,stat(path,&info), "stat" );
    return S_ISDIR(info.st_mode);
}
int isRegular(char *filename){
    struct stat statPath;
    int r;
    SYSCALL(r,stat(filename,&statPath), "stat" );
    if(r!=0){perror("not regular"); return 0;}
    return S_ISREG(statPath.st_mode);
}
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
                enqueue(queue, updated_path);
            }
        }
    }
    if (errno != 0) {perror("readdir");}
    else {closedir(dir);}
    return;
}

