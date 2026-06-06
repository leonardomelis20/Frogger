#ifndef RANA_THREAD_H
#define RANA_THREAD_H

#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <pthread.h>

#include "strutture_thread.h"
#include "buffer_thread.h"
#include "disegni_thread.h"

bool is_inside (Messaggio msg); 
int num_tana (Messaggio msg); 
void* frog_thread (void* arg); 

#endif