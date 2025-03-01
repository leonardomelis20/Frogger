#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "rana.h"
#include "strutture.h"
#include "coccodrilli.h"
#include "disegni.h"

#define READ 0 //macro da usare nelle pipe x codice più leggibile
#define WRITE 1


//RANA = SPRITE 
//FROG = PARAMETRO

/*prossimi passi: 1) coccodrilli
                  2) modificare il main x coccodrilli
                  3) collisione rana-coccodrillo
                  4)proiettili.c
                  5)collisione proiettili granate
                  6)vite e punteggio
                  7)tempo di manche
                  8)tane */

void inizializza_schermo(); //per chiamare le funzioni ncurses
int main(){
    int pipe_fd[2];
    pid_t pid_rana, pid_coccodrillo;
    Messaggio msg;
    int x, y;
    int prev_x_rana = -1, prev_y_rana = -1; 

    inizializza_schermo();
    getmaxyx(stdscr, y, x);
    box(stdscr, 0, 0);  // Disegna un bordo attorno allo schermo
    refresh();

    if (pipe(pipe_fd) == -1){
        perror ("Errore creazione pipe");
        exit(EXIT_FAILURE);
    }

    pid_rana = fork();

    if (pid_rana == -1){
        perror("Fork rana fallita");
        exit(EXIT_FAILURE);
    } else if (pid_rana == 0){
        close(pipe_fd[READ]);  //chiudo la pipe in lettura
        frog(pipe_fd[WRITE]); //passo la pipe direttamente in scrittura
        exit(EXIT_SUCCESS);
    }

    pid_coccodrillo = fork();

    if (pid_coccodrillo == -1){
        perror("Fork coccodrillo fallita");
        exit(EXIT_FAILURE);
    } else if (pid_coccodrillo == 0){
        close(pipe_fd[READ]);  //chiudo la pipe in lettura
        crocodile(pipe_fd[WRITE]); //passo la pipe direttamente in scrittura
        exit(EXIT_SUCCESS);
    }
    // Processo padre chiude scrittura e legge dalla pipe
    close(pipe_fd[WRITE]);
   


    
    //dovremo mettere uno switch case 
    while(1){
       /* draw_river();
        draw_burrows();
        draw_safety_zones();
        refresh();*/
        if (read(pipe_fd[READ], &msg, sizeof(Messaggio)) > 0) {
            //printf("Ricevuto messaggio: oggetto=%d, x=%d, y=%d\n", msg.oggetto, msg.x, msg.y); 
            //clear();
            switch (msg.oggetto) {
                case ID_RANA: if (prev_x_rana != -1 && prev_y_rana != -1) {
                    clear_frog(prev_x_rana, prev_y_rana);
                }

                // Aggiorna la posizione e disegna la rana
                prev_x_rana = msg.x;
                prev_y_rana = msg.y;
                draw_frog(msg.x, msg.y);
                break;
                case ID_CROCODILE: draw_crocodile(msg.x, msg.y); break;
            }
            refresh();
        }
    }
    
    
    kill(pid_rana, SIGKILL);
    kill(pid_coccodrillo, SIGKILL);

    waitpid(pid_rana, NULL, 0);
    waitpid(pid_coccodrillo, NULL, 0);

    endwin();
    return 0;
 }



void inizializza_schermo(){
    
    initscr();
    noecho();
    cbreak();
    timeout(100);
    curs_set(0);
    
}
