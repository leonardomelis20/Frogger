
#include "gestione_partita.h" 

// Definizione degli sprite per la barra del tempo
char sprite_timer_full[TIMER_BAR_HEIGHT][TIMER_BAR_WIDTH+1] = {
    "|------------------------------|",
    "||||||||||||||||||||||||||||||||",
    "|------------------------------|"
};

char sprite_timer_empty[TIMER_BAR_HEIGHT][TIMER_BAR_WIDTH+1] = {
    "|------------------------------|",
    "|                              |",
    "|------------------------------|"
};

// Funzione che disegna la barra del tempo
void draw_timer_bar(int seconds_left) {


    int i, j;
    int bar_length = (seconds_left * (TIMER_BAR_WIDTH - 2)) / TIMER_DURATION;
    
    // Prima pulisci l'area della barra
    for (j = 0; j < TIMER_BAR_HEIGHT; j++) {
        mvprintw(TIMER_BAR_Y + j, TIMER_BAR_X, "%*s", TIMER_BAR_WIDTH, "");
    }
    
    // Scegli il colore in base al tempo rimanente
    if (seconds_left > 20) {
        attron(COLOR_PAIR(1));  // Verde
    } else if (seconds_left > 10) {
        attron(COLOR_PAIR(3));  // Giallo
    } else {
        attron(COLOR_PAIR(4));  // Rosso
    }
    
    // Disegna la barra superiore e inferiore
    mvprintw(TIMER_BAR_Y, TIMER_BAR_X, "%s", sprite_timer_empty[0]);
    mvprintw(TIMER_BAR_Y + 2, TIMER_BAR_X, "%s", sprite_timer_empty[0]);
    
    // Disegna la barra centrale con la parte riempita
    mvprintw(TIMER_BAR_Y + 1, TIMER_BAR_X, "|");
    for (i = 0; i < TIMER_BAR_WIDTH - 2; i++) {
        if (i < bar_length) {
            mvaddch(TIMER_BAR_Y + 1, TIMER_BAR_X + 1 + i, '|');
        } else {
            mvaddch(TIMER_BAR_Y + 1, TIMER_BAR_X + 1 + i, ' ');
        }
    }
    mvprintw(TIMER_BAR_Y + 1, TIMER_BAR_X + TIMER_BAR_WIDTH - 1, "|");
    
    if (seconds_left > 20) {
        attroff(COLOR_PAIR(1));
    } else if (seconds_left > 10) {
        attroff(COLOR_PAIR(3));
    } else {
        attroff(COLOR_PAIR(4));
    }
    
    refresh();
}

// Inizializza la struttura del timer
void init_timer(info_timer* timer) {
    timer->seconds_left = 60;
    timer->last_update = time(NULL);
    timer->is_active = true;
}

// Aggiorna il timer e restituisce true se il tempo è scaduto
bool update_timer(info_timer* timer) {
    if (!timer->is_active) {
        return false;
    }
    
    time_t current_time = time(NULL);
    time_t elapsed = current_time - timer->last_update;
    
    // Aggiorna il timer solo se è passato almeno un secondo
    if (elapsed >= 1) {
        timer->seconds_left -= elapsed;
        timer->last_update = current_time;
        
        // Disegna la barra del tempo aggiornata
        draw_timer_bar(timer->seconds_left);
        
        // Controlla se il tempo è scaduto
        if (timer->seconds_left <= 0) {
            timer->seconds_left = 0;
            timer->is_active = false;
            return true;  // Tempo scaduto
        }
    }
    
    return false;  // Il tempo non è ancora scaduto
}

// Resetta il timer al valore iniziale
void reset_timer(info_timer* timer) {
    timer->seconds_left = TIMER_DURATION;
    timer->last_update = time(NULL);
    timer->is_active = true;
}

/**
 * Funzione che disegna i cuori rappresentanti le vite rimaste
 * @param vite numero di vite rimaste
 */
