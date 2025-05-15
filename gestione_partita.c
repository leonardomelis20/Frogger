#include "gestione_partita.h" 

#include <curses.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "strutture.h"

#define TIMER_BAR_WIDTH 30
#define TIMER_BAR_HEIGHT 2
#define TIMER_BAR_Y 40
#define TIMER_BAR_X 0

// Funzione che disegna la barra del tempo
void draw_timer_bar(int seconds_left) {
    int i, j;
    int bar_length = (seconds_left * TIMER_BAR_WIDTH) / TIMER_DURATION;
    
    // Prima pulisci l'area della barra
    for (j = 0; j < TIMER_BAR_HEIGHT; j++) {
        for (i = 0; i < TIMER_BAR_WIDTH; i++) {
            mvaddch(TIMER_BAR_Y + j, TIMER_BAR_X + i, ' ');
        }
    }
    
    // Ora disegna la barra del tempo
    attron(A_BOLD);
    
    // Scegli il colore in base al tempo rimanente
    if (seconds_left > 20) {
        attron(COLOR_PAIR(3));  // Verde
    } else if (seconds_left > 10) {
        attron(COLOR_PAIR(4));  // Giallo
    } else {
        attron(COLOR_PAIR(5));  // Rosso
    }
    
    // Disegna la barra superiore
    mvaddch(TIMER_BAR_Y, TIMER_BAR_X, ACS_ULCORNER);
    for (i = 1; i < TIMER_BAR_WIDTH - 1; i++) {
        mvaddch(TIMER_BAR_Y, TIMER_BAR_X + i, ACS_HLINE);
    }
    mvaddch(TIMER_BAR_Y, TIMER_BAR_X + TIMER_BAR_WIDTH - 1, ACS_URCORNER);
    
    // Disegna la parte interna della barra
    for (i = 0; i < bar_length; i++) {
        mvaddch(TIMER_BAR_Y + 1, TIMER_BAR_X + i, '#');
    }
    
    // Disegna la barra inferiore
    mvaddch(TIMER_BAR_Y + TIMER_BAR_HEIGHT - 1, TIMER_BAR_X, ACS_LLCORNER);
    for (i = 1; i < TIMER_BAR_WIDTH - 1; i++) {
        mvaddch(TIMER_BAR_Y + TIMER_BAR_HEIGHT - 1, TIMER_BAR_X + i, ACS_HLINE);
    }
    mvaddch(TIMER_BAR_Y + TIMER_BAR_HEIGHT - 1, TIMER_BAR_X + TIMER_BAR_WIDTH - 1, ACS_LRCORNER);
    
    // Disegna i lati verticali
    for (j = 1; j < TIMER_BAR_HEIGHT - 1; j++) {
        mvaddch(TIMER_BAR_Y + j, TIMER_BAR_X, ACS_VLINE);
        mvaddch(TIMER_BAR_Y + j, TIMER_BAR_X + TIMER_BAR_WIDTH - 1, ACS_VLINE);
    }
    
    // Scrivi il tempo rimanente
    char time_str[10];
    sprintf(time_str, "%2d sec", seconds_left);
    mvprintw(TIMER_BAR_Y + 1, TIMER_BAR_X + TIMER_BAR_WIDTH + 2, time_str);
    
    attroff(A_BOLD);
    if (seconds_left > 20) {
        attroff(COLOR_PAIR(3));
    } else if (seconds_left > 10) {
        attroff(COLOR_PAIR(4));
    } else {
        attroff(COLOR_PAIR(5));
    }
    
    refresh();
}

// Inizializza la struttura del timer
void init_timer(TimerInfo* timer) {
    timer->seconds_left = TIMER_DURATION;
    timer->last_update = time(NULL);
    timer->is_active = true;
}

// Aggiorna il timer e restituisce true se il tempo è scaduto
bool update_timer(TimerInfo* timer) {
    if (!timer->is_active) {
        return false;
    }
    
    time_t current_time = time(NULL);
    time_t elapsed = current_time - timer->last_update;
    
    // Aggiorna il timer solo se è passato almeno un secondo
    if (elapsed >= 1) {
        timer->seconds_left -= elapsed;
        timer->last_update = current_time;
        
        // Disegna la barra del tempo aggiornata
        draw_timer_bar(timer->seconds_left);
        
        // Controlla se il tempo è scaduto
        if (timer->seconds_left <= 0) {
            timer->seconds_left = 0;
            timer->is_active = false;
            return true;  // Tempo scaduto
        }
    }
    
    return false;  // Il tempo non è ancora scaduto
}

// Resetta il timer al valore iniziale
void reset_timer(TimerInfo* timer) {
    timer->seconds_left = TIMER_DURATION;
    timer->last_update = time(NULL);
    timer->is_active = true;
}
