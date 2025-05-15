#include "disegni.h"

/**
 * funzione principale che disegna la schermata di gioco senza la logica.
 * inizializza la libreria ncurses, i colori e gestisce il loop principale di disegno 
 * che visualizza il fiume, le zone sicure e le tane.
 * @return 0 nel caso di successo
 */
int disegni() {
    initscr(); //inizializziamo ncurses
    noecho(); //non mostriamo l'input dell'utente
    curs_set(0); //nascondiamo il cursore
    start_color(); //abilitiamo l'uso dei colori


    /*loop principale di disegno*/
    while (1) { 
        clear(); //puliamo lo schermo
        draw_safety_zones(); //disegniamo le zone sicure
        draw_river(); //disegniamo il fiume
        draw_burrows(); //disegniamo le tane

        refresh(); //aggiorniamo lo schermo con le modifche
        napms(100); //attendiamo 100 millisecondi prima del prossimo ciclo
    }

    endwin(); //terminiamo la modalità ncurses (!!!!!!!!anche se non verrrà mai interrotto!!!!!!!!)
    return 0;
}

/**
 * funzione che disegna l'area del fiume al centro dello schermo.
 * utilizza il simbolo '~' con colore blu su sfondo nero per rappresentare l'acqua del fiume 
 * nella zona centrale dello schermo tra le due zone sicure.
 */
void draw_river() {
    attron(COLOR_PAIR(1)); //attiviamo la coppia di colore blu

    /*cicliamo sulle righe centrali*/
    for (int i = ALTEZZA_RANA; i < GAME_HEIGHT - ALTEZZA_RANA; i++) {
        mvhline(i, 0, '~', GAME_WIDTH); //disegniamo '~' per simulare l'acqua
    }

    attroff(COLOR_PAIR(1)); //disattiviamo il colore
}

/**
 * funzione che disegna le tane aperte nella parte superiore dello schermo. 
 * visualizza NUM_BURROWS tane aperte, che sono rappresentate come "[       ]" 
 * e distribuite nella parte superiore dello schermo usando il colore giallo.
 */
void draw_burrows() {
    int spacing = 0; //variabile che serve per salvare lo spazio far una tana e un'altra
    int x = 0; //variabile che serve per salvare la tana al centro del suo spazio

    attron(COLOR_PAIR(4)); //attiviamo la coppia di colore giallo

    spacing = GAME_WIDTH / NUM_BURROWS; //calcoliamo lo spazio fra una tana e l'altra

    /*cicliamo su ogni tana*/
    for (int i = 0; i < NUM_BURROWS; i++) {
        x = i * spacing + (spacing - LARGHEZZA_TANA) / 2; //centriamo la tana orizzontalmente nel suo spazio
        mvprintw(1, x, "[       ]"); //stampiamo la tana aperta
    }

    attroff(COLOR_PAIR(4)); //disattiviamo il colore
}

/**
 * funzione che disegna una tana chiusa specifica.
 * visualizza una singola tana chiusa, che è appresentata come "[///////]"
 * nella posizione specificata dal parametro tana, usando il colore giallo.
 * @param tana struct Messaggio che contiene le coordinate della tana da chiudere
 */
void draw_closed_burrows(Messaggio tana) {
    int spacing = 0; //variabile che serve per salvare lo spazio far una tana e un'altra
    int x = 0; //variabile che serve per salvare la tana chiusa nella posizione x 

    attron(COLOR_PAIR(4)); //attiviamo il colore giallo

    spacing = GAME_WIDTH / NUM_BURROWS; //calcoliamo lo spazio fra le tane

    x = (tana.x-1) * spacing + (spacing - LARGHEZZA_TANA) / 2; //calcoliamo la posizione x della tana chiusa
    mvprintw(1, x, "[///////]"); //stampiamo la tana chiusa
    
    attroff(COLOR_PAIR(4)); //disattiviamo il colore
}

/**
 * funzione che disegna le due zone sicure sopra e sotto il fiume
 * visualizza due linee orizzontali di caratteri '=' con colore verde per rappresentare i marciapiedi sicuri 
 * dove la rana può stare senza pericolo, uno sopra e uno sotto il fiume.
 */
void draw_safety_zones() {
    //attron(COLOR_PAIR(1)); //attiviamo il colore verde
 
    mvhline(SAFE_ZONE_UP, 0, '=', GAME_WIDTH); //disegniamo la linea superiroe per il marciapiede superiore
    mvhline(SAFE_ZONE_DOWN, 0, '=', GAME_WIDTH); //disegniamo la line ainferiore per il marciapiede superiore

    //attroff(COLOR_PAIR(1)); //disattiviamo il colore
}