#include "collisioni_thread.h"

/**
 * funzione che controlla se avviene una collisione tra la rana e un coccodrillo
 * @param frog struct Messaggio che contiene le coordinate della rana
 * @param croc puntatore all'array di struct Messaggio che contiene le informazioni sui coccodrilli
 * @return indice del coccodrillo che collide o -1 se non ci sono collisioni
 */
int check_collision (Messaggio frog, Messaggio* croc) {
    int flag = -1; //variabile per far restituire l'indice del coccodrillo con cui è avvenuta la collisione 

    /*ciclo for che cicla per tutti i coccodrilli*/
    for (int i = 0; i < NUM_CROC; i++) {
        /*confrontiamo la x e la y di rana e coccodrillo per verificare la collisione*/
        if (frog.x >= croc[i].x && (frog.x + LARGHEZZA_RANA) <= (croc[i].x + LARGHEZZA_COCCODRILLO) && frog.y == croc[i].y && frog.y == (croc[i].y)) {        
            flag = i; //diamo a flag l'indice del coccodrillo con cui si è verificata la collisione

            return flag; //ritorniamo flag 
        } 
    }

    /*se non è avvenuta la collsione*/
    return flag; //facciamo ritornare flag
}

/**
 * funzione che aggiorna lo stato della rana quando si trova su un coccodrillo
 * @param frog puntatore alla struct Messaggio della rana da aggiornare
 * @param croc puntatore all'array di struct Messaggio dei coccodrilli
 */
void frog_with_croc (Messaggio* frog, Messaggio* croc) {
    int i = 0; //variabile per assegnare l'indice del coccodrillo su cui è la rana 

    i = check_collision(*frog, croc); //assegnamo ad i l'indice del coccodrillo su cui si trova la rana 

    /*settiamo a true per dire che la rana si trova su un coccodrillo e assegniamo l'indice del coccodrillo su cui è la rana*/
    frog->on_croc = true; 
    frog->croc_index = i;

    /*se la rana non è su un coccdirllo*/
    if (i == -1) {
        /*settiamo a false per dire che la rana non si trova su nessun coccodrillo e assegnamo all'indice del coccodrillo -1*/
        frog->on_croc = false; 
        frog->croc_index = -1;
    }    
}

/**
 * funzione che verifica se la rana si trova in una zona sicura
 * @param frog struct Messaggio che contiene le coordinate della rana
 * @return true se la rana è in una zona sicura, false altrimenti
 */
bool check_safe_zone (Messaggio frog) {
    /*se la rana si trova sul marciapiede inferiore*/
    if (frog.y >= SAFE_ZONE_DOWN && frog.y <= SAFE_ZONE_DOWN_2) {
        return true; //ritorniamo true per dire che è in una zona sicura
    } 
    /*altrimenti se la rana si trova sul marciapiede superiore*/
    else if (frog.y >= 0 && frog.y <= SAFE_ZONE_UP) {
        return true; //ritorniamo true per dire che è in una zona sicura
    }
    /*altrimenti*/
    return false; //ritorniamo false per dire che non si trova in una zona sicura
}

/**
 * funzione che determina se la rana è nel fiume senza essere sopra un coccodrillo
 * @param frog struct Messaggio che contiene le informazioni sulla rana
 * @return true se la rana è nel fiume senza essere su un coccodrillo, false altrimenti
 */
bool river (Messaggio frog) {
    /*se la rana non si trova in una safe zone o sopra un coccodrillo*/
    if (!check_safe_zone(frog) && !frog.on_croc) {
        return true; //restituiamo true per dire che è nel fiume
    }
    /*altrimenti*/
    return false; //restituiamo false per dire che non è nel fiume
}

/**
 * funzione che gestisce le collisioni tra i proiettili e le granate
 * @param bullet puntatore all'array di struct Messaggio dei proiettili
 * @param grenade puntatore all'array di struct Messaggio delle granate
 * @param count_bullet numero totale dei proiettili (non utilizzato)
 * @param count_grenade numero totale delle granate (non utilizzato)
 * @return true se è avvenuta una collisione, false altrimenti
 */
bool collision_b_g (Messaggio* bullet, Messaggio* grenade, int count_bullet, int count_grenade) {
    bool collision_found = false; //variabile booleana per dire se è stata rilevata una collisione, false se non c'è stata, true altrimenti

    /*ciclo for che scorre l'array dei proiettili attivi fino a quando non trova una collisione*/
    for (int i = 0; i < MAX_BULLETS && !collision_found; i++) {
        /*se c'è un proiettile attivo*/
        if (bullet[i].is_active) {
            /*ciclo for che scorre l' array delle granate attive fino a quando non trova una collisione*/
            for (int j = 0; j < MAX_GRENADE && !collision_found; j++) {
                /*se c'è una granata attiva*/
                if (grenade[j].is_active) {
                    /*verifichiamo la collisione con una tolleranza 2, per evitare che appaiano sovrapposti ma siano matematicamente distanti*/
                    if (abs(bullet[i].x - grenade[j].x) <= 2 && abs(bullet[i].y - grenade[j].y) <= 1) {
                        collision_found = true; //settiamo il booleano delle collisione a true per di                
                        
                        /*disattiviamo il proiettile */
                        bullet[i].is_active = false;
                        bullet[i].x = -100;
                        bullet[i].y = -100;
                        
                        /*disattiviamo la granata */
                        grenade[j].is_active = false;
                        grenade[j].x = -100; 
                        grenade[j].y = -100;
                        
                        break;
                    }
                }
            }
        }
    }
    
    return collision_found; //ritorniamo true se è stata trovata una collisione, false altrimenti
}