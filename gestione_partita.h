#ifndef GESTIONE_PARTITA_H
#define GESTIONE_PARTITA_H

#include <stdio.h>
#include <ncurses.h>
#include <time.h>
#include <stdbool.h>

#include "strutture.h"

// Disegna la barra del tempo con i secondi rimanenti
void draw_timer_bar(int seconds_left);

// Inizializza la struttura del timer
void init_timer(TimerInfo* timer);

// Aggiorna il timer e restituisce true se il tempo è scaduto
bool update_timer(TimerInfo* timer);

// Resetta il timer al valore iniziale
void reset_timer(TimerInfo* timer);

#endif