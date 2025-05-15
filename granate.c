#include "granate.h"

char grenade = 'G'; //definizione del carattere che rappresenta la granata 

/**
 * funzione che disegna la granata alla posizione (x, y)
 * @param x coordinata orizzontale della granata
 * @param y coordinata verticale della granata
 */
void draw_grenade(int x, int y) {

    attron(COLOR_PAIR(5)); //attiviamo il colore rosso per la granata
    mvaddch(y, x, grenade); //usiamo la funzione ncurses per disegnare la granata nella coordinata (x,y)
    attroff(COLOR_PAIR(5)); //disattiviamo il colore rosso
}

/**
 * funzione che cancella la granata dalla posizione (x, y) sostituendola con uno spazio
 * @param x coordinata orizzontale della granata
 * @param y coordinata verticale della granata
 */
void clear_grenade(int x, int y) {
    mvaddch(y, x, ' '); //usiamo la funzione ncurses per disegnare lo spazio vuoto nella coordinata (x,y)
}

/**
 * funzione che aggiorna la posizione della granata spostandola verso sinistra
 * @param grenade puntatore alla struct Messaggio che contiene le informazioni sulla granata
 */
void movement_grenade_left(Messaggio* grenade) {
    grenade->x -= 1; //decrementiamo la coordinata x per muovere la granata verso sinistra
}

/**
 * funzione che aggiorna la posizione della granata spostandola verso destra
 * @param grenade puntatore alla struct Messaggio che contiene le informazioni sulla granata
 */
void movement_grenade_right(Messaggio* grenade) {
    grenade->x += 1; //incrementiamo la coordinata x per muovere la granata verso destra
}

/**
 * funzione che controlla se la granata ha toccato i bordi dell'area di gioco
 * @param grenade struct Messaggio contenente le informazioni sulla granata
 * @return true se la granata è al di fuori dei bordi, false altrimenti
 */
bool check_grenade_borders(Messaggio grenade) {
    return (grenade.x <= 0 || grenade.x >= GAME_WIDTH); //controlliamo se la coordinata x è fuori dai limiti dell'area di gioco
}

/**
 * funzione principale per la gestione del comportamento della granata
 * @param pipe_fd file descriptor della pipe per comunicare con il processo principale
 * @param grenade struct Messaggio che contiene le informazioni iniziali della granata
 */
void main_grenade(int pipe_fd, Messaggio grenade) {
    if (grenade.direzione == 1) {
        grenade.x = grenade.x + 2; 
    }

    /*inizializziamo la granata*/
    grenade.is_active = true; //settiamo la flag a true per indicare che la granata è attiva 
    grenade.velocita = GRENADE_SPEED; //impostiamo la velocità della granata mediante la macro apposita costante
    grenade.oggetto = ID_GRENADE; //impostiamo il tipo dell'oggetto per identificarlo come granata 
    grenade.pid = getpid(); //salviamo il PID del processo corrente nella struct Messaggio

    write(pipe_fd, &grenade, sizeof(Messaggio)); //inviamo alla pipe lo stato iniziale della granata

    /*cicliamo all'infinito per il moviemnto della granata*/
    while(1) {
    
        if (grenade.direzione == 1) {
            grenade.x += 2; 
        } else {
            grenade.x -= 2;
        }
       
        if (grenade.direzione == 1) {
            movement_grenade_right(&grenade);
        } else {
            movement_grenade_left(&grenade);
        }
    
       
        if (check_grenade_borders(grenade)) {
            grenade.is_active = false; 
            write(pipe_fd, &grenade, sizeof(Messaggio)); 
            break; 
        }

   
        write(pipe_fd, &grenade, sizeof(Messaggio));
        usleep(GRENADE_SPEED);
    }
    
}