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

int main_croc(int, int, int, int, bool);
void draw_crocodile(int, int); 
int get_idx_croc(Messaggio*, pid_t);
void movement_croc(Messaggio*);
bool check_borders(Messaggio);
void clear_croc(Messaggio);

#endif