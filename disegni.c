#include "disegni.h"

int disegni() {
    initscr(); //inizializza ncurses
    noecho(); //non mostra l'input dell'utente
    curs_set(0); //nasconde il cursore

    start_color(); 
    /*definizione colori*/
    init_pair(1, COLOR_BLUE, COLOR_BLACK); // Fiume
    init_pair(2, COLOR_GREEN, COLOR_BLACK); // Prato
    init_pair(4, COLOR_YELLOW, COLOR_BLACK); // Tane

    /*loop principale*/
    while (1) { 
        clear();
        draw_safety_zones();
        draw_river();
        draw_burrows();
        refresh();
        napms(100); //ritardo di aggiornamento
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
        int x = i * spacing + (spacing - LARGHEZZA_TANA) / 2;
        mvprintw(1, x, "[       ]");
    }
    attroff(COLOR_PAIR(4));
}

void draw_closed_burrows(Messaggio tana) {
    attron(COLOR_PAIR(4));
    int spacing = GAME_WIDTH / NUM_BURROWS;
    int x = (tana.x-1) * spacing + (spacing - LARGHEZZA_TANA) / 2;
        mvprintw(1, x, "[///////]");
    
    attroff(COLOR_PAIR(4));
}

void draw_safety_zones() {
    attron(COLOR_PAIR(2));
    mvhline(SAFE_ZONE_UP, 0, '=', GAME_WIDTH); // Prato superiore
    mvhline(SAFE_ZONE_DOWN, 0, '=', GAME_WIDTH); // Prato inferiore
    attroff(COLOR_PAIR(2));
}