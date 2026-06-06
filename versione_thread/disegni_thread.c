#include "disegni_thread.h"

/**
 * funzione che disegna le tane aperte 
 */
void draw_burrows () {
    int spacing = 0; //distanza tra le tane 
    int x = 0; //coordinata x per disegnare

    attron(COLOR_PAIR(5)); //impostiamo il colore delle tane

    spacing = GAME_WIDTH / NUM_BURROWS; //calcoliamo la distanza tra le tane

    /*ciclo for che scorre per tutte le tane*/
    for (int i = 0; i < NUM_BURROWS; i++) {
        x = i * spacing + (spacing - LARGHEZZA_TANA) / 2; //calcoliamo la x per sapere dove disegnare la tana
        
        mvprintw(1, x, "[       ]"); //disegniamo la tana aperta
    }

    attroff(COLOR_PAIR(5)); //disattiviamo il colore delle tane
}

/**
 * funzione che disegna la tane chiusa 
 * @param tana struct Messaggio che contiene le coordinate della tana da chiudere
 */
void draw_closed_burrows (Messaggio tana) {
    int spacing = 0; //distanza tra le tane 
    int x = 0; //coordinata x per disegnare

    attron(COLOR_PAIR(5)); //impostiamo il colore delle tane

    spacing = GAME_WIDTH / NUM_BURROWS; //calcoliamo la distanza tra le tane
    
    x = (tana.x-1) * spacing + (spacing - LARGHEZZA_TANA) / 2; //calcoliamo la x per sapere dove disegnare la tana 

    mvprintw(1, x, "[///////]"); //disegniamo la tana chiusa
    
    attroff(COLOR_PAIR(5)); //disattiviamo il colore delle tane
}

/**
 * funzione che disegna le safe zones sopra e sotto il fiume
 */
void draw_safety_zones () {
    mvhline(SAFE_ZONE_UP, 0, '=', GAME_WIDTH);
    mvhline(SAFE_ZONE_DOWN, 0, '=', GAME_WIDTH);
}