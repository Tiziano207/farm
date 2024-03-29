#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <stdint.h>

    /**
     * Funzione che gestisce in maniera corretta la phtread_create(...)
     */
void Pthread_create(pthread_t *thread_id, const pthread_attr_t *attr, void* (*start_fcn) (void *), void* arg) {
    uintptr_t err;
    if ( ( err = pthread_create(thread_id, attr, start_fcn, arg)) != 0 ) {errno = err; perror("create"); pthread_exit((void*)err);}
}

    /**
     * Funzione che gestisce in maniera corretta la phtread_join(...)
     */
void Pthread_join(pthread_t thread_id,void** status_ptr){
    uintptr_t err;
    if ( ( err = pthread_join(thread_id, status_ptr)) != 0 ) {errno = err; perror("join"); pthread_exit((void*)err);}
}

    /**
     * Funzione che gestisce in maniera corretta la phtread_mutex_lock(...)
     */
void Pthread_mutex_lock(pthread_mutex_t *mtx) {
    uintptr_t err;
    if ( ( err=pthread_mutex_lock(mtx)) != 0 ) {errno=err; perror("lock"); pthread_exit((void*) err);}
}

    /**
     * Funzione che gestisce in maniera corretta la phtread_mutex_unlock(...)
     */
void Pthread_mutex_unlock(pthread_mutex_t * mtx) {
    uintptr_t err;
    if ( ( err = pthread_mutex_unlock(mtx)) != 0 ) {errno = err; perror("unlock"); pthread_exit((void*) err);}
}

    /**
     * Funzione che gestisce in maniera corretta la phtread_cond_wait(...)
     */
void Pthread_cond_wait(pthread_cond_t * cond, pthread_mutex_t * mtx) {
    uintptr_t err;
    if ( ( err = pthread_cond_wait(cond, mtx)) != 0 ) {errno = err; perror("wait"); pthread_exit((void*)err);}
}

    /**
     * Funzione che gestisce in maniera corretta la phtread_mutex_init(...)
     */
void Pthread_mutex_init(pthread_mutex_t * mtx,   const pthread_mutexattr_t * restrict__attr) {
    uintptr_t err;
    if ( ( err = pthread_mutex_init(mtx, restrict__attr)) != 0 ) {errno = err; perror("init"); pthread_exit((void*)err);}
}

    /**
     * Funzione che gestisce in maniera corretta la phtread_mutex_destroy(...)
     */
void Pthread_mutex_destroy(pthread_mutex_t * mtx){
    uintptr_t err;
    if ( ( err = pthread_mutex_destroy(mtx)) != 0 ) {errno = err; perror("destroy"); pthread_exit((void*) err);}
}

    /**
    * Funzione che gestisce in maniera corretta la phtread_cond_signal(...)
    */
void Pthread_cond_signal(pthread_cond_t * cond) {
    uintptr_t err;
    if ( ( err = pthread_cond_signal(cond)) != 0 ) {errno = err; perror("signal"); pthread_exit((void*) err);}
}

    /**
     * Print del messaggio di usage
     */
void usage() {
    printf("Options:\n");
    printf("\t-n NUM_WORKERS\t Set the maximum number of worker processes (default 1)\n");
    printf("\t-q SIZE\t\t Size of the queue (default 8)\n");
    printf("\t-t TIME\t\t Set the delay time in seconds (default 0)\n");
    printf("\t-d DIR\t\t Navigate the directory specified and process all files and subdirectories\n");
    exit(EXIT_FAILURE);
}


