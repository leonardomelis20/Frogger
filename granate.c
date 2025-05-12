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
#include "coccodrilli.h"
#include "proiettili.h"
#include "granate.h"

char grenade='G';

void draw_grenade(int x, int y){
    mvaddch(y, x, grenade);
}

void clear_grenade(int x, int y){
    mvaddch(y, x, ' ');
}

// Funzione per aggiornare il movimento della granata
void movement_grenade_left(Messaggio *grenade){
    grenade->x -= 1;
}
void movement_grenade_right(Messaggio *grenade){
    grenade->x += 1;
}

bool check_grenade_borders(Messaggio grenade) {
    return (grenade.x < 0 || grenade.x >= GAME_WIDTH);
}

void main_grenade(int pipe_fd, Messaggio grenade){
    

     log_coordinates(grenade.x, grenade.y);

    if (grenade.direzione == 1){
        grenade.x = grenade.x + 2;
    }
    grenade.is_active = true;

    // Inizializza la granata
     grenade.is_active = true;
     grenade.velocita = GRENADE_SPEED;
     grenade.oggetto = ID_GRENADE;
     grenade.pid = getpid();

     while(1){
        if (grenade.direzione == 1){
            movement_grenade_right(&grenade);
        } else {
            movement_grenade_left(&grenade);
        }


        // Controlla se il proiettile è uscito dai bordi
        if (check_grenade_borders(grenade)) {
            // Il proiettile è uscito dallo schermo, termina
            break;
        }
        

        // Invia la posizione aggiornata del proiettile
        write(pipe_fd, &grenade, sizeof(Messaggio));
        
        // Attendi prima del prossimo aggiornamento
        usleep(grenade.velocita);

     }
}