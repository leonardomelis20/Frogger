#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h> 
#include <stdbool.h>

#include "strutture.h"
#include "coccodrilli.h"
#include "proiettili.h"

char sprite_coccodrillo[ALTEZZA_COCCODRILLO][LARGHEZZA_COCCODRILLO+1] = {
    " ~~~~~___~~~~~ ",  
    " (o)-------(o) ",  
    " ~~~~~___~~~~~ "   
};

// Disegna il coccodrillo sullo schermo, ma solo se la parte da disegnare è visibile
void draw_crocodile(int x, int y){
    // Scorre ogni riga del coccodrillo (3 righe in totale)
    for (int i = 0; i < ALTEZZA_COCCODRILLO; i++){
          // Scorre ogni carattere della riga corrente (11 colonne)
        for (int j = 0; j < LARGHEZZA_COCCODRILLO; j++){
            int colonna = x + j; // Calcola la posizione
            // Se la colonna è dentro lo schermo, allora disegna il carattere
            if (colonna >= 0 && colonna < GAME_WIDTH) {
                mvaddch(y + i, colonna, sprite_coccodrillo[i][j]);
            }
        }
    }
}

//funzione per aggiornare il movimento del coccodrillo
void movement_croc(Messaggio * croc){
    croc->x += croc->direzione;
}


bool check_borders(Messaggio croc) {
    // Aggiungi debug
    //printf("Check borders: x=%d, dir=%d, LARGHEZZA=%d\n", 
          // croc.x, croc.direzione, LARGHEZZA_COCCODRILLO);
    
    if(croc.direzione == 1) {
        return (croc.x >= GAME_WIDTH);  // Destra: controlla solo x
    } else {
        return (croc.x <= -LARGHEZZA_COCCODRILLO); // Sinistra: considera la lunghezza
    }
}

//funzione per cancellare i coccodrilli
void clear_croc(Messaggio msg){
    for(int i = 0; i < ALTEZZA_COCCODRILLO; i++){
        for(int j = 0; j < LARGHEZZA_COCCODRILLO; j++){
            mvprintw(msg.y + i, msg.x + j, " ");
        }
    }
}

//funzione per recuperare l'indice corretto del coccodrillo
int get_pid_croc(Messaggio msg[], pid_t pid){
    for(int i = 0; i < NUM_CROC; i++){
        if(msg[i].pid == pid){
            return i;
        }
    }

    return 1;
}


int main_croc(int pipe_fd, int num, int direzione, int speed, bool flag){
    
    if(!flag){  
        usleep(5000000); // 1.5 seconds delay for the second crocodile
    }

    
    Messaggio msg;
    int status;
    int adjusted_num;
    time_t last_shot_time = time(NULL);
  
    
    // Handle the second set of crocodiles (indices 9-17)
    if (num >= 9) {
        // Convert to first 9 indices (0-8) to reuse the same stream positions
        adjusted_num = num - 9;
        
        } else {
            adjusted_num = num;
    }
    msg.index = num;
    msg.pid = getpid();
    msg.oggetto = ID_CROCODILE;

   msg.direzione = direzione;

   msg.y = 6 + (adjusted_num * 3);  // Ogni y è distante 3 unità
    
    // Setto la x in base alla posizione
    if (msg.direzione == 1) {
        msg.x = 0;  //Inizia d sinistra se si sta muovendo verso destra
        } else {
            msg.x = GAME_WIDTH - LARGHEZZA_COCCODRILLO;  // Inizia da destra se si sta muovendo verso sinistra
    }
    
    msg.velocita = speed; 
    usleep(50000);

    write(pipe_fd, &msg, sizeof(Messaggio));
    while(1){

        //aggiorno la posizione
        movement_croc(&msg);

         // Controllo se è il momento di sparare un proiettile (ogni 3 secondi)
         time_t current_time = time(NULL);
         if (current_time - last_shot_time >= 3) {
            msg.is_shooting = true; // Indica che il coccodrillo sta sparando
             msg.oggetto = ID_BULLET;
             // Scrivo il proiettile nella pipe
             write(pipe_fd, &msg, sizeof(Messaggio));
             last_shot_time = current_time; // Aggiorno il tempo dell'ultimo sparo
             msg.oggetto = ID_CROCODILE; // Ripristino l'oggetto a coccodrillo
         }

            if (check_borders(msg)){
                msg.oggetto = -1; 
                write(pipe_fd, &msg, sizeof(Messaggio));
                break;
            }

        
        
        //scrivo nella pipe
        
        write(pipe_fd, &msg, sizeof(Messaggio));
        usleep(msg.velocita);
    }
}
