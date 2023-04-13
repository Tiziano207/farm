#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <pthread.h>

volatile sig_atomic_t sigusr1_received = 0;
volatile sig_atomic_t  sig_term_received = 0;
extern struct sockaddr_un sa;

#define SYSCALL(r,c,e) \
    if((r=c)==-1) { perror(e);exit(errno); }

int sendSigusr1(int fd_skt){
    int n;
    int r;
    char * response = malloc(sizeof (char)* 5);
    if(response == NULL){perror("malloc");exit(errno);}
    SYSCALL(r,write(fd_skt,"SIGUSR1",8), "write");

    SYSCALL(n, read(fd_skt, response, (sizeof(char) * 5)), "read");
    if(strncmp(response,"ACK", 4) != 0){ printf("OH NO SOMETHING GOES WRONG\n");return 0;}
    free(response);

    return 1;
}

int createSocket(){
    int fd_skt;
    fd_skt=socket(AF_UNIX,SOCK_STREAM,0);
    errno=0;

    while((connect(fd_skt, (struct sockaddr*)&sa, sizeof(sa)))==-1){
        if(errno>0){
            sleep(1);
        }
    }

    return fd_skt;
}

void sigusr1_handler(int fd_sig_thread){
   //sigusr1_received = 1;
    sendSigusr1(fd_sig_thread);
    return;
}

static void cleanup_routine(void * arg){
    printf("CLEAN UP\n");
    int * fd_sig = (int*) arg;
    close(*fd_sig);
    return;
}

void *signal_thread_handler(void *arg){
    printf("pid %d\n", getpid());
    int fd_sig_thread = createSocket();
    int sig;
    sigset_t *set = (sigset_t*)arg;

    while(1){
        sigwait(set, &sig);
        if(sig == SIGUSR1){
            sigusr1_handler(fd_sig_thread);
            continue;
        }
        else if(sig == SIGINT || sig == SIGQUIT || sig == SIGTERM || sig == SIGHUP){
            sig_term_received = 1;
            continue;
        }

    }
    return NULL;
}

sigset_t signal_mask(){
    sigset_t mask;  //maschera

    //realizzo la maschera blocco tutti i segnali
    sigemptyset(&mask);  //la setto tutta a 0
    sigaddset(&mask,SIGHUP);  //mette a 1 SIGHUP
    sigaddset(&mask,SIGINT);  //mette a 1 SIGINT
    sigaddset(&mask,SIGQUIT);	//mette a 1 SIGQUIT
    sigaddset(&mask,SIGTERM); //mette a 1 SIGTERM
    sigaddset(&mask,SIGUSR1);  //mette a 1 SIGUSR1
    return mask;
}