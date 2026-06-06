#include "game_handlers_thread.h"

/**
 * funzione per getsire i messaggi del movimento della rana, aggiornare la sua posizione e controllare se muore
 * @param game_data puntatore alla struttura dati del gioco
 * @param msg puntatore al messaggio che contiene i dati della rana
 * @param prev_x_rana puntatore alla coordinata x precedente della rana
 * @param prev_y_rana puntatore alla coordinata y precedente della rana
 */
void handle_frog_message (Game_data* game_data, Messaggio* msg, int* prev_x_rana, int* prev_y_rana) {
    int new_x , new_y; //variabili per le nuove coordinate x e y della rana
    bool in_safe_zone = true; //variabile booleana per controllare se la rana è nella safe zone
    bool out_of_bounds = false; //variabile booleana per controllare se la rana è fuori dai bordi
    
    pthread_mutex_lock(&game_data->frog_mutex); //accesso eslcusivo al mutex
    
    /*se la rana non si è mossa*/
    if (msg->x == 0 && msg->y == 0) {
        /*usciamo dalla funzione*/
        pthread_mutex_unlock(&game_data->frog_mutex);
        return;
    }
    
    /*calcoliamo la nuova posizione della rana*/
    new_x = game_data->frog_copy.x + msg->x;
    new_y = game_data->frog_copy.y + msg->y;
    
    /*controlliamo se la rana è nella safe zone*/
    in_safe_zone = (new_y >= SAFE_ZONE_DOWN && new_y <= SAFE_ZONE_DOWN_2);
    
    /*controlliamo se la rana si trova dentro lo schermo*/
    out_of_bounds = (new_x < 0 || new_x >= GAME_WIDTH - LARGHEZZA_RANA || new_y < 0 || new_y >= MAP_HEIGHT - ALTEZZA_RANA);
    
    /*se la rana è dentro lo schermo oppure è nella safe zone*/
    if (!out_of_bounds || in_safe_zone) {
        /*se è nella safe zone*/
        if (in_safe_zone) {
            /*se è rientra nei limiti dei bordi della safe zone*/
            if (new_x >= 0 && new_x < GAME_WIDTH - LARGHEZZA_RANA) {
                game_data->frog_copy.x = new_x; //aggiorniamo la coordinata x
            }
            /*se è rientra nei limiti dei bordi della safe zone*/
            if (new_y >= SAFE_ZONE_DOWN && new_y <= MAP_HEIGHT - ALTEZZA_RANA) {
                game_data->frog_copy.y = new_y; //aggiorniamo la coordinata y
            }
        /*altrimenti se non è nella safe zone ma dentro lo schermo*/
        } 
        else {
            game_data->frog_copy.x = new_x; //aggiorniamo la cooridnata x
            game_data->frog_copy.y = new_y; //aggiorniamo la cooridnata y
        }
    }
    
    /*verifichiamo se la rana è su un coccodrillo*/
    frog_with_croc(&game_data->frog_copy, game_data->coccodrilli);
    
    /*se la rana è morta, quindi si trova al di fuori d*i bordi e non è in una safe zone, oppure si trova nel fiume*/
    /*if ((out_of_bounds && !in_safe_zone) || river(game_data->frog_copy)) {
        /*assegniamo alle coordinate della rana -1*/
     /*   *prev_x_rana = -1; 
        *prev_y_rana = -1;

        pthread_mutex_unlock(&game_data->frog_mutex); //rilasciamo il mutex

        handle_frog_death(game_data); //richiamiamo la funzione per gestire la morte della rana
        return;
    }*/
    
    /*aggiorniamo le coordinate precedenti*/
    *prev_x_rana = game_data->frog_copy.x;
    *prev_y_rana = game_data->frog_copy.y;

    pthread_mutex_unlock(&game_data->frog_mutex); //rilasciamo il mutex
}

/**
 * funzione per gestire i messaggi dei coccodrilli, aggiornare la loro posizione e verificare se la rana è sopra di essi per il trasporto
 * @param game_data puntatore alla struttura dati del gioco
 * @param msg puntatore al messaggioche codntiene i el coccodrillo
 * @param prev_x_cocc puntatore alla coordinata x precedente del coccodrillo
 * @param prev_y_cocc Pùpuntatore alla coordinata y precedente del coccodrillo
 */
void handle_crocodile_message (Game_data* game_data, Messaggio* msg, int* prev_x_cocc, int* prev_y_cocc) {
    int new_frog_x; //variabile per la coordinata x della rana
    /*aggiorniamo la posizione del coccodrillo*/
    game_data->coccodrilli[msg->index] = *msg;
    game_data->coccodrilli[msg->index].index = msg->index;
    *prev_x_cocc = msg->x;
    *prev_y_cocc = msg->y;
    
    pthread_mutex_lock(&game_data->frog_mutex); //accesso esclusivo al mutex 
    
    /*se la rana è sopra il coccodrillo e l'indice corrisponde al coccodrillo*/
    if (game_data->frog_copy.on_croc && game_data->frog_copy.croc_index == msg->index) {
        new_frog_x = game_data->frog_copy.x + msg->direzione; //aggiorniamo la posizione della rana
        
        /*se la rana esce dallo schermo*/
       /* if (new_frog_x < 0 || new_frog_x >= GAME_WIDTH - LARGHEZZA_RANA) {
            pthread_mutex_unlock(&game_data->frog_mutex); //rilasciamo il mutex

            handle_frog_death(game_data); //richiamiamo la funzione per la gestione della morte della rana
            return;
        } */
        
            game_data->frog_copy.x = new_frog_x; //aggiorniamo la posizione della rana
    }
    
    /*se la rana p sul coccodrillo ma la collisione non è ancora stata rilevata*/
    if (!game_data->frog_copy.on_croc && 
        msg->carry &&
        game_data->frog_copy.y >= msg->y && 
        game_data->frog_copy.y < msg->y + ALTEZZA_COCCODRILLO &&
        game_data->frog_copy.x + LARGHEZZA_RANA > msg->x && 
        game_data->frog_copy.x < msg->x + LARGHEZZA_COCCODRILLO) {
        
        /*settiamo a true per dire che la rana è sul coccodrillo, l'indice e la y sempre del coccodrillo*/
        game_data->frog_copy.on_croc = true;
        game_data->frog_copy.croc_index = msg->index; 
        game_data->frog_copy.y = msg->y; 
    }
    
    pthread_mutex_unlock(&game_data->frog_mutex); //rilasciamo il mutex
}

