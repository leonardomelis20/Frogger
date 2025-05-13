#include "proiettili.h"

char bullet = 'P';

void draw_bullet(int x, int y) {
    mvaddch(y, x, bullet);
}

void clear_bullet(int x, int y) {
    mvaddch(y, x, ' ');
}

/*funzione per aggiornare il movimento del proiettile*/
void movement_bullet(Messaggio *croc, Messaggio *bullet) {
    bullet->x += croc->direzione;
}

/*funzione per controllare se un proiettile è uscito dai bordi dello schermo*/
bool check_bullet_borders(Messaggio bullet) {
    return (bullet.x < 0 || bullet.x >= GAME_WIDTH);
}

void main_bullet(int pipe_fd, Messaggio croc) {
    Messaggio bullets;
    int adjusted_i;
    int i; 
    
    /*inizializzazione proiettile*/
    bullets.is_active = true;
    bullets.velocita = BULLET_SPEED;
    bullets.direzione = croc.direzione;
    bullets.index = croc.index;
    bullets.oggetto = ID_BULLET;
    bullets.pid = getpid();
    
    /*posiziono il proiettile alla testa del coccodrillo*/
    if (bullets.direzione == 1) {
        /*se il coccodrillo va verso destra allora il proiettile parte dalla testa destra*/
        bullets.x = croc.x + LARGHEZZA_COCCODRILLO;
    } 
    else {
        /*se il coccodrillo va verso sinistra allora il proiettile parte dalla testa sinistra*/
        bullets.x = croc.x - 1;
    }
    
    /*calcola la posizione y del proiettile, ovvero il centro del coccodrillo*/
    i = croc.index;
    if (i >= 9) {
        adjusted_i = i - 9;
    } 
    else {
        adjusted_i = i;
    }

    bullets.y = 6 + (adjusted_i * 3) + 1; //centro verticale del coccodrillo
    
    /*ciclo principale*/
    while(1) {
        /*aggiorno la posizione del proiettile*/
        bullets.x += bullets.direzione;
        
        /*controllo se il proiettile è uscito dai bordi*/
        if (check_bullet_borders(bullets)) {
            break; //è uscito dallo schermo quindi termina
        }
        
        write(pipe_fd, &bullets, sizeof(Messaggio)); //invio la posizione aggiornata del proiettile
        usleep(bullets.velocita); //attendo prima del prossimo aggiornamento
    }
    
    bullets.is_active = false; //aggiorna lo stato del proiettile
    write(pipe_fd, &bullets, sizeof(Messaggio));
}