#ifndef RANA_H
#define RANA_H

#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#include "strutture.h"
#include "disegni.h"
#include "buffer.h"

void draw_frog(int x, int y);
void clear_frog(int x, int y);
bool is_inside (Messaggio msg);
int num_tana(Messaggio msg); 
void tane(Messaggio msg);
void* frog_thread(void* arg); 

#endif
