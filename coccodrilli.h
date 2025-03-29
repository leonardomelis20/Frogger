#ifndef COCCODRILLI_H
#define COCCODRILLI_H

#define ID_CROCODILE 2


int main_croc(int*, InfoCocc*, Messaggio);
void draw_crocodile(int , int ); 
int get_pid_croc(InfoCocc*, pid_t );
void inizializza_coccodrilli(int*, InfoCocc*);
void movement_croc(InfoCocc*);
bool check_borders(InfoCocc*);
void clear_croc(InfoCocc);

#endif