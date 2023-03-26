
#ifndef HEADER_FILE_H
#define HEADER_FILE_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>

#define MAX_MESSAGE_LENGTH 2048
#define MAX_FILENAME_LENGTH 256
#define SYSCALL(r,c,e) \
    if((r=c)==-1) { perror(e);exit(errno); }

typedef struct node {
    char file_name[MAX_FILENAME_LENGTH];
    int data;
    struct node *next;
} Node;

void free_list(Node* testa);
void selection_sort(Node *head);
void print_list(Node *head);
void insert_node(Node **head, int data, char * filename);
int update(fd_set *set);
void print_result(char* filename, double num);
void tokenizer_r(char * message, Node **head);
void collector(struct sockaddr_un *psa);

#endif