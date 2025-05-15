#ifndef COLLISIONI_H
#define COLLISIONI_H

#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "strutture.h"
#include "proiettili.h"
#include "granate.h"

int check_collision(Messaggio frog, Messaggio* croc); //prima era bool check_collision(int, Messaggio frog, Messaggio* croc); 
Messaggio correct_x_frog (Messaggio frog);
void frog_with_croc(Messaggio* frog, Messaggio* croc);
bool check_borders_frog(Messaggio frog);
bool check_borders_bullet(Messaggio bullet);
bool check_safe_zone(Messaggio frog);
bool river(Messaggio frog);
bool collision_b_g(Messaggio* bullet, Messaggio* grenade, int count_bullet, int count_grenade);

#endif