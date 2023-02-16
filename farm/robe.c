//
// Created by ubuntu on 16/02/23.
//

// definizione struttura dati per i task da elaborare

void process_file(char* filename, int client_socket) {
    FILE* fp = fopen(filename, "rb");
    if (fp == NULL) {
        printf("Cannot open file %s\n", filename);
        return;
    }

    // legge il contenuto del file
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char* file_buffer = (char*)malloc(file_size);
    fread(file_buffer, file_size, 1, fp);
    fclose(fp);

    // calcola il risultato
    long result = 0;
    for (int i = 0; i < file_size; i++) {
        result += file_buffer[i];
    }

    // invia il risultato al processo Collector tramite la connessione socket
    char buffer[BUFFER_SIZE];
    snprintf(buffer, BUFFER_SIZE, "%s:%ld", filename, result);
    send(client_socket, buffer, BUFFER_SIZE, 0);
    free(file_buffer);
}

void* worker_thread(void* arg) {
    int client_socket = *(int*)arg;

    while (1) {
        // riceve un nome di file dalla coda dei task
        char buffer[BUFFER_SIZE];
        int recv_result = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (recv_result <= 0) {
            break;
        }

        // processa il file
        process_file(buffer, client_socket);
    }

    close(client_socket);
    pthread_exit(NULL);
}
