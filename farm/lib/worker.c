#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include "queue_lib.h"

#define MAX_MESSAGE_LENGTH 2048
#define MAX_FILENAME_LENGTH 256
#define SYSCALL(r,c,e) \
    if((r=c)==-1) { perror(e);exit(errno); }

extern int t_delay;
extern struct sockaddr_un sa;
extern pthread_mutex_t lock;
extern int fd_c;

    /**
     * Funzione che si occupa di mandare un messaggio su una determinata connessione già aperta,
     * dopo aver mandato il messaggio la funzione attende la recezione del messaggio ACK che conferma
     * la recezione del messaggio da parte del processo COllector.
     */
int sendCollector(char* message, int fd_skt){
    pthread_mutex_lock(&lock);
    int n;
    int r;
    char * response = malloc(sizeof (char)* 5);
    if(response == NULL){perror("malloc");exit(errno);}
    SYSCALL(r,write(fd_skt,message,strlen(message)), "write");

    SYSCALL(n, read(fd_skt, response, (sizeof(char) * 5)), "read");
    if(strncmp(response,"ACK", 5) != 0){ printf("Something goes wrong\n");pthread_mutex_unlock(&lock);return 0;}
    free(response);

    pthread_mutex_unlock(&lock);  //unlock
    return 1;
}

    /**
     * Funzione che preso un array di numeri calcola il risultato
     */
long long calculateResult( long long *nums, int n) {
    long long  result = 0.0;
    for (int i = 0; i < n; i++) {
        result += (i * (long long)nums[i]);
    }
    return result;
}

    /**
     * Funzione che si occupa di leggere il contenuto dei file e riempire
     * l'array che verrà passato a calculateResult per calcolare il risultato
     */
size_t read_file_contents(const char* filename) {
    long long *array;
    FILE* fp = fopen(filename, "rb");
    if (fp == NULL) {
        fprintf(stderr, "Errore: impossibile aprire il file %s\n", filename);
        return 0;
    }

    // Determina la dimensione del file
    fseek(fp, 0, SEEK_END);
    size_t file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // Alloca un array per contenere il contenuto del file
    size_t num_elements = file_size / sizeof(long long);
    array = (long long*) malloc(num_elements * sizeof(long long));
    if(array == NULL){perror("malloc");exit(errno);}
    // Legge il contenuto del file nell'array
    size_t elements_read = fread(array, sizeof(long long), num_elements, fp);
    if (elements_read != num_elements) {
        fprintf(stderr, "Errore: il numero di elementi letti dal file %s non corrisponde alla dimensione del file.\n", filename);
    }

    fclose(fp);
    long long result =  calculateResult(array, elements_read);
    free(array);
    return result;
}
    /**
     * Funzione che viene usata dal thread, Il Thread dopo aver ottenuto la lock
     * della coda concorrente, acquisirà un nome di un file attraverso la funzione
     * dequeue calcolerà il risultato e poi lo invierà al processo Collector,
     * acquisendo la variabile globale fd_c, per poter utilizzare la connessione
     * stabilita precedentemene dal Master Worker
    */
void *worker(void *args) {
    _queue *q = (_queue *) args;
    //ora il worker ha il nome del file
    while (1) {
        char * file  = malloc(MAX_FILENAME_LENGTH * sizeof (char));
        if(file == NULL){perror("malloc");exit(errno);}

        if(dequeue(q, file) == 1 ){
            free(file);
            return (void *) NULL;
        }

        FILE *fp = fopen(file, "rb");
        if (!fp) {perror("openfile"); pthread_exit((void *) 1);}

        errno = 0;
        rewind(fp);
        if (errno != 0) {perror("rewind"); pthread_exit((void *) 1);}
        // Allochiamo spazio per gli elementi

        if (errno != 0) { perror("rewind"); pthread_exit((void *) 1);}
        long long result = read_file_contents(file);

        fclose(fp);
        char *message = malloc(sizeof(char)*MAX_FILENAME_LENGTH);
        if(message == NULL){perror("malloc");exit(errno);}
        sprintf(message, "%lld;%s<",result,file);
        if(sendCollector(message,fd_c) == 0){pthread_exit((void *) 1);}
        free(file);
        free(message);
        sleep(t_delay  * 0.001);
    }

}
