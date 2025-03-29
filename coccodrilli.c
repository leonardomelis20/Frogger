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
        coccodrilli[flusso].velocita = MIN_VELOCITA + rand() % (MAX_VELOCITA - MIN_VELOCITA + 1); // Velocità casuale tra i due estremi
        write(pipe_fd[WRITE], coccodrilli, sizeof(InfoCocc));
        write(pipe_fd[WRITE], &msg, sizeof(Messaggio));
        close(pipe_fd[WRITE]);
        exit(EXIT_SUCCESS);
    }
    //usleep(200000);  // Ritardo di 200ms tra la creazione dei coccodrilli
    }
}


void movement_croc(InfoCocc* croc){

    while (1)
    {
        croc->x += croc->direzione;

  }
    
}


bool check_borders(InfoCocc* croc){

    if (( croc->direzione == 1 && croc->x >= GAME_WIDTH) || (croc->direzione == -1 && croc->x <= -LARGHEZZA_COCCODRILLO)) {
        return true;
    }

    return false;
}

void clear_croc(InfoCocc croc){
    for(int i = 0; i < ALTEZZA_COCCODRILLO; i++){
        for(int j = 0; j < LARGHEZZA_COCCODRILLO; j++){
            mvprintw(croc.y + i, croc.x + j, " ");
        }
    }
}

int get_pid_croc(InfoCocc coccodrilli[], pid_t pid){
    for(int i = 0; i < NUM_CROC; i++){
        if(coccodrilli[i].pid == pid){
            return i;
        }
    }

    return -1;
}

int main_croc(int pipe_fd[2], InfoCocc* croc, Messaggio msg){
    int index = 0;
    while(1){
        movement_croc(croc);
        printf("%d, %d", croc->x, croc->y);
        fflush(stdout);
        
        draw_crocodile(croc->x, croc->y);
        if (check_borders(croc)){
            index = get_pid_croc(croc, msg.pid);
            clear_croc(croc[index]);
            // kill e respwan
        }

        
        
        if(write(pipe_fd[WRITE], croc, sizeof(InfoCocc)) == -1){
            perror("Error movement pipe");
            exit(EXIT_FAILURE);
        }

    }
    
}



