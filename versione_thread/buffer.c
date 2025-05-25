#include "buffer.h"

void init_buffer (Circular_buffer* buffer) {
    buffer->head = 0; 
    buffer->tail = 0; 
    buffer->count = 0; 

    /*inizializziamo mutex e semafori*/
    if (pthread_mutex_init(&buffer->mutex, NULL) != 0) {
        perror("errore inizializzazione"); 
        exit(EXIT_FAILURE); 
    }

    /*inizializzazione semaforo slot vuoti*/
    if (sem_init(&buffer->empty_slots, 0, BUFFER_SIZE) != 0) {
        perror("errore inizializzazione"); 
        exit(EXIT_FAILURE); 
    }

    /*inizializzazione semaforo slot pieni*/
    if (sem_init(&buffer->full_slots, 0, BUFFER_SIZE) != 0) {
        perror("errore inizializzazione"); 
        exit(EXIT_FAILURE); 
    }
}