#ifndef COLLECTOR_H
#define COLLECTOR_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>

typedef struct node {
    char *file_name;
    int data;
    struct node *next;
} Node;

void free_list(Node* testa);
void print_list(Node *head);
void insert_node(Node **head, int data, char * filename);
int update(fd_set *set);
void substring(char * str, char * subs);
void tokenizer_r(char * message, Node **head);
void collector(struct sockaddr_un *psa);

#endif //COLLECTOR.H