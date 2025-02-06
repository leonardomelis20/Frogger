#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "rana.h"
#include "strutture.h"

//RANA = SPRITE 
//FROG = PARAMETRO

void grafica(int pipe_fd[2]);


void create_pipe(int pipe_fd[2]){
    if (pipe(pipe_fd) == -1){
        perror("Errore pipe");
        exit(1);
    }
}

void close_pipe(int pipe_fd[2]){
    close(pipe_fd[0]);
    close(pipe_fd[1]);
}

int main(){
    int pipe_fd[2];
    pid_t pid_rana, pid_grafica;
    pid_rana = fork();
    Messaggio msg;


    if (pid_rana == -1){
        perror("Fork rana fallita");
    } else if (pid_rana == 0){
        frog(pipe_fd);
        return 0;
        // execl("./rana", "rana", NULL);  DA NON FARE
        // perror("Errore esecuzione rana.c");
        // exit(1);
    } else {
        switch (msg.oggetto)
        {
        case FROG:
            // aggiornamento grafica
            break;
        case COCCODRILLO:
         //altre cose
            break;
        }
    }
}