/**
 * funzione per gestire il respawn dei coccodrilli che si occupa di terminare il thread e creare quello nuovo con nuovi parametri
 * @param game_data puntatore alla struttura dati del gioco
 * @param msg puntatore al messaggio contenente indice del coccodrillo da rispawnare
 * @param croc_tids array dei thread ID dei coccodrilli
 * @param croc_args array degli argomenti per i thread dei coccodrilli
 * @param direzioni array delle direzioni dei coccodrilli
 */
void handle_respawn_message (Game_data* game_data, Messaggio* msg, pthread_t* croc_tids, Thread_args* croc_args, int* direzioni) {
    int indice = msg->index; //assegniamo alla variabile indice, l'indice del coccodrillo
    int result = 0; //variabile per verificare se bisogna terminare il thread 

    /*verifichiamo se il tid del coccodrillo è valido*/
    if (croc_tids[indice] != 0) {
        result = pthread_kill(croc_tids[indice], 0); //killiamo il thread 

        /*se la kill ha avuto successo*/
        if (result == 0) {
            /*attendiamo la terminazione*/
            pthread_cancel(croc_tids[indice]);
            pthread_join(croc_tids[indice], NULL);
        }

        croc_tids[indice] = 0; //resettiamo il tid 
    }
    
    usleep(10000); // 10ms
    
    /*creiamo un nuovo thread per il nuovo coccodrillo*/
    croc_args[indice].buffer = game_data->buffer;
    croc_args[indice].index = indice;
    croc_args[indice].direzione = direzioni[indice];

    if (indice >= 9){
        croc_args[indice].velocita = croc_args[indice-9].velocita;
    } else {
        croc_args[indice].velocita = croc_args[indice].velocita;
    }

    croc_args[indice].flag = true;
    
    /*se la creazione del thread ha avuto successo*/
    if (pthread_create(&croc_tids[indice], NULL, crocodile_thread, &croc_args[indice]) == 0) {
        game_data->coccodrilli[indice].tid = croc_tids[indice]; //assegniamo il tid al coccodrillo 
    } 
    /*altrimenti comunichiamo l'errore*/
    else {
        perror("Errore nella creazione del thread coccodrillo respawn");
        croc_tids[indice] = 0;
    }
}

/**
 * funzione per la creazione di nuovi proiettili e l'avvio del relativo thread
 * @param game_data puntatore alla struttura dati del gioco
 * @param msg puntatore al messaggio contenente posizione e direzione del proiettile
 * @param bullet_tids array dei thread ID dei proiettili
 * @param bullet_count puntatore al contatore dei proiettili attivi
 */
void handle_create_bullet (Game_data* game_data, Messaggio* msg, pthread_t* bullet_tids, int* bullet_count) {
    int free_slot = -1; //variabile per trovare lo slot libero nell'array
    bool already_has_bullet; //variabile booleana per segnare se il coccodrillo ha gia sparato o no 
    Thread_args* bullet_args; //variabile per creare gli argomenti del proiettile da passare al thread
    Messaggio* bullet_data; //variabile per creare il messaggio
    Bullet_params* params ; //variabile per passare messaggio e argomenti al thread
    
    pthread_mutex_lock(&game_data->bullets_mutex); //accesso escluivo al mutex 
    
    /*ciclo for che scorre per tutti i proittili*/
    for (int i = 0; i < MAX_BULLETS; i++) {
        /*se il proiettile non è attivo*/
        if (!game_data->active_bullets[i].is_active) {
            free_slot = i; //assegniamo alla varibile per gli slot l'indice di quel proiettile
            break;
        }
    }
    
    /*se la posizione dell'array è valida*/
    if (free_slot != -1) {
        
        already_has_bullet = false; //settiamo a false la variabile del coccodrillo per dire che non ha sparato

        /*ciclo for che scorre per tutti i proiettili*/
        for (int i = 0; i < MAX_BULLETS; i++) {
            /*se il proiettile è attivo ed è stato il coccodrillo corrente a sparare*/
            if (game_data->active_bullets[i].is_active && game_data->active_bullets[i].index == msg->index) {
                already_has_bullet = true; //settiamo la variabile a true per dire che ha sparato
                break;
            }
        }
        
        /*se il coccodrillo non ha ancora sparato proiettili*/
        if (!already_has_bullet) {
            /*inizializziamo il proiettile*/
            game_data->active_bullets[free_slot].is_active = true;
            game_data->active_bullets[free_slot].x = msg->x;
            game_data->active_bullets[free_slot].y = msg->y;
            game_data->active_bullets[free_slot].direzione = msg->direzione;
            game_data->active_bullets[free_slot].oggetto = ID_BULLET;
            game_data->active_bullets[free_slot].index = msg->index;
            
            (*bullet_count)++; //aumetiamo il contatore dei proiettili attivi 
            
            /*allochiamo e riempiamo lo spazio per la struttra dei proiettili*/
            bullet_args = malloc(sizeof(Thread_args));
            bullet_args->buffer = game_data->buffer;
            bullet_args->index = msg->index;
            bullet_args->direzione = msg->direzione;
            bullet_args->velocita = BULLET_SPEED;
            
            /*allochiamo e riempiamo lo spazio per i messaggi*/
            bullet_data = malloc(sizeof(Messaggio));
            *bullet_data = game_data->active_bullets[free_slot];
            bullet_data->x = msg->x;
            bullet_data->y = msg->y;
            
            /*allochiamo e mettiamo argomenti e messaggi dei proiettili*/
            params = malloc(sizeof(Bullet_params));
            params->args = bullet_args;
            params->bullet_info = bullet_data;
            
            /*se la creazione del thread è andata a buon fine*/
            if (pthread_create(&bullet_tids[free_slot], NULL, bullet_thread, params) == 0) {
                game_data->active_bullets[free_slot].tid = bullet_tids[free_slot]; //assegniamo il tid 
            } 
            /*altrimenti */
            else {
                /*liberiamo e puliamo lo spazio */
                free(bullet_args);
                free(bullet_data);
                free(params);
                
                game_data->active_bullets[free_slot].is_active = false; //settiamo che il proiettile non è più attivo 
                (*bullet_count)--; //dimminuiamo il contatore dei proiettili attivi
            }
        }
    }
    
    pthread_mutex_unlock(&game_data->bullets_mutex); //rilasciamo il mutex
}

