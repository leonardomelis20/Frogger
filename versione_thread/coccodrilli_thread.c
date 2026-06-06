#include "coccodrilli_thread.h"

/**
 * funzione che aggiorna la posizione del coccodrillo in base alla sua direzione
 * @param croc puntatore alla struct Messaggio che contiene le informazioni sul coccodrillo
 */
void movement_croc (Messaggio* croc) {
    croc->x += croc->direzione;
}

/**
 * funzione che controlla se il coccodrillo ha superato i bordi dello schermo
 * @param croc struct Messaggio che contiene le informazioni sul coccodrillo
 * @return true se il coccodrillo è uscito dai bordi, false altrimenti
 */
bool check_borders (Messaggio croc) {
    /*se la direzione del coccodrillo è verso destra*/
    if(croc.direzione == 1) {
        return (croc.x >= GAME_WIDTH); //ritorniamo true se è fuori dallo schermo o false altrimenti 
    } 
    /*se la direzione è verso sinistra*/
    else {
        return (croc.x <= -LARGHEZZA_COCCODRILLO); //ritorniamo true se è fuori dallo schermo o false altrimenti 
    }
}

/**
 * funzione principale per la gestione del thread coccodrillo
 * @param arg puntatore ai parametri del thread
 * @return NULL
 */
void* crocodile_thread (void* arg) {
    Thread_args* args = (Thread_args*)arg; //per accedere ai parametri del thread
    Messaggio msg; //messaggio 
    int adjusted_num; //per calcolare l'indice corretto 
    bool shooting = false; //per controllare che il coccodrillo stia sparando o no (false se non sta sparando, true se si)
    
    srand(time(NULL) ^ (unsigned int)pthread_self()); 

    /*se flag è false */
    if(!args->flag) {  
        usleep(5000000); //facciamo aspettare 5 secondi per far spawnare i coccodrilli
    }

    /*se l'indice del coccodrillo è maggiore di 9*/
    if (args->index >= 9) {
        adjusted_num = args->index - 9; //settiamo l'indice uguale a quello corrispondente del primo gruppo di coccodrilli 
    } 
    /*se non è maggiore di 9*/
    else {
        adjusted_num = args->index; //lasciamo l'indice invariato
    }

    /*inizializziamo il messaggio*/
    msg.index = args->index;
    msg.tid = pthread_self();
    msg.oggetto = ID_CROCODILE;
    msg.direzione = args->direzione;
    msg.y = 6 + (adjusted_num * 3);
    
    if (msg.direzione == 1) {
        msg.x = 0;
    } 
    else {
        msg.x = GAME_WIDTH - LARGHEZZA_COCCODRILLO;
    }
    
    msg.velocita = args->velocita;
    msg.is_active = true;
    msg.carry = (rand() % 100 < 70);
    
    usleep(50000); 

    buffer_put(args->buffer, msg); //mettiamo il messaggio nel buffer per comunicare lo stato iniziale

    /*ciclo principale che cicla fino a quando non finisce il gioco*/
    while(msg.is_active) {
        /*accesso eslcusivo al mutex globale, se il gioco è in pausa allora rimane bloccato fino a che handle_pause non lo rilascia, 
        altrimenti lo rilascia subito e continua il gioco*/
        pthread_mutex_lock(&pause_mutex);
        pthread_mutex_unlock(&pause_mutex);

        movement_croc(&msg); //movimento del coccodrillo

        /*i coccodrilli hanno una probabilità del 3% di sparare*/
        if (rand() % 100 < 3) {
            msg.is_shooting = true; //settiamo a true per dire che sta sparando 
            msg.oggetto = CREATE_BULLET; //settiamo l'oggetto all'ID del proiettile 

            /*se non coccodrillo non ha ancora sparato*/
            if(shooting == false) {
                shooting = true; //settuamo a true
                
                buffer_put(args->buffer, msg); //mettiamo il messaggio nel buffer
            }

            msg.oggetto = ID_CROCODILE; //risettiamo l'oggetto all'ID del coccodrillo 
        }

        /*controlliamo quando i cocccodrilli devono respawnare*/
        if (check_borders(msg)) {

            msg.oggetto = RESPAWN; //settiamo l'oggetto all'ID respawn
            
            buffer_put(args->buffer, msg); //mettiamo il messaggio nel buffer
            
            break;
        } 

        buffer_put(args->buffer, msg); //mettiamo il messaggio nel buffer
        usleep(msg.velocita);
    }
    
    return NULL;
}