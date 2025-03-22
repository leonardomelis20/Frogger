#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h> //pipe non bloccante

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
void termina_gioco(pid_t, pid_t*);
void inizializza_coccodrilli(int pipe_fd[2], pid_t pid_coccodrillo[NUM_STREAMS * COCCODRILLI_X_FLUSSO]);
int main(){
    int pipe_fd[2];
    pid_t pid_rana, pid_coccodrillo[NUM_STREAMS*COCCODRILLI_X_FLUSSO];
    Messaggio msg;
    InfoFlussi info[NUM_STREAMS];
    int prev_x_rana = -1, prev_y_rana = -1; 
    int prev_x_coccodrillo = -1, prev_y_coccodrillo = -1;
    int direzione, x_max;
    int coccodrilli_x[NUM_STREAMS] = {0};
    int coccodrilli_y[NUM_STREAMS] = {0};
    int vite = 5;
    int prev_x_c = -1, prev_y_c = -1;
    int status = 0;
    

    
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
   inizializza_coccodrilli(pipe_fd, pid_coccodrillo);
    
    
  
    //dovremo mettere uno switch case 

    // Imposta la pipe in modalità non bloccante
    fcntl(pipe_fd[READ], F_SETFL, O_NONBLOCK);   
    while (1) {
        Messaggio msg;
        ssize_t bytes_read;

        // Leggi tutti i messaggi disponibili dalla pipe
        while ((bytes_read = read(pipe_fd[READ], &msg, sizeof(Messaggio))) > 0) {
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
                if (prev_x_coccodrillo != -1 && prev_y_coccodrillo != -1) {
                    clear_cocodrile(prev_x_coccodrillo, prev_y_coccodrillo, msg.velocita, getpid());
                }
                prev_x_coccodrillo = msg.x;
                prev_y_coccodrillo = msg.y;
                draw_crocodile(msg.x, msg.y);
                break;
                case -1:
                int i = msg.index;  // Get the index from the message
                
                // Use SIGTERM instead of SIGKILL to allow proper cleanup
                kill(pid_coccodrillo[i], SIGTERM);
                waitpid(pid_coccodrillo[i], &status, 0);
                
                // Set the proper values in the info structure before respawning
                // This is the key part that's missing
                if (info[i].direzione = 1) {  // The direction is stored in velocita field when oggetto = -1
                    info[i].x_pos = 0;  // Start from left
                } else {
                    info[i].x_pos = GAME_WIDTH - LARGHEZZA_COCCODRILLO;  // Start from right
                }
                info[i].y_pos = msg.y;  // Keep the same y position
                info[i].speed = msg.velocita;  // Get the actual speed
                
                // Now fork with the updated info
                pid_coccodrillo[i] = fork();
                if (pid_coccodrillo[i] == 0) {
                    close(pipe_fd[READ]);
                    crocodile(pipe_fd[WRITE], info, i);
                    exit(EXIT_SUCCESS);
                }
                break;
                
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

void inizializza_coccodrilli(int pipe_fd[2], pid_t pid_coccodrillo[NUM_STREAMS * COCCODRILLI_X_FLUSSO]){
    int direzione; // Memorizza la direzione attuale
    int num = MIN_COCCODRILLO + rand() % (MAX_COCCODRILLO - MIN_COCCODRILLO +1);
    InfoFlussi info[NUM_STREAMS];
    int coccodrillo_index = 0;


 //assegno la prima direzione in modo casuale
    if (num % 2 == 0){
        direzione = 1;
    } else {
        direzione = -1; 
    }

      // Inizializzo i flussi con direzioni alternate
      for (int j = 0; j < NUM_STREAMS; j++) {
        // La direzione viene decisa in base alla prima
        if (j % 2 == 0) {
            info[j].direzione = direzione;
        } else {
            info[j].direzione = -direzione;
        }
        
        // Setto la x in base alla posizione
        if (info[j].direzione == 1) {
            info[j].x_pos = 0;  //Inizia d sinistra se si sta muovendo verso destra
        } else {
            info[j].x_pos = GAME_WIDTH - LARGHEZZA_COCCODRILLO;  // Inizia da destra se si sta muovendo verso sinistra
        }
        
        // Setto la y e la velocità
        info[j].y_pos = 6 + (j * 3);  // Ogni y è distante 3 unità
        info[j].speed = MIN_VELOCITA + rand() % (MAX_VELOCITA - MIN_VELOCITA + 1); // Velocità casuale tra i due estremi
    }
        
    
  // Crea un coccodrillo iniziale per flusso
  for (int flusso = 0; flusso < NUM_STREAMS; flusso++) {
    pid_coccodrillo[coccodrillo_index] = fork();
    if (pid_coccodrillo[coccodrillo_index] == -1) {
        perror("Fork coccodrillo fallita");
        exit(EXIT_FAILURE);
    } else if (pid_coccodrillo[coccodrillo_index] == 0) {
        close(pipe_fd[READ]);
        /*while (1) {
            crocodile(pipe_fd[WRITE], info, flusso);
            sleep(1);  // Aspetta prima di rigenerare il coccodrillo
        }*/
        crocodile(pipe_fd[WRITE], info, flusso);
        exit(EXIT_SUCCESS);
    }
    coccodrillo_index++;
    usleep(200000);  // Ritardo di 200ms tra la creazione dei coccodrilli
}
}
