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

char *bullet="°";

void draw_bullet(int x, int y){
    mvprintw(y, x, "%s", bullet);
}
void clear_bullet(int x, int y){
    mvaddch(y, x, ' ');
}

//funzione per aggiornare il movimento del coccodrillo
void movement_bullet(Messaggio *croc, Messaggio *bullet){
    bullet->x += croc->direzione;
}

void main_bullet(int pipe_fd, Messaggio croc){
    Messaggio bullet;

    bullet.velocita = BULLET_SPEED;
    bullet.direzione = croc.direzione;

    if (bullet.direzione == 1) {
        bullet.x = croc.x - LARGHEZZA_COCCODRILLO; //spawna dalla testa
    } else {
        bullet.x = croc.x;
    }
    //bullet.x = croc.x;
    bullet.y = croc.y;
    bullet.oggetto = ID_BULLET;


    while(1){
        movement_bullet(&croc, &bullet);

        if (check_borders_bullet(bullet)) {
            clear_bullet(bullet.x, bullet.y);
        }
        // Scrivi il messaggio nella pipe
        write(pipe_fd, &bullet, sizeof(Messaggio));
        usleep(bullet.velocita);
        // Cancella il proiettile dalla posizione precedente
    }


}


