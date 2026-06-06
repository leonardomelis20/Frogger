#include "buffer_thread.h"

/**
 * fuznione per inizializzare il buffer
 * @param buffer puntatore al buffer da inizializzare
 */
void init_buffer (Shared_buffer* buffer) {
    buffer->head = 0;
    buffer->tail = 0;
    buffer->count = 0;
    
    /*inizializzazione dei semafori*/
    sem_init(&buffer->empty, 0, BUFFER_SIZE);
    sem_init(&buffer->full, 0, 0);
    
    /*inizializzazione del mutex*/
    pthread_mutex_init(&buffer->mutex, NULL);
}

/**
 * funzione per distruggere il buffer
 * @param buffer puntatore al buffer da eliminare
 */
void destroy_buffer (Shared_buffer* buffer) {
    sem_destroy(&buffer->empty);
    sem_destroy(&buffer->full);
    pthread_mutex_destroy(&buffer->mutex);
}

/**
 * funzione per mettere il messaggio nel buffer
 * @param buffer puntatore al buffer
 * @param msg messaggio da inserire
 */
void buffer_put(Shared_buffer* buffer, Messaggio msg) {
    sem_wait(&buffer->empty); //attendiamo che ci sia uno slot vuoto

    pthread_mutex_lock(&buffer->mutex); //accediamo in modo esclusivo al buffer
    
    /*inseriamo il messaggio nel buffer*/
    buffer->buffer[buffer->head] = msg;
    buffer->head = (buffer->head + 1) % BUFFER_SIZE;
    buffer->count++;
    
    pthread_mutex_unlock(&buffer->mutex); //rilasciamo il mutex
    
    sem_post(&buffer->full); //avvisiamo che è presnete un nuovo elemento nel buffer 
}

/**
 * funzione bloccante per leggere il messaggio dal buffer
 * @param buffer puntatore al buffer
 * @return messaggio estratto dal buffer
 */
Messaggio buffer_get(Shared_buffer* buffer) {
    Messaggio msg; //variabile per il messaggio da leggere
    
    sem_wait(&buffer->full); //aspettiamo che ci sia un elemento nel buffer
    
    pthread_mutex_lock(&buffer->mutex); //accediamo in modo esclusivo al buffer
    
    /*estraiamo il messaggio dal buffer*/
    msg = buffer->buffer[buffer->tail];
    buffer->tail = (buffer->tail + 1) % BUFFER_SIZE;
    buffer->count--;
    
    pthread_mutex_unlock(&buffer->mutex); //rilasciamo il mutex 
    
    sem_post(&buffer->empty); //avvisiamo che è pvo slot vuoto 
    
    return msg; //restituiamo il messaggio
}

/**
 * funzione per inizializzare gioco 
 * @param game_data puntatore alla struttura del gioco
 */
void init_game_data(Game_data* game_data) {
    /*inzializziamo il buffer*/
    game_data->buffer = malloc(sizeof(Shared_buffer));
    init_buffer(game_data->buffer);
    
    /*inizializzaziamo i mutex*/
    pthread_mutex_init(&game_data->game_mutex, NULL);
    pthread_mutex_init(&game_data->frog_mutex, NULL);
    pthread_mutex_init(&game_data->bullets_mutex, NULL);
    pthread_mutex_init(&game_data->grenades_mutex, NULL);
    pthread_mutex_init(&game_data->burrows_mutex, NULL);
   
    /*inizializziamo le variabili di gioco*/
    game_data->game_over = false;
    game_data->restart = true;
    game_data->paused = false;
    game_data->vite = 5;
    game_data->score = 0;
    game_data->manche = 1;
    game_data->count_burrows = 0;
    
    /*inizializziamo i flag delle tane*/
    for (int i = 0; i <= NUM_BURROWS; i++) {
        game_data->flag[i] = false;
    }
    
    /*inizializziamo l'array dei coccodrilli*/
    for (int i = 0; i < NUM_CROC; i++) {
        game_data->coccodrilli[i].index = i;
        game_data->coccodrilli[i].x = -100;
        game_data->coccodrilli[i].y = -100;
        game_data->coccodrilli[i].is_active = false;
        game_data->coccodrilli[i].tid = 0;
    }
    
    /*inizializziamo l'array dei proiettili*/
        for (int i = 0; i < MAX_BULLETS; i++) {
        game_data->active_bullets[i].oggetto = ID_BULLET;
        game_data->active_bullets[i].is_active = false;
        game_data->active_bullets[i].index = i;
        game_data->active_bullets[i].x = -100;
        game_data->active_bullets[i].y = -100;
        game_data->active_bullets[i].tid = 0;
    }

    /*inizializziamo l'array delle granate*/
    for (int i = 0; i < MAX_GRENADE; i++) {
        game_data->active_grenades[i].oggetto = ID_GRENADE;
        game_data->active_grenades[i].is_active = false;
        game_data->active_grenades[i].index = i;
        game_data->active_grenades[i].x = -100;
        game_data->active_grenades[i].y = -100;
        game_data->active_grenades[i].tid = 0;
    }
    
    /*inizalizziamo l'array delle tane*/
    for (int i = 0; i < NUM_BURROWS; i++) {
        game_data->closed_burrows[i].index = -1;
        game_data->closed_burrows[i].oggetto = ID_BURROWS;
    }
    
    /*inizializziamo la rana*/
    int centro_y = MAP_HEIGHT - ALTEZZA_RANA;
    int centro_x = GAME_WIDTH / 2;
    
    game_data->frog_copy.oggetto = ID_RANA;
    game_data->frog_copy.x = centro_x;
    game_data->frog_copy.y = centro_y;
    game_data->frog_copy.on_croc = false;
    game_data->frog_copy.croc_index = -1;
    game_data->frog_copy.tid = 0;
    game_data->frog_copy.is_active = true;
}

/**
 * funzione per la pulizia del gioco
 * @param game_data puntatore alla struttura del gioco
 */
void destroy_game_data(Game_data* game_data) {
    /*se il puntatore è diversp da NULL distruggiamo e liberiamo il buffer*/
    if (game_data->buffer) {
        destroy_buffer(game_data->buffer);
        free(game_data->buffer);
    }
    
    /*distruggiamo i mutex*/
    pthread_mutex_destroy(&game_data->game_mutex);
    pthread_mutex_destroy(&game_data->frog_mutex);
    pthread_mutex_destroy(&game_data->bullets_mutex);
    pthread_mutex_destroy(&game_data->grenades_mutex);
    pthread_mutex_destroy(&game_data->burrows_mutex);
 
}

/**
 * funzione non bloccante per leggere il messaggio dal buffer
 * @param buffer puntatore al buffer
 * @param msg messaggio da leggere
 * @return booleano per dire se la lettura del messaggio è avvenuta con successo o no
 */
bool buffer_try_get(Shared_buffer* buffer, Messaggio* msg) {
    /*se il buffer è vuoto restituiamo false*/
    if (sem_trywait(&buffer->full) != 0) {
        return false;
    }
    
    /*altrimenti*/
    pthread_mutex_lock(&buffer->mutex); //accediamo in modo esclusivo al buffer
    
    /*estraiamo il messaggio dal buffer*/
    *msg = buffer->buffer[buffer->tail];
    buffer->tail = (buffer->tail + 1) % BUFFER_SIZE;
    buffer->count--;
    
    pthread_mutex_unlock(&buffer->mutex); //rilasciamo il mutex

    sem_post(&buffer->empty); //avvisiamo che è presente un nuovo elemento nel buffer

    return true; //restituiamo true per dire che la lettura del messaggio è avvenuta con successo
}