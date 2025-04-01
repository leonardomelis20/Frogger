#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
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

bool is_inside (Messaggio msg){
    //per verificare quando la rana è dentro la tana a partire dall'inizio alla fine
if (msg.y == 0 &&
    ((msg.x >= 5 && msg.x <= 13) ||
     (msg.x >= 22 && msg.x <= 29) ||
     (msg.x >= 39 && msg.x <= 46) ||
     (msg.x >= 56 && msg.x <= 63) ||
     (msg.x >= 73 && msg.x <= 80))) {
        
    return true;

    } 
    return false;
}

int num_tana(Messaggio msg){
    int tana = -1;
    //in base alle coordinate della rana restituisco il nuemro della tana
if (msg.y == 0) {
    if (msg.x >= 5 && msg.x <= 13)
        tana = 1;
    else if (msg.x >= 22 && msg.x <= 29)
        tana = 2;
    else if (msg.x >= 39 && msg.x <= 46)
        tana = 3;
    else if (msg.x >= 56 && msg.x <= 63)
        tana = 4;
    else if (msg.x >= 73 && msg.x <= 80)
        tana = 5;
    else
        tana = -1; 

        return tana;
    }
}

void tane(int pipe_fd, Messaggio msg){
    Messaggio tana;
    //verifico il numero di tana e la riempio
    tana.x = num_tana(msg);
        if (is_inside(msg)){
        draw_closed_burrows(tana);
        clear_frog(msg.x, msg.y);
        refresh();
    }
}

void frog(int pipe_fd, bool* flag) {
    // IMPORTANTE: Non inizializziamo ncurses qui
    
    Messaggio msg;
    int num_tane;
  
    int centro_y = GAME_HEIGHT - ALTEZZA_RANA;
    int centro_x = GAME_WIDTH    / 2;
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
            case 'q':  // Uscita
                close(pipe_fd);
                exit(EXIT_SUCCESS);
                return;
                break;
        }

        num_tane = num_tana(msg);
        //controlla se è dentro la tana oppure se entra in mezzo a due tane
        if(is_inside(msg) || msg.y == 0 && num_tane == -1){
            msg.oggetto = TANE;
            flag[num_tane] = true; //setto il flag a true per segnalare che non può più entrare in questa tana
            write(pipe_fd, &msg, sizeof(Messaggio));
            break;
        }

        // Invia la posizione aggiornata
        write(pipe_fd, &msg, sizeof(Messaggio));
    }
}