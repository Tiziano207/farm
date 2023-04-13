#ifndef HEADER_FILE_H
#define HEADER_FILE_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include "signal_installer.h"

#define MAX_MESSAGE_LENGTH 2048
#define SYSCALL(r,c,e) \
    if((r=c)==-1) { perror(e);exit(errno); }

typedef struct node {
    char *file_name;
    int data;
    struct node *next;
} Node;

    /*
     * Funzione che si occupa di deallocare la memoria dinamica
     * utilizzata per memorizzare la lista
     */
void free_list(Node* testa);

    /*
     * Funzione che si occupa di ordinare la lista in modo
     * crescente secondo il campo data
     */
void sort_list(struct node** head);

    /*
     * Funzione adibita alla stampa della lista
     */
void print_list(Node *head);

    /*
     * Funzione per l'inserimento un nodo in lista
     */
void insert_node(Node **head, int data, char * filename);

    /*
     * Funzione che si occupa di "aggiornare" il valore del descrittore,
     * restituendo il valore del descrittore più grande nel set
     */
int update(fd_set *set);

    /*
     * Funzione che ricava da una stringa una sottostringa
     */
void substring(char * str, char * subs);

    /*
    * Funzione che si occopa di dividere il messaggio utilizzando il
     * carattere separatore ';'
    */
void tokenizer_r(char * message, Node **head);

    /*
     * Il Collector è il server che gestisce le richieste dei vari client.
     * Si occupa di ricevere i vari messaggi e memorizzare i valori in una lista
     * concatenata che poi ordinerà e stamperà in output.
     * Inoltre, il collector gestisce i vari segnali e termina quando riceve il messaggio
     * STOP dai client
     */
void collector(struct sockaddr_un *psa);
#endif