/**
 * funzione per gestire i messaggi dei proiettili, aggiornare la loro posizione e contorllare le collisioni con la rana
 * 
 * @param game_data puntatore alla struttura dati del gioco
 * @param msg puntatore al messaggio contenente nuova posizione del proiettile
 * @param bullet_tids array dei thread ID dei proiettili
 * @param bullet_count puntatore al contatore dei proiettili attivi
 */
void handle_bullet_message (Game_data* game_data, Messaggio* msg, pthread_t* bullet_tids, int* bullet_count) {
    int bullet_idx = -1; //variabile per salvare l'indice del proiettile settata a -1 perchè non esiste
    bool collision_detected = false; //variabile booleana per dire se è stata rilevata una collisione con la rana, true in caso positivo, false altrimenti
    int centro_x, centro_y; //variabili che rappresentano le coordinate della rana, per riposizionarla dopo essere stata colpita da un proiettile
    
    pthread_mutex_lock(&game_data->bullets_mutex); //accesso eclusivo al mutex
    
    /*se l'indice del messaggio è valido*/
    if (msg->index >= 0 && msg->index < NUM_CROC) {        
        /*ciclo for che scorre per tutti i proiettili*/
        for (int i = 0; i < MAX_BULLETS; i++) {
            /*se il proiettile è attivo e il tid corrisponde a quello del messaggio*/
            if (game_data->active_bullets[i].is_active && pthread_equal(game_data->active_bullets[i].tid, msg->tid)) {
                bullet_idx = i; //assegniamo l'indice corrispondente 
                break;
            }
        }
        
        /*se l'indice del proiettile è valido*/
        if (bullet_idx != -1) {
            /*aggiorniamo le coordinate del proiettile*/
            game_data->active_bullets[bullet_idx].x = msg->x;
            game_data->active_bullets[bullet_idx].y = msg->y;
            
            /*se il proiettile che ha mandato il messaggio non è attivo*/
            if (!msg->is_active) {
                /*resettiamo i dati*/
                game_data->active_bullets[bullet_idx].is_active = false;
                game_data->active_bullets[bullet_idx].x = -100;
                game_data->active_bullets[bullet_idx].y = -100;
                
                /*se ci sono dei proiettili attivi*/
                if (*bullet_count > 0) {
                    (*bullet_count)--; //decrementiamo il numero di proiettili attivi
                }
            } 
            /*altrimenti, se il proiettile che ha mandato il messaggio è attivo*/
            else {
                collision_detected = false; //settiamo a false la variabile per il controllo della collisione con la rana
                
                /*se l'accesso al mutex in modo non bloccante va a buon fine*/
                if (pthread_mutex_trylock(&game_data->frog_mutex) == 0) {

                    /*controlliamo se avviene la collisione tra proiettile e rana*/
                    if (game_data->frog_copy.x < msg->x + 1 && 
                        game_data->frog_copy.x + LARGHEZZA_RANA > msg->x &&
                        game_data->frog_copy.y < msg->y + 1 && 
                        game_data->frog_copy.y + ALTEZZA_RANA > msg->y) {
                        collision_detected = true;
                    }

                    pthread_mutex_unlock(&game_data->frog_mutex); //rilasciamo il mutex
                    
                    /*se è avvenuta la collisione tra rana e proiettile*/
                    /*if (collision_detected) {
                        
                        pthread_mutex_lock(&game_data->game_mutex); //accesso escluisvo al mutex condiviso 
                        
                        /*aggiorniamo le variabili di gioco*/
                      /*  game_data->score += POINT_BULLETS;
                        game_data->vite--;
                        reset_timer(&game_data->game_timer);
                        
                        /*riposizioniamo la rana*/
                        /*centro_x = GAME_WIDTH / 2;
                        centro_y = MAP_HEIGHT - ALTEZZA_RANA;
                        game_data->frog_copy.x = centro_x;
                        game_data->frog_copy.y = centro_y;
                        game_data->frog_copy.on_croc = false;
                        game_data->frog_copy.croc_index = -1;
                        
                        /*controlliamo se è avvenuto il game over*/
                       /* if (game_data->vite <= 0 || game_data->score <= -1000) {
                            game_data->game_over = true; //settiamo a true la variabile che controlla la fine del gioco
                        }

                        pthread_mutex_unlock(&game_data->game_mutex); //rilasciamo il mutex condiviso
                        
                        /*disattiviamo il proiettile con cui è avvenuta la collisione*/
                       /* game_data->active_bullets[bullet_idx].is_active = false;
                        game_data->active_bullets[bullet_idx].x = -100;
                        game_data->active_bullets[bullet_idx].y = -100;
                        
                        /*se ci sono proiettili attivi*/
                       /* if (*bullet_count > 0) {
                            (*bullet_count)--; //decrementiamo il numero di proiettili attivi
                        }
                        
                        pthread_mutex_unlock(&game_data->bullets_mutex); //rilasciamo il mutex
                        return;
                    }*/
                }
            }
        }
    }
    
    pthread_mutex_unlock(&game_data->bullets_mutex); //rilasciamo il mutex
}

