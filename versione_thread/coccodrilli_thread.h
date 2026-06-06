#ifndef COCCODRILLI_THREAD_H
#define COCCODRILLI_THREAD_H

#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <stdbool.h>

#include "strutture_thread.h"
#include "buffer_thread.h"

void movement_croc (Messaggio* croc); 
bool check_borders (Messaggio croc); 
void* crocodile_thread (void* arg); 

#endif