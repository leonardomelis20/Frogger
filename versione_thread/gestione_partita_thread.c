#include "gestione_partita_thread.h"

/**
 * funzione che inizializza il timer
 * @param timer puntatore alla struttura Info_timer 
 */
void init_timer (Info_timer* timer) {
    timer->seconds_left = TIMER_DURATION; //inizializziamo i secondi rimanenti 

    timer->last_update = time(NULL); //salviamo il tempo corrente 

    timer->is_active = true; //settiamo a true la variabile che serve per attivare il timer, true se è così, false altrimenti
}

/**
 * funzione per aggiornare il timer e controllare se è fnito il tempo
 * @param timer puntatore alla struttura Info_timer 
 * @return true se il tempo è scaduto, false altrimenti
 */
bool update_timer (Info_timer* timer) {
    time_t current_time; //variabile per sapere il tempo corrente
    time_t elapsed; //variabile per calcolare il tempo trascorso dall'ultimo ciclo di timer 

    /*se il timer non è attivo*/
    if (!timer->is_active) {
        return false; //ritorniamo false per dire che non deve fare nulla
    }
    
    /*calcoliamo il tempo trascorso dall'ultimo ciclo di timer*/
    current_time = time(NULL);
    elapsed = current_time - timer->last_update;
    
    /*se è passato del tempo*/
    if (elapsed >= 1) {
        timer->seconds_left -= elapsed; //togliamo i secondi passati dal timer

        timer->last_update = current_time; //aggiorniamo il tempo dall'ultimo cambiamento
        
        /*se il tempo è finito*/
        if (timer->seconds_left <= 0) {
            /*aggiorniamo le variabili per disattivare il timer*/
            timer->seconds_left = 0;
            timer->is_active = false;

            return true; //ritorniamo true per dire che il tempo è finito
        }
    }
    
    return false; //ritorniamo false perchè il tenpo non è ancora scauto 
}

/**
 * funzione per ripristinare il timer al valore di partenza
 * @param timer puntatore alla struttura Info_timer
 */
void reset_timer (Info_timer* timer) {
    timer->seconds_left = TIMER_DURATION; //risettiamo il numero di secondi al valore di partenza 

    timer->last_update = time(NULL); //risettiamo a NULL l'ultimo aggiornamento

    timer->is_active = true; //settiamo a true la variabile per dire che il timer è attivo, false altrimenti 
}

/**
 * funzione che termina tutti i thread attivi
 * @param game_data puntatore alla struttura dati principale
 */