/**
 * funzione per la creazione di nuove granate e l'avvio del relativo thread
 * @param game_data puntatore alla struttura dati del gioco
 * @param msg puntatore al messaggio contenente posizione e direzione della granata
 * @param grenade_tids array dei thread ID delle granate
 * @param grenate_count puntatore al contatore delle granate attive
 */
void handle_create_grenade(Game_data* game_data, Messaggio* msg, pthread_t* grenade_tids, int* grenade_count) {
    Thread_args* args_left; //variabile per creare gli argomenti della granata sinistra da passare al thread
    Messaggio* grenade_data_left; //variabile per creare il messaggio della granata sinistra
    Grenade_params* params_left; //variabile per passare messaggio e argomenti della granata sinistra al thread 
    Thread_args* args_right;  //variabile per creare gli argomenti della granata destra da passare al thread
    Messaggio* grenade_data_right; //variabile per creare il messaggio della granata destra
    Grenade_params* params_right; //variabile per passare messaggio e argomenti della granata destra al thread 

    int free_slot = -1; //variabile per trovare lo slot libero nell'array
    
    pthread_mutex_lock(&game_data->grenades_mutex); //accesso eslusivo al mutex
    
    /*ciclo for che scorre per tutte le granate*/
    for (int i = 0; i < MAX_GRENADE - 1; i++) {
        /*se la granata è attiva*/
        if (!game_data->active_grenades[i].is_active && !game_data->active_grenades[i+1].is_active) {
            free_slot = i; //assegniamo alla variabile per gli slot, l'indice di quella granata
            break;
        }
    }
    
    /*se la posizione dell'array è valida*/
    if (free_slot != -1) {

        pthread_mutex_lock(&game_data->frog_mutex); //accediamo al mutex della rana per prendere le coordinate corrette
        
        /*inizializziamo granata sinistra*/
        game_data->active_grenades[free_slot].is_active = true;
        game_data->active_grenades[free_slot].x = game_data->frog_copy.x;
        game_data->active_grenades[free_slot].y = game_data->frog_copy.y + 1;
        game_data->active_grenades[free_slot].oggetto = ID_GRENADE;
        game_data->active_grenades[free_slot].direzione = -1;
        game_data->active_grenades[free_slot].index = free_slot;
        
        /*inizializziamo granata destra*/
        game_data->active_grenades[free_slot+1].is_active = true;
        game_data->active_grenades[free_slot+1].x = game_data->frog_copy.x + 2;
        game_data->active_grenades[free_slot+1].y = game_data->frog_copy.y + 1;
        game_data->active_grenades[free_slot+1].oggetto = ID_GRENADE;
        game_data->active_grenades[free_slot+1].direzione = 1;
        game_data->active_grenades[free_slot+1].index = free_slot + 1;
        
        pthread_mutex_unlock(&game_data->frog_mutex); //rilasciamo il mutex
        
        *grenade_count += 2; //incrementiamo di due alla volta il numero di granate attive
        
        /*allochiamo e riempiamo lo spazio per la struttura e i messaggi delle granate che vanno verso sinistra*/
        args_left = malloc(sizeof(Thread_args));
        args_left->buffer = game_data->buffer;
        grenade_data_left = malloc(sizeof(Messaggio));
        *grenade_data_left = game_data->active_grenades[free_slot];
        
        /*allochiamo e mettiamo argomenti e messaggi delle granate che vanno verso sinistra */
        params_left = malloc(sizeof(Grenade_params));
        params_left->args = args_left;
        params_left->grenade_info = grenade_data_left;
        
        /*allochiamo e riempiamo lo spazio per la struttura e i messaggi delle granate che vanno verso destra*/
        args_right = malloc(sizeof(Thread_args));
        args_right->buffer = game_data->buffer;
        grenade_data_right = malloc(sizeof(Messaggio));
        *grenade_data_right = game_data->active_grenades[free_slot+1];
        
        /*allochiamo e mettiamo argomenti e messaggi delle granate che vanno verso destra */
        params_right = malloc(sizeof(Grenade_params));
        params_right->args = args_right;
        params_right->grenade_info = grenade_data_right;
        
        /*se la creazione del thread della granata sinistra è andata a buon fine*/
        if (pthread_create(&grenade_tids[free_slot], NULL, grenade_thread, params_left) == 0) {
            game_data->active_grenades[free_slot].tid = grenade_tids[free_slot]; //assegniamo il tid
        } 
        /*altrimenti*/
        else {
            /*liberiamo e puliamo lo spazio*/
            free(args_left);
            free(grenade_data_left);
            free(params_left);
        }
        
        /*se la creazione del thread della granata destra è andata a buon fine*/
        if (pthread_create(&grenade_tids[free_slot+1], NULL, grenade_thread, params_right) == 0) {
            game_data->active_grenades[free_slot+1].tid = grenade_tids[free_slot+1]; //assegniamo il tid
        } 
        /*altrimenti*/
        else {
            /*liberiamo e puliamo lo spazio*/
            free(args_right);
            free(grenade_data_right);
            free(params_right);
        }
    }
    
    pthread_mutex_unlock(&game_data->grenades_mutex); //rilasciamo il mutex
}

/**
 * funzione per gestire i messaggi delle granate, aggiornare la loro posizione e controllare se escono dai bordi
 * 
 * @param game_data puntatore alla struttura dati del gioco
 * @param msg puntatore al messaggio contenente nuova posizione della granata
 * @param grenade_tids array dei thread ID delle granate
 * @param grenade_count puntatore al contatore delle granate attive
 */
