//
// Created by ubuntu on 09/03/23.
//

#ifndef SIGNAL_INSTALLER_H
#define SIGNAL_INSTALLER_H

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

extern volatile sig_atomic_t sigusr1_received;
extern volatile sig_atomic_t sig_term_received;


void *signal_thread_handler(void *arg);
    /*
     * Funzione che maschera i i segnali
     */
sigset_t signal_mask();

    /*
    *Funzione che installa i gestori dei segnali per vari segnali in modo che il
    * programma possa gestirli adeguatamente.
    */
void signal_installer();

    /*
     * Funzione di gestione del segnale SIGHUP, SIGINT, SIGQUIT e SIGTERM
     */
void term_handler(int sig);

    /*
     *Funzione che elimina un file
     */
void delete_file(char * file);

    /*
     * Funzione di gestione del segnale SIGUSR1
     * */
void sigusr1_handler(int sig);

#endif //SIGNAL_INSTALLER_H
