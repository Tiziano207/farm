#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#define MAX_QUEUE_SIZE 10
#define MAX_FILE_NAME_LENGTH 256

// Struttura per la coda
typedef struct {
    char file_names[MAX_QUEUE_SIZE][MAX_FILE_NAME_LENGTH];
    int head;
    int tail;
    int count;
    int full;
    int empty;
    pthread_mutex_t mutex;
    pthread_cond_t not_full;
    pthread_cond_t not_empty;
} queue_t;

// Inizializza la coda
void queue_init(queue_t* queue) {
    queue->head = 0;
    queue->tail = -1;
    queue->count = 0;
    queue->full = 0;
    queue->empty = 1;
    pthread_mutex_init(&queue->mutex, NULL);
    pthread_cond_init(&queue->not_full, NULL);
    pthread_cond_init(&queue->not_empty, NULL);
}

void usage() {
    printf("Usage: program_name [OPTIONS] [FILE1] [FILE2] ... [FILEn]\n");
    printf("Options:\n");
    printf("\t-n NUM_WORKERS\t Set the maximum number of worker processes (default 1)\n");
    printf("\t-q \t\t Quiet mode (no output)\n");
    printf("\t-t TIME\t\t Set the maximum execution time in seconds (default 0, unlimited)\n");
    printf("\t-d DIR\t\t Navigate the directory specified and process all files and subdirectories\n");
    exit(EXIT_FAILURE);
}

// Aggiunge un nome di file alla coda
bool queue_push(queue_t* queue, const char* file_name) {
    if (queue->count >= MAX_QUEUE_SIZE) {
        return false;
    }
    queue->tail = (queue->tail + 1) % MAX_QUEUE_SIZE;
    strncpy(queue->file_names[queue->tail], file_name, MAX_FILE_NAME_LENGTH);
    queue->count++;
    return true;
}

// Preleva un nome di file dalla coda
bool queue_pop(queue_t* queue, char* file_name) {
    if (queue->count <= 0) {
        return false;
    }
    strncpy(file_name, queue->file_names[queue->head], MAX_FILE_NAME_LENGTH);
    queue->head = (queue->head + 1) % MAX_QUEUE_SIZE;
    queue->count--;
    return true;
}

// Funzione del thread
void* worker_thread(void* arg) {
    queue_t* queue = (queue_t*) arg;
    char file_name[MAX_FILE_NAME_LENGTH];
    while (true) {
        // Preleva un nome di file dalla coda
        if (!queue_pop(queue, file_name)) {
            // Se la coda è vuota, attendi un secondo prima di riprovare
            sleep(1);
            continue;
        }
        // Stampa il nome del file a schermo
        printf("File name: %s\n", file_name);
    }
    return NULL;
}

int main() {
    queue_t queue;
    queue_init(&queue);

    // Crea i thread
    pthread_t thread1, thread2;
    pthread_create(&thread1, NULL, worker_thread, &queue);
    pthread_create(&thread2, NULL, worker_thread, &queue);

    // Aggiunge alcuni nomi di file alla coda
    queue_push(&queue, "file1.txt");
    queue_push(&queue, "file2.txt");
    queue_push(&queue, "file3.txt");

    // Attendi che i thread terminino
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    return 0;
}