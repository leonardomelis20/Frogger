#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h> 
#include <stdbool.h>

#include "strutture.h"
#include "collisioni.h"

char bullet='p';

void draw_bullet(int x, int y){
    mvaddch(y, x, bullet);
}

void clear_bullet(int x, int y){
    mvaddch(y, x, ' ');
}

// Funzione per aggiornare il movimento del proiettile
void movement_bullet(Messaggio *croc, Messaggio *bullet){
    bullet->x += croc->direzione;
}

// Funzione per controllare se un proiettile è uscito dai bordi dello schermo
bool check_bullet_borders(Messaggio bullet) {
    return (bullet.x < 0 || bullet.x >= GAME_WIDTH);
}

void main_bullet(int pipe_fd, Messaggio croc){
    Messaggio bullets;
    int adjusted_i;
    
    // Inizializza il proiettile
    bullets.is_active = true;
    bullets.velocita = BULLET_SPEED;
    bullets.direzione = croc.direzione;
    bullets.index = croc.index;
    bullets.oggetto = ID_BULLET;
    bullets.pid = getpid();
    
    // Posiziona il proiettile alla testa del coccodrillo
    if (bullets.direzione == 1) {
        // Se il coccodrillo va verso destra, il proiettile parte dalla testa destra
        bullets.x = croc.x + LARGHEZZA_COCCODRILLO;
    } else {
        // Se il coccodrillo va verso sinistra, il proiettile parte dalla testa sinistra
        bullets.x = croc.x - 1;
    }
    
    // Calcola la posizione y del proiettile (centro del coccodrillo)
    int i = croc.index;
    if (i >= 9){
        adjusted_i = i - 9;
    } else {
        adjusted_i = i;
    }

    bullets.y = 6 + (adjusted_i * 3) + 1; // Centro verticale del coccodrillo
    
    // Ciclo principale del proiettile
    while(1) {
        // Aggiorna la posizione del proiettile
        bullets.x += bullets.direzione;
        
        // Controlla se il proiettile è uscito dai bordi
        if (check_bullet_borders(bullets)) {
            // Il proiettile è uscito dallo schermo, termina
            break;
        }
        
        // Invia la posizione aggiornata del proiettile
        write(pipe_fd, &bullets, sizeof(Messaggio));
        
        // Attendi prima del prossimo aggiornamento
        usleep(bullets.velocita);
    }
    
    // Prima di terminare, aggiorna lo stato del proiettile
    bullets.is_active = false;
    write(pipe_fd, &bullets, sizeof(Messaggio));
}