void handle_grenade_message(Game_data* game_data, Messaggio* msg, pthread_t* grenade_tids, int* grenade_count) {
    int grenade_idx = -1; //variabile per salvare l'indice della granata settato a -1 perchè non esiste ancora
    
    pthread_mutex_lock(&game_data->grenades_mutex); //accesso eslcusivo al mutex

    /*ciclo for che scorre per tutte le granate*/
    for (int i = 0; i < MAX_GRENADE; i++) {
        /*se la garanata è attiva e il tid corrisponde a quello del messaggio*/
        if (pthread_equal(game_data->active_grenades[i].tid, msg->tid)) {
            grenade_idx = i; //assegniamo l'indice corrente
            break;
        }
    }
    
    /*se l'indice della granata è valido*/
    if (grenade_idx != -1) {
        /*aggiorniamo le coordinate della granata*/
        game_data->active_grenades[grenade_idx].x = msg->x;
        game_data->active_grenades[grenade_idx].y = msg->y;
        game_data->active_grenades[grenade_idx].is_active = true;
    }
    
    /*controlliamo se la granata non è uscita dai bordi*/
    if (msg->x <= 0 || msg->x >= GAME_WIDTH) {
        /*se l'indice è valido*/
        if (grenade_idx != -1) {
            /*disattiviamo la granata*/
            game_data->active_grenades[grenade_idx].is_active = false;
            game_data->active_grenades[grenade_idx].x = -100;
            game_data->active_grenades[grenade_idx].y = -100;
            
            /*se ci sono granate attive*/
            if (*grenade_count > 0) {
                (*grenade_count)--; //decrementiamo il numero di granate attive
            }
        }
    }
    
    pthread_mutex_unlock(&game_data->grenades_mutex); //rilasciamo il mutex
}

/**
 * funzione per gestire la morte della rana
 * @param game_data puntatore alla struttura dati del gioco
 */
void handle_frog_death (Game_data* game_data) {
    int centro_x, centro_y; //variabili per le coordinate centrali della rana

    pthread_mutex_lock(&game_data->game_mutex); //accesso esclusivo al mutex condiviso
    
    /*aggiornamento delle variabili di gioco*/
    game_data->score += POINT_WATER;
    game_data->vite--;
    reset_timer(&game_data->game_timer);
    
    /*aggiorniamo le coordinate della rana*/
    centro_x = GAME_WIDTH / 2;
    centro_y = MAP_HEIGHT - ALTEZZA_RANA;
    
    game_data->frog_copy.x = centro_x;
    game_data->frog_copy.y = centro_y;
    game_data->frog_copy.on_croc = false;
    game_data->frog_copy.croc_index = -1;
    
    /*controlliamo se è veriicata la condzione di game over*/
    if (game_data->vite <= 0 || game_data->score <= -1000) {
        game_data->game_over = true; //settiamo a true la variabile per dire che il gioco è terminato
    }
    
    pthread_mutex_unlock(&game_data->game_mutex); //rilasciamo il mutex condiviso
}

/**
 * funzione per gestire il timer
 * @param game_data puntatore alla struttura dati del gioco
 */
void handle_timeout (Game_data* game_data) {
    int centro_x, centro_y; //variabili per la posizione centrale della rana

    pthread_mutex_lock(&game_data->game_mutex); //accesso esclusiivo al mutex condiviso
    
    /*aggiorniamo le variabili di gioco*/
    game_data->score += POINT_TIME;
   // game_data->vite--;
    
    /*riposizioniamo la rana*/
    centro_x = GAME_WIDTH / 2;
    centro_y = MAP_HEIGHT - ALTEZZA_RANA;
    
    game_data->frog_copy.x = centro_x;
    game_data->frog_copy.y = centro_y;
    game_data->frog_copy.on_croc = false;
    game_data->frog_copy.croc_index = -1;
    
    /*controlliamo se si è verificata la condizione di game over*/
    if (game_data->vite <= 0 || game_data->score <= -1000) {
        game_data->game_over = true; //settiamo a true la variabile per dire che è finito il gioco
    }
    
    reset_timer(&game_data->game_timer); //richiamiamo la funzione per far ripartire il tempo
    
    pthread_mutex_unlock(&game_data->game_mutex); //rilasciamo il mutex condiviso
}

/**
 * funzione per la gestione della pausa
 * @param game_data puntatore alla struttura dati del gioco
 */
