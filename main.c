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
#include "collisioni.h"

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
void inizializza_coccodrilli(int pipe_fd[2], pid_t pid_coccodrillo[8]);
int main(){
    int pipe_fd[2];
    pid_t pid_rana, pid_coccodrillo[8];
    Messaggio msg;
    int x = GAME_HEIGHT, y = GAME_WIDTH;
    int prev_x_rana = -1, prev_y_rana = -1; 
    int direzione, x_max;
    int coccodrilli_x[NUM_STREAMS] = {0};
    int coccodrilli_y[NUM_STREAMS] = {0};
    int found;
    inizializza_schermo();
    //getmaxyx(stdscr, y, x);
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
   inizializza_coccodrilli(pipe_fd, pid_coccodrillo);
    
    
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
                case ID_CROCODILE: 
                for (int i = 0; i < NUM_STREAMS && !found; i++)
                {
                    if (coccodrilli_x[i] == 0){
                        coccodrilli_x[i] = msg.x;
                        coccodrilli_y[i] = msg.y;
                        found = 1;
                    }
                }
                
                draw_crocodile(msg.x, msg.y); break;
            }
            if (!verifica_collisione(prev_x_rana, prev_y_rana, coccodrilli_x, coccodrilli_y)){
                printf("La rana è caduta\n");
                endwin();
                exit(EXIT_FAILURE);
            }
        } 
        refresh();
    }
    
    
    kill(pid_rana, SIGKILL);
    waitpid(pid_rana, NULL, 0);
    for (int i = 0; i < 8; i++)
    {
        kill(pid_coccodrillo[i], SIGKILL);
        waitpid(pid_coccodrillo[i], NULL, 0);
    }
    
    endwin();
    return 0;
 }



void inizializza_schermo(){
    
    initscr();
    noecho();
    cbreak();
    timeout(100);
    curs_set(0);
    resize_term(GAME_HEIGHT, GAME_WIDTH);
    clear();
}

void inizializza_coccodrilli(int pipe_fd[2], pid_t pid_coccodrillo[8]){
    srand(time(NULL) ^ getpid());
    int direzione; // Memorizza la direzione attuale
    int num = MIN_COCCODRILLO + rand() % (MAX_COCCODRILLO - MIN_COCCODRILLO +1);
    int x_max;

    if (num % 2 == 0){
        direzione = 1;
        x_max = 0; 
    } else {
        direzione = -1; 
        x_max = GAME_WIDTH - LARGHEZZA_COCCODRILLO; 
    }
    for (int i = 0; i < 8; i++) {
        pid_coccodrillo[i] = fork();

        if (pid_coccodrillo[i] == -1) {
            perror("Fork coccodrillo fallita");
            exit(EXIT_FAILURE);
        } else if (pid_coccodrillo[i] == 0) {
            int y_pos = 2 + (i * 3);
            
            close(pipe_fd[READ]);  
            crocodile(pipe_fd[WRITE], y_pos, direzione, x_max);  
            exit(EXIT_SUCCESS);
        }

        if (direzione == 1) {
            direzione = -1;
            x_max = GAME_WIDTH - LARGHEZZA_COCCODRILLO;
        } else {
            direzione = 1;
            x_max = 0;
        }

        //direzione *= -1; // Alterna la direzione per il prossimo coccodrillo
    }
}
