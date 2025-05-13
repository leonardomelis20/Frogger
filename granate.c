#include "granate.h"

char grenade = 'G';

void draw_grenade(int x, int y) {
    mvaddch(y, x, grenade);
}

void clear_grenade(int x, int y) {
    mvaddch(y, x, ' ');
}

/*funzione per aggiornare il movimento della granata sinistra*/
void movement_grenade_left(Messaggio *grenade) {
    grenade->x -= 1;
}

/*funzione per aggiornare il movimento della granata destra*/
void movement_grenade_right(Messaggio *grenade) {
    grenade->x += 1;
}

bool check_grenade_borders(Messaggio grenade) {
    return (grenade.x < 0 || grenade.x >= GAME_WIDTH);
}

void main_grenade(int pipe_fd, Messaggio grenade) {
    if (grenade.direzione == 1) {
        grenade.x = grenade.x + 2;
    }

    grenade.is_active = true;

    /*inizializzo la granata*/
     grenade.is_active = true;
     grenade.velocita = GRENADE_SPEED;
     grenade.oggetto = ID_GRENADE;
     grenade.pid = getpid();

    while(1) {
        if (grenade.direzione == 1) {
            movement_grenade_right(&grenade);
        } 
        else {
            movement_grenade_left(&grenade);
        }

        /*controllo se il proiettile è uscito dai bordi*/
        if (check_grenade_borders(grenade)) {
            break; //è uscito dallo schermo quindi termina
        }

        write(pipe_fd, &grenade, sizeof(Messaggio)); //invio la posizione aggiornata del proiettile
        usleep(grenade.velocita); //attendo prima del prossimo aggiornamento
    }
}