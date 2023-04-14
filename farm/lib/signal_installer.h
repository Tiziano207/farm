#ifndef SIGNAL_INSTALLER_H
#define SIGNAL_INSTALLER_H

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>

#define SYSCALL(r,c,e) \
    if((r=c)==-1) { perror(e);exit(errno); }

int create_Socket();
int sendSigusr1(int fd_skt);
void sigusr1_handler();
static void cleanup_routine(void *arg);
void *signal_thread_handler(void *arg);
sigset_t signal_mask();

#endif //SIGNAL_INSTALLER_H
