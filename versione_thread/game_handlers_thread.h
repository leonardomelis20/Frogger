#ifndef GAME_HANDLERS_THREAD_H
#define GAME_HANDLERS_THREAD_H

#include <signal.h> 
#include <pthread.h>
#include <semaphore.h>

#include "strutture_thread.h"
#include "buffer_thread.h"
#include "rana_thread.h"
#include "coccodrilli_thread.h"
#include "proiettili_thread.h"
#include "granate_thread.h"
#include "collisioni_thread.h"
#include "gestione_partita_thread.h"
#include "sprites_thread.h"

void handle_frog_message (Game_data* game_data, Messaggio* msg, int* prev_x_rana, int* prev_y_rana); 
void handle_crocodile_message (Game_data* game_data, Messaggio* msg, int* prev_x_cocc, int* prev_y_cocc); 
void handle_respawn_message (Game_data* game_data, Messaggio* msg, pthread_t* croc_tids, Thread_args* croc_args, int* direzioni); 
void handle_create_bullet (Game_data* game_data, Messaggio* msg, pthread_t* bullet_tids, int* bullet_count); 
void handle_bullet_message (Game_data* game_data, Messaggio* msg, pthread_t* bullet_tids, int* bullet_count); 
void handle_create_grenade(Game_data* game_data, Messaggio* msg, pthread_t* grenade_tids, int* grenade_count); 
void handle_grenade_message(Game_data* game_data, Messaggio* msg, pthread_t* grenade_tids, int* grenade_count); 
void handle_frog_death (Game_data* game_data); 
void handle_timeout (Game_data* game_data); 
void handle_pause(Game_data* game_data); 
void check_game_conditions (Game_data* game_data); 
void update_display (Game_data* game_data); 

#endif