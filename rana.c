#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "strutture.h"
#include "rana.h"


int frog(int pipe_fd[2]){

    initscr();
    noecho();
    cbreak();
    curs_set(0);

    Messaggio msg;
    int pipe_rana[2];
    int x, y;
    getmaxyx(stdscr, y, x);
    int centro_y = y -1;
    int centro_x = x / 2;
    int input;

    close(pipe_rana[0]);

    msg.oggetto = ID_RANA;
    msg.x = centro_x;
    msg.y= centro_y;


    while(1){
        input = getch();
        if (input == 'q') { //debug
            // Invia una posizione speciale (-1, -1) per segnalare la chiusura del gioco
            msg.x = -1;
            msg.y = -1;
            write(pipe_rana[1], &msg, sizeof(Messaggio));
            close(pipe_rana[1]);
            _exit(0);
        }
         switch (input) {
            case KEY_UP:
                if (msg.y > 0) msg.y--;
                break;
            case KEY_DOWN:
                if (msg.y < y - 1) msg.y++;
                break;
            case KEY_LEFT:
                if (msg.x > 0) msg.x--;
                break;
            case KEY_RIGHT:
                if (msg.x < x - 1) msg.x++;
                break;
    }
    write(pipe_rana[1], &msg, sizeof(Messaggio));
    usleep(50000); // da vedere

    }

}
