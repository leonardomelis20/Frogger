#include "proiettili.h"

char bullet = 'P'; //definizione del carattere che rappresenta il proiettile 

/**
 * funzione che disegna il proiettile alla posizione (x, y)
 * @param x coordinata orizzontale del proiettile
 * @param y coordinata verticale del proiettile
 */
void draw_bullet(int x, int y) {
    attron(COLOR_PAIR(3)); //attiviamo il colore m,agente per il proiettile
    mvaddch(y, x, bullet); //uso la funzione ncurses per disegnare il proiettile nella coordinata (x,y)
    attroff(COLOR_PAIR(3)); //disattiviamo il colore magenta
}

/**
 * funzione che cancella il proiettile dalla posizione (x, y) sostituendolo con uno spazio
 * @param x coordinata orizzontale del proiettile
 * @param y coordinata verticale del proiettile
 */
void clear_bullet(int x, int y) {
    mvaddch(y, x, ' '); //uso la funzione ncurses per disegnare lo spazio vuoto nella coordinata (x,y)
}

/*!!!!!!!!!!*/
/*questa funzione si potrebbe mettere nel file delel collisioni*/
/*!!!!!!!!!!*/
/*funzione per controllare se un proiettile è uscito dai bordi dello schermo*/
bool check_bullet_borders(Messaggio bullet) {
    return (bullet.x < 0 || bullet.x >= GAME_WIDTH);
}

/**
 * funzione che si occupa del thread del proiettile
 * @param arg puntatore ai parametri del thread
 * @return NULL
 */
void* bullet_thread(void* arg) {
    Bullets_arg* params = (Bullets_arg*) arg; 
    Circular_buffer* buffer = params->buffer; 
    Messaggio copy = params->copy; 

    Messaggio bullets;
    
    /*inizializziamo il proiettile*/
    bullets.is_active = true;
    bullets.velocita = BULLET_SPEED;
    bullets.direzione = copy.direzione;
    bullets.index = copy.index;
    bullets.oggetto = ID_BULLET;
    bullets.tid = pthread_self();

    /*posizioniamo il proiettile alla testa del coccodrillo*/
    if (bullets.direzione == 1) {
        /*se il coccodrillo va verso destra allora il proiettile parte dalla testa destra*/
        bullets.x = copy.x + LARGHEZZA_COCCODRILLO;
    } else {
        /*se il coccodrillo va verso sinistra allora il proiettile parte dalla testa sinistra*/
        bullets.x = copy.x - 1;
    }
    
    bullets.y = copy.y + 1; //centro verticale del coccodrillo

    /*!!!!!
    nel while al posto di 1 game_running
    è una variabile globale che si trova nel file buffer.c ->
    -> bool game_running = true;
    !!!!!*/
    /*ciclo principale*/
    while(1) {
        /*aggiorno la posizione del proiettile*/
        bullets.x += bullets.direzione;

        /*controllo se il proiettile è uscito dai bordi*/
        if (check_bullet_borders(bullets)) {
            bullets.is_active = false;  // Marca come inattivo
            produce_message(buffer, bullets);  // Comunica la disattivazione
            break; //è uscito dallo schermo quindi termina
        }
        
        produce_msg(buffer, bullets);
        usleep(bullets.velocita); //attendo prima del prossimo aggiornamento
    }
    
    free(params); 
    return NULL; 
}