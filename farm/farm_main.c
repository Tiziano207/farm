#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <string.h>
#include "./lib/signal_installer.h"
#include "./lib/collector.h"
#include "./lib/generic.h"
#include "./lib/worker.h"
#include "./lib/master_worker.h"

#define MAX_QUEUE_SIZE 30
#define MAX_WORKERS 10
#define QUEUE_SIZE 8
#define SOCK_NAME_LENGTH 15
#define SOCK_NAME "./farm.sck"

int t_delay;
struct sockaddr_un sa;
volatile sig_atomic_t queue_interrupt = 0;
int fd_signal_thread;

int main(int argc, char *argv[]) {

    int num_workers = 1;
    int queue_size = QUEUE_SIZE;
    int opt;
    char * dir_name = NULL;
    while ((opt = getopt(argc, argv, "n:q:t:d:")) != -1) {
        switch (opt) {
            case 'n':
                num_workers = strtol(optarg, NULL, 10);
                if (num_workers > MAX_WORKERS) {
                    printf("Maximum number of workers is %d\n", MAX_WORKERS);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'q':
                queue_size = strtol(optarg, NULL, 10);
                break;
            case 't':
                t_delay = strtol(optarg, NULL, 10);
                break;
            case 'd':
                dir_name = optarg;
                break;
            default:
                usage();
        }
    }

    strncpy(sa.sun_path, SOCK_NAME, SOCK_NAME_LENGTH);
    sa.sun_family=AF_UNIX;

    /*
    * Blocco tutti i segnali
    */
    sigset_t mask = signal_mask();
    if(pthread_sigmask(SIG_BLOCK, &mask, NULL) != 0)
    {
        perror("pthread_sigmask");
        exit(EXIT_FAILURE);
    }
    pthread_t signal_thread;
    Pthread_create(&signal_thread, NULL, signal_thread_handler, &mask);
    int pid, status;
    SYSCALL(pid, fork(), "fork");
    if (pid == 0){ collector(&sa);}
    master_worker(argc, argv, dir_name, optind, num_workers, queue_size);
    pthread_cancel(signal_thread);
    Pthread_join(signal_thread, NULL);
    waitpid(pid, &status, 0);

    int r;
    SYSCALL(r,sigemptyset(&mask),"sigemptyset");
    SYSCALL(r,pthread_sigmask(SIG_SETMASK,&mask,NULL), "pthread_sigmask");
    return 0;
}