void cleanup_game_threads (Game_data* game_data) {
    int result; //variabile per controllare se la kill ha avuto successo

    pthread_mutex_lock(&game_data->game_mutex); //accesso esclusivo al mutex condiviso
    game_data->game_over = true; //settiamo a true la variabile per dire che si è verificata la condizione di game over
    pthread_mutex_unlock(&game_data->game_mutex); //rilasciamo il mutex condiviso 
    
    pthread_mutex_lock(&game_data->bullets_mutex); //accesso esclusivo al mutex dei proiettili
    /*ciclo for che scorre per tutti i proiettili*/
    for (int i = 0; i < MAX_BULLETS; i++) {
        game_data->active_bullets[i].is_active = false; //settiamo a false la variabile dei proiettili attivi per assicurarci che siano tutti disattivati 
    }
    pthread_mutex_unlock(&game_data->bullets_mutex); //rilasciamo il mutex dei proiettili
    
    pthread_mutex_lock(&game_data->grenades_mutex); //accesso esclusivo al mutex delle granate
    /*ciclo for che scorre per tutte le granate*/
    for (int i = 0; i < MAX_GRENADE; i++) {
        game_data->active_grenades[i].is_active = false;  //settiamo a false la variabile delle granate attive per dire che tutte devono essere disattivate 
    }
    pthread_mutex_unlock(&game_data->grenades_mutex); //rilasciamo il mutex delle granate
    
    /*ciclo for che scorre per tutti i coccodrilli*/
    for (int i = 0; i < NUM_CROC; i++) {
        game_data->coccodrilli[i].is_active = false; //settiamo a false la variabile dei coccodrilli attivi per disattivarli tutti
    }
    
    game_data->frog_copy.is_active = false; //impostiamo a false la variabile per disattivare la rana
    
    usleep(500000); 
    
    /*se il tid è valido*/
    if (game_data->frog_copy.tid != 0) {
        /*terminiamo il thred della rana*/
        result = pthread_kill(game_data->frog_copy.tid, 0);
        /*se la kill ha avuto successo*/
        if (result == 0) {
            pthread_cancel(game_data->frog_copy.tid);
            pthread_join(game_data->frog_copy.tid, NULL);
        }
        game_data->frog_copy.tid = 0;
    }
    
     /*ciclo for che scorre per tutti i coccodrilli*/
    for (int i = 0; i < NUM_CROC; i++) {
        /*se il tid è valido*/
        if (game_data->coccodrilli[i].tid != 0) {
            /*terminiamo il thread del coccodrillo*/
            result = pthread_kill(game_data->coccodrilli[i].tid, 0);
            /*se la kill ha avuto successo*/
            if (result == 0) {
                pthread_cancel(game_data->coccodrilli[i].tid);
                pthread_join(game_data->coccodrilli[i].tid, NULL);
            }
            game_data->coccodrilli[i].tid = 0;
        }
    }
    
    pthread_mutex_lock(&game_data->bullets_mutex);  //accesso esclusivo al mutex dei proiettili
    /*ciclo for che scorre per tutti i proiettili*/
    for (int i = 0; i < MAX_BULLETS; i++) {
         /*se il tid è valido*/
        if (game_data->active_bullets[i].tid != 0) {
             /*terminiamo il thread del proiettile*/
            result = pthread_kill(game_data->active_bullets[i].tid, 0);
            /*se la kill ha avuto successo*/
            if (result == 0) {
                pthread_cancel(game_data->active_bullets[i].tid);
                pthread_join(game_data->active_bullets[i].tid, NULL);
            }
            game_data->active_bullets[i].tid = 0;
        }
    }
    pthread_mutex_unlock(&game_data->bullets_mutex); //rilasciamo il mutex dei proiettili
    
    pthread_mutex_lock(&game_data->grenades_mutex);  //accesso esclusivo al mutex delle granate
    /*ciclo for che scorre per tutte le granate*/
    for (int i = 0; i < MAX_GRENADE; i++) {
        /*se il tid è valido*/
        if (game_data->active_grenades[i].tid != 0) {
            /*terminiamo il thread delle granate*/
            result = pthread_kill(game_data->active_grenades[i].tid, 0);
            /*se la kill ha avuto successo*/
            if (result == 0) {
                pthread_cancel(game_data->active_grenades[i].tid);
                pthread_join(game_data->active_grenades[i].tid, NULL);
            }
            game_data->active_grenades[i].tid = 0;
        }
    }
    pthread_mutex_unlock(&game_data->grenades_mutex);  //rilasciamo il mutex delle granate
   
    pthread_mutex_lock(&game_data->buffer->mutex); //accesso esclusivo al mutex condiviso 
    /*ciclo while che scorre fino a quando ci sono elementi nel buffer*/
    while (game_data->buffer->count > 0) {
        sem_trywait(&game_data->buffer->full); //impostiamo un semaforo non bloccante per l'accesso al buffer
        game_data->buffer->tail = (game_data->buffer->tail + 1) % BUFFER_SIZE; //andiamo avanti nella coda
        game_data->buffer->count--; //diminuiamo il contatore di elementi nel buffer
        sem_post(&game_data->buffer->empty); //rilasciamo il semaforo
    }
    /*impostiamo a 0 testa e coda del buffer*/
    game_data->buffer->head = 0;
    game_data->buffer->tail = 0;
    pthread_mutex_unlock(&game_data->buffer->mutex); //rilasciamo il mutex condiviso
}

/**
 * funzione che gestisce la schermata di fine gioco
 * @param game_data puntatore alla struttura dati del gioco 
 * @param message stringa contenente il messaggio 
 * @return true se l'utente vuole ricominciare (preme S), false se vuole uscire (preme E)
 */
