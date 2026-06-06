#ifndef COLLISIONI_THREAD_H
#define COLLISIONI_THREAD_H

#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <pthread.h>

#include "strutture_thread.h"
#include "proiettili_thread.h"
#include "granate_thread.h"

int check_collision (Messaggio frog, Messaggio* croc); 
void frog_with_croc (Messaggio* frog, Messaggio* croc); 
bool check_safe_zone (Messaggio frog); 
bool river (Messaggio frog); 
bool collision_b_g (Messaggio* bullet, Messaggio* grenade, int count_bullet, int count_grenade); 

#endif