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
   
    int cx_start, cx_end, cy;
    bool sopra, mid_sopra; 
    int lim_dx, lim_sx;
    int mid_rana;
    int flag;
    mid_rana = LARGHEZZA_RANA / 2;
    for (int i = 0; i < NUM_STREAMS; i++)
    {
        if (x_coccodrillo[i] != 0)
        {
           cx_start = x_coccodrillo[i];
           cx_end = cx_start + LARGHEZZA_COCCODRILLO;
           cy = y_coccodrillo[i];

           sopra = (y_rana >= y_coccodrillo && y_rana < y_coccodrillo + ALTEZZA_COCCODRILLO);

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
        

        }
    }
        return flag;
}
    