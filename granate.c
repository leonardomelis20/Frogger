#include "granate.h"


char grenade = 'G'; //definizione del carattere che rappresenta la granata 

/**
 * funzione che disegna la granata alla posizione (x, y)
 * @param x coordinata orizzontale della granata
 * @param y coordinata verticale della granata
 */
void draw_grenade(int x, int y) {
    mvaddch(y, x, grenade); //uso la funzione ncurses per disegnare la granata nella coordinata (x,y)
}

/**
 * funzione che cancella la granata dalla posizione (x, y) sostituendola con uno spazio
 * @param x coordinata orizzontale della granata
 * @param y coordinata verticale della granata
 */
void clear_grenade(int x, int y) {
    mvaddch(y, x, ' '); //uso la funzione ncurses per disegnare lo spazio vuoto nella coordinata (x,y)
}

/**
 * funzione che aggiorna la posizione della granata spostandola verso sinistra
 * @param grenade puntatore alla struct Messaggio che contiene le informazioni sulla granata
 */
void movement_grenade_left(Messaggio* grenade) {
    grenade->x -= 1; //decremento la coordinata x per muovere la granata verso sinistra
}

/**
 * funzione che aggiorna la posizione della granata spostandola verso destra
 * @param grenade puntatore alla struct Messaggio che contiene le informazioni sulla granata
 */
void movement_grenade_right(Messaggio* grenade) {
    grenade->x += 1; //incremento la coordinata x per muovere la granata verso destra
}

/**
 * funzione che controlla se la granata ha toccato i bordi dell'area di gioco
 * @param grenade struct Messaggio contenente le informazioni sulla granata
 * @return true se la granata è al di fuori dei bordi, false altrimenti
 */
bool check_grenade_borders(Messaggio grenade) {
    return (grenade.x < 0 || grenade.x >= GAME_WIDTH); //controllo se la coordinata x è fuori dai limiti dell'area di gioco
}

/**
 * funzione principale per la gestione del comportamento della granata
 * @param pipe_fd descrittore della pipe per comunicare con il processo principale
 * @param grenade struct Messaggio contenente le informazioni iniziali della granata
 */
void main_grenade(int pipe_fd, Messaggio grenade) {
    /*se la direzione è 1, quindi destra, sposto la granata di 2 posizioni a destra come posizione iniziale*/
    if (grenade.direzione == 1) {
        grenade.x = grenade.x + 2;
    }

    /*inizializzo la granata*/
    grenade.is_active = true; //setto la flag a true per indicare che la granata è attiva 
    grenade.velocita = GRENADE_SPEED; //imposto la velocità della granata mediante la macro apposita 
    grenade.oggetto = ID_GRENADE; //imposto l'ID dell'oggetto per identificarlo come granata 
    grenade.pid = getpid(); //salvo il PID del processo corrente nella struct Messaggio

    /*ciclo infinito per il moviemnto della granata*/
    

    while(1) {
        /*se la direzione della granata è 1, quinid destra*/
        
        // Muovi la granata di 2 posizioni
        if (grenade.direzione == 1) {
            grenade.x += 2;
        } else {
            grenade.x -= 2;
        }
        // Movimento normale
        if (grenade.direzione == 1) {
            movement_grenade_right(&grenade);
        } else {
            movement_grenade_left(&grenade);
        }
    
        /*controllo se la granata è uscita dai bordi*/
        if (check_grenade_borders(grenade)) {
            log_coordinates(grenade.pid, grenade.x, grenade.y, grenade.velocita); 
            break; //se è uscita, termino il ciclo e termino il processo 
        }
        //loggo le coordinate della granata
        write(pipe_fd, &grenade, sizeof(Messaggio)); //invio la posizione aggiornata della granata al processo principale attraverso la pipe
        usleep(GRENADE_SPEED); //attendo un periodo determinato in base alla velocità della granata prima di aggiornare nuovamente la posizione 
    }
    
}