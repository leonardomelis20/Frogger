#include "proiettili_thread.h"

char bullet = 'P'; //carattere del proiettile

/**
 * funzione per aggiornare il movimento del proiettile
 * @param croc puntatore alla struct Messaggio del coccodrillo che ha sparato
 * @param bullet puntatore alla struct Messaggio del proiettile
 */
void movement_bullet (Messaggio *croc, Messaggio *bullet) {
    bullet->x += croc->direzione;
}

/**
 * funzione per controllare se un proiettile è uscito dai bordi dello schermo
 * @param bullet struct Messaggio contenente le informazioni del proiettile
 * @return true se è uscito dai bordi, false altrimenti
 */
bool check_bullet_borders (Messaggio bullet) {
    return (bullet.x < 0 || bullet.x >= GAME_WIDTH);
}

/**
 * funzione principale per la gestione del thread proiettile
 * @param arg puntatore ai parametri del thread
 * @return NULL
 */
void* bullet_thread (void* arg) {
    Bullet_params* params = (Bullet_params*)arg;//per convertire il parametro generico in tipo Bullet_params*
    Thread_args* args = params->args; //per prendere il campo args dalla struttura
    Messaggio* bullet_info = params->bullet_info; //per prendere le info del proiettile
    Messaggio bullets; //rappresenta il proiettile da gestire
    
    /*inizializziamo il proiettile*/
    bullets.is_active = true;
    bullets.velocita = BULLET_SPEED;
    bullets.direzione = bullet_info->direzione;
    bullets.index = bullet_info->index;
    bullets.oggetto = ID_BULLET;
    bullets.tid = pthread_self();
    
    /*se il proiettile va verso destra*/
    if (bullets.direzione == 1) {
        bullets.x = bullet_info->x + LARGHEZZA_COCCODRILLO; //assegniamo la partenza verso la destra del coccodrillo
    } 
    /*se invece va verso sinistra*/
    else {
        bullets.x = bullet_info->x - 1; //assegniamo la partenza verso sinistra rispetto al coccodrillo
    }
    
    bullets.y = bullet_info->y + 1; //centriamo il coccodrillo 
    
    /*ciclo while che continua fino a che ci sono proiettili attivi*/
    while(bullets.is_active) {
        /*accesso eslcusivo al mutex globale, se il gioco è in pausa allora rimane bloccato fino a che handle_pause non lo rilascia, 
        altrimenti lo rilascia subito e continua il gioco*/
        pthread_mutex_lock(&pause_mutex); 
        pthread_mutex_unlock(&pause_mutex); 
        
        bullets.x += bullets.direzione; //aggiorniamo la posizione del proiettile

        /*controlliamo se il proiettile è uscito dai bordi*/
        if (bullets.x <= 0){
            bullets.x = 0;
            bullets.direzione = 1;
        } else if (bullets.x >= GAME_WIDTH -1){
            bullets.x = GAME_WIDTH -1;
            bullets.direzione = -1;
        }
        
        buffer_put(args->buffer, bullets); //mettiamo il messaggio nel buffer

        usleep(bullets.velocita);
    }
    
    /*liberiamo la memoria*/
    free(params->args);
    free(params->bullet_info);
    free(params);
    
    return NULL;
}