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


void draw_crocodile(int x, int y){
    for (int i = 0; i < ALTEZZA_COCCODRILLO; i++){
        mvprintw( y +i, x, "%s", sprite_coccodrillo[i]);
    }

}


void inizializza_coccodrilli(int pipe_fd[2], InfoCocc coccodrilli[NUM_CROC]){
    int direzione; // Memorizza la direzione attuale
    int num = MIN_COCCODRILLO + rand() % (MAX_COCCODRILLO - MIN_COCCODRILLO +1);
    InfoFlussi info[NUM_CROC];
    Messaggio msg;
    pid_t pid;

 //assegno la prima direzione in modo casuale
    if (num % 2 == 0){
        direzione = 1;
    } else {
        direzione = -1; 
    }
   
  // Crea un coccodrillo iniziale per flusso
  for (int flusso = 0; flusso < NUM_STREAMS; flusso++) {
        pid = fork();

    if (pid == -1) {
        perror("Fork coccodrillo fallita");
        exit(EXIT_FAILURE);

    } else if (pid == 0) {
        close(pipe_fd[READ]);
        coccodrilli[flusso].pid = getpid();
        msg.oggetto = ID_CROCODILE;
        msg.pid = coccodrilli[flusso].pid;
        
        if (flusso % 2 == 0) {
            coccodrilli[flusso].direzione = direzione;
        } else {
            coccodrilli[flusso].direzione = -direzione;
        }
        
        // Setto la x in base alla posizione
        if (coccodrilli[flusso].direzione == 1) {
            coccodrilli[flusso].x = 0;  //Inizia d sinistra se si sta muovendo verso destra
        } else {
            coccodrilli[flusso].x = GAME_WIDTH - LARGHEZZA_COCCODRILLO;  // Inizia da destra se si sta muovendo verso sinistra
        }
        
        // Setto la y e la velocità
        coccodrilli[flusso].y = 6 + (flusso * 3);  // Ogni y è distante 3 unità
        coccodrilli[flusso].velocita = MIN_VELOCITA + rand() % (MAX_VELOCITA - MIN_VELOCITA + 1);
         // Velocità casuale tra i due estremi
        msg.direzione = coccodrilli[flusso].direzione;
        msg.x = coccodrilli[flusso].x;
        msg.y = coccodrilli[flusso].y;
        msg.velocita = coccodrilli[flusso].velocita;
        msg.index = coccodrilli[flusso].index;

        write(pipe_fd[WRITE], &msg, sizeof(Messaggio));
        close(pipe_fd[WRITE]);
    }
    //usleep(200000);  // Ritardo di 200ms tra la creazione dei coccodrilli
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

int main_croc(int pipe_fd[2],int num){
    int index = 0;
    int direzione; // Memorizza la direzione attuale
    InfoFlussi info[NUM_CROC];
    Messaggio msg;
    pid_t pid;

    msg.index = num;
    //assegno la prima direzione in modo casuale
    if (num % 2 == 0){
        direzione = 1;
    } else {
        direzione = -1; 
    }

    msg.oggetto = ID_CROCODILE;
    msg.pid = getpid();
    
    if (num % 2 == 0) {
        msg.direzione = direzione;
    } else {
        msg.direzione = -direzione;
    }
    
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
        write(pipe_fd[WRITE], &msg, sizeof(Messaggio));
        // if(write(pipe_fd[WRITE], &msg, sizeof(Messaggio)) == -1){
        //     perror("Error movement pipe");
        //     exit(EXIT_FAILURE);
        // }

        usleep(20000);
    }
    
}



