#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

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
    refresh();
}

void clear_frog(int x, int y) {
    for (int i = 0; i < ALTEZZA_RANA; i++) {
        mvprintw(y + i, x, "     ");  // 5 spazi per cancellare la rana
    }
}

void frog(int pipe_fd){


    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    timeout(100);
    curs_set(0);

    Messaggio msg;
    int x_max, y_max;
    getmaxyx(stdscr, y_max, x_max);
    int centro_y = y_max - ALTEZZA_RANA; // Ora la rana è completamente visibile
    int centro_x = x_max / 2;
    int input;

    msg.oggetto = ID_RANA;
    msg.x = centro_x;
    msg.y= centro_y;


    while(1){
        input = getch();
         switch (input) {
            case KEY_UP:
                if (msg.y > 0) msg.y--;
                break;
            case KEY_DOWN:
                if (msg.y < y_max - ALTEZZA_RANA) msg.y++;
                break;
            case KEY_LEFT:
                if (msg.x > 0) msg.x--;
                break;
            case KEY_RIGHT:
                if (msg.x < x_max - 1) msg.x++;
                break;
            case ' ':
                if (msg.y > SALTO_ALTEZZA) {
                // Fase 1: la rana si alza
                msg.y -= SALTO_ALTEZZA;
                msg.x += SALTO_DISTANZA / 2;  // Si sposta leggermente avanti
                write(pipe_fd, &msg, sizeof(Messaggio));
        usleep(100000); // Pausa per mostrare il salto
    }
    // Fase 2: la rana scende
    msg.y += SALTO_ALTEZZA;
    msg.x += SALTO_DISTANZA / 2;  // Continua ad avanzare un po'
    write(pipe_fd, &msg, sizeof(Messaggio));
    break
            case 'q':
            endwin();
            close(pipe_fd);
            return;
    }
    write(pipe_fd, &msg, sizeof(Messaggio));
    fflush(stdout);  // Forza la stampa immediata

    usleep(50000); // da vedere
    }
    endwin();

}
