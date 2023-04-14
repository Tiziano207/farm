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
extern volatile sig_atomic_t  sig_term_received ;
extern struct sockaddr_un sa;
int fd_sig_thread;

#define SYSCALL(r,c,e) \
    if((r=c)==-1) { perror(e);exit(errno); }

    /**
    * Funzione utilizzata per connettersi al processo Collector
    */
int create_Socket(){
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


    /**
     * Funzione che si occupa di propagare il segnale al collector tramite una connessione
     * precedentemente stabilita
    */
int sendSigusr1(int fd_skt){
    int n;
    int r;
    char * response = malloc(sizeof (char)* 5);
    if(response == NULL){perror("malloc");exit(errno);}
    SYSCALL(r,write(fd_skt,"SIGUSR1",8), "write");

    SYSCALL(n, read(fd_skt, response, (sizeof(char) * 5)), "read");
    if(strncmp(response,"ACK", 4) != 0){ printf("Something goes wrong\n");
        exit(EXIT_FAILURE);}
    free(response);

    return 1;
}

    /**
     * Funzione adibita al controllo del segnale SIGUSR1
    */
void sigusr1_handler(){
   //sigusr1_received = 1;
    sendSigusr1(fd_sig_thread);
    return;
}

    /**
     * Funzione che si occupa di chiudere la connessione alla terminazione del thread se viene eseguita
     * una thread cancel
    */
static void cleanup_routine(void * arg){
    close(fd_sig_thread);
    return;
}

    /**
     * Funzione del thread che si occupa della gestione dei segnali
    */
void *signal_thread_handler(void *arg){
    fd_sig_thread = create_Socket();
    pthread_cleanup_push(cleanup_routine,NULL);
    int sig;
    sigset_t *set = (sigset_t*)arg;


    while(1){
        sigwait(set, &sig);
        if(sig == SIGUSR1){
            sigusr1_handler();
            continue;
        }
        else if(sig == SIGINT || sig == SIGQUIT || sig == SIGTERM || sig == SIGHUP){
            sig_term_received = 1;
            continue;
        }

    }
    pthread_cleanup_pop(0);
    return NULL;
}

    /**
     * Fnzione che crea una maschera per la gestione dei segnali, pi in particolare
     * i segnali che vengono mascherati sono SIGHUP, SIGINT, SIGQUIT, SIGTERM e SIGUSR1
   */
sigset_t signal_mask(){
    sigset_t mask;

    sigemptyset(&mask);
    sigaddset(&mask,SIGHUP);
    sigaddset(&mask,SIGINT);
    sigaddset(&mask,SIGQUIT);
    sigaddset(&mask,SIGTERM);
    sigaddset(&mask,SIGUSR1);
    return mask;
}