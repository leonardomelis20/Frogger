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

char sprite_coccodrillo[ALTEZZA_COCCODRILLO][LARGHEZZA_COCCODRILLO+1] = {
    "  ~~~___~~ ",  
    " (o)---(o) ",  
    "  ~~~   ~~ "   
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


/*bool check_borders(Messaggio croc){

    if (( croc->direzione == 1 && croc->x >= GAME_WIDTH) || (croc->direzione == -1 && croc->x <= -LARGHEZZA_COCCODRILLO)) {
        return true;
    }

    return false;
}*/

//funzione per cancellare i coccodrilli
void clear_croc(Messaggio msg){
    for(int i = 0; i < ALTEZZA_COCCODRILLO; i++){
        for(int j = 0; j < LARGHEZZA_COCCODRILLO; j++){
            mvprintw(msg.y + i, msg.x + j, " ");
        }
    }
}

//funzione per recuperare l'indice corretto del coccodrillo
int get_pid_croc(InfoCocc coccodrilli[], pid_t pid){
    for(int i = 0; i < NUM_CROC; i++){
        if(coccodrilli[i].pid == pid){
            return i;
        }
    }

    return -1;
}

int main_croc(int pipe_fd, int num, int direzione){
    srand(getpid()); 
    Messaggio msg;
    pid_t pid;

    msg.index = num;
    msg.oggetto = ID_CROCODILE;
    msg.pid = getpid();
    srand(msg.pid); 
    
   msg.direzione = direzione;
    
    // Setto la x in base alla posizione
    if (msg.direzione == 1) {
        msg.x = 0;  //Inizia d sinistra se si sta muovendo verso destra
    } else {
        msg.x = GAME_WIDTH - LARGHEZZA_COCCODRILLO;  // Inizia da destra se si sta muovendo verso sinistra
    }
        
    // // Setto la y e la velocità
     msg.y = 6 + (num * 3);  // Ogni y è distante 3 unità
     msg.velocita = MIN_VELOCITA + rand() % (MAX_VELOCITA - MIN_VELOCITA + 1);
    // Velocità casuale tra i due estremi
     


    while(1){

        //aggiorno la posizione
        movement_croc(&msg);
        
        //scrivo nella pipe
        write(pipe_fd, &msg, sizeof(Messaggio));
        // if(write(pipe_fd[WRITE], &msg, sizeof(Messaggio)) == -1){
        //     perror("Error movement pipe");
        //     exit(EXIT_FAILURE);
        // }

        usleep(msg.velocita);
    }
    
}



