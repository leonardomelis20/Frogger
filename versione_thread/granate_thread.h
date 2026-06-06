#ifndef GRANATE_THREAD_H
#define GRANATE_THREAD_H

#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <stdbool.h>

#include "strutture_thread.h"
#include "buffer_thread.h"

void movement_grenade_left (Messaggio* grenade); 
void movement_grenade_right (Messaggio* grenade); 
bool check_grenade_borders (Messaggio grenade); 
void* grenade_thread (void* arg); 

#endif