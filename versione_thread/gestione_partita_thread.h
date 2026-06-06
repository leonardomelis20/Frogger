#ifndef GESTIONE_PARTITA_THREAD_H
#define GESTIONE_PARTITA_THREAD_H

#define _XOPEN_SOURCE_EXTENDED  //wide characters

#include <stdio.h>
#include <ncurses.h>
#include <time.h>
#include <stdbool.h>
#include <pthread.h>
#include <wchar.h>
#include <string.h>
#include <signal.h> 

#include <wchar.h>              
#include <locale.h>      

#include "strutture_thread.h"
#include "buffer_thread.h"
#include "disegni_thread.h"

void init_timer (Info_timer* timer);
bool update_timer (Info_timer* timer);
void reset_timer (Info_timer* timer); 
void cleanup_game_threads (Game_data* game_data); 
bool exit_game (Game_data* game_data); 
bool victory (Game_data* game_data); 
bool menu_iniziale (); 

#endif