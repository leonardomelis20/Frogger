#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>


#include "strutture.h"
#include "coccodrilli.h"
#include "rana.h"

int check_collision(Messaggio frog, Messaggio* croc){
    int flag = -1;
    for (int i = 0; i < NUM_CROC; i++)
    {
        if (frog.x >= croc[i].x && frog.x <= (croc[i].x + LARGHEZZA_COCCODRILLO) &&
        frog.y >= croc[i].y && frog.y <= (croc[i].y )) {        
        flag = i;
        return flag;
        } 

    }
    return flag;
}

void frog_with_croc(int pipe_fd, Messaggio* frog, Messaggio* croc) {
    int i = check_collision(*frog, croc);
    frog->on_croc = true;
    frog->croc_index = i;

    
    if (i != -1) {
        //frog->x = croc[i].x + (LARGHEZZA_COCCODRILLO - LARGHEZZA_RANA) / 2;
        frog->y = croc[i].y;
        frog->velocita = croc[i].velocita;
        frog->direzione = croc[i].direzione;
        frog->oggetto = ID_RANA;
        frog->on_croc = true;
        frog->croc_index = i;
        frog->x += croc[i].direzione;
       
    } else {
        frog->on_croc = false;
        frog->croc_index = -1;
    }

    
    write(pipe_fd, &frog, sizeof(Messaggio));

    
}

bool check_borders_frog(Messaggio frog) {
    if (frog.x < 0 || frog.x >= GAME_WIDTH - LARGHEZZA_RANA || 
        frog.y < 0 || frog.y > GAME_HEIGHT - ALTEZZA_RANA) {
        return true; //fuori dai bordi
    }
    return false;
}

bool check_safe_zone(Messaggio frog) {
    if (frog.y >= 33 && frog.y <= 39) {
        return true; // dentro la zona sicura
    }
    return false;
}

bool river(Messaggio frog) {
    if (!check_safe_zone(frog) && frog.on_croc == false) {
       return true; // dentro il fiume
    }
    else false;
}