bool exit_game (Game_data* game_data) {
    int input = 0; //varaibile per prendere l'input dell'utente 
    bool flag = false; //variabile booleana per dire se il gioco deve riniziare o terminare
    int start_y, start_x; //variabili per le coordinate di inizio della scritta
    char* exit_instruction; //stringa per le istruzioni 
    /*dichiarazione sprite*/
    wchar_t *sprite_sconfitta[ALTEZZA_SPRITE] = {
        L"██╗░░██╗░█████╗░██╗  ██████╗░███████╗██████╗░░██████╗░█████╗░██╗",
        L"██║░░██║██╔══██╗██║  ██╔══██╗██╔════╝██╔══██╗██╔════╝██╔══██╗██║",
        L"███████║███████║██║  ██████╔╝█████╗░░██████╔╝╚█████╗░██║░░██║██║",
        L"██╔══██║██╔══██║██║  ██╔═══╝░██╔══╝░░██╔══██╗░╚═══██╗██║░░██║╚═╝",
        L"██║░░██║██║░░██║██║  ██║░░░░░███████╗██║░░██║██████╔╝╚█████╔╝██╗",
        L"╚═╝░░╚═╝╚═╝░░╚═╝╚═╝  ╚═╝░░░░░╚══════╝╚═╝░░╚═╝╚═════╝░░╚════╝░╚═╝",
    };

    pthread_mutex_lock(&screen_mutex); //accesso escluisvo al mutex di ncurses
    clear(); //puliamo tutto lo shermo
    pthread_mutex_unlock(&screen_mutex); //rilasciamo il mutex di ncurses
   
    /*calcoliamo la posizione di partenza per far scrivere il messaggio*/
    start_y = (GAME_HEIGHT - ALTEZZA_SPRITE) / 2;
    start_x = (GAME_WIDTH - wcslen(sprite_sconfitta[0])) / 2;
    
    pthread_mutex_lock(&screen_mutex); //accesso eslcusivo al mutex di ncurses

    attron(COLOR_PAIR(4)); //attiviamo il colore della scritta hai perso color rosso
    
    /*ciclo for che scorre per l'altezza della sprite*/
    for (int i = 0; i < ALTEZZA_SPRITE; i++) {
        mvaddwstr(start_y + i, start_x, sprite_sconfitta[i]); //stampiamo il messaggio 
    }
        
    /*facciamo stampare le istruzioni per continuare il gioco */
    exit_instruction = "Premi S per ricominciare o E per uscire";
    mvprintw(start_y + ALTEZZA_SPRITE + 4, (GAME_WIDTH - strlen(exit_instruction)) / 2, "%s", exit_instruction);
    
    attroff(COLOR_PAIR(4)); //disattiviamo il colore della scritta
    
    refresh();
    timeout(-1);

    pthread_mutex_unlock(&screen_mutex); //rilasciamo il mutex di ncursses
    
    /*do while che cila fino a quando l'utente non preme o s o e per riprendere o chiudere il gioco*/
    do {
        pthread_mutex_lock(&screen_mutex); //accesso eslcusivo al mutex di ncurses
        
        input = wgetch(stdscr); //prende l'input

        pthread_mutex_unlock(&screen_mutex); //rilasciamo il mutex di ncurses
    } while (input != 's' && input != 'e' && input != 'S' && input != 'E');

    /*controlliamo se l'utente vuole ricominciare il giococ*/
    if (input == 's' || input == 'S') {
        flag = true; //impostiamo la falg a true per dire che vuole ricominciare la partita
        
        pthread_mutex_lock(&screen_mutex); //accesso esclusivo al mutex di ncurses
       
        clear();
        refresh();
        
        pthread_mutex_unlock(&screen_mutex); //rilasciamo il mutex di ncurses
    } 
    /*se l'utente vuole uscire dal gioco*/
    else if (input == 'e' || input == 'E') {
        flag = false; //impostiamo a false la flag per dire che vuole uscire 
    }
    
    return flag; //facciamo ritornare la flag 
}

/**
 * funzione che gestisce la schermata di vittoria
 * @param game_data puntatore alla struttura dati del gioco (non utilizzato attualmente)
 * @param message stringa contenente il messaggio personalizzato da mostrare sotto l'ASCII art
 * @return true se l'utente vuole rigiocare (preme S), false se vuole uscire (preme E)
 */
