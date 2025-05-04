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
#include "collisioni.h"

char *bullet="°";

void draw_bullet(int x, int y){
    mvprintw(y, x, "%s", bullet);
}
void clear_bullet(int x, int y){
    mvaddch(y, x, ' ');
}

//funzione per aggiornare il movimento del coccodrillo
void movement_bullet(Messaggio *croc, Messaggio *bullet){
    bullet->x += croc->direzione;
}

void main_bullet(int pipe_fd, Messaggio croc){

    Messaggio bullets[NUM_CROC];
    time_t last_shot_time[NUM_CROC];
    bool bullet_active[NUM_CROC];
    int adjusted_i;
    int shoot_interval[NUM_CROC]; // intervallo casuale di attesa
    
    // Inizializza gli array per tenere traccia dei proiettili
    for (int i = 0; i < NUM_CROC; i++) {
        bullet_active[i] = false;
        last_shot_time[i] = time(NULL); // Imposta il tempo iniziale
        shoot_interval[i] = (rand() % (MAX_SHOOT_DELAY - MIN_SHOOT_DELAY + 1)) + MIN_SHOOT_DELAY;
    
    }

    while(1) {
        time_t current_time = time(NULL);
        for (int i = 0; i < NUM_CROC; i++) {
            // Verifica se è passato tempo sufficiente dall'ultimo sparo (3 secondi)
            if (!bullet_active[i] && (current_time - last_shot_time[i]) >= shoot_interval[i]) {
                // Crea un nuovo proiettile per questo coccodrillo
                bullets[i].velocita = BULLET_SPEED;
                bullets[i].direzione = croc.direzione;
                bullets[i].index = i;
                bullets[i].oggetto = ID_BULLET;
                
                // Posiziona il proiettile alla testa del coccodrillo (centro)
                if (bullets[i].direzione == 1) {
                    // Se il coccodrillo va verso destra, il proiettile parte dalla testa destra
                    bullets[i].x = croc.x + LARGHEZZA_COCCODRILLO;
                } else {
                    // Se il coccodrillo va verso sinistra, il proiettile parte dalla testa sinistra
                    bullets[i].x = croc.x - 1;
                }
                
                // Calcola la posizione y del proiettile (centro del coccodrillo)
                
                if (i >= 9){
                    adjusted_i = i - 9;
                } else {
                    adjusted_i = i;
                }

                bullets[i].y = 6 + (adjusted_i * 3) + 1; // Centro verticale del coccodrillo
                
                // Attiva il proiettile e aggiorna il tempo dell'ultimo sparo
                bullet_active[i] = true;
                last_shot_time[i] = current_time;
                shoot_interval[i] = (rand() % (MAX_SHOOT_DELAY - MIN_SHOOT_DELAY + 1)) + MIN_SHOOT_DELAY;
                
                // Invia il messaggio per disegnare il proiettile
                log_coordinates(bullets[i].x, bullets[i].y);
                write(pipe_fd, &bullets[i], sizeof(Messaggio));
            } 
            // Se il proiettile è attivo, aggiorna la sua posizione
            else if (bullet_active[i]) {
                // Cancella il proiettile dalla posizione precedente
                clear_bullet(bullets[i].x, bullets[i].y);
                
                // Aggiorna la posizione del proiettile
                movement_bullet(&croc, &bullets[i]);
                
                // Verifica se il proiettile ha colpito un bordo
                if (check_borders_bullet(bullets[i])) {
                    // Disattiva il proiettile se esce dai bordi
                    bullet_active[i] = false;
                    continue;
                }
                 // Invia il messaggio per disegnare il proiettile nella nuova posizione
                 write(pipe_fd, &bullets[i], sizeof(Messaggio));
                }
            }
            
            // Piccola pausa per non sovraccaricare la CPU
            usleep(BULLET_SPEED);
        }
    }
            
