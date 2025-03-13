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

int verifica_collisione(int x_rana, int y_rana, int x_coccodrillo[], int y_coccodrillo[]){

    int cx_start = 0, cx_end = 0, cy = 0;
    bool sopra, mid_sopra, between = false; 
    int lim_dx = 0, lim_sx = 0;
    int mid_rana = 0;
    int flag = 0;
    mid_rana = LARGHEZZA_RANA / 2;

    // x = 68 y = 23



    if (y_rana >= SAFE_ZONE_START - 3 && y_rana <= GAME_HEIGHT){ //controllo marciapiede iniziale
        flag = 1;
    }

    
    for (int i = 0; i < NUM_STREAMS; i++) {
        if (x_coccodrillo[i] != 0)
        {
           cx_start = x_coccodrillo[i];
           cx_end = cx_start + LARGHEZZA_COCCODRILLO;
           cy = y_coccodrillo[i];

           sopra = (y_rana >= y_coccodrillo[i] && y_rana < y_coccodrillo[i] + ALTEZZA_COCCODRILLO);

           if (sopra){
            lim_sx = cx_start - mid_rana;
            lim_dx = cx_end + mid_rana;

            mid_sopra = (x_rana >= lim_sx && (x_rana + LARGHEZZA_RANA) <= lim_dx);

            if (mid_sopra){
                flag = 1; 

            } else {
                flag = 0;
            }
           }
           //non funziona?????
           if (i < NUM_STREAMS - 1) {
            int cx_next_start = x_coccodrillo[i + 1];
            int cx_next_end = cx_next_start + LARGHEZZA_COCCODRILLO;
            
            if (cx_end < cx_next_start && x_rana > cx_end -3 && x_rana < cx_next_start +3) {
                between = true;
            }
        }

        

        }
    }
    if (flag || between) {
        return 1;  // Collisione valida, la rana è sopra qualcosa di sicuro
    } else {
        return 0;  // Nessuna collisione valida, la rana è in acqua
    }
}