bool victory (Game_data* game_data) {
    int input = 0; //varaibile per prendere l'input dell'utente 
    bool flag = false; //variabile booleana per dire se il gioco deve riniziare o terminare
    int start_y, start_x; //variabili per le coordinate di inizio della scritta
    char* victory_instruction; //stringa per le istruzioni 
    /*dichiarazione sprite*/
    wchar_t *sprite_vittoria[ALTEZZA_SPRITE_V] = {
        L"██╗░░██╗░█████╗░██╗  ██╗░░░██╗██╗███╗░░██╗████████╗░█████╗░██╗",
        L"██║░░██║██╔══██╗██║  ██║░░░██║██║████╗░██║╚══██╔══╝██╔══██╗██║",
        L"███████║███████║██║  ╚██╗░██╔╝██║██╔██╗██║░░░██║░░░██║░░██║██║",
        L"██╔══██║██╔══██║██║  ░╚████╔╝░██║██║╚████║░░░██║░░░██║░░██║╚═╝",
        L"██║░░██║██║░░██║██║  ░░╚██╔╝░░██║██║░╚███║░░░██║░░░╚█████╔╝██╗",
        L"╚═╝░░╚═╝╚═╝░░╚═╝╚═╝  ░░░╚═╝░░░╚═╝╚═╝░░╚══╝░░░╚═╝░░░░╚════╝░╚═╝"
    };
    
    pthread_mutex_lock(&screen_mutex); //accesso escluisvo al mutex di ncurses
    clear(); //puliamo tutto lo shermo
    pthread_mutex_unlock(&screen_mutex); //rilasciamo il mutex di ncurses
    
    /*calcoliamo la posizione di partenza per far scrivere il messaggio*/   
    start_y = (GAME_HEIGHT - ALTEZZA_SPRITE_V) / 2;
    start_x = (GAME_WIDTH - wcslen(sprite_vittoria[0])) / 2;
    
    pthread_mutex_lock(&screen_mutex); //accesso eslcusivo al mutex di ncurses
   
    attron(COLOR_PAIR(1)); //attiviamo il colore della scritta hai vinto color verde
    
    /*ciclo for che scorre per l'altezza della sprite*/
    for (int i = 0; i < ALTEZZA_SPRITE_V; i++) {
        mvaddwstr(start_y + i, start_x, sprite_vittoria[i]); //stampiamo il messaggio 
    }
   
    /*facciamo stampare le istruzioni per continuare il gioco */
    victory_instruction = "Premi S per rigiocare o E per uscire";
    mvprintw(start_y + ALTEZZA_SPRITE_V + 4, (GAME_WIDTH - strlen(victory_instruction)) / 2, "%s", victory_instruction);
    
    attroff(COLOR_PAIR(1)); //disattiviamo il colore della scritta
    
    refresh();
    timeout(-1);

    pthread_mutex_unlock(&screen_mutex); //rilasciamo il mutex di ncursses
    
    /*do while che cila fino a quando l'utente non preme o s o e per riprendere o chiudere il gioco*/   
    do {
        pthread_mutex_lock(&screen_mutex); //accesso eslcusivo al mutex di ncurses
        
        input = wgetch(stdscr); //prende l'input
        
        pthread_mutex_unlock(&screen_mutex); //rilasciamo il mutex di ncurses

    } while (input != 's' && input != 'e' && input != 'S' && input != 'E');

    /*controlliamo se l'utente vuole ricominciare il giococ*/
    if (input == 's' || input == 'S') {
        flag = true; //impostiamo la falg a true per dire che vuole ricominciare la partita

        pthread_mutex_lock(&screen_mutex); //accesso esclusivo al mutex di ncurses
        clear();
        refresh();

        pthread_mutex_unlock(&screen_mutex); //rilasciamo il mutex di ncurses
    } 
    /*se l'utente vuole uscire dal gioco*/
    else if (input == 'e' || input == 'E') {
        flag = false; //impostiamo a false la flag per dire che vuole uscire 
    }

    return flag; //facciamo ritornare la flag
}

/**
 * funzione che gestisce il menu iniziale del gioco 
 * @return true se l'utente vuole iniziare il gioco (preme S), false se vuole uscire (preme E)
 */
