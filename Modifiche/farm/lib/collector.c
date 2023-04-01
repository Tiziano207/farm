
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
    long long data;
    struct node *next;
} Node;

void free_list(Node* testa) {
    Node* current_node = testa;
    while (current_node != NULL) {
        Node* next_node = current_node->next;
        free(current_node->file_name);
        free(current_node);
        current_node = next_node;
    }
}
void sort_list(struct node** head) {
    struct node *current, *next;
    long long temp_data;
    char* temp_stringa;

    if (*head == NULL) {
        return;
    }

    for (current = *head; current != NULL; current = current->next) {
        for (next = current->next; next != NULL; next = next->next) {
            if (current->data > next->data) {
                temp_data = current->data;
                current->data = next->data;
                next->data = temp_data;

                temp_stringa = current->file_name;
                current->file_name = next->file_name;
                next->file_name = temp_stringa;
            }
        }
    }
}
void print_list(Node *head) {
    Node *current = head;

    while (current != NULL) {
        printf("%lld %s\n", current->data, current->file_name);
        current = current->next;
    }
    return;
}
void insert_node(Node **head, long long data, char * filename) {
    Node *new_node = malloc(sizeof(Node));
    new_node->file_name = (char*)malloc(strlen(filename)+1);
    strncpy(new_node->file_name, filename, strlen(filename)+1);
    new_node->data = data;
    new_node->next = NULL;
    new_node->next = *head;
    *head = new_node;
}
int update(fd_set *set) {
    int fd_num = 0, fd;
    for (fd = 0; fd <= FD_SETSIZE; fd++) {
        if (FD_ISSET(fd, set)) {
            if (fd > fd_num) fd_num = fd;
        }
    }
    return fd_num;
}
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
/*Funzione che mi permette di leggere da una socket (é quella presente nel DidaWiki)*/
static inline int readn(long fd, void *buf, size_t size) {
    size_t left = size;
    int r;
    char *bufptr = (char*)buf;
    while(left>0) {
        if ((r=read((int)fd ,bufptr,left)) == -1) {
            if (errno == EINTR) continue;
            return -1;
        }
        if (r == 0) {return 0;}   // EOF
        left-= r;
        bufptr+= r;
    }
    return size;
}

/*Funzione che mi permette di scrivere su una socket (é quella presente nel DidaWiki)*/
static inline int writen(long fd, void *buf, size_t size) {
    size_t left = size;
    int r;
    char *bufptr = (char*)buf;
    while(left>0) {
        if ((r=write((int)fd ,bufptr,left)) == -1) {
            if (errno == EINTR) continue;
            return -1;
        }
        if (r == 0) return 0;
        left    -= r;
        bufptr  += r;
    }
    return 1;
}

void collector(struct sockaddr_un *psa) {
    signal_mask();
        int stop;
        int master_socket, client_socket, num_active_descriptor = 0, new_socket;
        char buf[MAX_MESSAGE_LENGTH];
        fd_set set, rdset;
        int nread, r;
        Node *head = NULL;

        SYSCALL(master_socket, socket(AF_UNIX, SOCK_STREAM, 0), "socket")
        SYSCALL(r, bind(master_socket, (struct sockaddr *) psa, sizeof(*psa)), "bind");
        SYSCALL(r, listen(master_socket, SOMAXCONN), "listen");

        if (master_socket > num_active_descriptor) num_active_descriptor = master_socket;
        FD_ZERO(&set);
        FD_SET(master_socket, &set);

        int message_received = 0;

        while (1) {
            if (sigusr1_received) {
                sigusr1_received = 0;
                break;
            }

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
                            if(strncmp("DONE", buf, 4) == 0){
                                SYSCALL(r,write(new_socket,"ACK",4* sizeof(char)), "write");
                                stop = 1;
                            }else{
                                char *message = malloc(sizeof(char) * MAX_MESSAGE_LENGTH);
                                strcpy(message, buf);
                                SYSCALL(r,write(new_socket,"ACK",4* sizeof(char)), "write");
                                tokenizer_r(message, &head);
                                message_received++;
                                free(message);
                            }

                        }
                    }
                }
            }
            if(stop == 1){ break;}
        }
        close(master_socket);
        sort_list(&head);
        print_list(head);
        free_list(head);
        unlink("./farm.sck");
        exit(0);
}
