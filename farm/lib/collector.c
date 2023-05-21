#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>

#define MAX_MESSAGE_LENGTH 2048
#define SYSCALL(r,c,e) \
    if((r=c)==-1) { perror(e);exit(errno); }

typedef struct node {
    char *file_name;
    long long data;
    struct node *next;
} Node;

    /**
     * Funzione che si occupa di deallocare la memoria dinamica
     * utilizzata per memorizzare la lista
     */
void free_list(Node* testa) {
    Node* current_node = testa;
    while (current_node != NULL) {
        Node* next_node = current_node->next;
        free(current_node->file_name);
        free(current_node);
        current_node = next_node;
    }
}

    /**
     * Funzione adibita alla stampa della lista
     */
void print_list(Node *head) {
    Node *current = head;

    while (current != NULL) {
        printf("%lld %s\n", current->data, current->file_name);
        current = current->next;
    }
    return;
}

    /**
     * Funzione per l'inserimento ordinato di un nodo in lista
     */
void insert_node(Node **head, long long data, char * filename) {
    Node *new_node = malloc(sizeof(Node));
    if(new_node == NULL){perror("malloc");exit(errno);}
    new_node->file_name = (char*)malloc(strlen(filename)+1);
    if(new_node->file_name  == NULL){perror("malloc");exit(errno);}
    strncpy(new_node->file_name, filename, strlen(filename)+1);
    new_node->data = data;
    new_node->next = NULL;

    Node *current = *head;
    if(current == NULL){
        *head = new_node;
        return;
    }

    if(current->data >= new_node->data){
        new_node->next =current;
        *head = new_node;
        return;
    }

    while(current->next != NULL && current->next->data <= new_node->data){
        current = current->next;
    }
    new_node->next = current->next;
    current->next = new_node;
    return;
}

    /**
     * Funzione che si occupa di "aggiornare" il valore del descrittore,
     * restituendo il valore del descrittore più grande nel set
     */
int update(fd_set *set) {
    int fd_num = 0, fd;
    for (fd = 0; fd <= FD_SETSIZE; fd++) {
        if (FD_ISSET(fd, set)) {
            if (fd > fd_num) fd_num = fd;
        }
    }
    return fd_num;
}

    /**
     * Funzione che ricava da una stringa una sottostringa
     */
void substring(char * str, char * subs){
    int i;
    for (i = 0; i < strlen(str); i++) {
        if (str[i] == '<') {
            break; // interrompe il ciclo appena incontra il carattere <
        }
        subs[i] = str[i];
    }
    subs[i] = '\0';
    return;
}

    /**
     * Funzione che si occopa di dividere il messaggio utilizzando il
     * carattere separatore ';'
    */
void tokenizer_r(char * message, Node **head){
    char* saveptr;
    char* token;
    char* endptr;
    long long num;
    const char s[2] = ";";
    char subs[MAX_MESSAGE_LENGTH];
    substring(message,subs);
    token = strtok_r(subs, s, &saveptr);

    num = strtoll(token, &endptr, 10);
    token = strtok_r(NULL, s, &saveptr);
    insert_node(head,num,token);
    return;
}

    /**
     * Il Collector è il server che gestisce le richieste dei vari client.
     * Si occupa di ricevere i vari messaggi e memorizzare i valori in una lista
     * concatenata, in maniera ordinata (per valore) e al termine stamperà in output.
     * Inoltre, il collector gestisce i vari segnali e termina quando riceve il messaggio "DONE"
     * dai client.
     */
void collector(struct sockaddr_un *psa) {
    int stop;
    int master_socket, client_socket, num_active_descriptor = 0, new_socket;
    char buf[MAX_MESSAGE_LENGTH];
    fd_set set, rdset;
    int nread, r;
    Node *head = NULL;

    SYSCALL(master_socket, socket(AF_UNIX, SOCK_STREAM, 0), "socket");
    SYSCALL(r, bind(master_socket, (struct sockaddr *) psa, sizeof(*psa)), "bind");
    SYSCALL(r, listen(master_socket, SOMAXCONN), "listen");

    if (master_socket > num_active_descriptor) num_active_descriptor = master_socket;
    FD_ZERO(&set);
    FD_SET(master_socket, &set);

    while (1) {
        rdset = set;
        SYSCALL(r, select(num_active_descriptor + 1, &rdset, NULL, NULL, NULL), "select");

        for (new_socket = 0; new_socket <= num_active_descriptor; new_socket++) {
            if (FD_ISSET(new_socket, &rdset)) {
                if (new_socket == master_socket) {/* sock connect pronto */
                    SYSCALL(client_socket, accept(master_socket, NULL, 0), "accept");
                    FD_SET(client_socket, &set);
                    if (client_socket > num_active_descriptor) num_active_descriptor = client_socket;
                } else {/* sock I/0 pronto */
                    SYSCALL(nread, read(new_socket, buf, MAX_MESSAGE_LENGTH), "read");
                    if (nread == 0) {
                        FD_CLR(new_socket, &set);
                        num_active_descriptor = update(&set);
                        close(new_socket);
                    } else {
                            /**
                             * Se il Collector riceverà il messaggio DONE, terminerà il ciclo, impostando la variabile
                             * stop a 1
                             */
                        if(strncmp("DONE", buf, 4) == 0){
                            SYSCALL(r,write(new_socket,"ACK",4* sizeof(char)), "write");
                            stop = 1;
                            /**
                             * Se viene ricevuto il messaggio SIGUSR1 da un client, significa che il processo
                             * MasterWorker (più in particolare il signal_thread) ha ricevuto il segnale SIGUSR1.
                             * Il processo quindi stamperà la lista (ordinata poiché è presente un inserimento
                             * ordinato dei file presenti all'arrivo della recezione del segnale.
                             */
                        }else if (strncmp("SIGUSR1", buf, 8) == 0){
                            SYSCALL(r,write(new_socket,"ACK",4* sizeof(char)), "write");
                            print_list(head);
                            /**
                             * Quando il collector riceve un messaggio, la stringa contenuta nel messaggio viene
                             * tokenizzata e inserita nella lista. Inoltre, ad ogni messaggio il Collector manderà
                             * un messaggio di ackloledge per notificare al client l'avvenuta recezione del messaggio.
                             */
                        }else{
                            char *message = malloc(sizeof(char) * MAX_MESSAGE_LENGTH);
                            if(message == NULL){perror("malloc");exit(errno);}
                            strcpy(message, buf);
                            SYSCALL(r,write(new_socket,"ACK",4* sizeof(char)), "write");
                            tokenizer_r(message, &head);
                            free(message);
                        }

                    }
                }
            }
        }
            if(stop == 1){ break;}
        }

    close(master_socket);
    print_list(head);
    free_list(head);
    unlink("./farm.sck");
    exit(0);
}
