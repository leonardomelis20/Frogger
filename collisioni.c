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
        if (frog.x >= croc[i].x && (frog.x + LARGHEZZA_RANA) <= (croc[i].x + LARGHEZZA_COCCODRILLO) &&
        frog.y == croc[i].y && frog.y == (croc[i].y )) {        
        flag = i;
        return flag;
        } 

    }
    return flag;
}

Messaggio correct_x_frog (Messaggio frog){
    Messaggio new_cord;
    new_cord.y = frog.y;

    new_cord.x = (frog.x + (LARGHEZZA_RANA/2)) / LARGHEZZA_RANA * LARGHEZZA_RANA;

    return new_cord;

}



void frog_with_croc(Messaggio* frog, Messaggio* croc) {
    int i = check_collision(*frog, croc);
    frog->on_croc = true;
    frog->croc_index = i;
    
    if (i == -1) {
        frog->on_croc = false;
        frog->croc_index = -1;
    }    
}

bool check_borders_frog(Messaggio frog) {
    if (frog.x < 0 || frog.x >= GAME_WIDTH - LARGHEZZA_RANA || 
        frog.y < 0 || frog.y > GAME_HEIGHT - ALTEZZA_RANA) {
        return true; //fuori dai bordi
    }
    return false;
}

bool check_borders_bullet(Messaggio bullet) {
    if (bullet.x < 0 || bullet.x >= GAME_WIDTH || 
        bullet.y < 0 || bullet.y > GAME_HEIGHT) {
        return true; //fuori dai bordi
    }
    return false;
}

bool check_safe_zone(Messaggio frog) {
    if (frog.y >= SAFE_ZONE_DOWN && frog.y <= SAFE_ZONE_DOWN_2) {
        return true; // dentro la zona sicura
    } else if (frog.y >= 0 && frog.y <= SAFE_ZONE_UP) {
        return true; // dentro la tana
    }
    return false;
}

bool river(Messaggio frog) {
    if (!check_safe_zone(frog) && !frog.on_croc) {
        return true;
    }
    return false;
}

