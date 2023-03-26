//
// Created by ubuntu on 09/03/23.
//
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
extern pthread_cond_t not_empty;
extern pthread_cond_t not_full;

void run_client(struct sockaddr_un * psa, long long result , char* file_name) {
    /* figlio, client */
    int client_socket, r;
    SYSCALL(client_socket,socket(AF_UNIX, SOCK_STREAM, 0),"socket")

    while (connect(client_socket, (struct sockaddr *) psa, sizeof(*psa)) == -1) {
        if (errno == ENOENT) sleep(1);
        else exit(EXIT_FAILURE);
    }
    char *message = malloc(sizeof(char)*MAX_FILENAME_LENGTH);
    sprintf(message, "%lld;%s<",result,file_name);
    SYSCALL(r,write(client_socket,message,strlen(message)), "write");
    close(client_socket);
    free(message);
    return;
}





long long calculateResult( long long *nums, int n) {
    long long  result = 0.0;
    for (int i = 0; i < n; i++) {
        result += (i * (long long)nums[i]);
    }
    return result;
}
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

void *worker(void *args) {
    _queue *q = (_queue *) args;
    //ora il worker ha il nome del file
    while (1) {
        char * file  = malloc(MAX_FILENAME_LENGTH * sizeof (char));
        if(dequeue(q, file) == 1){free(file);return (void *) NULL;}

        FILE *fp = fopen(file, "rb");
        if (!fp) {perror("openfile"); pthread_exit((void *) 1);}

        errno = 0;
        rewind(fp);
        if (errno != 0) {perror("rewind"); pthread_exit((void *) 1);}
        // Allochiamo spazio per gli elementi

        if (errno != 0) { perror("rewind"); pthread_exit((void *) 1);}
        long long result = read_file_contents(file);

        fclose(fp);
        run_client(&sa, result, file);
        free(file);
        sleep(t_delay);
    }
}
