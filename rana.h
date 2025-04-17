#ifndef RANA_H
#define RANA_H

#define ID_RANA 1


void frog(int pipe_fd, bool*);
bool is_inside (Messaggio);
void tane(Messaggio);
int num_tana(Messaggio);
void start_frog(int pipe_fd);
void draw_frog(int x, int y); // Nuova funzione per disegnare la rana
void clear_frog(int x, int y);

#endif
