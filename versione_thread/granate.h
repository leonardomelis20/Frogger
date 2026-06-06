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
#include "buffer.h"

void draw_grenade(int x, int y);
void clear_grenade(int x, int y); 
bool check_grenade_borders(Messaggio grenade);
void* grenade_thread(void* arg);

#endif