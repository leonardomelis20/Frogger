#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "strutture.h"
#include "coccodrilli.h"

char spriteCoccodrillo[ALTEZZA_COCCODRILLO][LARGHEZZA_COCCODRILLO+1] = {
    "  ~~~___~~ ",  
    " (o)---(o) ",  
    "  ~~~   ~~ "   
};

void draw_crocodile(int x, int y){
    for (int i = 0; i < ALTEZZA_COCCODRILLO; i++){
        mvprintw(y+i, x, "%s", spriteCoccodrillo[i]);
    }
    refresh();
}


void crocodile(int pipe_fd){

    srand(time(NULL));
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    timeout(100);
    curs_set(0);

    Messaggio msg;
    int x_max = 1, y_max = 1;
    int vx = 2, vy = 2; //velocità
    int direzione;
    if (rand() % 2 == 0){
        direzione = 1;
    } else {
        direzione = -1;
    }
   // getmaxyx(stdscr, y_max, x_max);

    msg.oggetto = ID_CROCODILE;
    msg.x = x_max;
    msg.y = y_max;

    while (1)
    {
        msg.x += direzione;
        if (msg.x >= GAME_WIDTH - LARGHEZZA_COCCODRILLO || msg.x < 0){
            direzione *= -1;
        }
       // printf("Coccodrillo x = %d y = %d", msg.x, msg.y);
        if (write(pipe_fd, &msg, sizeof(Messaggio)) == -1) {
            perror("Errore durante la scrittura della pipe");
            endwin();
            exit(EXIT_FAILURE);
        }
        usleep(300000);
        fflush(stdout);
    }
    
    endwin();
    
}
