#ifndef STRUTTURE_H
#define STRUTTURE_H

#define SCREEN_WIDTH 50
#define SCREEN_HEIGHT 20

enum oggetto {FROG, COCCODRILLO};

typedef struct {
    int x;
    int y;
    int oggetto;
} Messaggio;

#endif 