void draw_hearts(int vite) {
    int hearts_x = 36;  // Posizione x iniziale dei cuori
    int hearts_y = 40;  // Posizione y dei cuori
    int heart_spacing = 3;  // Spazio tra un cuore e l'altro 
    char* heart = "<3";  // Simbolo 
    

  
    // Disegno i cuori
    attron(COLOR_PAIR(4));  // Arancione pastello per le vite
    for (int i = 0; i < vite && i < 5; i++) {
        mvprintw(hearts_y, hearts_x + (i * heart_spacing),"<");
        mvprintw(hearts_y, hearts_x + (i * heart_spacing)+1,"3");
        mvprintw(hearts_y, hearts_x + (i * heart_spacing), "%s", heart);
    }

    attroff(COLOR_PAIR(4));
    
    
    // Mostro i cuori persi in grigio
    if (vite < 5) {
        attron(A_DIM);  // Attributo per rendere il testo meno luminoso
        for (int i = vite; i < 5; i++) {
            mvprintw(hearts_y, hearts_x + (i * heart_spacing), "%s", heart);
        }
        attroff(A_DIM);
    }
    
}


/**
 * Funzione che termina correttamente tutti i processi attivi del gioco
 * @param pipe_fd file descriptor della pipe in scrittura
 * @param croc_array array dei coccodrilli attivi
 * @param bullet_array array dei proiettili attivi
 * @param grenade_array array delle granate attive
 * @param frog struttura della rana
 */
void terminate_all_processes(int pipe_fd, Messaggio croc_array[NUM_CROC], 
                           Messaggio bullet_array[MAX_BULLETS], 
                           Messaggio grenade_array[MAX_GRENADE], 
                           Messaggio frog) {
    int status;
    int killed = 0;
    int waited = 0;
    
    // Chiudiamo la pipe per evitare che i processi continuino a scrivere
    close(pipe_fd);
    
    mvprintw(GAME_HEIGHT/2, GAME_WIDTH/2 - 10, "Caricamento del gioco in corso...");
    refresh();
    
    // Terminiamo tutti i processi coccodrillo
    for (int i = 0; i < NUM_CROC; i++) {
        if (croc_array[i].pid > 1) {  // Verifichiamo che sia un pid valido
            if (kill(croc_array[i].pid, SIGTERM) == 0) {
                killed++;
                // Aspettiamo un breve periodo per dare tempo al processo di terminare
                usleep(1000);
                // Usiamo waitpid con WNOHANG per non bloccare il processo principale
                if (waitpid(croc_array[i].pid, &status, WNOHANG) > 0) {
                    waited++;
                }
            }
        }
    }
    
    // Terminiamo tutti i proiettili attivi
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullet_array[i].is_active && bullet_array[i].pid > 1) {
            if (kill(bullet_array[i].pid, SIGTERM) == 0) {
                killed++;
                usleep(1000);
                if (waitpid(bullet_array[i].pid, &status, WNOHANG) > 0) {
                    waited++;
                }
            }
        }
    }
    
    // Terminiamo tutte le granate attive
    for (int i = 0; i < MAX_GRENADE; i++) {
        if (grenade_array[i].is_active && grenade_array[i].pid > 1) {
            if (kill(grenade_array[i].pid, SIGTERM) == 0) {
                killed++;
                usleep(1000);
                if (waitpid(grenade_array[i].pid, &status, WNOHANG) > 0) {
                    waited++;
                }
            }
        }
    }
    
    // Terminiamo il processo della rana
    if (frog.pid > 1) {
        if (kill(frog.pid, SIGTERM) == 0) {
            killed++;
            usleep(1000);
            if (waitpid(frog.pid, &status, WNOHANG) > 0) {
                waited++;
            }
        }
    }
    
    // Per i processi che non sono terminati con SIGTERM, inviamo SIGKILL
    for (int i = 0; i < NUM_CROC; i++) {
        if (croc_array[i].pid > 1 && kill(croc_array[i].pid, 0) == 0) {  // verifichiamo se il processo esiste ancora
            kill(croc_array[i].pid, SIGKILL);
            waitpid(croc_array[i].pid, NULL, 0);
        }
    }
    
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullet_array[i].is_active && bullet_array[i].pid > 1 && kill(bullet_array[i].pid, 0) == 0) {
            kill(bullet_array[i].pid, SIGKILL);
            waitpid(bullet_array[i].pid, NULL, 0);
        }
    }
    
    for (int i = 0; i < MAX_GRENADE; i++) {
        if (grenade_array[i].is_active && grenade_array[i].pid > 1 && kill(grenade_array[i].pid, 0) == 0) {
            kill(grenade_array[i].pid, SIGKILL);
            waitpid(grenade_array[i].pid, NULL, 0);
        }
    }
    
    if (frog.pid > 1 && kill(frog.pid, 0) == 0) {
        kill(frog.pid, SIGKILL);
        waitpid(frog.pid, NULL, 0);
    }
    
    // Puliamo qualsiasi processo zombie rimanente
    while (waitpid(-1, NULL, WNOHANG) > 0);
    sleep(1);  // Mostriamo il messaggio per un secondo
}


