#include "coccodrilli.h"

/**
 * definizione dell'aspetto grafico del coccodrillo 
 * occupa 15 caratteri in larghezza e 3 in altezza
 */
char sprite_coccodrillo[ALTEZZA_COCCODRILLO][LARGHEZZA_COCCODRILLO+1] = {
    " ~~~~~___~~~~~ ",  
    " (o)-------(o) ",  
    " ~~~~~___~~~~~ "   
};

/**
 * funzione che disegna il coccodrillo alla posizione (x, y) e tiene conto dei limiti dell'area di gioco per non disegnare al di fuori di questa 
 * @param x coordinata orizzontale dell'angolo superiore sinistro del coccodrillo
 * @param y coordinata verticale dell'angolo superiore sinistro del coccodrillo
 */
void draw_crocodile(int x, int y) {

    attron(COLOR_PAIR(2)); //attiviamo il colore verde per il coccodrillo
    int colonna = 0; //variabile per memorizzare la posizione orizzontale corrente

    /*scorriamo ogni riga del coccodrillo*/
    for (int i = 0; i < ALTEZZA_COCCODRILLO; i++) {

        /*scorriamo ogni carattere della riga corrente*/
        for (int j = 0; j < LARGHEZZA_COCCODRILLO; j++) {
            colonna = x + j; //calcoliamo la posizione orizzontale corrente

            /*se il carattere è dentro lo schermo allora lo disegnamo*/
            if (colonna >= 0 && colonna < GAME_WIDTH) {
                mvaddch(y + i, colonna, sprite_coccodrillo[i][j]); //disegniamo il carattere del coccodrillo nella posizione corretta
            }
        }
    }
    attroff(COLOR_PAIR(2)); //disattiviamo il colore verde
}

/**
 * funzione che aggiorna la posizione del coccodrillo in base alla sua direzione
 * @param croc puntatore alla struct Messaggio che contiene le informazioni sul coccodrillo
 */
void movement_croc(Messaggio* croc) {
    croc->x += croc->direzione; //aggiorniamo la posizione orizzontale, ovvero positiva per destra e negativa per sinistra
}

/**
 * funzione che controlla se il coccodrillo ha superato i bordi dello schermo
 * @param croc struct Messaggio che contiene le informazioni sul coccodrillo
 * @return true se il coccodrillo è uscito dai bordi, false altrimenti
 */
bool check_borders(Messaggio croc) {
    /*se va verso destra*/
    if(croc.direzione == 1) {
        return (croc.x >= GAME_WIDTH); //controlliamo se ha superato il bordo destro dello schermo
    } 
    /*altrimenti, se va verso sinistra*/
    else {
        return (croc.x <= -LARGHEZZA_COCCODRILLO); //controlliamo se è completamente uscito dal bordo sinistro dello schermo
    }
}

/**
 * funzione che cancella il coccodrillo dallo schermo sostituendolo con spazi vuoti
 * @param msg struct Messaggio che contiene le coordinate del coccodrillo da cancellare
 */
void clear_croc(Messaggio msg) {
    /*scorriamo le righe*/
    for(int i = 0; i < ALTEZZA_COCCODRILLO; i++) {
        
        /*scorriamo le colonne*/
        for(int j = 0; j < LARGHEZZA_COCCODRILLO; j++) {
            mvprintw(msg.y + i, msg.x + j, " "); //sostituiamo ogni carattere del coccodrillo con lo spazio vuoto
        }
    }
}

/**
 * funzione che si occupa del thread del coccodrillo
 * @param arg puntatore ai parametri del thread 
 * @return NULL
 */
void* crocodile_thread(void* arg) {
    Crocs_arg* params = (Crocs_arg*) arg; 
    Circular_buffer* buffer = params->buffer; 

    int num = params->tid; 
    int direzione = params->direzione; 
    int speed = params->speed;
    bool flag = params->flag; 
    Messaggio msg;
    int adjusted_num; 
    bool shooting = false; 

    /*inizializzaziamo il generatore di numeri casuali*/
    srand(time(NULL) ^ (unsigned int) pthread_self()); 

    /*se non è il primo gruppo di coccodrilli*/
    if(!flag) {
        usleep(5000000); //attendiamo 5 secondi prima di iniziare
    }

    /*se è uno dei secondi coccodrilli, quelli da 9 a 17*/
    if (num >= 9) {
        adjusted_num = num - 9; 
    }
    else {
        adjusted_num = num; 
    }

    /*inizializziamo la struct Messaggio*/
    msg.index = num; 
    msg.tid = pthread_self(); 
    msg.oggetto = ID_CROCODILE; 
    msg.direzione = direzione; 
    msg.y = 6 + (adjusted_num * 3); 

    /*se la direzione è verso destra*/
    if (msg.direzione == 1) {
        msg.x = 0; 
    }
    else {
        msg.x = GAME_WIDTH - LARGHEZZA_COCCODRILLO; 
    }

    msg.velocita = speed; 
    usleep(50000); 

    produce_message(buffer, msg); 
    
    /*!!!!!
    nel while al posto di 1 game_running
    è una variabile globale che si trova nel file buffer.c ->
    -> bool game_running = true;
    !!!!!*/
    /*ciclo infinito per il movimento continuo del coccodrillo*/
    while(1) {
        /*aggiorniamo la posizione del coccodrillo*/
        movement_croc(&msg);

        /*diamo al coccodrillo una probabilità del 3% di sparare ad ogni ciclo */
        if (rand() % 100 < 3) {
            msg.is_shooting = true; //impostiamo il falg che indica che il coccodrillo sta sparando
            msg.oggetto = CREATE_BULLET; //settiamo il tipo dell'oggetto per indicare che si deve creare un proiettile
            msg.index = num; //passiamo l'indice del coccodrillo che sta sparando

            /*se il coccodrillo non ha ancora sparato in questo ciclo*/
            if(shooting == false) {
                shooting = true; //impostiamo il flag che indica che il coccodrillo ha sparato
                produce_msg(buffer, msg); //inviamo il messaggio alla pipe per creare il proiettile
            }

            msg.oggetto = ID_CROCODILE; //risettiamo il tipo dell'oggetto a coccodrillo
        }

        /*se il coccodrillo non ha superato i limiti dell'area di gioco*/
        if (check_borders(msg)) {
            msg.oggetto = RESPAWN; //impostiamo il tipo dell'oggetto a respawn per specificare che deve riapparire
            produce_msg(buffer, msg); //inviamo il messaggio alla pipe lper far si che avvenga il respawn
            
            /*resettiamo il respawn*/
            shooting = false; 
            if (msg.direzione == 1) {
                msg.x = 0; 
            }
            else {
                msg.x = GAME_HEIGHT - LARGHEZZA_COCCODRILLO;
            }

            msg.oggetto = ID_CROCODILE; 
            usleep(100000);
        } 

        produce_msg(buffer, msg); //inviamo la posizione aggiornata al processo principale
        usleep(msg.velocita); //attendiamo un periodo in base dalla velocità prima del prossimo aggiornamento
    }
    
    return NULL; 
}