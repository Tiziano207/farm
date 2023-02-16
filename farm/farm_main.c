#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <pthread.h>
#include <errno.h>
#include <string.h>
#define BUFFER_SIZE 1024
#define MAX_FILENAME_LENGTH 256
#define MAX_BUFFER_SIZE 1024
#define NUM_MAX_FILE 200
#define MAX_QUEUE_SIZE 1024
#define MAX_WORKERS 10


void Pthread_create(pthread_t *thread_id, const pthread_attr_t *attr, void* (*start_fcn) (void *), void* arg) {
    int err;
    if ( ( err = pthread_create(thread_id, attr, start_fcn, arg)) != 0 ) {errno = err; perror("create"); pthread_exit((void*)errno);}
}
void Pthread_join(pthread_t thread_id,void** status_ptr){
    int err;
    if ( ( err = pthread_join(thread_id, status_ptr)) != 0 ) {errno = err; perror("join"); pthread_exit((void*) errno);}
}

typedef struct {
    char file_array[MAX_QUEUE_SIZE][MAX_FILENAME_LENGTH];
    int head;
    int tail;
    int full;
    int empty;
    pthread_mutex_t mutex;
    pthread_cond_t not_full;
    pthread_cond_t not_empty;
} queue_t;
void queue_init(queue_t *q) {
    //q->file_array = malloc(sizeof(char*) * NUM_MAX_FILE);
    q->head = 0;
    q->tail = 0;
    q->full = 0;
    q->empty = 1;
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->not_full, NULL);
    pthread_cond_init(&q->not_empty, NULL);
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
void queue_push(char *file_name, queue_t *q) {
    //eseguo la lock poiché vado a modificare la struttura dati
    pthread_mutex_lock(&q->mutex);
    while (q->full) {
        //se la coda è piena allora aspetto che venga eseguita una sigal e rilascio la lock
        pthread_cond_wait(&q->not_full, &q->mutex);
    }

    strncpy(q->file_array[q->tail], file_name, MAX_FILENAME_LENGTH);
    q->tail = (q->tail + 1) % MAX_QUEUE_SIZE;
    q->empty = 0;
    //se la coda è uguale alla testa allora è full
    if (q->tail == q->head) {
        q->full = 1;
    }
    pthread_cond_signal(&q->not_empty);
    pthread_mutex_unlock(&q->mutex);
}

void queue_pop( char *destination,queue_t *q) {

    //acquisisco la lock
    pthread_mutex_lock(&q->mutex);
    //se è vuota mi metto in attesa
    while (q->empty) {
        pthread_cond_wait(&q->not_empty, &q->mutex);
    }
    //copio il messaggio
    strncpy(destination, q->file_array[q->head] ,MAX_FILENAME_LENGTH);
    q->head = (q->head + 1) % MAX_QUEUE_SIZE;
    q->full = 0;
    if (q->head == q->tail) {
        q->empty = 1;
    }
    pthread_cond_signal(&q->not_full);
    pthread_mutex_unlock(&q->mutex);
}

int is_regular_file(const char *path) {
    struct stat path_stat;
    if (stat(path, &path_stat) != 0) {
        return 0;
    }
    return S_ISREG(path_stat.st_mode);
}
int is_dir(char * dir_name, queue_t * queue){
    if(dir_name == NULL) return 0;
    DIR *dir;
    struct dirent *entry;
    if ((dir = opendir(dir_name)) == NULL) { perror("opendir"); exit(EXIT_FAILURE);}
        while ((entry = readdir(dir)) != NULL) {
            char file_path[1024];
            snprintf(file_path, sizeof(file_path), "%s/%s", dir_name, entry->d_name);
            if (is_regular_file(file_path)) {
                // Process file
                printf("Processing file: %s\n", file_path);
                queue_push(file_path, queue);
                printf("FILE %s PUSHED", file_path);
            } else if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                // Process subdirectory
                printf("Processing directory: %s\n", file_path);
            }
        }
        closedir(dir);
        return 1;
}

double calculateResult(long *nums, int n) {
    double result = 0.0;
    for (int i = 0; i < n; i++) {
        result += (i * (double)nums[i]);
    }
    return result;
}

void *worker(void *args) {
    queue_t * queue = (queue_t*) args;
   // worker_args_t *cargs = (worker_args_t *) args;
   while(1){
       char* file_name = malloc(MAX_FILENAME_LENGTH*sizeof(char));
        //ora il worker ha il nome del file
        queue_pop(file_name,queue);

        FILE *fp = fopen(file_name, "rb");
        if (!fp) {perror("openfile");pthread_exit((void*)1); }

        int n ;
        fread(&n, sizeof(int), 1, fp);

        errno = 0;
        rewind(fp);
        if (errno != 0){perror("rewind");pthread_exit((void*)1);}

        // Allochiamo spazio per gli elementi
        long *nums = (long *)malloc(n * sizeof(long));
        if(nums== NULL){perror("malloc");pthread_exit((void*)1);}

        // Leggiamo gli elementi dal file
        fread(nums, sizeof(long), n, fp) ;

        // Chiudiamo il file
        fclose(fp);
        if (errno != 0){perror("rewind");pthread_exit((void*)1);}

        // Calcoliamo il risultato
        double result = calculateResult(nums, n);
        //  Stampiamo il risultato
        printf("Il risultato del file %s è: %0.2f\n", file_name, result);
        // Liberiamo la memoria allocata
        free(nums);
        free(file_name);
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    int num_workers = 1;
    int quiet_mode = 0;
    int max_execution_time = 0;
    char *dir_name = NULL;

    int opt;
    while ((opt = getopt(argc, argv, "n:qt:d:")) != -1) {
        switch (opt) {
            case 'n':
                num_workers = atoi(optarg);
                if (num_workers > MAX_WORKERS) {
                    printf("Maximum number of workers is %d\n", MAX_WORKERS);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'q':
                quiet_mode = 1;
                break;
            case 't':
                max_execution_time = atoi(optarg);
                break;
            case 'd':
                dir_name = optarg;
                break;
            default:
                usage();
        }
    }

    queue_t queue;
    queue_init(&queue);

    if(!is_dir(dir_name, &queue)) {
        for (int i = optind; i < argc; i++) {
            if (is_regular_file(argv[i])) {
                // Process file
                printf("Processing file: %s\n", argv[i]);
                queue_push(argv[i],&queue);

            } else {
                printf("%s is not a regular file\n", argv[i]);
            }
        }
    }
   // queue_pop(dest, &queue);
    pthread_t pid;
    //Pthread_create(&pid, NULL, worker, &queue);
    //Pthread_join(pid,NULL);
    // Creiamo un array di thread
    pthread_t threads[num_workers];
    // Lanciamo un thread per ogni file/*
    for (int i = 0; i < num_workers; i++) {
        pthread_create(&threads[i], NULL, worker,  &queue);
    }

    // Aspettiamo la terminazione di tutti i thread
   for (int i = 0; i < num_workers; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}