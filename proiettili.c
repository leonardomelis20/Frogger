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

/*funzione per aggiornare il movimento del proiettile*/
void movement_bullet(Messaggio *croc, Messaggio *bullet) {
    bullet->x += croc->direzione;
}

/*!!!!!!!!!!*/
/*questa funzione si potrebbe mettere nel file delel collisioni*/
/*!!!!!!!!!!*/
/*funzione per controllare se un proiettile è uscito dai bordi dello schermo*/
bool check_bullet_borders(Messaggio bullet) {
    return (bullet.x < 0 || bullet.x >= GAME_WIDTH);
}

void main_bullet(int pipe_fd, Messaggio copy) {
    Messaggio bullets;
    int adjusted_i;
    int i; 
    
    /*inizializzazione proiettile*/
    bullets.is_active = true;
    bullets.velocita = BULLET_SPEED;
    bullets.direzione = copy.direzione;
    bullets.index = copy.index;
    bullets.oggetto = ID_BULLET;
    bullets.pid = getpid();
    
    

    /*posiziono il proiettile alla testa del coccodrillo*/
    if (bullets.direzione == 1) {
        /*se il coccodrillo va verso destra allora il proiettile parte dalla testa destra*/
        bullets.x = copy.x + LARGHEZZA_COCCODRILLO;
    } else {
        /*se il coccodrillo va verso sinistra allora il proiettile parte dalla testa sinistra*/
        bullets.x = copy.x - 1;
    }
    
    /*calcola la posizione y del proiettile, ovvero il centro del coccodrillo*/
    

    bullets.y = copy.y +1; //centro verticale del coccodrillo

  
    
    /*ciclo principale*/
    while(1) {
        /*aggiorno la posizione del proiettile*/
        bullets.x += bullets.direzione;

        /*controllo se il proiettile è uscito dai bordi*/
        if (check_bullet_borders(bullets)) {
            bullets.is_active = false;  // Marca come inattivo
            write(pipe_fd, &bullets, sizeof(Messaggio));  // Comunica la disattivazione
            break; //è uscito dallo schermo quindi termina
        }
        
        write(pipe_fd, &bullets, sizeof(Messaggio));
        usleep(bullets.velocita); //attendo prima del prossimo aggiornamento
    }
    

   
}