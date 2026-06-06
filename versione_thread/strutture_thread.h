#ifndef STRUTTURE_THREAD_H
#define STRUTTURE_THREAD_H

#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>

/*macro area di gioco*/
#define GAME_WIDTH 86 
#define GAME_HEIGHT 45
#define SAFE_ZONE_DOWN 33
#define SAFE_ZONE_DOWN_2 39
#define SAFE_ZONE_UP 5
#define MAP_HEIGHT 39
#define ALTEZZA_SPRITE 6
#define ALTEZZA_SPRITE_V 6


/*macro rana*/
#define LARGHEZZA_RANA 5 
#define ALTEZZA_RANA 3 

/*macro coccodrilli e flussi del fiume*/
#define LARGHEZZA_COCCODRILLO 15
#define ALTEZZA_COCCODRILLO 3
#define NUM_STREAMS 9 
#define NUM_CROC 18 
#define COCCODRILLI_X_FLUSSO 1 
#define MIN_COCCODRILLO 1
#define MAX_COCCODRILLO 100 

/*macro tane*/
#define NUM_BURROWS 5
#define LARGHEZZA_TANA 7

/*macro proiettili*/
#define BULLET_SPEED 50000
#define MAX_BULLETS 100 

/*macro granate*/
#define GRENADE_SPEED 100000 
#define MAX_GRENADE 100 

/*macro per buffer*/
#define BUFFER_SIZE 1024 

/*macro per identificare oggetti*/
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

/*macro tempo e vite*/
#define TIMER_BAR_WIDTH 32
#define TIMER_BAR_HEIGHT 3
#define TIMER_BAR_Y 39
#define TIMER_BAR_X 1
#define TIMER_DURATION 30 
#define ID_TIMER 7 
#define TIMER_TIMEOUT 8 

/*macro punteggi*/
#define POINT_GRENADE 50
#define POINT_BURROWS 500
#define POINT_BULLETS (-25)
#define POINT_WATER (-200)
#define POINT_TIME (-500)
#define POINT_TAKEN_BURROWS (-1000)

/*macro vite*/
#define VITE 5 

/*struttura contente i dati dei thread*/
typedef struct {
    int x;
    int y;
    int oggetto;
    int velocita;
    int index;
    int direzione;
    pthread_t tid;
    bool on_croc; //booleano per controllare se la rana è su un coccodrillo 
    int croc_index; //indice dei coccodrilli 
    bool is_shooting; //booleano per controllare se i coccodrilli sparano proiettili
    bool is_active; //booleano per controllare se proiettili e granate sono attivi 
    bool carry;
} Messaggio;

/*struttura per lo stato del timer*/
typedef struct {
    int seconds_left;     
    time_t last_update;   
    bool is_active;       
} Info_timer;

/*struttura per il buffer*/
typedef struct {
    Messaggio buffer[BUFFER_SIZE]; //array circolare di messaggi 
    int head; //indice per scrivere in testa
    int tail; //indice per leggere in coda 
    int count; //contatore per il numero di elementi nel buffer
    sem_t empty; //semaforo che conta gli slot disponibili 
    sem_t full; //semaforo che conta gli slot pieni 
    pthread_mutex_t mutex; //mutex 
} Shared_buffer;


/*struttura per passare i parametri ai thread*/
typedef struct {
    Shared_buffer* buffer; //puntatore al buffer condiviso 
    int index; 
    int direzione;
    int velocita;
    bool flag; //flag per controllare quando far spawnare i coccodrilli 
    bool* game_flags; //puntatore all'array delle flag delle tane 
    int* speed_array; //puntatore all'array delle velocita
} Thread_args;

/*struttura per passare argomenti e messaggi dei proiettili*/
typedef struct {
    Thread_args* args; 
    Messaggio* bullet_info;
 } Bullet_params;

/*struttura per passare argomenti e messaggi delle grante*/
typedef struct {
    Thread_args* args;
    Messaggio* grenade_info;
} Grenade_params;

/*struttura per i dati condivisi nel gioco*/
typedef struct {
    Shared_buffer* buffer; //puntatore al buffer condiviso 

    /*stati correnti di tutti gli oggetti nel gioco */
    Messaggio coccodrilli[NUM_CROC]; 
    Messaggio active_bullets[MAX_BULLETS];
    Messaggio active_grenades[MAX_GRENADE];
    Messaggio closed_burrows[NUM_BURROWS];
    Messaggio frog_copy; //copia della rana per i thread dei coccodrilli 

    /*flag di controllo*/
    bool game_over; //booleano per controllare se il gioco è terminato 
    bool restart; //booleano per controllare se il gioco deve ripartire 
    bool paused; //booleano per contorllare se il gioco è in pausa 
    
    /*variabili di gioco generali*/
    int vite;
    int score;
    int manche;
    int count_burrows;
    bool flag[NUM_BURROWS+1];

    Info_timer game_timer; //timer
    
    /*mutex per gli oggetti nel gioco*/
    pthread_mutex_t game_mutex;
    pthread_mutex_t frog_mutex;
    pthread_mutex_t bullets_mutex;
    pthread_mutex_t grenades_mutex;
    pthread_mutex_t burrows_mutex;
} Game_data;

/*variabili globali*/
extern pthread_mutex_t screen_mutex; //per ncurses 
extern pthread_mutex_t pause_mutex; //per comunicare che il gioco è in pausa 

#endif