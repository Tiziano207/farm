#ifndef GENERIC_H
#define GENERIC_H

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pthread.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdint.h>

#define SYSCALL(r,c,e) \
    if((r=c)==-1) { perror(e);exit(errno); }
// Funzioni di gestione dei file
void Pthread_create(pthread_t *thread_id, const pthread_attr_t *attr, void* (*start_fcn) (void *), void* arg);
void Pthread_join(pthread_t thread_id,void** status_ptr);
void Pthread_mutex_lock(pthread_mutex_t *mtx);
void Pthread_mutex_unlock(pthread_mutex_t * mtx);
void Pthread_cond_wait(pthread_cond_t * cond, pthread_mutex_t * mtx);
void Pthread_mutex_init(pthread_mutex_t * mtx,   const pthread_mutexattr_t * restrict__attr);
void Pthread_mutex_destroy(pthread_mutex_t * mtx);
void Pthread_cond_signal(pthread_cond_t * cond);

void usage();

#endif /*GENERIC_H*/