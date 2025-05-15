#include "collisioni.h"

/**
 * funzione che controlla se avviene una collisione tra la rana e un coccodrillo
 * @param frog struct Messaggio che contiene le coordinate della rana
 * @param croc puntatore all'array di struct Messaggio che contiene le informazioni sui coccodrilli
 * @return indice del coccodrillo che collide o -1 se non ci sono collisioni
 */
int check_collision(Messaggio frog, Messaggio* croc) {
    int flag = -1; //flag di default dove -1 sta per nessuna collisione

    /*cicliamo per controllare tutti i coccodrilli*/
    for (int i = 0; i < NUM_CROC; i++) {

        /*verifichiamo se la rana si trova all'interno del corpo del coccodrillo e se sono alla stessa altezza*/
        if (frog.x >= croc[i].x && (frog.x + LARGHEZZA_RANA) <= (croc[i].x + LARGHEZZA_COCCODRILLO) && frog.y == croc[i].y && frog.y == (croc[i].y )) {        
            flag = i; //viene rilevata la collisione e quindi salviamo l'indice del coccodrillo

            return flag; //ritorniamo l'indice del coccodrillo trovato
        } 
    }

    return flag; //non viene rilevata nessuna collisione quindi ritorniamo -1
}

/**
 * funzione che corregge la coordinata x della rana per allinearla alla griglia di gioco
 * arrotonda la posizione x al multiplo più vicino della larghezza della rana
 * @param frog struct Messaggio che contiene le coordinate della rana
 * @return nuova struct Messaggio con le coordinate corrette
 */
Messaggio correct_x_frog (Messaggio frog) {
    Messaggio new_cord; //nuova struct Messaggio per memorizzare le coordinate corrette

    new_cord.y = frog.y; //manteniamo la stessa coordinata y
    new_cord.x = (frog.x + (LARGHEZZA_RANA/2)) / LARGHEZZA_RANA * LARGHEZZA_RANA; //arrotondiamo la x al multiplo più vicino della larghezza della rana
    
    return new_cord; //ritorniamo la nuova struct con la coordinata x corretta
}

/**
 * funzione che aggiorna lo stato della rana quando si trova su un coccodrillo
 * impostando i flag appropriati e l'indice del coccodrillo su cui si trova
 * @param frog puntatore alla struct Messaggio della rana da aggiornare
 * @param croc puntatore all'array di struct Messaggio dei coccodrilli
 */
void frog_with_croc(Messaggio* frog, Messaggio* croc) {
    int i = 0; 

    i = check_collision(*frog, croc); //verifichiamo se c'è una collisione e ottieniamo l'indice del coccodrillo

    frog->on_croc = true; //impostiamo il flag che indica che la rana è su un coccodrillo
    frog->croc_index = i; //salviamo l'indice del coccodrillo su cui si trova la rana

    /*se la rana non è sopra nessun coccodrillo*/
    if (i == -1) {
        frog->on_croc = false; //impostiamo il falg a false
        frog->croc_index = -1; //impostiamo l'indice ad un valore non valido
    }    
}

/**
 * funzione che controlla se la rana è uscita dai bordi dello schermo dell'area di gioco
 * @param frog struct Messaggio che contiene le coordinate della rana
 * @return true se la rana è fuori dai bordi, false altrimenti
 */
bool check_borders_frog(Messaggio frog) {
    /*controlliamo sia orizzontalemente che verticalmente*/
    if (frog.x < 0 || frog.x >= GAME_WIDTH - LARGHEZZA_RANA || frog.y < 0 || frog.y > GAME_HEIGHT - ALTEZZA_RANA) {
        return true; //ritorniamo true se la rana è fuori dai bordi
    }

    return false; //altrimenti ritorniamo false se è dentro i bordi
}

/**
 * funzione che controlla se un proiettile ha superato i bordi dello schermo dell'area di gioco
 * @param bullet struct Messaggio che contiene le coordinate del proiettile
 * @return true se il proiettile è fuori dai bordi, false altrimenti
 */
bool check_borders_bullet(Messaggio bullet) {
    /*controlliamo sia orizzontalemente che verticalmente*/
    if (bullet.x < 0 || bullet.x >= GAME_WIDTH || bullet.y < 0 || bullet.y > GAME_HEIGHT) {
        return true; //ritorniamo true se è fuori dai bordi
    }
    return false; //altrimenti ritorniamo false se è dentro i bordi
}