bool exit_game(int pipe_fd_write, int pipe_fd_read, 
              Messaggio crocs[NUM_CROC], 
              Messaggio bullets[MAX_BULLETS], 
              Messaggio grenades[MAX_GRENADE], 
              Messaggio frog, 
              const char* message) {
    int input = 0;
    bool flag = false;
    // Prima pulisci lo schermo
    clear();
    
    // Definisci l'ASCII art per "HAI PERSO"
    #define ALTEZZA_SPRITE 6
    wchar_t *sprite_sconfitta[ALTEZZA_SPRITE] = {
        L"██╗░░██╗░█████╗░██╗  ██████╗░███████╗██████╗░░██████╗░█████╗░██╗",
        L"██║░░██║██╔══██╗██║  ██╔══██╗██╔════╝██╔══██╗██╔════╝██╔══██╗██║",
        L"███████║███████║██║  ██████╔╝█████╗░░██████╔╝╚█████╗░██║░░██║██║",
        L"██╔══██║██╔══██║██║  ██╔═══╝░██╔══╝░░██╔══██╗░╚═══██╗██║░░██║╚═╝",
        L"██║░░██║██║░░██║██║  ██║░░░░░███████╗██║░░██║██████╔╝╚█████╔╝██╗",
        L"╚═╝░░╚═╝╚═╝░░╚═╝╚═╝  ╚═╝░░░░░╚══════╝╚═╝░░╚═╝╚═════╝░░╚════╝░╚═╝",
    };
    
    // Calcola la posizione centrale per il testo
    int start_y = (GAME_HEIGHT - ALTEZZA_SPRITE) / 2;
    int start_x = (GAME_WIDTH - wcslen(sprite_sconfitta[0])) / 2;
    
    // Imposta colorazione per il messaggio di Game Over
    attron(COLOR_PAIR(4));  // Rosso fluo per l'effetto drammatico
    
    // Stampa ogni riga dell'ASCII art
    for (int i = 0; i < ALTEZZA_SPRITE; i++) {
        mvaddwstr(start_y + i, start_x, sprite_sconfitta[i]);
    }
    
    // Aggiungi il messaggio specifico sotto l'ASCII art - CENTRATO CORRETTAMENTE
    mvprintw(start_y + ALTEZZA_SPRITE + 2, (GAME_WIDTH - strlen(message)) / 2, "%s", message);
    
    // Aggiungi istruzioni per uscire - CENTRATO CORRETTAMENTE
    char* exit_instruction = "Premi S per ricominciare o E per uscire";
    mvprintw(start_y + ALTEZZA_SPRITE + 4, (GAME_WIDTH - strlen(exit_instruction)) / 2, "%s", exit_instruction);
    
    attroff(COLOR_PAIR(4));
    
    // Aggiorna lo schermo e attendi l'input dell'utente
    refresh();
    timeout(-1); // Disabilita il timeout per attendere l'input dell'utente
    
    // Chiudi tutti i processi coccodrillo
    for (int i = 0; i < NUM_CROC; i++) {
        if (crocs[i].pid > 1) {
            kill(crocs[i].pid, SIGKILL);
            waitpid(crocs[i].pid, NULL, 0);
        }
    }
    
    // Chiudi tutti i proiettili
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].is_active && bullets[i].pid > 1) {
            kill(bullets[i].pid, SIGKILL);
            waitpid(bullets[i].pid, NULL, 0);
        }
    }
    
    // Chiudi tutte le granate
    for (int i = 0; i < MAX_GRENADE; i++) {
        if (grenades[i].is_active && grenades[i].pid > 1) {
            kill(grenades[i].pid, SIGKILL);
            waitpid(grenades[i].pid, NULL, 0);
        }
    }
    
    // Termina il processo della rana
    if (frog.pid > 1) {
        kill(frog.pid, SIGKILL);
        waitpid(frog.pid, NULL, 0);
    }
    
    do {
        input = getch();
    } while (input != 's' && input != 'e' && input != 'S' && input != 'E');

    if (input == 's' || input == 'S') {
        flag = true;
        clear();
        refresh();      
    } else if (input == 'e' || input == 'E') {
        flag = false;
        // Solo se si esce chiudiamo tutto
        close(pipe_fd_write);
        close(pipe_fd_read);
    }
    
    return flag;
}