void handle_pause(Game_data* game_data) {
    int pause_y, pause_x1, pause_x2; //variabili per l'altezza della scritta, per la prima scritta e per la seconda scritta
    time_t pause_start_time; //variabile per registrare il tempo per poi resettare il timer
    int resume_key; //input per far ripartire il gioco
    time_t pause_duration; //variabile per calcolare la durata della pausa
    pthread_mutex_lock(&pause_mutex); //accesso eslcusivo al mutex per la pausa
    
    
    game_data->paused = true; //settiamo a true la variabile che tiene conto della pausa
    
    pause_start_time = time(NULL); //inizia a contare il tempo

    pthread_mutex_lock(&screen_mutex); //acesso esclusivo per il mutex di ncurses
    
    /*settiamo la posizione del messaggio*/
    pause_y = GAME_HEIGHT / 2;
    pause_x1 = (GAME_WIDTH - 19) / 2; // "GIOCO IN PAUSA" ha 19 caratteri
    pause_x2 = (GAME_WIDTH - 23) / 2; // "Premi P per continuare" ha 23 caratteri
    
    /*controlliamo che il messaggio sia dentro i bordi*/
    if (pause_y >= 0 && pause_y < GAME_HEIGHT && pause_x1 >= 0 && pause_x1 + 19 < GAME_WIDTH) {
        attron(COLOR_PAIR(4) | A_BOLD); //settiamo il colore
        mvprintw(pause_y, pause_x1, "GIOCO IN PAUSA"); //scritta 
        attroff(COLOR_PAIR(4) | A_BOLD); //disattiviamo il colore
    }

    /*controlliamo che il messaggio sia dentro i bordi*/
    if (pause_y + 2 >= 0 && pause_y + 2 < GAME_HEIGHT && pause_x2 >= 0 && pause_x2 + 23 < GAME_WIDTH) {
        attron(COLOR_PAIR(4) | A_BOLD); //settiamo il colore
        mvprintw(pause_y + 2, pause_x2, "Premi P per continuare"); //scritta 
        attroff(COLOR_PAIR(4) | A_BOLD); //disattiviamo il colore
    }
    
    refresh();

    pthread_mutex_unlock(&screen_mutex); //rilasciamo il mutex di ncurses
    
    /*ciclo do while che continua fino a che non viene preso l'input corretto*/
    do {
        pthread_mutex_lock(&screen_mutex); //accesso esclusivo al mutex di ncurses

        resume_key = wgetch(stdscr); //prende l'input

        pthread_mutex_unlock(&screen_mutex); //rilasciamo il mutex di ncurses
    } while (resume_key != 'p' && resume_key != 'P');

    game_data->paused = false; //settiamo la variabile della pausa a false per dire che non siamo più in pausa
    
    pause_duration = time(NULL) - pause_start_time; //calcoliamo quanto tempo è passato dalla pausa 

    game_data->game_timer.last_update += pause_duration; //settiamo il tempo del timer in modo corretto
    
    pthread_mutex_unlock(&pause_mutex); //rilasciamo il mutex per la pausa
}

/**
 * funzione per controllare se la rana è entrata nelle tane, quante vite ha e il punteggio
 * @param game_data puntatore alla struttura dati del gioco
 */
void check_game_conditions (Game_data* game_data) {
    int centro_x, centro_y; //variabili per le coordinate centrali della rana
    int burrow_index; //variabile per l'indice delle tane
    int num_tane; //variabile per il numero della tana
    Messaggio burrow_copy; //variabile per il messaggio della rana

    pthread_mutex_lock(&game_data->game_mutex); //accesso esclusivo al mutex condiviso
    
    /*se la rana si trova all'interno di una tana*/
    if (is_inside(game_data->frog_copy)) {

        num_tane = num_tana(game_data->frog_copy); //richiamiamo la funzione che restituisce il numero della tana
        
        /*se la tana è aperta e il numero di tane è valido*/
        if (game_data->flag[num_tane] == false && num_tane != 6) {
            game_data->flag[num_tane] = true; //impostiamo a true la variabile che riguarda la tana chiusa
            
            pthread_mutex_lock(&game_data->burrows_mutex); //accesso eslcusivo al mutex delle tane

            /*settiamo i dati della tana chiusa*/
            burrow_index = num_tane - 1;
            burrow_copy.oggetto = ID_BURROWS;
            burrow_copy.x = num_tane;
            burrow_copy.y = num_tane;
            game_data->closed_burrows[burrow_index] = burrow_copy;

            pthread_mutex_unlock(&game_data->burrows_mutex); //rilasciamo il mutex delle tane
            
            /*riposizioniamo la rana*/
            centro_x = GAME_WIDTH / 2;
            centro_y = MAP_HEIGHT - ALTEZZA_RANA;
            
            game_data->frog_copy.x = centro_x;
            game_data->frog_copy.y = centro_y;
            game_data->frog_copy.on_croc = false;
            game_data->frog_copy.croc_index = -1;
            
            /*aggiorniamo le variabili di gioco*/
            game_data->count_burrows++;
            game_data->score += POINT_BURROWS;
            
            /*controlliamo se la condizione di game over è verificata*/
            if (game_data->count_burrows >= NUM_BURROWS) {
                game_data->game_over = true; //se si impostiamo a true la variabile che lo segnala
            }
            
            reset_timer(&game_data->game_timer); //richiamiamo la funzione per far ripartire il timer
        } 
        /*altrimenti, se entriamo in una tana già ocupata*/
        /*else {
            /*aggiorniamo le variabili di gioco*/
          /*  game_data->score += POINT_TAKEN_BURROWS;
            game_data->vite--;

            reset_timer(&game_data->game_timer); //facciamo ripartire il timer
            
            /*continuiamo ad aggiornare le variabili di gioco*/
          //  game_data->manche++;
            
            /*riposizioniamo la rana*/
           /* centro_x = GAME_WIDTH / 2;
            centro_y = MAP_HEIGHT - ALTEZZA_RANA;
            
            game_data->frog_copy.x = centro_x;
            game_data->frog_copy.y = centro_y;
            game_data->frog_copy.on_croc = false;
            game_data->frog_copy.croc_index = -1;
        }*/
    }
    
    /*controlliamo se la condizione di game over è verificata*/
    if (game_data->vite <= 0 || game_data->score <= -1000) {
        game_data->game_over = true; //se si impostiamo a true la variabile che lo segnala
    }
    
    pthread_mutex_unlock(&game_data->game_mutex); //rilasciamo il mutex condiviso
}

/**
 * funzione che aggiorna il display del gioco ridisegnando tutti gli elementi grafici
 * @param game_data puntatore alla struttura dati del gioco contenente tutti gli oggetti da disegnare
 */
