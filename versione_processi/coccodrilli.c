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
 * funzione che cerca l'indice di un coccodrillo nell'array di coccodrilli in base al suo pid
 * @param msg array di struct Messaggio che contiene le informazioni sui coccodrilli
 * @param pid l'iidentificatore del processo da cercare
 * @return indice del coccodrillo trovato o -1 se non trovato
 */
int get_index_croc(Messaggio msg[], pid_t pid) {
    /*scorriamo tutti i coccodrilli*/
    for(int i = 0; i < NUM_CROC; i++) {

        /*se troviamo il pid corrispondente*/
        if(msg[i].pid == pid) {
            return i; //restituiamo l'indice del coccodrillo trovato 
        }
    }  

    return -1; //oppure restituiamo -1 come valore di default se non trova corrispondenza
}

/**
 * funzione principale per la gestione del processo coccodrillo
 * @param pipe_fd file descriptor della pipe per comunicare con il processo principale
 * @param num indice del coccodrillo
 * @param direzione direzione del coccodrillo, ovvero 1 per destra e -1 per sinistra
 * @param speed velocità del coccodrillo
 * @param flag indica se è il primo gruppo di coccodrilli
 * @return valore di uscita del processo
 */
int main_croc(int pipe_fd, int num, int direzione, int speed, bool flag) {
    Messaggio msg; //struct Messaggio da inviare tramite pipe
    int adjusted_num; //numero modificato per il posizionamento verticale
    bool shooting = false; //flag che controlla se il coccodrillo ha già sparato un proiettile
    
    /* se usiamo solo srand(time(NULL)); i due processi coccodrillo per ogni flusso vengono creati quasi contemporaneamente 
     * e quindi ereditano lo stesso stato del generatore di numeri casuali rand(),
     * producendo la stessa sequenza di numeri casuali all'inizio e facendoli sparare nello stesso momento.
     * 
     * usando srand(time(NULL) ^ getpid()) abbiamo un valore diverso per ogni processo coccodrillo, 
     * usando quindi il pid e il tempo corrente per ottenere più casualità.
     */
    srand(time(NULL) ^ getpid());  //inizializziamo il generatore di numeri casuali con un seed unico

    /*se non è il primo gruppo di coccodrilli*/
    if(!flag) {  
        usleep(5000000); //attendiamo 5 secondi prima di iniziare per scaglionare l'apparizione dei coccodrilli
    }

    /*se è uno dei secondi coccodrilli, quelli da 9 a 17*/
    if (num >= 9) {
        adjusted_num = num - 9; //lo riportiamo nell'intervallo da 0 a 8 per calcolare la posizione verticale
    } 
    /*altrimenti lo lasciamo normale*/
    else {
        adjusted_num = num;
    }

    /*inizializziamo la struct Messaggio*/
    msg.index = num; //impostiamo l'indice del coccodrillo
    msg.pid = getpid(); //salviamo il pid del processo corrente
    msg.oggetto = ID_CROCODILE; //settiamo il tipo dell'oggetto come coccodrillo
    msg.direzione = direzione; //impostiamo la direzione del coccodrillo
    msg.y = 6 + (adjusted_num * 3); //calcoliamo la posizione verticale, ovvero distante ogni 3 righe
    
    /*se la direzione è verso destra*/
    if (msg.direzione == 1) {
        msg.x = 0; //lo facciamo partire dasinistra
    } 
    /*altrimenti se la direzione è verso sinistra*/
    else {
        msg.x = GAME_WIDTH - LARGHEZZA_COCCODRILLO; //lo facciamo partire da destra
    }
    
    msg.velocita = speed; //impostiamo la velocità del coccodrillo
    usleep(50000); //attendiamo un breve periodo prima di iniziare il movimento

    write(pipe_fd, &msg, sizeof(Messaggio)); //inviamo le informazioni iniziali al processo principale

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
                write(pipe_fd, &msg, sizeof(Messaggio)); //inviamo il messaggio alla pipe per creare il proiettile
            }

            msg.oggetto = ID_CROCODILE; //risettiamo il tipo dell'oggetto a coccodrillo
        }

        /*se il coccodrillo non ha superato i limiti dell'area di gioco*/
        if (check_borders(msg)) {
            msg.oggetto = RESPAWN; //impostiamo il tipo dell'oggetto a respawn per specificare che deve riapparire
            write(pipe_fd, &msg, sizeof(Messaggio)); //inviamo il messaggio alla pipe lper far si che avvenga il respawn
            break; //usciamo dal ciclo infinito
        } 

        write(pipe_fd, &msg, sizeof(Messaggio)); //inviamo la posizione aggiornata al processo principale
        usleep(msg.velocita); //attendiamo un periodo in base dalla velocità prima del prossimo aggiornamento
    }
}