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
#include "disegni.h"

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

/*1: 5 13
2: 22 29
3: 39 46
4: 56 63
5: 73 80*/

void tane(int pipe_fd, Messaggio msg){
    Messaggio tana;
    if (msg.y == 1 && (5>msg.x<12) || (22>msg.x<29) || (39>msg.x<46) || (56>msg.x<63) || (73>msg.x<80)){
        draw_closed_burrows();
       refresh();

        msg.y = GAME_WIDTH - ALTEZZA_RANA;
        msg.x = GAME_HEIGHT / 2;
    write(write, &msg, sizeof(Messaggio));
    }
}


void frog(int pipe_fd) {
    
    Messaggio msg;
    int input;

    msg.oggetto = ID_RANA;
    msg.y= GAME_WIDTH - ALTEZZA_RANA;
    msg.x = GAME_HEIGHT / 2;
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
                if (msg.y < GAME_HEIGHT - ALTEZZA_RANA) {
                    msg.y += 3;
                }
                break;
            case KEY_LEFT:
                if (msg.x > 0) {
                    msg.x -= 3;
                }
                break;
            case KEY_RIGHT:
                if (msg.x < GAME_WIDTH - LARGHEZZA_RANA) {
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

        tane(pipe_fd, msg);
        
        // Invia la posizione aggiornata
        write(pipe_fd, &msg, sizeof(Messaggio));
    }
}