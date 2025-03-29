#include <stdio.h>
#include <curses.h>
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
void termina_gioco(pid_t, pid_t*);

int main(){
    srand(time(NULL));
    int pipe_fd[2];
    InfoCocc coccodrilli[NUM_CROC];
    pid_t pid_rana, pid_coccodrillo[NUM_STREAMS*COCCODRILLI_X_FLUSSO];
    Messaggio msg;
    InfoFlussi info[NUM_STREAMS];
    int prev_x_rana = -1, prev_y_rana = -1; 
    int vite = 5;
    int status = 0;
    int i= 0; 
    
    

    
    inizializza_schermo();
    //getmaxyx(stdscr, y, x);
    box(stdscr, 0, 0); // Disegna un bordo attorno allo schermo
    draw_safety_zones();
    draw_burrows(); 
    //mvprintw(GAME_HEIGHT/2, GAME_WIDTH/2 - 10, "Caricamento in corso...");
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
   inizializza_coccodrilli(pipe_fd, coccodrilli);
    
    
  
    //dovremo mettere uno switch case 

    
        while (1) {
        Messaggio msg;
        ssize_t bytes_read;
        ssize_t bytes_read2;
        // Leggi tutti i messaggi disponibili dalla pipe
        if (read(pipe_fd[READ], &msg, sizeof(Messaggio)))  {
            switch (msg.oggetto) {
                case ID_RANA:
                    if (prev_x_rana != -1 && prev_y_rana != -1) {
                        clear_frog(prev_x_rana, prev_y_rana);
                    }
                    prev_x_rana = msg.x;
                    prev_y_rana = msg.y;
                    draw_frog(msg.x, msg.y);
                    break;
                case ID_CROCODILE:
                main_croc(pipe_fd, coccodrilli, msg);
                
                /*
                    for(int i = 0; i < NUM_STREAMS*2; i++){
                        if (prev_x_coccodrillo != -1 && prev_y_coccodrillo != -1) {
                            clear_cocodrile(coccodrilli[i].x, coccodrilli[i].y, msg.direzione, getpid());
                        }
                        coccodrilli[i].x = msg.x;
                        coccodrilli[i].y = msg.y;
                        log_coordinates(coccodrilli[i].y, coccodrilli[i].x);
                        draw_crocodile(coccodrilli[i].x, coccodrilli[i].y);
                    }
                    

                    int index = getPidCoccodrillo(coccodrilli, msg.pid);
                    if (prev_x_coccodrillo != -1 && prev_y_coccodrillo != -1) {
                        clear_cocodrile(coccodrilli[index].x, coccodrilli[index].y, msg.direzione, getpid());
                    }
                    coccodrilli[index].x = msg.x;
                    coccodrilli[index].y = msg.y;
                    //log_coordinates(coccodrilli[i].y, coccodrilli[i].x);
                    draw_crocodile(coccodrilli[index].x, coccodrilli[index].y);
                break;
                case (-1):
                i = msg.index;  // Get the index from the message
                // Use SIGTERM instead of SIGKILL to allow proper cleanup

                info[i].direzione = msg.direzione;
                
                // Set the proper values in the info structure before respawning
                // This is the key part that's missing
                if (info[i].direzione == 1) {  // The direction is stored in velocita field when oggetto = -1
                    info[i].x_pos = 0;  // Start from left
                } else {
                    info[i].x_pos = GAME_WIDTH - LARGHEZZA_COCCODRILLO;  // Start from right
                }
                info[i].y_pos = msg.y;  // Keep the same y position
                info[i].speed = msg.velocita;  // Get the actual speed

                kill(msg.pid, SIGTERM);
                waitpid(msg.pid, &status, 0);

                clear_cocodrile(coccodrilli[getPidCoccodrillo(coccodrilli, msg.pid)].x, coccodrilli[getPidCoccodrillo(coccodrilli, msg.pid)].y, msg.direzione, msg.pid);
                
                // Now fork with the updated info
                pid_coccodrillo[i] = fork();
                if (pid_coccodrillo[i] == 0) {
                    close(pipe_fd[READ]);
                    crocodile(pipe_fd[WRITE], info, i);
                    exit(EXIT_SUCCESS);
                }
                break;*/
                
            }
        }

            
        //printf("x %d y %d", prev_x_rana, prev_y_rana);
    /*if (prev_x_rana != -1 && prev_y_rana != -1) {
        int collisione = verifica_collisione(prev_x_rana, prev_y_rana, coccodrilli_x, coccodrilli_y);
        if (!collisione) {  // Se la collisione è 0, la rana è caduta
        printf("💀 La rana è caduta in acqua! GAME OVER.\n");
        fflush(stdout);
        sleep(3);  // Aspetta 10 secondo per permettere la stampa
        endwin();
        vite--;
        exit(EXIT_SUCCESS);
    }
                
            }*/
            refresh();
             // Aggiungi un piccolo ritardo per evitare di sovraccaricare la CPU
        usleep(50000);  // 50ms

    }

    kill(pid_rana, SIGKILL);
    waitpid(pid_rana, NULL, 0);

    // Termina tutti i coccodrilli
    for (int i = 0; i < NUM_STREAMS * COCCODRILLI_X_FLUSSO; i++) {
        kill(pid_coccodrillo[i], SIGKILL);
        waitpid(pid_coccodrillo[i], NULL, 0);
    }

      // Processo padre chiude scrittura e legge dalla pipe
      close(pipe_fd[WRITE]);
        close(pipe_fd[READ]);
    
    endwin();
    return 0;
 }



void inizializza_schermo(){
    
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    timeout(100);
    curs_set(0);
    resize_term(GAME_HEIGHT, GAME_WIDTH);
    clear();
}


/* dobbiamo creare un processo che si occupi solo di chiudere gli altri processi chiamando questa funzione*/
void termina_gioco(pid_t pid_rana, pid_t pid_coccodrillo[]) {
    // Termina la rana
   

    // Chiude ncurses e stampa un messaggio di uscita
    endwin();
    printf("Gioco terminato!\n");
    exit(EXIT_SUCCESS);
}

