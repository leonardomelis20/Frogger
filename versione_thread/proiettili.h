#ifndef PROIETTILI_H
#define PROIETTILI_H

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

void draw_bullet(int x, int y);
void clear_bullet(int x, int y);
void movement_bullet(Messaggio* croc, Messaggio* bullet); 
bool check_bullet_borders(Messaggio bullet); 
void main_bullet(int pipe_fd, Messaggio copy);

#endif