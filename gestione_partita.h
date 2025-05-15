#ifndef GESTIONE_PARTITA_H
#define GESTIONE_PARTITA_H

#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h> 
#include <stdbool.h>

#include "strutture.h"


// Prototipi delle funzioni
int main_timer(int pipe_write);
void draw_time_bar(int seconds);
int reset_timer(int pipe_fd, pid_t *pid_timer);

#endif