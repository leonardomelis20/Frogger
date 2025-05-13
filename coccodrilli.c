#include "coccodrilli.h"

char sprite_coccodrillo[ALTEZZA_COCCODRILLO][LARGHEZZA_COCCODRILLO+1] = {
    " ~~~~~___~~~~~ ",  
    " (o)-------(o) ",  
    " ~~~~~___~~~~~ "   
};

/*disegna il coccodrillo sullo schermo, solo se la parte da disegnare è visibile*/
void draw_crocodile(int x, int y) {
    int colonna;
    /*scorre ogni riga del coccodrillo*/
    for (int i = 0; i < ALTEZZA_COCCODRILLO; i++) {
        /*scorre ogni carattere della riga corrente (11 colonne)*/
        for (int j = 0; j < LARGHEZZA_COCCODRILLO; j++) {
            colonna = x + j; //calcola la posizione
            /*se la colonna è dentro lo schermo, allora disegna il carattere*/
            if (colonna >= 0 && colonna < GAME_WIDTH) {
                mvaddch(y + i, colonna, sprite_coccodrillo[i][j]);
            }
        }
    }
}

/*funzione per aggiornare il movimento del coccodrillo*/
void movement_croc(Messaggio* croc) {
    croc->x += croc->direzione;
}

bool check_borders(Messaggio croc) {
    if(croc.direzione == 1) {
        return (croc.x >= GAME_WIDTH); //destra, controlla solo x
    } 
    else {
        return (croc.x <= -LARGHEZZA_COCCODRILLO); //sinistra, considera la lunghezza
    }
}

/*funzione che cancella i coccodrilli*/
void clear_croc(Messaggio msg) {
    for(int i = 0; i < ALTEZZA_COCCODRILLO; i++) {
        for(int j = 0; j < LARGHEZZA_COCCODRILLO; j++) {
            mvprintw(msg.y + i, msg.x + j, " ");
        }
    }
}

/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*DA RIVEDERE IL NOMEEEEEEEEEEE*/
/*funzione per recuperare l'indice corretto del coccodrillo*/
int get_pid_croc(Messaggio msg[], pid_t pid) {
    for(int i = 0; i < NUM_CROC; i++) {
        if(msg[i].pid == pid) {
            return i;
        }
    }
    return 1;
}
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

int main_croc(int pipe_fd, int num, int direzione, int speed, bool flag) {
    if(!flag) {  
        usleep(5000000); //1,5 secondi di ritardo per il secondo coccodrillo
    }

    Messaggio msg;
    int adjusted_num;
    bool shooting = false;
  
    /*gestisce la seconda serie di coccodrilli (indici 9-17)*/
    if (num >= 9) {
        //conversione dei primi 9 indici (0-8) per riutilizzare le stesse posizioni del flusso
        adjusted_num = num - 9;
    } 
    else {
        adjusted_num = num;
    }

    msg.index = num;
    msg.pid = getpid();
    msg.oggetto = ID_CROCODILE;

    msg.direzione = direzione;

    msg.y = 6 + (adjusted_num * 3);  //ogni y è distante 3
    
    /*setto la x in base alla posizione*/
    if (msg.direzione == 1) {
        msg.x = 0; //inizia da sinistra se si sta muovendo verso destra
    } 
    else {
        msg.x = GAME_WIDTH - LARGHEZZA_COCCODRILLO; //inizia da destra se si sta muovendo verso sinistra
    }
    
    msg.velocita = speed; 
    usleep(50000);
    write(pipe_fd, &msg, sizeof(Messaggio));
    
    while(1) {
        movement_croc(&msg); //aggiorno la posizione

        /*controllo se è il momento di sparare un proiettile, ovvero ogni 3 secondi*/
        if (rand() % 100 < 3) {
            msg.is_shooting = true; //il coccodrillo sta sparando
            msg.oggetto = CREATE_BULLET; //il coccodrillo sta creando un proiettile
            msg.index = num; //passo l'indice del coccodrillo

            if(shooting == false) {
                shooting = true; //il coccodrillo ha sparato
                write(pipe_fd, &msg, sizeof(Messaggio));
            }

            msg.oggetto = ID_CROCODILE; //ripristino l'oggetto a coccodrillo
        }

        if (check_borders(msg)) {
            msg.oggetto = RESPAWN; 
            write(pipe_fd, &msg, sizeof(Messaggio));
            break;
        }

        write(pipe_fd, &msg, sizeof(Messaggio)); //scrivo nella pipe
        usleep(msg.velocita);
    }
}