void update_display (Game_data* game_data) {
    int seconds_left; //variabile per tenere conto dei secondi rimasti
    int bar_length ; //variabile per tenere conto della lunghezza della barra del timr   
    int x_pos, y_pos; //variabili per le coordinate dei coccodrilli
    int spacing, x; //variabili per lo spazio tra le tane 
    int hearts_x, hearts_y; //variabili per le coordinate dei cuori di partenza (36)
    int heart_spacing; //variabile per lo spazio tra i cuori
    char* heart; //variabile per il carattere del cuore
    int heart_x; //variabile per la posizione di ogni singolo cuore 
    int vite_display; //variabile per le vite

    pthread_mutex_lock(&screen_mutex); //accesso escluivo al mutex di ncurses
    
    erase();

    box(stdscr, 0, 0); //richiamiamo box per disegnare i bordi dello schermo
    draw_burrows(); //richiamiamo la funzione per disegnare le tane
    draw_safety_zones(); //richiamiamo la fuznione per disegnare le zone sicure
    
    /*controlliamo se i secondi rimasti sono validi e li aggiorniamo*/
    seconds_left = game_data->game_timer.seconds_left;
    if (seconds_left < 0) seconds_left = 0;
    if (seconds_left > TIMER_DURATION) seconds_left = TIMER_DURATION;
    
    /*controlliamo se la lunghezza della barra del timer è valida e aggiorniamo*/
    bar_length = (seconds_left * (TIMER_BAR_WIDTH - 2)) / TIMER_DURATION;
    if (bar_length < 0) bar_length = 0;
    if (bar_length > TIMER_BAR_WIDTH - 2) bar_length = TIMER_BAR_WIDTH - 2;
    
    /*se sono rimasti più di 20 secondi*/
    if (seconds_left > 20) {
        attron(COLOR_PAIR(1)); //coloriamo di verde
    } 
    /*se sono rimasti più 10 secondi*/
    else if (seconds_left > 10) {
        attron(COLOR_PAIR(3)); //coloriamo di giallo
    } 
    /*se sono rimasti meno di 10 secondi*/
    else {
        attron(COLOR_PAIR(4)); //coloriamo di rosso
    }
    
    /*disegnaimo la barra del tempo*/
    mvprintw(TIMER_BAR_Y, TIMER_BAR_X, "|------------------------------|");
    mvprintw(TIMER_BAR_Y + 2, TIMER_BAR_X, "|------------------------------|");
    mvprintw(TIMER_BAR_Y + 1, TIMER_BAR_X, "|");
    for (int i = 0; i < TIMER_BAR_WIDTH - 2; i++) {
        if (i < bar_length) {
            mvaddch(TIMER_BAR_Y + 1, TIMER_BAR_X + 1 + i, '|');
        } else {
            mvaddch(TIMER_BAR_Y + 1, TIMER_BAR_X + 1 + i, ' ');
        }
    }
    mvprintw(TIMER_BAR_Y + 1, TIMER_BAR_X + TIMER_BAR_WIDTH - 1, "|");
    
    /*spegniamo i colori della barra del tempo*/
    if (seconds_left > 20) {
        attroff(COLOR_PAIR(1));
    } else if (seconds_left > 10) {
        attroff(COLOR_PAIR(3));
    } else {
        attroff(COLOR_PAIR(4));
    }
    
    /*ciclo for che scorre per tutti i coccodrilli*/
    for (int i = 0; i < NUM_CROC; i++) {
        /*se i coccodrilli sono dentro i bordi*/
        if (game_data->coccodrilli[i].x >= -LARGHEZZA_COCCODRILLO && 
            game_data->coccodrilli[i].x < GAME_WIDTH + LARGHEZZA_COCCODRILLO &&
            game_data->coccodrilli[i].y >= 0 && 
            game_data->coccodrilli[i].y < GAME_HEIGHT) {
            
            attron(COLOR_PAIR(2)); //coloriamo di verde i coccodrilli

            /*ciclo for che scorre per tutte le righe dei coccodrilli*/
            for (int row = 0; row < ALTEZZA_COCCODRILLO; row++) {
                /*ciclo for che scorre per tutte le colonne dei coccodrilli*/
                for (int col = 0; col < LARGHEZZA_COCCODRILLO; col++) {
                    /*assegniamo la posizione ai coccodrilli*/
                    x_pos = game_data->coccodrilli[i].x + col;
                    y_pos = game_data->coccodrilli[i].y + row;
                    
                    /*se i coccodrilli sono all'interno dell'area di gioco*/
                    if (x_pos >= 0 && x_pos < GAME_WIDTH && y_pos >= 0 && y_pos < GAME_HEIGHT) {
                        mvaddch(y_pos, x_pos, sprite_coccodrillo[row][col]); //li disegniamo
                    }
                }
            }

            attroff(COLOR_PAIR(2)); //disattiviamo i colori dei coccodrilli
        }
    }
    
    pthread_mutex_lock(&game_data->bullets_mutex); //accesso esclusivo al mutex dei proiettili per controllare se sono attivi

    /*ciclo for che scorre per il numero di proiettili*/
    for (int i = 0; i < MAX_BULLETS; i++) {
        /*se i proiettili stanno all'interno dell'area di gioco*/
        if (game_data->active_bullets[i].is_active && 
            game_data->active_bullets[i].x >= 0 && 
            game_data->active_bullets[i].x < GAME_WIDTH &&
            game_data->active_bullets[i].y >= 0 && 
            game_data->active_bullets[i].y < GAME_HEIGHT) {

            attron(COLOR_PAIR(3)); //attiviamo il giallo per i proiettili 

            mvaddch(game_data->active_bullets[i].y, game_data->active_bullets[i].x, 'P'); //li disegniamo

            attroff(COLOR_PAIR(3)); //disattiviamo il colore per i proiettili
        }
    }

    pthread_mutex_unlock(&game_data->bullets_mutex); //rilasciamo il mutex dei proiettili
    

    pthread_mutex_lock(&game_data->grenades_mutex); //accesso esclusivo al mutex delle granate per controllare se sono attive

    /*ciclo for che scorre per il numero di granate*/
    for (int i = 0; i < MAX_GRENADE; i++) {
        /*controlliamo se si trovano all'interno dei bordi dello schermo di gioco*/
        if (game_data->active_grenades[i].is_active && 
            game_data->active_grenades[i].x >= 0 && 
            game_data->active_grenades[i].x < GAME_WIDTH &&
            game_data->active_grenades[i].y >= 0 && 
            game_data->active_grenades[i].y < GAME_HEIGHT) {

            attron(COLOR_PAIR(4)); //attiviamo il colore rosso per le granate

            mvaddch(game_data->active_grenades[i].y, game_data->active_grenades[i].x, 'G'); //le disegniamo

            attroff(COLOR_PAIR(4)); //disattiviamo il colore delle granate
        }
    }
     
    pthread_mutex_unlock(&game_data->grenades_mutex); //rilasciamo il mutex delle granate
    
    
    pthread_mutex_lock(&game_data->frog_mutex); //accesso eslcuvo al mutex dellarana
    
    /*se la rana si trova all'interno dei bordi dell'area di gioco*/
    if (game_data->frog_copy.x >= 0 && 
        game_data->frog_copy.x < GAME_WIDTH - LARGHEZZA_RANA &&
        game_data->frog_copy.y >= 0 && 
        game_data->frog_copy.y < GAME_HEIGHT - ALTEZZA_RANA) {

        attron(COLOR_PAIR(1)); //attiviamo il colore verde per la rana

        /*ciclo for che scorre per l'altezza della rana*/
        for (int i = 0; i < ALTEZZA_RANA; i++) {
            mvprintw(game_data->frog_copy.y + i, game_data->frog_copy.x, "%s", spriteRana[i]); //disegniamo la rana
        }

        attroff(COLOR_PAIR(1)); //disattiviamo il colore della rana
    }

    pthread_mutex_unlock(&game_data->frog_mutex); //rilasciamo il mutex della rana
    
    pthread_mutex_lock(&game_data->burrows_mutex); //accesso esclusivo al mutex delle tane chiuse 

    /*ciclo for che scorre per tutte le tane*/
    for (int i = 0; i < NUM_BURROWS; i++) {
        /*controlliamo se il numero delle tane è valido*/
        if (game_data->closed_burrows[i].index != -1 && 
            game_data->closed_burrows[i].x >= 1 && 
            game_data->closed_burrows[i].x <= NUM_BURROWS) {
            
            spacing = GAME_WIDTH / NUM_BURROWS; //calcoliamo lo spazio tra una tana e l'altra

            x = (game_data->closed_burrows[i].x-1) * spacing + (spacing - LARGHEZZA_TANA) / 2; //calcoliamo la posizione di ogni singola tana
            
            /*se la posizione è corretta*/
            if (x >= 0 && x + LARGHEZZA_TANA < GAME_WIDTH) {
                attron(COLOR_PAIR(5)); //attiviamo il colore marrone delle tane 

                mvprintw(1, x, "[///////]"); //disegniamo la tana chiusa

                attroff(COLOR_PAIR(5)); //disattiviamo il colore delle tane
            }
        }
    }

    pthread_mutex_unlock(&game_data->burrows_mutex); //rilasciamo il mutex delle tane chiuse
    
    /*inizializziamo le variabili dei cuori*/
    hearts_x = 36;
    hearts_y = 40;
    heart_spacing = 3;
    heart = "<3";
    
    /*controlliamo quante vite rimangono*/
    vite_display = game_data->vite;
    if (vite_display < 0) {
        vite_display = 0;
    } 
    if (vite_display > 5) {
        vite_display = 5;
    }
    
    attron(COLOR_PAIR(4)); //attiviamo il colore rosso dei cuori

    /*ciclo for che scorre per il numero delle vite*/
    for (int i = 0; i < vite_display && i < 5; i++) {
        heart_x = hearts_x + (i * heart_spacing); //calcoliamo la posizione del cuore

        /*se la posizione è corretta*/
        if (heart_x >= 0 && heart_x + 2 < GAME_WIDTH && hearts_y >= 0 && hearts_y < GAME_HEIGHT) {
            mvprintw(hearts_y, heart_x, "%s", heart); //disegniamo il cuore
        }
    }

    attroff(COLOR_PAIR(4)); //disattiviamo il colore dei cuori
    
    /*quando perdiamo una vita*/
    if (vite_display < 5) {
        attron(A_DIM); //attiviamo l'abbassamento della luminosità del testo

        /*ciclo for che scorre per il numero delle vite*/
        for (int i = vite_display; i < 5; i++) {
            heart_x = hearts_x + (i * heart_spacing); //calcoliamo la posizione del cuore

            /*se la poszione è corretta*/
            if (heart_x >= 0 && heart_x + 2 < GAME_WIDTH && hearts_y >= 0 && hearts_y < GAME_HEIGHT) {
                mvprintw(hearts_y, heart_x, "%s", heart); //disegniamo il cuore
            }
        }

        attroff(A_DIM); //resettiamo la luminosità dei cuori
    }
    
    /*controlliamo le variabili di gioco che riguardan punti e manche attivando e disattivando i loro colori, rispettivamete azzurro e arancione*/
    if (55 >= 0 && 55 + 15 < GAME_WIDTH && 40 >= 0 && 40 < GAME_HEIGHT) {
        attron(COLOR_PAIR(7));  
        mvprintw(40, 55, "PUNTI %d ", game_data->score);
        attroff(COLOR_PAIR(7));
    }
    
    if (70 >= 0 && 70 + 12 < GAME_WIDTH && 40 >= 0 && 40 < GAME_HEIGHT) {
        attron(COLOR_PAIR(6)); 
        mvprintw(40, 70, "MANCHE %d ", game_data->manche);
        attroff(COLOR_PAIR(6));
    }

    refresh();
    
    pthread_mutex_unlock(&screen_mutex); //rilasciamo il mutex di ncurses
}