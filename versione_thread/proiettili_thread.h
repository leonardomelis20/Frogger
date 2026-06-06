#ifndef PROIETTILI_THREAD_H
#define PROIETTILI_THREAD_H

#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <stdbool.h>

#include "strutture_thread.h"
#include "buffer_thread.h"

void movement_bullet (Messaggio *croc, Messaggio *bullet); 
bool check_bullet_borders (Messaggio bullet); 
void* bullet_thread (void* arg); 

#endif