bool victory(int pipe_fd_write, int pipe_fd_read, 
              Messaggio crocs[NUM_CROC], 
              Messaggio bullets[MAX_BULLETS], 
              Messaggio grenades[MAX_GRENADE], 
              Messaggio frog, 
              const char* message) {
    int input = 0;
    bool flag = false;
    
    // Prima pulisci lo schermo
    clear();
    
    // Definisci l'ASCII art per "HAI VINTO"
    #define ALTEZZA_SPRITE 6
    wchar_t *sprite_vittoria[ALTEZZA_SPRITE] = {
        L"██╗░░██╗░█████╗░██╗  ██╗░░░██╗██╗███╗░░██╗████████╗░█████╗░██╗",
        L"██║░░██║██╔══██╗██║  ██║░░░██║██║████╗░██║╚══██╔══╝██╔══██╗██║",
        L"███████║███████║██║  ╚██╗░██╔╝██║██╔██╗██║░░░██║░░░██║░░██║██║",
        L"██╔══██║██╔══██║██║  ░╚████╔╝░██║██║╚████║░░░██║░░░██║░░██║╚═╝",
        L"██║░░██║██║░░██║██║  ░░╚██╔╝░░██║██║░╚███║░░░██║░░░╚█████╔╝██╗",
        L"╚═╝░░╚═╝╚═╝░░╚═╝╚═╝  ░░░╚═╝░░░╚═╝╚═╝░░╚══╝░░░╚═╝░░░░╚════╝░╚═╝"
    };
    
    // Calcola la posizione centrale per il testo
    int start_y = (GAME_HEIGHT - ALTEZZA_SPRITE) / 2;
    int start_x = (GAME_WIDTH - wcslen(sprite_vittoria[0])) / 2;
    
    // Imposta colorazione per il messaggio di vittoria
    attron(COLOR_PAIR(1));  // Verde per l'effetto positivo
    
    // Stampa ogni riga dell'ASCII art
    for (int i = 0; i < ALTEZZA_SPRITE; i++) {
        mvaddwstr(start_y + i, start_x, sprite_vittoria[i]);
    }
    
    // Aggiungi il messaggio specifico sotto l'ASCII art - CENTRATO CORRETTAMENTE
    mvprintw(start_y + ALTEZZA_SPRITE + 2, (GAME_WIDTH - strlen(message)) / 2, "%s", message);
    
    // Aggiungi istruzioni per uscire - CENTRATO CORRETTAMENTE
    char* victory_instruction = "Premi S per rigiocare o E per uscire";
    mvprintw(start_y + ALTEZZA_SPRITE + 4, (GAME_WIDTH - strlen(victory_instruction)) / 2, "%s", victory_instruction);
    
    attroff(COLOR_PAIR(1));
    
    // Aggiorna lo schermo e attendi l'input dell'utente
    refresh();
    timeout(-1); // Disabilita il timeout per attendere l'input dell'utente
    
    // Chiudi tutti i processi coccodrillo
    for (int i = 0; i < NUM_CROC; i++) {
        if (crocs[i].pid > 1) {
            kill(crocs[i].pid, SIGKILL);
            waitpid(crocs[i].pid, NULL, 0);
        }
    }
    
    // Chiudi tutti i proiettili
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].is_active && bullets[i].pid > 1) {
            kill(bullets[i].pid, SIGKILL);
            waitpid(bullets[i].pid, NULL, 0);
        }
    }
    
    // Chiudi tutte le granate
    for (int i = 0; i < MAX_GRENADE; i++) {
        if (grenades[i].is_active && grenades[i].pid > 1) {
            kill(grenades[i].pid, SIGKILL);
            waitpid(grenades[i].pid, NULL, 0);
        }
    }
    
    // Termina il processo della rana
    if (frog.pid > 1) {
        kill(frog.pid, SIGKILL);
        waitpid(frog.pid, NULL, 0);
    }

    do {
        input = getch();
    } while (input != 's' && input != 'e' && input != 'S' && input != 'E');

    if (input == 's' || input == 'S') {
        flag = true;
        // Pulisci lo schermo e aggiorna per il riavvio
        clear();
        refresh();
    } else if (input == 'e' || input == 'E') {
        flag = false;
        // Solo se si esce chiudiamo tutto
        close(pipe_fd_write);
        close(pipe_fd_read);
    }
    
    return flag;
}


