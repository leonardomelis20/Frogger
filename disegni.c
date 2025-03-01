#include <ncurses.h>
#include <stdlib.h>
#include "strutture.h"
#include "disegni.h"

int disegni() {
    initscr();            // Inizializza ncurses
    noecho();             // Non mostra input dell'utente
    curs_set(0);          // Nasconde il cursore
    start_color();
    
    // Definizione colori
    init_pair(1, COLOR_BLUE, COLOR_BLACK); // Fiume
    init_pair(2, COLOR_GREEN, COLOR_BLACK); // Prato
    init_pair(4, COLOR_YELLOW, COLOR_BLACK); // Tane

    while (1) { // Loop principale
        clear();
        draw_safety_zones();
        draw_river();
        draw_burrows();
        refresh();
        napms(100); // Ritardo di aggiornamento
    }

    endwin();
    return 0;
}

void draw_river() {
    attron(COLOR_PAIR(1));
    for (int i = ALTEZZA_RANA; i < GAME_HEIGHT - ALTEZZA_RANA; i++) {
        mvhline(i, 0, '~', GAME_WIDTH);
    }
    attroff(COLOR_PAIR(1));
}

void draw_burrows() {
    attron(COLOR_PAIR(4));
    int spacing = GAME_WIDTH / NUM_BURROWS;
    for (int i = 0; i < NUM_BURROWS; i++) {
        int x = i * spacing + (spacing - LARGHEZZA_RANA) / 2;
        mvhline(1, x, '[', LARGHEZZA_RANA);
    }
    attroff(COLOR_PAIR(4));
}

void draw_safety_zones() {
    attron(COLOR_PAIR(2));
    mvhline(0, 0, '=', GAME_WIDTH); // Prato superiore
    mvhline(GAME_HEIGHT - 1, 0, '=', GAME_WIDTH); // Prato inferiore
    attroff(COLOR_PAIR(2));
}
