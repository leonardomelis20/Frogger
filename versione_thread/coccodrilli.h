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
#include "buffer.h"

void draw_crocodile(int x, int y);
void movement_croc(Messaggio* croc);
bool check_borders(Messaggio croc);
void clear_croc(Messaggio msg); 
void* crocodile_thread(void* arg);

#endif