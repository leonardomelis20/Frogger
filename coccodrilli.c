#include "coccodrilli.h"

/*sprite del coccodrillo*/
char sprite_coccodrillo[ALTEZZA_COCCODRILLO][LARGHEZZA_COCCODRILLO+1] = {
    " ~~~~~___~~~~~ ",  
    " (o)-------(o) ",  
    " ~~~~~___~~~~~ "   
};

/*funzione che disegna il coccodrillo alla posizione specificata*/
void draw_crocodile(int x, int y) {
    int colonna = 0; 

    /*scorre ogni riga del coccodrillo*/
    for (int i = 0; i < ALTEZZA_COCCODRILLO; i++) {

        /*scorre ogni carattere della riga corrente*/
        for (int j = 0; j < LARGHEZZA_COCCODRILLO; j++) {
            colonna = x + j; //calcolo della posizione corrente

            /*se il carattere è dentro lo schermo allora lo disegna*/
            if (colonna >= 0 && colonna < GAME_WIDTH) {
                mvaddch(y + i, colonna, sprite_coccodrillo[i][j]);
            }
        }
    }
}

/*funzione che aggiorna il movimento del coccodrillo*/
void movement_croc(Messaggio* croc) {
    croc->x += croc->direzione; //aggiorna la posizione orizzontale
}

/*funzione che controlla se il coccodrillo ha superato i bordi dello schermo*/
bool check_borders(Messaggio croc) {
    /*se va verso destra*/
    if(croc.direzione == 1) {
        return (croc.x >= GAME_WIDTH); //controllo che non superi la larghezza dell'area di gioco
    } 
    /*altrimenti, se va verso sinistra*/
    else {
        return (croc.x <= -LARGHEZZA_COCCODRILLO); //considero lo sprite
    }
}

/*funzione che cancella lo sprite del coccodrillo dallo schermo*/
void clear_croc(Messaggio msg) {
    /*scorro le righe*/
    for(int i = 0; i < ALTEZZA_COCCODRILLO; i++) {
        
        /*scorro le colonne*/
        for(int j = 0; j < LARGHEZZA_COCCODRILLO; j++) {
            mvprintw(msg.y + i, msg.x + j, " ");
        }
    }
}

/*funzione che trova l'indice di un coccodrillo dato il suo pid*/
int get_index_croc(Messaggio msg[], pid_t pid) {
    /*scorro tutti i coccodrilli*/
    for(int i = 0; i < NUM_CROC; i++) {

        /*se trovo il pid corrispondente*/
        if(msg[i].pid == pid) {
            return i; //restituisco l'indice
        }
    }

    return 1;
}

/*funzione principale del processo coccodrillo*/
int main_croc(int pipe_fd, int num, int direzione, int speed, bool flag) {
    Messaggio msg; //struct Messaggio da inviare tramite pipe
    int adjusted_num; //numero modificato per il posizionamento 
    bool shooting = false; //falg che controlla se ha già sparato 

    /*se non è il primo gruppo di coccodrilli*/
    if(!flag) {  
        usleep(5000000); //attendo 5 secondi prima di iniziare
    }

    /*se è uno dei secondi coccodrilli, quelli da 9 a 17*/
    if (num >= 9) {
        adjusted_num = num - 9; //lo riporto nell'intervallo da 0 a 8
    } 
    /*altrimenti lo lascio normale*/
    else {
        adjusted_num = num;
    }

    /*assegnamenti*/
    msg.index = num; //impostazione dell'indice del coccodrillo
    msg.pid = getpid(); //salvo il pid del processo
    msg.oggetto = ID_CROCODILE; //identifico il tipo dell'oggetto
    msg.direzione = direzione;
    msg.y = 6 + (adjusted_num * 3); //posizione verticale, è distante ogni 3 righe
    
    /*se la direzione sta partendo da destra*/
    if (msg.direzione == 1) {
        msg.x = 0; //lo faccio partire da sinistra
    } 
    /*altrimenti se sta partendo da sinistra*/
    else {
        msg.x = GAME_WIDTH - LARGHEZZA_COCCODRILLO; //lo faccio iniziare da destra
    }
    
    msg.velocita = speed; //imposto la velocità 
    usleep(50000); //faccio attendere prima dell'inizio

    write(pipe_fd, &msg, sizeof(Messaggio)); //invio il messaggio alla pipe

    while(1) {
        /*aggiorno la posizione*/
        movement_croc(&msg);

        /*gli do il 3% di possibilità di sparo*/
        if (rand() % 100 < 3) {
            msg.is_shooting = true; //il coccodrillo sta sparando
            msg.oggetto = CREATE_BULLET; //il coccodrillo sta creando un proiettile
            msg.index = num; //passo l'indice del coccodrillo

            /*se il coccodrillo non ha sparato*/
            if(shooting == false) {
                shooting = true; //il coccodrillo ha sparato
                write(pipe_fd, &msg, sizeof(Messaggio)); //invio il messaggio alla pipe
            }

            msg.oggetto = ID_CROCODILE; //ripristino l'oggetto a coccodrillo
        }

        /*se non ha superato i limiti*/
        if (check_borders(msg)) {
            msg.oggetto = RESPAWN; //imposto l'oggetto a respawn
            write(pipe_fd, &msg, sizeof(Messaggio)); //invio la richiesta di respawn
            break; //serve per uscire dal ciclo infitio
        } 

        write(pipe_fd, &msg, sizeof(Messaggio)); //scrivo nella pipe
        usleep(msg.velocita); //attendo in base alla velocità
    }
}
