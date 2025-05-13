#ifndef GRANATE_H
#define GRANATE_H

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

void draw_grenade(int x, int y);
void clear_grenade(int x, int y);
void movement_grenade_left(Messaggio *grenade);
void movement_grenade_right(Messaggio *grenade);
bool check_grenade_borders(Messaggio grenade);
void main_grenade(int pipe_fd, Messaggio grenade);

#endif