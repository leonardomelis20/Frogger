#include "buffer.h"

/**
 * funzione che inizializza il buffer 
 * @param buffer puntatore al buffer 
 */
void init_buffer (Circular_buffer* buffer) {
    buffer->head = 0; 
    buffer->tail = 0; 
    buffer->count = 0; 

    /*inizializziamo mutex e semafori*/
    if (pthread_mutex_init(&buffer->mutex, NULL) != 0) {
        perror("errore inizializzazione"); 
        exit(EXIT_FAILURE); 
    }

    /*inizializzaziamo semafori per slot vuoti*/
    if (sem_init(&buffer->empty_slots, 0, BUFFER_SIZE) != 0) {
        perror("errore inizializzazione"); 
        exit(EXIT_FAILURE); 
    }

    /*inizializziamo semafori per slot pieni*/
    if (sem_init(&buffer->full_slots, 0, 0) != 0) {
        perror("errore inizializzazione"); 
        exit(EXIT_FAILURE); 
    }
}

/**
 * funzione che distrugge il buffer e libera le risorse 
 * @param buffer puntatore al buffer da distruggere
 */
void destroy_buffer(Circular_buffer* buffer) {
    pthread_mutex_destroy(&buffer->mutex); 
    sem_destroy(&buffer->empty_slots); 
    sem_destroy(&buffer->full_slots); 
}

/**
 * funzione che un messaggio nel buffer
 * @param buffer puntatore al buffer 
 * @param msg messaggio da mandare
 * @return 0 in caso di successo, -1 in caso di errore
 */
int produce_msg(Circular_buffer* buffer, Messaggio msg) {
    /*attendiamo uno slot vuoto*/
    if (sem_wait(&buffer->empty_slots) != 0) {
        if (errno == EINTR) {
            return 0; //interrompiamo il segnale
        }
        return -1; 
    }

    /*acquisiamo il mutex per l'accesso esclusivo nel buffer*/
    if (pthread_mutex_lock(&buffer->mutex) != 0) {
        sem_post(&buffer->empty_slots); //rilasciamo il semaforo in caso di errore
        
        return -1; 
    }

    /*inseriamo il messaggio al buffer*/
    buffer->buffer[buffer->head] = msg; 
    buffer->head = (buffer->head + 1) % BUFFER_SIZE; 
    buffer->count++; 

    /*rilasciamo il mutex*/
    pthread_mutex_unlock(&buffer->mutex); 

    /*segnaliamo che è disponibile un nuovo elemento*/
    sem_post(&buffer->full_slots); 

    return 0; 
}

/**
 * funzione che 
 * @param buffer
 * @param msg 
 * @return 0 in caso di successo, -1 in caso di errore
 */
int consume_msg(Circular_buffer* buffer, Messaggio* msg) {
    /*attendiamo un elemento disponibile*/
    if (sem_wait(&buffer->full_slots) != 0) {
        if (errno == EINTR) {
            return 0; //interrompiamo il segnale
        }   
        return -1; 
    }

    /*acquisiamo il mutex per l'accesso esclusivo al buffer*/
    if (pthread_mutex_lock(&buffer->mutex) != 0) {
        sem_post(&buffer->full_slots); //rilasciamo il semaforo in caso di errore
        
        return -1; 
    }

    /*leggiamo il messaggio dal buffer*/
    *msg = buffer->buffer[buffer->tail]; 
    buffer->tail = (buffer->tail + 1) % BUFFER_SIZE; 
    buffer->count--; 

    /*rilasciamo il mutex*/
    pthread_mutex_unlock(&buffer->mutex); 

    /*segnaliamo che è disponibile un nuovo slot vuoto*/
    sem_post(&buffer->empty_slots); 

    return 0; 
}