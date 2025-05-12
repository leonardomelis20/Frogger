#ifndef GRANATE_H
#define GRANATE_H

void draw_grenade(int x, int y);
void clear_grenade(int x, int y);
void movement_grenade_left(Messaggio *grenade);
void movement_grenade_right(Messaggio *grenade);
bool check_grenade_borders(Messaggio grenade);
void main_grenade(int pipe_fd, Messaggio grenade);

#endif