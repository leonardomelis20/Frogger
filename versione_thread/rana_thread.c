#include "rana_thread.h"

/**
 * funzione che verifica se la rana è dentro una tana
 * @param msg struct Messaggio che contiene le coordinate della rana
 * @return true se la rana è dentro una tana, false altrimenti
 */
bool is_inside (Messaggio msg) {
    /*se la rana è dentro la tana*/
    if (msg.y == 0 && ((msg.x >= 0 && msg.x <= 80))) {
        return true; //restituisce true
    } 

    return false; //altrimenti se non è dentro una tana, restituisce false
}

/**
 * funzione che determina il numero della tana in cui si trova la rana
 * @param msg struct Messaggio che contiene le coordinate della rana
 * @return numero della tana da 1 a 5 o 6 se non è in nessuna tana
 */
int num_tana (Messaggio msg) {
    int tana = 6; //variabile inizializzata a nessuna tana

    /*se la rana è nell'area delle tane*/
    if (msg.y == 0) {
        /*se la rana è nella prima tana*/
        if (msg.x >= 5 && msg.x <= 13) {
            tana = 1; //assegniamo il numero della tana 1
        }
        /*se la rana è nella seconda tana*/
        else if (msg.x >= 22 && msg.x <= 29) {
            tana = 2; //assegniamo il numero della tana 2
        }
        /*se la rana è nella terza tana*/
        else if (msg.x >= 39 && msg.x <= 46) {
            tana = 3; //assegniamo il numero della tana 3
        }
        /*se la rana è nella quarta tana*/
        else if (msg.x >= 56 && msg.x <= 63) {
            tana = 4; //asseegniamo il numero della tana 4
        }
        /*se la rana è nella quinta tana*/
        else if (msg.x >= 73 && msg.x <= 80) {
            tana = 5; //assegniamo il numero della tana 5
        }
        /*altrimenti se la rana non è in nessuna tana*/
        else {
            tana = 6; //assegniamo alla tana il 6 per dire che non è in nessuna 
        }
        return tana; //facciamo ritornare il numero della tana
    }
    return tana; //facciamo ritornare il numero della tana
}

/**
 * funzione principale per la gestione del thread rana
 * @param arg puntatore ai parametri del thread
 * @return NULL
 */
void* frog_thread (void* arg) {
    Thread_args* args = (Thread_args*)arg; //per accedere ai parametri del thread 
    Messaggio msg; //messaggio
    int input; //variabile per prendere l'input dell'utente
    static int pause_check; //variabile statica per controllare se si verifica la pausa

    /*iniziazziamo la rana*/
    msg.oggetto = ID_RANA; 
    msg.tid = pthread_self();
    msg.on_croc = false;
    msg.croc_index = -1;
    msg.x = 0;
    msg.y = 0;
    msg.is_active = true;

    buffer_put(args->buffer, msg); //mettiamo il messaggio nel buffer

    pthread_mutex_lock(&screen_mutex); //accesso esclusivo al mutex di ncurses

    nodelay(stdscr, TRUE);
    timeout(0); 

    pthread_mutex_unlock(&screen_mutex); //rilasciamo il mutex di ncurses

    /*ciclo principale che scorre fino a qaundo non fiisce il gioco*/
    while (msg.is_active) {
        pause_check = 0; //resettiamo a 0 la variabile del controllo della pausa

        /*ogni 20 iterazioni controlliamo se il gioco è in pausa*/
        if (++pause_check % 20 == 0) {
            /*accesso eslcusivo al mutex globale, se il gioco è in pausa allora rimane bloccato fino a che handle_pause non lo rilascia, 
            altrimenti lo rilascia subito e continua il gioco*/
            pthread_mutex_lock(&pause_mutex);
            pthread_mutex_unlock(&pause_mutex);
        }
        
        usleep(5000); 
        
        pthread_mutex_lock(&screen_mutex); //accesso esclusivo al mutex di ncurses
        input = wgetch(stdscr); //legge l'input non bloccante dell'utente
        pthread_mutex_unlock(&screen_mutex); //rilasciamo il mutex di ncurses
        
        /*switch per gestire l'input dell'utente*/
        switch (input) {
            /*se ha premuto il tasto per andare su*/
            case KEY_UP:
                /*impostiamo il movimento, riassegniamo l'oggetto ad ID rana e inviamo il messaggio*/
                msg.x = 0; msg.y = -3; 
                msg.oggetto = ID_RANA;
                buffer_put(args->buffer, msg);
                break;
            /*se ha premuto il tasto per andare giu*/
            case KEY_DOWN:
                /*impostiamo il movimento, riassegniamo l'oggetto ad ID rana e inviamo il messaggio*/
                msg.x = 0; msg.y = 3;
                msg.oggetto = ID_RANA;
                buffer_put(args->buffer, msg);
                break;
            /*se ha premuto il tasto per andare a sinistra*/
            case KEY_LEFT:
                /*impostiamo il movimento, riassegniamo l'oggetto ad ID rana e inviamo il messaggio*/
                msg.x = -3; msg.y = 0;
                msg.oggetto = ID_RANA;
                buffer_put(args->buffer, msg);
                break;
            /*se ha premuto il tasto per andare a destra*/
            case KEY_RIGHT:
                /*impostiamo il movimento, riassegniamo l'oggetto ad ID rana e inviamo il messaggio*/
                msg.x = 3; msg.y = 0;
                msg.oggetto = ID_RANA;
                buffer_put(args->buffer, msg);
                break;
            /*se ha premuto la barra dello spazio*/
            case ' ':
                /*non impostiamo movimento, assegniamo l'oggetto alla granata e inviamo il messaggio*/
                msg.x = 0; msg.y = 0;
                msg.oggetto = CREATE_GRENADE;
                buffer_put(args->buffer, msg);
                break;
            /*se ha premuto o p o P per la pausa*/
            case 'p':
            case 'P':
                /*non impostiamo movimento, assegniamo l'oggetto a pausa e inviamo il messaggio*/
                msg.x = 0; msg.y = 0;
                msg.oggetto = PAUSE;
                buffer_put(args->buffer, msg);
                break;
            case ERR: //se non prende nessun altro input
            default: //per tutti gli altri input
                break; //non fa nulla
        }
    }
    
    return NULL;
}