bool menu_iniziale() {
    bool flag;
    int input = 0;
    // Definisci l'ASCII art per "FROGGER"
    wchar_t *sprite_titolo[] = {
        L"███████╗██████╗░░█████╗░░██████╗░░██████╗░███████╗██████╗░██╗",
        L"██╔════╝██╔══██╗██╔══██╗██╔════╝░██╔════╝░██╔════╝██╔══██╗██║",
        L"█████╗░░██████╔╝██║░░██║██║░░██╗░██║░░██╗░█████╗░░██████╔╝██║",
        L"██╔══╝░░██╔══██╗██║░░██║██║░░╚██╗██║░░╚██╗██╔══╝░░██╔══██╗╚═╝",
        L"██║░░░░░██║░░██║╚█████╔╝╚██████╔╝╚██████╔╝███████╗██║░░██║██╗",
        L"╚═╝░░░░░╚═╝░░╚═╝░╚════╝░░╚═════╝░░╚═════╝░╚══════╝╚═╝░░╚═╝╚═╝"
    };
    
    // Definisci le regole del gioco
    char *regole[] = {
        "REGOLE DEL GIOCO:",
        "- Muoviti con le frecce direzionali (↑ ↓ ← →)",
        "- Premi p per mettere in pausa",
        "- Chiudi tutte le tane per vincere",
        "- Premi SPAZIO per sparare granate e difenderti dai proiettili",
        "- Perdi una vita quando:",
        "  * Vieni colpito da un proiettile",
        "  * Cadi in acqua",
        "  * Entri in una tana già chiusa",
        "- Hai 5 vite a disposizione",
        "- Completa tutte le manche per vincere!"
    };
    
    // Messaggio per iniziare/uscire (separato dall'array delle regole)
    char *messaggio_finale = "Premi S per iniziare... Premi E per uscire...";
    
    // Pulisci lo schermo
    clear();
    
    // Disegna un bordo attorno allo schermo
    box(stdscr, 0, 0);
    
    // Calcola la posizione centrale per il titolo
    int titolo_y = 5;
    int titolo_x = (GAME_WIDTH - wcslen(sprite_titolo[0])) / 2;
    
    // Imposta il colore per il titolo (verde fluo)
    attron(COLOR_PAIR(1));
    
    // Stampa ogni riga dell'ASCII art
    for (int i = 0; i < 6; i++) {
        mvaddwstr(titolo_y + i, titolo_x, sprite_titolo[i]);
    }
    attroff(COLOR_PAIR(1));
    
    // Calcola posizione per le regole
    int regole_y = titolo_y + 8;
    
    // Stampa il titolo delle regole in giallo
    attron(COLOR_PAIR(3));
    mvprintw(regole_y, (GAME_WIDTH - strlen(regole[0])) / 2, "%s", regole[0]);
    attroff(COLOR_PAIR(3));
    
    // Stampa le regole in ciano
    regole_y += 2;
    attron(COLOR_PAIR(8));  // Ciano per le regole
    for (int i = 1; i < 11; i++) {  // Stampa tutte le regole da 1 a 10
        mvprintw(regole_y + i - 1, (GAME_WIDTH - strlen(regole[i])) / 2, "%s", regole[i]);
    }
    attroff(COLOR_PAIR(8));
    
    // Posizionamento preciso per il messaggio finale
    // Calcola una posizione Y che sia DOPO tutte le regole
    int messaggio_y = regole_y + 12;  // Aumenta questo valore se necessario
    
    // Stampa il messaggio finale per iniziare in fucsia
    attron(COLOR_PAIR(4));  // Fucsia
    mvprintw(messaggio_y, (GAME_WIDTH - strlen(messaggio_finale)) / 2, "%s", messaggio_finale);
    attroff(COLOR_PAIR(4));
    
    // Aggiorna lo schermo
    refresh();
    
    // Disabilita il timeout per attendere l'input dell'utente
    timeout(-1);
    
    // Aspetta che l'utente prema un tasto
    do {
        input = getch();
    } while (input != 's' && input != 'e' && input != 'S' && input != 'E');

    if (input == 's' || input == 'S'){
        flag = true;
    } else if (input == 'e' || input == 'E'){
        flag = false;
    }
    
    // Pulisci lo schermo prima di iniziare il gioco
    clear();
    refresh();
    return flag;
}

/**
 * Funzione che mette in pausa tutti i processi del gioco
 * @param frog struct Messaggio della rana
 * @param crocs array di struct Messaggio dei coccodrilli
 * @param bullets array di struct Messaggio dei proiettili
 * @param grenades array di struct Messaggio delle granate
 */
