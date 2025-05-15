#ifndef GESTIONE_PARTITA_H
#define GESTIONE_PARTITA_H

#include <stdio.h>
#include <ncurses.h>
#include <time.h>
#include <stdbool.h>
#include <signal.h>

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

#endif