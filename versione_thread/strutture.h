#ifndef STRUTTURE_H
#define STRUTTURE_H

#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>

#define VITE 5

/*macro area di gioco*/
#define GAME_WIDTH 86 //larghezza dell'area di gioco
#define GAME_HEIGHT 45//altezza dell'area di gioco
#define SAFE_ZONE_DOWN 33
#define SAFE_ZONE_DOWN_2 39
#define SAFE_ZONE_UP 5
#define MAP_HEIGHT 39

/*macro rana*/
#define LARGHEZZA_RANA 5 
#define ALTEZZA_RANA 3 //usata sia per rana che per altezza tane 

/*macro coccodrilli e flussi del fiume*/
#define LARGHEZZA_COCCODRILLO 15
#define ALTEZZA_COCCODRILLO 3
#define NUM_STREAMS 9 //numero dei flussi 
#define NUM_CROC 18 //numero totale dei coccodrilli
#define COCCODRILLI_X_FLUSSO 1 
#define MIN_COCCODRILLO 1
#define MAX_COCCODRILLO 100 

/*macro tane*/
/*!!!!!!!*/
//fare una define apposita solo per l'altezza delle tane 
/*!!!!!!!*/
#define NUM_BURROWS 5
#define LARGHEZZA_TANA 7

/*macro proiettili*/
#define BULLET_SPEED 50000
#define MAX_BULLETS 100 //numero massimo di proiettili attivi contemporaneamente

/*macro grante*/
#define GRENADE_SPEED 100000 
#define MAX_GRENADE 100 //numero massimo di granate attive contemporaneamente

#define TIMER_BAR_WIDTH 32
#define TIMER_BAR_HEIGHT 3
#define TIMER_BAR_Y 39
#define TIMER_BAR_X 1

/*macro da usare nel buffer per codice più leggibile*/
#define RESPAWN -1
#define TANE -2
#define ID_RANA 1
#define ID_CROCODILE 2
#define ID_BULLET 3
#define CREATE_BULLET 4
#define CREATE_GRENADE 5
#define ID_GRENADE 6
#define PAUSE 7
#define ID_BURROWS 8


// Costanti specifiche per il timer
#define TIMER_DURATION 30 // Timer di 30 secondi
#define ID_TIMER 7 // ID per il messaggio del timer
#define TIMER_TIMEOUT 8 // ID per il messaggio di timeout del timer

#define POINT_GRENADE 50
#define POINT_BURROWS 500
#define POINT_BULLETS (-25)
#define POINT_WATER (-200)
#define POINT_TIME (-500)
#define POINT_TAKEN_BURROWS (-1000)

#define BUFFER_SIZE 1000

enum oggetto {FROG, COCCODRILLO};


typedef struct {
    int x;
    int y;
    int oggetto;
    int velocita;
    int index;
    int direzione;
    pthread_t tid;
    bool on_croc;
    int croc_index;
    bool is_shooting;
    bool is_active;
    int pos_array;
    int tempo_rimanente; 
} Messaggio;

typedef struct {
    Messaggio buffer[BUFFER_SIZE]; 
    int head ; //indice di scrittura 
    int tail; //indice di lettura 
    int count; //contatore elementi

    pthread_mutex_t mutex; 
    sem_t empty_slots; //semaforo per slot vuoti 
    sem_t full_slots; //semaforo per slot pieni
} Circular_buffer; 

// Struttura per tenere traccia dello stato del timer
typedef struct {
    int seconds_left;     // Secondi rimanenti
    time_t last_update;   // Ultimo aggiornamento
    bool is_active;       // Indica se il timer è attivo
} Info_timer;

/*!!!!
prima si chiamava Crocs_arg
!!!!*/
/*struttura per i parametri dei thread*/
typedef struct {
    Circular_buffer* buffer; 
    int tid; 
    int direzione; 
    int speed; 
    bool flag; 
} Crocs_arg;

typedef struct {
    Circular_buffer* buffer; 
    bool* flag_array; 
    int* speed_array; 
} Frog_arg;

typedef struct {
    Circular_buffer* buffer; 
    Messaggio copy; 
} Bullets_arg;

typedef struct {
    Circular_buffer* buffer; 
    Messaggio copy;
} Grenade_arg;

#endif