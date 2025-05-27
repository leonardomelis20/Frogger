#include "granate.h"

char grenade = 'G'; //definizione del carattere che rappresenta la granata 

/**
 * funzione che disegna la granata alla posizione (x, y)
 * @param x coordinata orizzontale della granata
 * @param y coordinata verticale della granata
 */
void draw_grenade(int x, int y) {
    attron(COLOR_PAIR(4)); //attiviamo il colore rosso per la granata
    mvaddch(y, x, grenade); //usiamo la funzione ncurses per disegnare la granata nella coordinata (x,y)
    attroff(COLOR_PAIR(4)); //disattiviamo il colore rosso
}

/**
 * funzione che cancella la granata dalla posizione (x, y) sostituendola con uno spazio
 * @param x coordinata orizzontale della granata
 * @param y coordinata verticale della granata
 */
void clear_grenade(int x, int y) {
    mvaddch(y, x, ' '); //usiamo la funzione ncurses per disegnare lo spazio vuoto nella coordinata (x,y)
}

/**
 * funzione che controlla se la granata ha toccato i bordi dell'area di gioco
 * @param grenade struct Messaggio contenente le informazioni sulla granata
 * @return true se la granata è al di fuori dei bordi, false altrimenti
 */
bool check_grenade_borders(Messaggio grenade) {
    return (grenade.x <= 0 || grenade.x >= GAME_WIDTH); //controlliamo se la coordinata x è fuori dai limiti dell'area di gioco
}

/**
 * funzione che si occupa del thread della granata
 * @param arg puntatore ai parametri del thread
 * @return NULL
 */
void* grenade_thread(void* arg) {
    Grenade_arg* params = (Grenade_arg*) arg; 
    Circular_buffer* buffer = params->buffer; 
    Messaggio grenade = params->copy;

    if (grenade.direzione == 1) {
        grenade.x = grenade.x + 2; 
    }

    /*inizializziamo la granata*/
    grenade.is_active = true; //settiamo la flag a true per indicare che la granata è attiva 
    grenade.velocita = GRENADE_SPEED; //impostiamo la velocità della granata mediante la macro apposita costante
    grenade.oggetto = ID_GRENADE; //impostiamo il tipo dell'oggetto per identificarlo come granata 
    grenade.tid = pthread_self(); //salviamo il pid del processo corrente nella struct Messaggio

    produce_msg(buffer, grenade); //inviamo alla pipe lo stato iniziale della granata

    /*!!!!!
    nel while al posto di 1 game_running
    è una variabile globale che si trova nel file buffer.c ->
    -> bool game_running = true;
    !!!!!*/
    /*cicliamo all'infinito per il moviemnto della granata*/
    while(1) {
    
        if (grenade.direzione == 1) {
            grenade.x += 2; 
        } 
        else {
            grenade.x -= 2;
        }
       
        if (grenade.direzione == 1) {
            grenade.x += 1;
        } 
        else {
            /*!!!!!
            in teoria è -= 
            !!!!!*/
            grenade.x -= 1; 
        }
    
        if (check_grenade_borders(grenade)) {
            grenade.is_active = false; 
            produce_msg(buffer, grenade); 
            break; 
        }

        produce_msg(buffer, grenade);
        usleep(GRENADE_SPEED);
    }
    
    free(params); 
    return NULL; 
}