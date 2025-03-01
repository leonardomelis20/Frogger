#ifndef STRUTTURE_H
#define STRUTTURE_H

#define GAME_WIDTH 80  // Larghezza dell'area di gioco
#define GAME_HEIGHT 33 // Altezza dell'area di gioco
#define LARGHEZZA_RANA 5
#define ALTEZZA_RANA 3
#define LARGHEZZA_COCCODRILLO 11
#define ALTEZZA_COCCODRILLO 3
#define NUM_STREAMS 8
#define NUM_BURROWS 5

enum oggetto {FROG, COCCODRILLO};

typedef struct {
    int x;
    int y;
    int oggetto;
} Messaggio;

#endif 