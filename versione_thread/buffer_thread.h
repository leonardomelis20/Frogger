#ifndef BUFFER_THREAD_H
#define BUFFER_THREAD_H

#include <stdio.h>
#include <stdlib.h>

#include "strutture_thread.h"

void init_buffer (Shared_buffer* buffer); 
void destroy_buffer (Shared_buffer* buffer); 
void buffer_put(Shared_buffer* buffer, Messaggio msg); 
Messaggio buffer_get(Shared_buffer* buffer); 
void init_game_data(Game_data* game_data);
void destroy_game_data(Game_data* game_data); 
bool buffer_try_get(Shared_buffer* buffer, Messaggio* msg); 

#endif