void pause_game(Messaggio frog, Messaggio* crocs, Messaggio* bullets, Messaggio* grenades) {
    // Invia un segnale SIGSTOP alla rana se il pid è valido
    if (frog.pid > 1) {
        kill(frog.pid, SIGSTOP);
    }
    
    // Invia un segnale SIGSTOP a tutti i coccodrilli con pid valido
    for (int i = 0; i < NUM_CROC; i++) {
        if (crocs[i].pid > 1) {
            kill(crocs[i].pid, SIGSTOP);
        }
    }
    
    // Invia un segnale SIGSTOP a tutti i proiettili attivi con pid valido
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].is_active && bullets[i].pid > 1) {
            kill(bullets[i].pid, SIGSTOP);
        }
    }
    
    // Invia un segnale SIGSTOP a tutte le granate attive con pid valido
    for (int i = 0; i < MAX_GRENADE; i++) {
        if (grenades[i].is_active && grenades[i].pid > 1) {
            kill(grenades[i].pid, SIGSTOP);
        }
    }
    
    // Mostra messaggio di pausa
    attron(COLOR_PAIR(4) | A_BOLD);
    mvprintw(GAME_HEIGHT / 2, (GAME_WIDTH - 19) / 2, "GIOCO IN PAUSA");
    mvprintw(GAME_HEIGHT / 2 + 2, (GAME_WIDTH - 31) / 2, "Premi un tasto per continuare");
    attroff(COLOR_PAIR(4) | A_BOLD);
    refresh();
}

/**
 * Funzione che riprende tutti i processi del gioco dopo una pausa
 * @param frog struct Messaggio della rana
 * @param crocs array di struct Messaggio dei coccodrilli
 * @param bullets array di struct Messaggio dei proiettili
 * @param grenades array di struct Messaggio delle granate
 */
void resume_game(Messaggio frog, Messaggio* crocs, Messaggio* bullets, Messaggio* grenades) {
    // Prima di tutto, pulisci completamente lo schermo dalle vecchie posizioni
    clear();
    
    // Ridisegna l'area di gioco base
    box(stdscr, 0, 0);
    draw_burrows();
    draw_safety_zones();
    
    // Pulisci il messaggio di pausa
    int pause_y = GAME_HEIGHT / 2;
    int pause_x = (GAME_WIDTH - 19) / 2;
    for (int i = 0; i < 4; i++) {
        mvhline(pause_y + i, pause_x - 2, ' ', 35);
    }
    
    // Ridisegna tutti i coccodrilli nelle loro posizioni correnti
    for (int i = 0; i < NUM_CROC; i++) {
        if (crocs[i].pid > 1 && crocs[i].x >= 0 && crocs[i].y >= 0) {
            draw_crocodile(crocs[i].x, crocs[i].y);
        }
    }
    
    // Ridisegna tutti i proiettili attivi
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].is_active && bullets[i].x >= 0 && bullets[i].y >= 0) {
            draw_bullet(bullets[i].x, bullets[i].y);
        }
    }
    
    // Ridisegna tutte le granate attive
    for (int i = 0; i < MAX_GRENADE; i++) {
        if (grenades[i].is_active && grenades[i].x >= 0 && grenades[i].y >= 0) {
            draw_grenade(grenades[i].x, grenades[i].y);
        }
    }
    
    // Ridisegna la rana
    if (frog.x >= 0 && frog.y >= 0) {
        draw_frog(frog.x, frog.y);
    }
    
    // Aggiorna lo schermo prima di riprendere i processi
    refresh();
    
    // Ora riprendi tutti i processi
    // Invia un segnale SIGCONT alla rana se il pid è valido
    if (frog.pid > 1) {
        kill(frog.pid, SIGCONT);
    }
    
    // Invia un segnale SIGCONT a tutti i coccodrilli con pid valido
    for (int i = 0; i < NUM_CROC; i++) {
        if (crocs[i].pid > 1) {
            kill(crocs[i].pid, SIGCONT);
        }
    }
    
    // Invia un segnale SIGCONT a tutti i proiettili attivi con pid valido
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].is_active && bullets[i].pid > 1) {
            kill(bullets[i].pid, SIGCONT);
        }
    }
    
    // Invia un segnale SIGCONT a tutte le granate attive con pid valido
    for (int i = 0; i < MAX_GRENADE; i++) {
        if (grenades[i].is_active && grenades[i].pid > 1) {
            kill(grenades[i].pid, SIGCONT);
        }
    }
}