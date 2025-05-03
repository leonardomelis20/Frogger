#ifndef COLLISIONI_H
#define COLLISIONI_H

bool check_collision(int, Messaggio frog, Messaggio* croc);
void frog_with_croc(Messaggio* frog, Messaggio* croc);
bool check_borders_frog(Messaggio frog);
bool check_borders_bullet(Messaggio bullet);
bool check_safe_zone(Messaggio frog);
bool river(Messaggio frog);
Messaggio correct_x_frog (Messaggio frog);

#endif