bool menu_iniziale () {
    bool flag; //variabile booleana per dire se il gioco deve riniziare o terminare
    int input = 0;  //varaibile per prendere l'input dell'utente 
    char *messaggio_finale; //stringa per le istruzioni
    int titolo_y, titolo_x; //variabili per le coordinate della posizione del titolo
    int regole_y; //variabile per l'altezza delle regole del gioco
    int messaggio_y; //variabile per l'altezza del messaggio finale
    /*dichiarazione sprite*/
    wchar_t *sprite_titolo[] = {
        L"███████╗██████╗░░█████╗░░██████╗░░██████╗░███████╗██████╗░██╗",
        L"██╔════╝██╔══██╗██╔══██╗██╔════╝░██╔════╝░██╔════╝██╔══██╗██║",
        L"█████╗░░██████╔╝██║░░██║██║░░██╗░██║░░██╗░█████╗░░██████╔╝██║",
        L"██╔══╝░░██╔══██╗██║░░██║██║░░╚██╗██║░░╚██╗██╔══╝░░██╔══██╗╚═╝",
        L"██║░░░░░██║░░██║╚█████╔╝╚██████╔╝╚██████╔╝███████╗██║░░██║██╗",
        L"╚═╝░░░░░╚═╝░░╚═╝░╚════╝░░╚═════╝░░╚═════╝░╚══════╝╚═╝░░╚═╝╚═╝"
    };
    /*dichiarazione regole del gioco*/
    char *regole[] = {
        "REGOLE DEL GIOCO:",
        "- Muoviti con le frecce direzionali (↑ ↓ ← →)",
        "- Premi P per mettere in pausa",
        "- Chiudi tutte le tane per vincere",
        "- Premi SPAZIO per sparare granate e difenderti dai proiettili",
        "- Perdi una vita quando:",
        "  * Vieni colpito da un proiettile",
        "  * Cadi in acqua",
        "  * Entri in una tana già chiusa",
        "- Hai 5 vite a disposizione",
        "- Completa tutte le manche per vincere!"
    };
    /*dichiarazione del messaggio finale*/
    messaggio_finale = "Premi S per iniziare... Premi E per uscire...";

    clear();

    box(stdscr, 0, 0); //disegniamo il bordo dello schermo
    
    /*calcoliamo la posizione del titolo*/
    titolo_y = 5;
    titolo_x = (GAME_WIDTH - wcslen(sprite_titolo[0])) / 2;
    
    attron(COLOR_PAIR(1)); //attiviamo il colore per l titolo verde
    
    /*ciclo for che scorre per tutta l'altezza del titolo*/
    for (int i = 0; i < 6; i++) {
        mvaddwstr(titolo_y + i, titolo_x, sprite_titolo[i]); //stampiamo il titolo
    }

    attroff(COLOR_PAIR(1)); //disattiviamo il titolo
    
    regole_y = titolo_y + 8; //calcoliamo la posizione della prima riga delle regole
    
    attron(COLOR_PAIR(3));//attiviamo il colore delle regole verde acqua
    
    mvprintw(regole_y, (GAME_WIDTH - strlen(regole[0])) / 2, "%s", regole[0]); //stampiamo la prima riga delle regole
    
    attroff(COLOR_PAIR(3)); //disattiviamo il colore delle regole
    
    regole_y += 2; //calcoliamo la posizione delle altre regole a distanza di 2

    attron(COLOR_PAIR(8)); //attiviamo il colore delle regole in ciano
    
    /*ciclo for che scorre per l'altezza delle regole*/
    for (int i = 1; i < 11; i++) {
        mvprintw(regole_y + i - 1, (GAME_WIDTH - strlen(regole[i])) / 2, "%s", regole[i]); //stampiamo le altre regole
    }

    attroff(COLOR_PAIR(8)); //disattiviamo il colore delle regole
    
    messaggio_y = regole_y + 12; //calcoliamo la posizione del messaggio di istruzioni
    
    attron(COLOR_PAIR(4)); //attiviamo il colore rosso del messaggio di istruzioni 

    mvprintw(messaggio_y, (GAME_WIDTH - strlen(messaggio_finale)) / 2, "%s", messaggio_finale); //stampiamo le istruzioni

    attroff(COLOR_PAIR(4)); //disattiviamo il colore 
    
    refresh();
    timeout(-1);
    
    /*ciclo for che scorre fino a quando l'utente non preme un tasto per partire o uscire*/
    do {
        pthread_mutex_lock(&screen_mutex); //accesso eslcusivo al mutex di ncurses
        
        input = wgetch(stdscr); //prende l'input
        
        pthread_mutex_unlock(&screen_mutex); //rilasciamo il mutex di ncurses
    } while (input != 's' && input != 'e' && input != 'S' && input != 'E');

    /*se l'utente vuole iniziare il gioco*/
    if (input == 's' || input == 'S'){
        flag = true; //settimo la variabile a true per dire che vuole iniziare 
    } 
    /*se l'utente vuole uscire dal gioco*/
    else if (input == 'e' || input == 'E'){
        flag = false; //settiamo la variabile a false per dire che vuole uscire
    }
    
    /*puliamo lo schermo prima di iniziare il gioco*/
    clear();
    refresh();

    return flag; //ritorniamo la flag
}