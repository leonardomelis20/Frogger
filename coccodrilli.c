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
        mvprintw( y +i, x, "%s", spriteCoccodrillo[i]);
    }
    refresh();

}

void clear_cocodrile(int x, int y, int direzione, pid_t pid) {
    int offset;

    if ((direzione == 1 && x >= GAME_WIDTH - LARGHEZZA_COCCODRILLO) || //direzione destra
        (direzione == -1 && x <= 0)) { //direzione sinistra
        
        for (int step = 0; step < LARGHEZZA_COCCODRILLO; step++) { //cancella carattere per carattere
            for (int i = 0; i < ALTEZZA_COCCODRILLO; i++) { //cancella riga per riga
                

                if (direzione == 1) {
                    // Se si muove a destra, cancella dalla testa (destra)
                    offset = LARGHEZZA_COCCODRILLO - step /*-1*/;
                } else  {
                    // Se si muove a sinistra, cancella dalla testa (sinistra)
                    offset = step;
                }

                mvprintw(y + i, x + offset, " ");  // Cancella la testa prima
            }
            refresh();
            usleep(30000);  // Ritardo per simulare la dissolvenza
        }

        // **Terminazione del processo del coccodrillo**
        //kill(pid, SIGKILL);
        //waitpid(pid, NULL, 0);
    }
}



void crocodile(int pipe_fd, int y_pos, int direzione, int x_start, int velocita){
    

    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    timeout(100);
    curs_set(0);

    Messaggio msg;
    int x_max;
    
    msg.oggetto = ID_CROCODILE;
    msg.x = x_start;
    msg.y = y_pos;
    msg.velocita = velocita;

    while (1)
    {      
        msg.x += direzione;
        if (msg.x >= GAME_WIDTH || msg.x < 0) { //controllo di collisione tra coccodrillo e schermo
            clear_cocodrile(msg.x, msg.y, direzione, getpid());
            
            if (msg.x <= 0 && msg.y>= 30){
                msg.x = 0;
                msg.y = 6;
                direzione = direzione;;
            }  else if (msg.x == GAME_WIDTH - LARGHEZZA_COCCODRILLO && msg.y >= 30){
                msg.x = GAME_WIDTH - LARGHEZZA_COCCODRILLO;
                msg.y = 6;
                direzione = direzione;
            }
                else {
                msg.y += 3;
                if (msg.y >= 30) {
                    msg.y = 30;
                }
                direzione = -direzione;
            if (direzione == 1){
                msg.x = 0;
            } else {
                msg.x = GAME_WIDTH - LARGHEZZA_COCCODRILLO;
            }

        }




            // x= 0 y= 6 direzione = 1 
            // x = 0 y = 30
           // exit(0);  // Termina il processo
        }
        
       // printf("Coccodrillo x = %d y = %d", msg.x, msg.y);
        if (write(pipe_fd, &msg, sizeof(Messaggio)) == -1) {
            perror("Errore durante la scrittura della pipe");
            endwin();
            exit(EXIT_FAILURE);
        }
        usleep(msg.velocita);

    }
    
    endwin();
    
}
