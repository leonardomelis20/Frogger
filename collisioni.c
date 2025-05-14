#include "collisioni.h"

/*funzione che controlla se la rana collide con un coccodrillo*/
int check_collision(Messaggio frog, Messaggio* croc) {
    int flag = -1; //flag di default, -1 indica nessuna collisione

    /*ciclo per tutti i coccodrilli*/
    for (int i = 0; i < NUM_CROC; i++) {

        /*se x della rana è dentro il corpo del coccodrillo e se le y sono uguali*/
        if (frog.x >= croc[i].x && (frog.x + LARGHEZZA_RANA) <= (croc[i].x + LARGHEZZA_COCCODRILLO) && frog.y == croc[i].y && frog.y == (croc[i].y )) {        
            flag = i; //viene rilevata la collisione e quindi si salva l'indice del coccodrillo

            return flag; //ritorno l'indice appena trovato
        } 
    }

    return flag; //non viene rilevata nessuna collisione
}

/*funzione che corregge la coordinata x della rana per allinearla alla griglia di gioco*/
Messaggio correct_x_frog (Messaggio frog) {
    Messaggio new_cord; //nuova struct Messaggio con coordinate corrette

    new_cord.y = frog.y; //tengo la stessa y
    new_cord.x = (frog.x + (LARGHEZZA_RANA/2)) / LARGHEZZA_RANA * LARGHEZZA_RANA; //arrotondo la x al multiplo più vicino di LARGHEZZA_RANA
    
    return new_cord; //ritorno la nuova struttura con la x corretta
}

/*funzione che aggiorna lo stato della rana se si trova su un coccodrillo*/
void frog_with_croc(Messaggio* frog, Messaggio* croc) {
    int i = check_collision(*frog, croc); //verifico la collisione

    frog->on_croc = true; //la rana è su un coccodrillo quindi setto a true
    frog->croc_index = i; //salvo l'indice

    /*se non è sopra nessun coccodrillo*/
    if (i == -1) {
        frog->on_croc = false; //setto a false
        frog->croc_index = -1; //l'indice non è valido
    }    
}

/*funzione che controlla se la rana è uscita dai bordi dello schermo*/
bool check_borders_frog(Messaggio frog) {
    /*controllo sia orizzontalemente che verticalmente*/
    if (frog.x < 0 || frog.x >= GAME_WIDTH - LARGHEZZA_RANA || frog.y < 0 || frog.y > GAME_HEIGHT - ALTEZZA_RANA) {
        return true; //ritorno true se è fuori dai bordi
    }

    return false; //ritorno false se è dentro i bordi
}

/*!!!!!!!!!!!!!!!!*/
/*aggiungi funzione check_borders_crocodile*/
/*!!!!!!!!!!!!!!!!*/

/*funzione che controlla se un proiettile ha superato i bordi dello schermo*/
bool check_borders_bullet(Messaggio bullet) {
    /*controllo sia orizzontalemente che verticalmente*/
    if (bullet.x < 0 || bullet.x >= GAME_WIDTH || bullet.y < 0 || bullet.y > GAME_HEIGHT) {
        return true; //ritorno true se è fuori dai bordi
    }
    return false; //ritorno false se è dentro i bordi
}

/*funzione che verifica se la rana è in una zona sicura*/
bool check_safe_zone(Messaggio frog) {
    /*se si trova nella zona sicura bassa, il marciapiede in basso*/
    if (frog.y >= SAFE_ZONE_DOWN && frog.y <= SAFE_ZONE_DOWN_2) {
        return true; //ritorno true perchè è dentro la zona sicura
    } 
    /*altrimenti se si trova nella zona sicura lata, le tane*/
    else if (frog.y >= 0 && frog.y <= SAFE_ZONE_UP) {
        return true; //ritoro sempre true perchè è dentro la tana
    }
    
    return false; //altrimenti se non è in una zona sicura ritorno false
}

/*funzione che determina se la rana è nel fiume senza essere sopra un coccodrillo*/
bool river(Messaggio frog) {
    /*se non si trova in una zona sicura e se non si trova sopra un coccodirllo*/
    if (!check_safe_zone(frog) && !frog.on_croc) {
        return true; //ritorno true perchè si trova nel fiume
    }

    return false; //altrimenti ritorno false se è al sicuro
}