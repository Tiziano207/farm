//
// Created by ubuntu on 09/03/23.
//
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

// Dichiarazione di una variabile globale per tenere traccia dell'arrivo di SIGUSR1
volatile sig_atomic_t sigusr1_received = 0;
sig_atomic_t  sig_term_received = 0;
// Funzione di gestione del segnale SIGUSR1
void sigusr1_handler(int sig) {
    sigusr1_received = 1;
}


void delete_file(char * file) {
   remove(file);
    return;
}


// Funzione di gestione del segnale SIGHUP, SIGINT, SIGQUIT e SIGTERM
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
