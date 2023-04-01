#ifndef GENERIC_H
#define GENERIC_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <stdint.h>


    /*
     * Funzione che gestisce in maniera corretta la phtread_create(...)
     */
void Pthread_create(pthread_t *thread_id, const pthread_attr_t *attr, void* (*start_fcn) (void *), void* arg);

    /*
     * Funzione che gestisce in maniera corretta la phtread_join(...)
     */
void Pthread_join(pthread_t thread_id,void** status_ptr);

    /*
     * Funzione che gestisce in maniera corretta la phtread_mutex_lock(...)
     */
void Pthread_mutex_lock(pthread_mutex_t *mtx);

    /*
     * Funzione che gestisce in maniera corretta la phtread_mutex_unlock(...)
     */
void Pthread_mutex_unlock(pthread_mutex_t * mtx);

    /*
    * Funzione che gestisce in maniera corretta la phtread_cond_wait(...)
    */
void Pthread_cond_wait(pthread_cond_t * cond, pthread_mutex_t * mtx);

    /*
    * Funzione che gestisce in maniera corretta la phtread_mutex_init(...)
    */
void Pthread_mutex_init(pthread_mutex_t * mtx,   const pthread_mutexattr_t * restrict__attr);

    /*
    * Funzione che gestisce in maniera corretta la phtread_mutex_destroy(...)
    */
void Pthread_mutex_destroy(pthread_mutex_t * mtx);

    /*
   * Funzione che gestisce in maniera corretta la phtread_cond_signal(...)
   */
void Pthread_cond_signal(pthread_cond_t * cond);

    /*
   * Funzione che gestisce in maniera corretta malloc(...)
   */
void *Malloc(int size);

    /*
   * Funzione che rimuove un file
   */
void deleteFile(char * file);

    /*
     * Print del messaggio di usage
     */
void usage();

#endif /*GENERIC_H*/