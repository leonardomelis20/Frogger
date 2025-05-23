#ifndef COCCODRILLI_H
#define COCCODRILLI_H

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

void draw_crocodile(int x, int y);
void movement_croc(Messaggio* croc);
bool check_borders(Messaggio croc);
void clear_croc(Messaggio msg);
int get_index_croc(Messaggio msg[], pid_t pid);
int main_croc(int pipe_fd, int num, int direzione, int speed, bool flag);

#endif