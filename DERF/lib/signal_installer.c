#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

volatile sig_atomic_t sigusr1_received = 0;
volatile sig_atomic_t  sig_term_received = 0;

void sigusr1_handler(int sig) {
    sigusr1_received = 1;
}

void delete_file(char * file) {
   remove(file);
    return;
}

void term_handler(int sig) {
    sig_term_received = 1;
    delete_file("./farm.sck");
    exit(EXIT_SUCCESS);
}

void signal_installer(){
    if (signal(SIGHUP, term_handler) == SIG_ERR) {
        perror("Errore nell'installazione del gestore di SIGHUP");
        exit(EXIT_FAILURE);
    }
    if (signal(SIGINT, term_handler) == SIG_ERR) {
        perror("Errore nell'installazione del gestore di SIGINT");
        exit(EXIT_FAILURE);
    }
    if (signal(SIGQUIT, term_handler) == SIG_ERR) {
        perror("Errore nell'installazione del gestore di SIGQUIT");
        exit(EXIT_FAILURE);
    }
    if (signal(SIGTERM, term_handler) == SIG_ERR) {
        perror("Errore nell'installazione del gestore di SIGTERM");
        exit(EXIT_FAILURE);
    }
    if (signal(SIGPIPE, term_handler) == SIG_ERR) {
        perror("Errore nell'installazione del gestore di SIGPIPE");
        exit(EXIT_FAILURE);
    }
    if (signal(SIGUSR1, sigusr1_handler) == SIG_ERR) {
        perror("Errore nell'installazione del gestore di SIGUSR1");
        exit(EXIT_FAILURE);
    }
}

void signal_mask(){
    sigset_t mask;
    sigfillset(&mask);
    sigprocmask(SIG_SETMASK, &mask, NULL);
    // installa il gestore di segnale per SIGUSR1
    signal(SIGUSR1, sigusr1_handler);
}
