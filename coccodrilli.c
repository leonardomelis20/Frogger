#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#include "strutture.h"
#include "coccodrilli.h"

void coccodrillo_sig_handler(int signo) {
    if (signo == SIGTERM) {
        // Chiudi correttamente e esci
        exit(EXIT_SUCCESS);
    }
}

char spriteCoccodrillo[ALTEZZA_COCCODRILLO][LARGHEZZA_COCCODRILLO+1] = {
    "  ~~~___~~ ",  
    " (o)---(o) ",  
    "  ~~~   ~~ "   
};

void draw_crocodile(int x, int y){
    for (int i = 0; i < ALTEZZA_COCCODRILLO; i++){
        mvprintw( y +i, x, "%s", spriteCoccodrillo[i]);
    }

}

void clear_cocodrile(int x, int y, int direzione, pid_t pid) {
    int offset;

    if ((direzione == 1 && x >= GAME_WIDTH - LARGHEZZA_COCCODRILLO) || //direzione destra
        (direzione == -1 && x <= 0)) { //direzione sinistra
        
        for (int step = 0; step < LARGHEZZA_COCCODRILLO; step++) { //cancella carattere per carattere
            for (int i = 0; i < ALTEZZA_COCCODRILLO; i++) { //cancella riga per riga
                

                if (direzione == 1) {
                    // Se si muove a destra, cancella dalla testa (destra)
                    offset = LARGHEZZA_COCCODRILLO - step -1;
                } else  {
                    // Se si muove a sinistra, cancella dalla testa (sinistra)
                    offset = step;
                }

                mvprintw(y + i, x + offset, " ");  // Cancella la testa prima
            }
            
        }
        refresh();
        // Aggiungi un piccolo ritardo per rendere l'effetto visibile
        usleep(100000);  // 100ms di ritardo tra ogni passo
    }
}


// In coccodrilli.c, modifica la funzione crocodile:
void crocodile(int pipe_fd, InfoFlussi* info, int index) {

    signal(SIGTERM, coccodrillo_sig_handler);
    srand(time(NULL) ^ getpid());

    Messaggio msg;
    msg.index = index;
    msg.x = info[index].x_pos;
    msg.y = info[index].y_pos;
    msg.oggetto = ID_CROCODILE;
    msg.velocita = info[index].speed;
    int direzione = info[index].direzione;

    while (1) {      
        msg.x += direzione;  // Muovi il coccodrillo nella direzione corretta
        
        // Se il coccodrillo è uscito dallo schermo, lo riposiziona
        if ((direzione == 1 && msg.x >= GAME_WIDTH) || (direzione == -1 && msg.x <= 0)) {
            // Invia un messaggio speciale per pulire il coccodrillo e passo le informazioni dei flussi
            msg.oggetto = -1;
            msg.x = info[index].x_pos;
            msg.y = info[index].y_pos;
            msg.oggetto = ID_CROCODILE;
            msg.velocita = info[index].speed;
            write(pipe_fd, &msg, sizeof(Messaggio));

        }

        // Scrittura nella pipe per aggiornare la posizione
        while (write(pipe_fd, &msg, sizeof(Messaggio)) == -1 ) {
            perror("Errore durante la scrittura della pipe");
            usleep(10000); // Aspetta 10ms prima di riprovare
        }

        usleep(msg.velocita);
        }
    }
