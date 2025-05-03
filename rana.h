#ifndef RANA_H
#define RANA_H




void frog(int pipe_fd, bool*, int*);
bool is_inside (Messaggio);
void tane(Messaggio);
int num_tana(Messaggio);
void start_frog(int pipe_fd);
void draw_frog(int x, int y); // Nuova funzione per disegnare la rana
void clear_frog(int x, int y);

#endif
