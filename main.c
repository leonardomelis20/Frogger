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
    Messaggio coccodrilli[NUM_CROC];    //array di coccodrilli
    pid_t pid_rana, pid_coccodrillo[NUM_STREAMS*COCCODRILLI_X_FLUSSO];
    Messaggio msg; //letto
    InfoFlussi info[NUM_STREAMS];
    int prev_x_rana = -1, prev_y_rana = -1; 
    int prev_x_cocc = -1, prev_y_cocc = -1;
    pid_t pid;
    Messaggio msg_copy;

    int direzione;
    int vite = 5;
    int status = 0;
    int i= 0; 
    int direzioni[NUM_CROC];
    
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

    //assegno la prima direzione in modo casuale
    
   
    if (rand() % 2 == 0){
        direzioni[0] = 1;
    } else {
        direzioni[0] = -1;
    }
    for (int i = 1; i < NUM_CROC; i++) {
        direzioni[i] = -direzioni[i - 1];  // Alterna rispetto al precedente
    }

    for(int i=0; i< NUM_CROC; i++){
        pid_coccodrillo[i]= fork();
        if (pid_coccodrillo[i] == -1){
            perror("Fork Coccodrillo fallita");
            exit(EXIT_FAILURE);
        } else if (pid_coccodrillo[i] == 0){
            close(pipe_fd[READ]);  //chiudo la pipe in lettura
            main_croc(pipe_fd[WRITE], i, direzioni[i]); //passo la pipe direttamente in scritturap
            exit(EXIT_SUCCESS);
        }

    }
     
    
     
        while (1) {
        // Leggi tutti i messaggi disponibili dalla pipe
        while (read(pipe_fd[READ], &msg, sizeof(Messaggio)) > 0)  {
            switch (msg.oggetto) {
                case ID_RANA:
                    // Cancella la vecchia posizione della rana
                    if (prev_x_rana != -1 && prev_y_rana != -1) {
                        clear_frog(prev_x_rana, prev_y_rana);
                    }
                    // Aggiorna e disegna la nuova posizione
                    prev_x_rana = msg.x;
                    prev_y_rana = msg.y;
                    draw_frog(msg.x, msg.y);
                    break;
                case ID_CROCODILE:
                    //cancello
                    clear_croc(msg);
                    //aggiorno
                    coccodrilli[msg.index] = msg;
                    //disegno
                    draw_crocodile(msg.x, msg.y);
                    break;         
                case -1: 
                    int direzione = msg.direzione;
                    int indice = msg.index;
                    kill(msg.pid, SIGKILL);
                    waitpid(msg.pid, &status, 0); // aspetta che il processo muoia
                    // Ricrea un nuovo processo coccodrillo con le stesse proprietà
                    pid_t nuovo_pid = fork();
                    if (nuovo_pid == -1 ){
                        perror("errore fork ");
                        exit(EXIT_FAILURE);
                    } else if (nuovo_pid == 0) {
                        close(pipe_fd[READ]);
                        // nuovo processo figlio parte da capo
                        main_croc(pipe_fd[WRITE], indice, direzione);
                        exit(EXIT_SUCCESS);
                    }
                    break;                      
            }
            refresh();
        }
        
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

