#include "disegni.h"

/*funzione principale che disegna la schermata di gioco senza la logica*/
int disegni() {
    initscr(); //inizializza ncurses
    noecho(); //non mostra l'input dell'utente
    curs_set(0); //nasconde il cursore
    start_color(); //abilita l'uso dei colori
    /*nizializza le coppie di colori da usare*/
    init_pair(1, COLOR_BLUE, COLOR_BLACK); //colore per il fiume (blu su sfondo nero)
    init_pair(2, COLOR_GREEN, COLOR_BLACK); //colore per le zone sicure (verde)
    init_pair(4, COLOR_YELLOW, COLOR_BLACK); //colore per le tane (giallo)

    /*loop principale*/
    while (1) { 
        clear(); //pulisce lo schermo
        draw_safety_zones(); //disegno le zone sicure
        draw_river(); //disegno il fiume
        draw_burrows(); //disegno le tane

        refresh(); //aggiorno lo schermo con le modifche
        napms(100); //attendo 100 millisecondi prima del prossimo ciclo
    }

    endwin(); //termino la modalità ncurses (!!!!!!!!anche se non verrrà mai interrotto!!!!!!!!)
    return 0;
}

/*funzione che disegna il fiume*/
void draw_river() {
    attron(COLOR_PAIR(1)); //attivo la coppia di colore blu

    /*ciclo sulle righe centrali*/
    for (int i = ALTEZZA_RANA; i < GAME_HEIGHT - ALTEZZA_RANA; i++) {
        mvhline(i, 0, '~', GAME_WIDTH); //disegna '~' per simulare l'acqua
    }

    attroff(COLOR_PAIR(1)); //disattivo il colore
}

/*funzione che serve per disegnare le tane aperte*/
void draw_burrows() {
    attron(COLOR_PAIR(4)); //attivo la coppia di colore giallo

    int spacing = GAME_WIDTH / NUM_BURROWS; //calcolo lo spazio fra una tana e l'altra

    /*ciclo su ogni tana*/
    for (int i = 0; i < NUM_BURROWS; i++) {
        int x = i * spacing + (spacing - LARGHEZZA_TANA) / 2; //centro la tana orizzontalmente nel suo spazio
        mvprintw(1, x, "[       ]"); //stampo la tana aperta
    }

    attroff(COLOR_PAIR(4)); //disattivo il colore
}

/*funzione che serve per disegnare le tane chiuse*/
void draw_closed_burrows(Messaggio tana) {
    attron(COLOR_PAIR(4)); //attivo il colore giallo

    int spacing = GAME_WIDTH / NUM_BURROWS; //calcolo lo spazio fra le tane

    int x = (tana.x-1) * spacing + (spacing - LARGHEZZA_TANA) / 2; //calcolo la posizione x della tana chiusa

    mvprintw(1, x, "[///////]"); //stampo la tana chiusa
    
    attroff(COLOR_PAIR(4)); //disattivo il colore
}

/*funzione che disegna le due zone sicure, ovvero sopra e sotto il fiume*/
void draw_safety_zones() {
    attron(COLOR_PAIR(2)); //attivo il colore verde
 
    mvhline(SAFE_ZONE_UP, 0, '=', GAME_WIDTH); //disegno la linea superiroe per il marciapiede superiore
    mvhline(SAFE_ZONE_DOWN, 0, '=', GAME_WIDTH); //disegno la line ainferiore per il marciapiede superiore

    attroff(COLOR_PAIR(2)); //disattivo il colore
}