/**
 * funzione che verifica se la rana si trova in una zona sicura, quindi o sul marciapiede inferiore o su quello superiore con le tane
 * @param frog struct Messaggio che contiene le coordinate della rana
 * @return true se la rana è in una zona sicura, false altrimenti
 */
bool check_safe_zone(Messaggio frog) {
    /*se si trova nella zona sicura bassa ovvero il marciapiede inferiore*/
    if (frog.y >= SAFE_ZONE_DOWN && frog.y <= SAFE_ZONE_DOWN_2) {
        return true; //ritorniamo true perchè è dentro la zona sicura
    } 
    /*altrimenti se si trova nella zona sicura lata ovvero il marciapiede superiore con le tane*/
    else if (frog.y >= 0 && frog.y <= SAFE_ZONE_UP) {
        return true; //ritoriamo sempre true perchè è dentro la zona sicura
    }
    
    return false; //altrimenti se non è in una zona sicura ritorniamo false
}

/**
 * funzione che determina se la rana è nel fiume senza essere sopra un coccodrillo
 * @param frog struct Messaggio che contiene le informazioni sulla rana
 * @return true se la rana è nel fiume senza essere su un coccodrillo, false altrimenti
 */
bool river(Messaggio frog) {
    /*se non si trova in una zona sicura e se non si trova sopra un coccodirllo*/
    if (!check_safe_zone(frog) && !frog.on_croc) {
        return true; //ritorniamo true perchè si trova nel fiume
    }

    return false; //altrimenti  se è al sicuro ritorniamo false
}

/**
 * funzione che gestisce le collisioni tra i proiettili e le granate
 * @param bullet puntatore all'array di struct Messaggio dei proiettili
 * @param grenade puntatore all'array di struct Messaggio delle granate
 * @param count_bullet numero totale dei proiettili
 * @param count_grenade numero totale delle granate
 */
bool collision_b_g(Messaggio* bullet, Messaggio* grenade, int count_bullet, int count_grenade) {
    int status = 0; 
    bool flag =  false;

    /*cicliamo per tutti i proiettili*/
    for (int i = 0; i < MAX_BULLETS; i++) {
        /*verifichiamo se il proiettile è attivo*/
        if (bullet[i].is_active) {
            /*scorriamo tutte le granate*/
            for (int j = 0; j < MAX_GRENADE; j++) {
                /*controlliamo se la granata è attiva*/
                if (grenade[j].is_active) {
                    /*verifichiamo se gli oggetti sono a una distanza massima di 2 unità orizzontali e 1 unità verticale*/
                    if (abs(bullet[i].x - grenade[j].x) <= 2 && abs(bullet[i].y - grenade[j].y) <= 1) {
                        /*cancelliamo entrambi gli oggetti dallo schermo*/
                        flag = true; //impostiamo il flag a true per indicare che è avvenuta una collisione
                        clear_bullet(bullet[i].x, bullet[i].y); 
                        clear_grenade(grenade[j].x, grenade[j].y); 
                        
                        /*terminiamo il processo del proiettile se il PID è valido*/
                        if (bullet[i].pid > 1) {
                            kill(bullet[i].pid, SIGKILL); //inviamo un segnale SIGKILL al processo del proiettile
                            waitpid(bullet[i].pid, &status, 0); //attendiamo la terminazione del processo
                        }

                        /*determina il processo della granata se il PID è valido*/
                        if (grenade[j].pid > 1) {
                            kill(grenade[j].pid, SIGKILL); //inviamo un segnale SIGKILL al processo del proiettile
                            waitpid(grenade[j].pid, &status, 0); //attendiamo la terminazione del processo
                        }
                        
                        /*settiamo il proiettile come inattivo*/
                        bullet[i].is_active = false; //disattiviamo il proiettile
                        /*spostiamo il proiettile fuori dallo schermo*/
                        bullet[i].x = -100; // Move off-screen
                        bullet[i].y = -100;
                        
                        /*settiamo la granata come inattiva*/
                        grenade[j].is_active = false; //disattiviamo la granata
                        /*spostiamo la granata fuori dallo schermo*/
                        grenade[j].x = -100; 
                        grenade[j].y = -100;
            
                        return;
                    }
                }
            }
        }
    }
    return flag; //ritorniamo il flag che indica se è avvenuta una collisione
}