#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#include "strutture.h"
#include "rana.h"

// Sprite della rana
char spriteRana[ALTEZZA_RANA][LARGHEZZA_RANA + 1] = {
    " o.o ", 
    "+-|-+", 
    "\\-|-/"
};

// Funzione per disegnare la rana alla posizione (x, y)
void draw_frog(int x, int y) {
    for (int i = 0; i < ALTEZZA_RANA; i++) {
        mvprintw(y + i, x, "%s", spriteRana[i]);
    }
}

void clear_frog(int x, int y) {
    for (int i = 0; i < ALTEZZA_RANA; i++) {
        mvprintw(y + i, x, "     ");  // 5 spazi per cancellare la rana
    }
}

void frog(int pipe_fd) {
    // IMPORTANTE: Non inizializziamo ncurses qui
    
    Messaggio msg;
    int x_max = GAME_WIDTH;
    int y_max = GAME_HEIGHT;
    int centro_y = y_max - ALTEZZA_RANA;
    int centro_x = x_max / 2;
    int input;

    msg.oggetto = ID_RANA;
    msg.x = centro_x;
    msg.y = centro_y;
    msg.pid = getpid();
    
    // Invia la posizione iniziale
    write(pipe_fd, &msg, sizeof(Messaggio));

    // Loop principale per gestire l'input
    while(1) {
        // Attendi un po' prima di controllare nuovamente l'input
        usleep(50000);
        
        // Legge l'input (non bloccante)
        input = getch();
        
        // Gestiamo l'input
        switch (input) {
            case KEY_UP:
                if (msg.y > 0) {
                    msg.y -= 3;
                }
                break;
            case KEY_DOWN:
                if (msg.y < y_max - ALTEZZA_RANA) {
                    msg.y += 3;
                }
                break;
            case KEY_LEFT:
                if (msg.x > 0) {
                    msg.x -= 3;
                }
                break;
            case KEY_RIGHT:
                if (msg.x < x_max - LARGHEZZA_RANA) {
                    msg.x += 3;
                }
                break;
            case ' ':  // Spazio
                if (msg.y > 0) {
                    msg.y -= 3;
                }
                break;
            case 'q':  // Uscita
                close(pipe_fd);
                exit(EXIT_SUCCESS);
                return;
            default:
                // Se non c'è input valido, invia comunque la posizione attuale
                // per mantenere la comunicazione con il processo principale
                break;
        }
        
        // Invia la posizione aggiornata
        write(pipe_fd, &msg, sizeof(Messaggio));
    }
}