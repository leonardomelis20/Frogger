#ifndef COCCODRILLI_H
#define COCCODRILLI_H

#define ID_CROCODILE 2

int main_croc(int, int, int, int, bool);
void draw_crocodile(int , int ); 
int get_pid_croc(Messaggio*, pid_t );
void inizializza_coccodrilli(int*, InfoCocc*);
void movement_croc(Messaggio*);
bool check_borders(Messaggio);
void clear_croc(Messaggio);

#endif