#ifndef STRUTTURE_H
#define STRUTTURE_H

#define GAME_WIDTH 85  // Larghezza dell'area di gioco
#define GAME_HEIGHT 39 // Altezza dell'area di gioco da fare da 30
#define LARGHEZZA_RANA 5
#define ALTEZZA_RANA 3 //usata sia per rana che per altezza tane
#define LARGHEZZA_COCCODRILLO 11
#define ALTEZZA_COCCODRILLO 3
#define NUM_STREAMS 9
#define NUM_BURROWS 5
#define MIN_COCCODRILLO 1
#define MAX_COCCODRILLO 100 
#define SAFE_ZONE_START 33
#define SAFE_ZONE_END 5
#define LARGHEZZA_TANA 7
#define MIN_VELOCITA 100000
#define MAX_VELOCITA 1000000


enum oggetto {FROG, COCCODRILLO};



//possiamo valutare di usare una struttura per chiudere bene tutti i processi
typedef struct{
    pid_t pid_rana;
    pid_t pid_coccodrillo[NUM_STREAMS];
} Processi;

typedef struct {
    int x;
    int y;
    int oggetto;
    int velocita;
} Messaggio;

#endif 