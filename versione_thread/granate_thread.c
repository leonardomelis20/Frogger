#include "granate_thread.h"

char grenade = 'G'; //carattere delle granate

/**
 * funzione che aggiorna la posizione della granata spostandola verso sinistra
 * @param grenade puntatore alla struct Messaggio che contiene le informazioni sulla granata
 */
void movement_grenade_left (Messaggio* grenade) {
    grenade->x -= 1;
}

/**
 * funzione che aggiorna la posizione della granata spostandola verso destra
 * @param grenade puntatore alla struct Messaggio che contiene le informazioni sulla granata
 */
void movement_grenade_right (Messaggio* grenade) {
    grenade->x += 1;
}

/**
 * funzione che controlla se la granata ha toccato i bordi dell'area di gioco
 * @param grenade struct Messaggio contenente le informazioni sulla granata
 * @return true se la granata è al di fuori dei bordi, false altrimenti
 */
bool check_grenade_borders (Messaggio grenade) {
    return (grenade.x <= 0 || grenade.x >= GAME_WIDTH);
}

/**
 * funzione principale per la gestione del thread granate
 * @param arg puntatore ai parametri del thread
 * @return NULL
 */
void* grenade_thread (void* arg) {
    Grenade_params* params = (Grenade_params*)arg; //per convertire il parametro generico in tipo Grenade_params*
    Thread_args* args = params->args; //per prendere il campo args dalla struttura 
    Messaggio* grenade_info = params->grenade_info; //per prendere le info della granata
    Messaggio grenade = *grenade_info; //copia dei dati della granata 
    
    /*se la direzione della granata è verso destra*/
    if (grenade.direzione == 1) {
        grenade.x = grenade.x + 2; //aumentiamo la posizione per renderla uguale a quella che va verso sinistra 
    }

    /*inizializziamo la granata*/
    grenade.is_active = true;
    grenade.velocita = GRENADE_SPEED;
    grenade.oggetto = ID_GRENADE;
    grenade.tid = pthread_self();

    buffer_put(args->buffer, grenade); //mettiamo la granata nel buffer  

    /*ciclo while che continua fino a quando la granata è attiva*/
    while(grenade.is_active) {
        /*accesso eslcusivo al mutex globale, se il gioco è in pausa allora rimane bloccato fino a che handle_pause non lo rilascia, 
        altrimenti lo rilascia subito e continua il gioco*/
        pthread_mutex_lock(&pause_mutex); 
        pthread_mutex_unlock(&pause_mutex); 

        /*se la direzione della granata è verso destra*/
        if (grenade.direzione == 1) {
            grenade.x += 2; //facciamo muovere la granata di 2 verso destra
        } 
        /*se la direzione va verso sinistra*/
        else {
            grenade.x -= 2; //facciamo muovere la granata di 2 verso sinistra
        }
       
        /*se la granata va verso destra*/
        if (grenade.direzione == 1) {
            movement_grenade_right(&grenade); //richiamiamo la funzione per effettiamente far muovere la granata verso sinistra
        } 
        /*se la granata va verso sinistra*/
        else {
            movement_grenade_left(&grenade); //richiamiamo la funzione per far muovere effettivamente la granata verso sinistra
        }
    
        /*se la granata ha toccato il bordo*/
        if (check_grenade_borders(grenade)) {
            grenade.is_active = false; //disattiviamo la granata 
            
            buffer_put(args->buffer, grenade); //mettiamo il messaggio nel buffer
            
            break; 
        }

        buffer_put(args->buffer, grenade); //mettiamo il messaggio nel buffer

        usleep(GRENADE_SPEED);
    }
    
    /*liberiamo la memoria*/
    free(params->args);
    free(params->grenade_info);
    free(params);
    
    return NULL;
}