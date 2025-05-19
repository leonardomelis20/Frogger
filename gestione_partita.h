#ifndef GESTIONE_PARTITA_H
#define GESTIONE_PARTITA_H

#include <stdio.h>
#include <ncurses.h>
#include <time.h>
#include <stdbool.h>
#include <signal.h>
#include <wchar.h>
#include <string.h>


#include "strutture.h"
#include "utility.h"

// Disegna la barra del tempo con i secondi rimanenti
void draw_timer_bar(int seconds_left);

// Inizializza la struttura del timer
void init_timer(TimerInfo* timer);

// Aggiorna il timer e restituisce true se il tempo è scaduto
bool update_timer(TimerInfo* timer);

// Resetta il timer al valore iniziale
void reset_timer(TimerInfo* timer);

void kill_everything (int pipe_fd, Messaggio* croc, Messaggio* bullet, Messaggio* grenade, Messaggio frog);
void draw_hearts(int vite);

bool menu_iniziale();
bool victory(int pipe_fd_write, int pipe_fd_read, 
              Messaggio crocs[NUM_CROC], 
              Messaggio bullets[MAX_BULLETS], 
              Messaggio grenades[MAX_GRENADE], 
              Messaggio frog, 
              const char* message);
bool exit_game(int pipe_fd_write, int pipe_fd_read, 
              Messaggio crocs[NUM_CROC], 
              Messaggio bullets[MAX_BULLETS], 
              Messaggio grenades[MAX_GRENADE], 
              Messaggio frog, 
              const char* message);
void terminate_all_processes(int pipe_fd, Messaggio croc_array[NUM_CROC], 
                           Messaggio bullet_array[MAX_BULLETS], 
                           Messaggio grenade_array[MAX_GRENADE], 
                           Messaggio frog) ;

#endif