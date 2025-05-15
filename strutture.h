#ifndef STRUTTURE_H
#define STRUTTURE_H

/*macro area di gioco*/
#define GAME_WIDTH 86 //larghezza dell'area di gioco
#define GAME_HEIGHT 39 //altezza dell'area di gioco
#define SAFE_ZONE_DOWN 33
#define SAFE_ZONE_DOWN_2 39
#define SAFE_ZONE_UP 5

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

/*macro da usare nelle pipe per codice più leggibile*/
#define READ 0 
#define WRITE 1
#define RESPAWN -1
#define TANE -2
#define ID_RANA 1
#define ID_CROCODILE 2
#define ID_BULLET 3
#define CREATE_BULLET 4
#define CREATE_GRENADE 5
#define ID_GRENADE 6

enum oggetto {FROG, COCCODRILLO};

/*!!!!!!!*/
//possiamo valutare di usare una struttura per chiudere bene tutti i processi
typedef struct {
    pid_t pid_rana;
    pid_t pid_coccodrillo[NUM_STREAMS];
} Processi;
/*!!!!!!!*/

typedef struct {
    int speed;
    int direzione;
    int x_pos;
    int y_pos;
    int cont; //numero di coccodrilli nel flusso
} InfoFlussi;

typedef struct {
    int x;
    int y;
    int oggetto;
    int velocita;
    int index;
    int direzione;
    int pid;
    bool on_croc;
    int croc_index;
    bool is_shooting;
    bool is_active;
    int pos_array;
} Messaggio;

/*questa non la usiamo*/
typedef struct {
    int direzione; 
    int velocita; 
    pid_t pid; 
    int x; 
    int y; 
    int index;
} InfoCocc;

#endif