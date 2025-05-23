#ifndef DISEGNI_H
#define DISEGNI_H

#include <ncurses.h>
#include <stdlib.h>

#include "strutture.h"


void draw_river();
void draw_burrows();
void draw_closed_burrows(Messaggio tana);
void draw_safety_zones();

#endif