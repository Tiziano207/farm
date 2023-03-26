//
// Created by ubuntu on 09/03/23.
//

#ifndef SIGNAL_INSTALLER_H
#define SIGNAL_INSTALLER_H

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

extern volatile sig_atomic_t sigusr1_received;
void signal_mask();
void signal_installer();
void term_handler(int sig);
void delete_file(char * file);
void sigusr1_handler(int sig);

#endif //SIGNAL_INSTALLER_H
