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

void frog(int pipe_fd, bool*, int*);
bool is_inside(Messaggio);
void tane(Messaggio);
int num_tana(Messaggio);
void draw_frog(int x, int y);
void clear_frog(int x, int y);

#endif
