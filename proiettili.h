#ifndef PROIETTILI_H
#define PROIETTILI_H


void draw_bullet(int x, int y);
void clear_bullet(int x, int y);
void movement_bullet(Messaggio * croc, Messaggio *bullet);
void main_bullet(int pipe_fd, Messaggio croc);

#endif