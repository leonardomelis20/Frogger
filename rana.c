#include "rana.h"

/*sprite della rana*/
char spriteRana[ALTEZZA_RANA][LARGHEZZA_RANA + 1] = {
    " o.o ", 
    "+-|-+", 
    "\\-|-/"
};

/**
 * funzione che disegna la rana alla posizione (x, y)
 * @param x coordinata orizzontale della rana
 * @param y coordinata verticale della rana
 */
void draw_frog(int x, int y) {
    /*ciclo che scorre*/
    attron(COLOR_PAIR(1)); //attiviamo il colore verde
    for (int i = 0; i < ALTEZZA_RANA; i++) {
        mvprintw(y + i, x, "%s", spriteRana[i]);
    }

    attroff(COLOR_PAIR(1)); //disattiviamo il colore

}

void clear_frog(int x, int y) {
    for (int i = 0; i < ALTEZZA_RANA; i++) {
        mvprintw(y + i, x, "     "); //5 spazi per cancellare la rana
    }
}

bool is_inside (Messaggio msg) {
    /*per verificare quando la rana è dentro la tana a partire dall'inizio alla fine*/
    if (msg.y == 0 && ((msg.x >= 0 && msg.x <= 80))) {
        return true;
    } 
    return false;
}

int num_tana(Messaggio msg) {
    int tana = 6; //valore di default per indicare che non è dentro una tana
    /*in base alle coordinate della rana restituisco il nuemro della tana*/
    if (msg.y == 0) {
        if (msg.x >= 5 && msg.x <= 13) {
            tana = 1;
        }
        else if (msg.x >= 22 && msg.x <= 29) {
            tana = 2;
        }
        else if (msg.x >= 39 && msg.x <= 46) {
            tana = 3;
        }
        else if (msg.x >= 56 && msg.x <= 63) {
            tana = 4;
        }
        else if (msg.x >= 73 && msg.x <= 80) {
            tana = 5;
        }
        else {
            tana = 6; 
        }
        return tana;
    }
}

void tane(Messaggio msg) {
    Messaggio tana;
    /*verifico il numero di tana e la riempio*/
    tana.x = num_tana(msg);
    if (is_inside(msg)) {
        draw_closed_burrows(tana);
        clear_frog(msg.x, msg.y);
        refresh();
    }
}

void frog(int pipe_fd, bool* flag, int* speed ) {
    Messaggio msg;
    msg.oggetto = ID_RANA;
    msg.pid = getpid();
    int input;
    msg.on_croc = false;
    msg.croc_index = -1;
    msg.x = 0;
    msg.y = 0;
    int i = 0;

    /*invia la posizione iniziale*/
    write(pipe_fd, &msg, sizeof(Messaggio));

    /*loop principale che gestisce l'input*/
    while(1) {
        usleep(100000); //0.1 secondi di attesa
        
        /*lettura input non bloccante*/
        input = getch();
        
        switch (input) {
            case KEY_UP:
                msg.y = -3;
                break;
            case KEY_DOWN:
                msg.y = 3;
                break;
            case KEY_LEFT:
                msg.x = -3;
                break;
            case KEY_RIGHT:
                msg.x = 3;
                break;
            case ' ':
                /*invio un messaggio per creare un proiettile*/
                msg.oggetto = CREATE_GRENADE;
                write(pipe_fd, &msg, sizeof(Messaggio));
                break;
            /*case d'uscita*/
            case 'q': 
                close(pipe_fd);
                exit(EXIT_SUCCESS);
                return;
                break;
        }
        
        msg.oggetto = ID_RANA; //ripristino l'oggetto a rana
        write(pipe_fd, &msg, sizeof(Messaggio)); //invio la posizione aggiornata
        msg.x = 0;
        msg.y = 0;
    }
}