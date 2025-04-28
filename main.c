#include <stdio.h>
#include <curses.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "strutture.h"
#include "rana.h"
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
void draw_map();


int speed[NUM_STREAMS];

int main(){
    srand(time(NULL));
    int pipe_fd[2];
    Messaggio coccodrilli[NUM_CROC];    //array di coccodrilli
    pid_t pid_rana, pid_coccodrillo[NUM_STREAMS*COCCODRILLI_X_FLUSSO];
    Messaggio msg; 
    InfoFlussi info[NUM_STREAMS];
    int prev_x_rana = -1, prev_y_rana = -1; 
    int prev_x_cocc = -1, prev_y_cocc = -1;
    pid_t pid;
    Messaggio frog_copy, croc_copy[NUM_CROC];
    int centro_y = GAME_HEIGHT - ALTEZZA_RANA;
    int centro_x = GAME_WIDTH / 2;
    int input;
    

    // Inizializza la rana
    frog_copy.oggetto = ID_RANA;
    frog_copy.x = centro_x;
    frog_copy.y = centro_y;
    frog_copy.on_croc = false;
    frog_copy.croc_index = -1;
    
    
    bool flag[NUM_BURROWS+1] = {false};
    bool flag_croc = true; //per i coccodrilli
    int tana;
    int direzione;
    int vite = 5;
    int status = 0;
    int i= 0; 
    int direzioni[NUM_CROC];

    bool in_safe_zone = false;
    bool out_of_bounds = false;
    int new_x, new_y;
    int count[NUM_CROC];
    
    inizializza_schermo();
    //getmaxyx(stdscr, y, x);
    box(stdscr, 0, 0); // Crea un bordo attorno alla finestra
    draw_burrows();
    draw_safety_zones();
    //mvprintw(GAME_HEIGHT/2, GAME_WIDTH/2 - 10, "Caricamento in corso...");
    refresh();
    
    if (pipe(pipe_fd) == -1){
        perror ("Errore creazione pipe");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < NUM_STREAMS; i++) {
        switch (i) {
            case 0: speed[i] = 120000; break;
            case 1: speed[i] = 140000; break;
            case 2: speed[i] = 115000; break;
            case 3: speed[i] = 135000; break;
            case 4: speed[i] = 125000; break;
            case 5: speed[i] = 145000; break;
            case 6: speed[i] = 110000; break;
            case 7: speed[i] = 130000; break;
            case 8: speed[i] = 150000; break;
        }
    }
    for (int i = 0; i < NUM_CROC; i++)
    {
        if (i >= 9){
            speed[i] = speed[i-9];
        }   
    }
        

    pid_rana = fork();


    if (pid_rana == -1){
        perror("Fork rana fallita");
        exit(EXIT_FAILURE);
    } else if (pid_rana == 0){
        close(pipe_fd[READ]);  //chiudo la pipe in lettura
        frog(pipe_fd[WRITE], flag, speed); //passo la pipe direttamente in scrittura
        exit(EXIT_SUCCESS);
    }

  
   
    if (rand() % 2 == 0){
        direzioni[0] = 1;
    } else {
        direzioni[0] = -1;
    }
    for (int i = 1; i < 9; i++) {
        direzioni[i] = -direzioni[i - 1];  // Alterna rispetto al precedente     
    }
        for (int i = 9; i < 18; i++){
            direzioni[i] = direzioni[i -9];
        }


        
        

    for(int i=0; i< NUM_CROC; i++){
        pid_coccodrillo[i]= fork();
        if (pid_coccodrillo[i] == -1){
            perror("Fork Coccodrillo fallita");
            exit(EXIT_FAILURE);
        } else if (pid_coccodrillo[i] == 0){
            close(pipe_fd[READ]);  //chiudo la pipe in lettura
            if (i >= 9){
                flag_croc = false;
            }
            main_croc(pipe_fd[WRITE], i, direzioni[i], speed[i], flag_croc); //passo la pipe direttamente in scritturap
            
            exit(EXIT_SUCCESS);
        }

    }


    for (int i = 0; i < NUM_CROC; i++) {
        croc_copy[i].index = i;  // Inizializzazione indici
        croc_copy[i].x = -100;   // Posizione off screen
        croc_copy[i].y = -100;
    }
    

    
    while (1) {
        // Leggi tutti i messaggi disponibili dalla pipe
        while (read(pipe_fd[READ], &msg, sizeof(Messaggio)) > 0)  {
            mvprintw(0, 0, "VITE %d ", vite);
            switch (msg.oggetto) {
                case ID_RANA:
                // Cancella la vecchia posizione della rana
                if (prev_x_rana != -1 && prev_y_rana != -1) {
                    clear_frog(prev_x_rana, prev_y_rana);
                }

                // Copia il messaggio
               // calcola nuova posizione
                 new_x = frog_copy.x + msg.x;
                 new_y = frog_copy.y + msg.y;

                // controlla se è nella safe zona
                 in_safe_zone = (new_y >= 33 && new_y <= 39);

                //controllo boundaries
                out_of_bounds = (new_x < 0 || new_x >= GAME_WIDTH - LARGHEZZA_RANA || 
                                    new_y < 0 || new_y >= GAME_HEIGHT - ALTEZZA_RANA);

                // aggiorno
                if (!out_of_bounds || in_safe_zone) {
                    // se è dentro i bordi e nella safe zone muoviti
                    if (in_safe_zone) {
                        // se è nella safe zone non muoverti in x
                        if (new_x >= 0 && new_x < GAME_WIDTH - LARGHEZZA_RANA) {
                            frog_copy.x = new_x;
                        }
                        
                        // controllo coordinate verticali nella safe zona
                        if (new_y >= 33 && new_y <= GAME_HEIGHT - ALTEZZA_RANA) {
                            frog_copy.y = new_y;
                        }
                    } else {
                        //movimento normale fuori dalla safe zone
                        frog_copy.x = new_x;
                        frog_copy.y = new_y;
                    }
                }
                // Verifica se la rana è su un coccodrillo e aggiorna le sue info
                frog_with_croc(pipe_fd[WRITE], &frog_copy, croc_copy);
                
                // Salva la nuova posizione per la prossima clear
                prev_x_rana = frog_copy.x;
                prev_y_rana = frog_copy.y;

                // Disegna la rana nella nuova posizione
                draw_frog(frog_copy.x, frog_copy.y);

                //se è fuori dallo schermo e fuori dalla safe zone perde vite e viene riposizionata
                if (out_of_bounds && !in_safe_zone || river(frog_copy)) {
                    vite--;
                    frog_copy.x = centro_x;
                    frog_copy.y = centro_y;
                    if (vite <= 0) {
                        endwin();
                        printf("Hai perso tutte le vite. Game Over!\n");
                        exit(EXIT_SUCCESS);
                    }
                }
                
             break;

            case ID_CROCODILE:
                //cancello
                clear_croc(msg);
                //aggiorno
                coccodrilli[msg.index] = msg;
                //disegno
                croc_copy[msg.index] = msg;
                croc_copy[msg.index].index = msg.index;
                prev_x_cocc = msg.x;
                prev_y_cocc = msg.y;
                log_coordinates(msg.index, msg.velocita);
                draw_crocodile(msg.x, msg.y);
                break;         
            case RESPAWN: 
               int direzione = msg.direzione;
                int indice = msg.index;
                int x = msg.x;
                kill(msg.pid, SIGKILL);
                waitpid(msg.pid, &status, 0); // aspetta che il processo muoia
                // Ricrea un nuovo processo coccodrillo con le stesse proprietà
              
                pid_t nuovo_pid = fork();
                flag_croc = true;
                if (nuovo_pid == -1 ){
                    perror("errore fork ");
                    exit(EXIT_FAILURE);
                } else if (nuovo_pid == 0) {
                    close(pipe_fd[READ]);
                    // nuovo processo figlio parte da capo
                    main_croc(pipe_fd[WRITE], indice, direzione, speed[indice], flag_croc);
                    exit(EXIT_SUCCESS);
            }
        
                
                
                break;    
            }    
            //controlla se è dentro la tana oppure se entra in mezzo a due tane
            if (is_inside(frog_copy)){
                int num_tane = num_tana(frog_copy);
                //se la rana è dentro una tana
                if (flag[num_tane] == false && num_tane != 6){
                    flag[num_tane] = true; //setto il flag a true per segnalare che non può più entrare in questa tana

                    tane(frog_copy);  // Chiude graficamente la tana
                    frog_copy.x = centro_x;
                    frog_copy.y = centro_y;
                    // Respawna la rana
                } else{
                    //se la tana è già occupata
                    vite--;
                    frog_copy.x = centro_x;
                    frog_copy.y = centro_y;
                    if (vite <= 0){
                        endwin();
                        printf("Hai perso tutte le vite. Game Over!\n");
                        exit(EXIT_SUCCESS);
                    }
                   
                   refresh();
                }
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

void log_coordinates(int frog_x, int crocodile_x) {
    FILE *file = fopen("coordinates_log.txt", "a"); // Apri il file in modalità append
    if (file == NULL) {
        perror("Errore nell'apertura del file");
        return;
    }

    // Scrivi i valori nel file
    fprintf(file, "---------------\n");
    fprintf(file, "indice: %d\n", crocodile_x);
    fprintf(file, "velocità: %d\n", frog_x);
    fprintf(file, "---------------\n");

    fclose(file); // Chiudi il file
}