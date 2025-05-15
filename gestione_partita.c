
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
    
    // Ora disegna la barra del tempo
    
    
    // Scegli il colore in base al tempo rimanente
    if (seconds_left > 20) {
        attron(COLOR_PAIR(3));  // Verde
    } else if (seconds_left > 10) {
        attron(COLOR_PAIR(4));  // Giallo
    } else {
        attron(COLOR_PAIR(5));  // Rosso
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
    
    // Scrivi il tempo rimanente
    char time_str[10];
    //sprintf(time_str, "%2d sec", seconds_left);
    mvprintw(TIMER_BAR_Y + 1, TIMER_BAR_X + TIMER_BAR_WIDTH + 2, time_str);
    
    
    if (seconds_left > 20) {
        attroff(COLOR_PAIR(3));
    } else if (seconds_left > 10) {
        attroff(COLOR_PAIR(4));
    } else {
        attroff(COLOR_PAIR(5));
    }
    
    refresh();
}

// Inizializza la struttura del timer
void init_timer(TimerInfo* timer) {
    timer->seconds_left = TIMER_DURATION;
    timer->last_update = time(NULL);
    timer->is_active = true;
}

// Aggiorna il timer e restituisce true se il tempo è scaduto
bool update_timer(TimerInfo* timer) {
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
void reset_timer(TimerInfo* timer) {
    timer->seconds_left = TIMER_DURATION;
    timer->last_update = time(NULL);
    timer->is_active = true;
}

/**
 * Funzione che disegna i cuori rappresentanti le vite rimaste
 * @param vite numero di vite rimaste
 */
/**
 * Funzione che disegna i cuori rappresentanti le vite rimaste
 * @param vite numero di vite rimaste
 */
void draw_hearts(int vite) {
    int hearts_x = 36;  // Posizione x iniziale dei cuori
    int hearts_y = 40;  // Posizione y dei cuori
    int heart_spacing = 3;  // Spazio tra un cuore e l'altro 
    char* heart = "<3";  // Simbolo 
    
    // PPulisco l'area
    attron(COLOR_PAIR(0));  // Colore di default (bianco su nero)
    attroff(COLOR_PAIR(0));
  
    // Disegno i cuori
    attron(COLOR_PAIR(6));  // Arancione pastello per le vite
    for (int i = 0; i < vite && i < 5; i++) {
        mvprintw(hearts_y, hearts_x + (i * heart_spacing), "%s", heart);
    }
    attroff(COLOR_PAIR(6));
    
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
    
    mvprintw(GAME_HEIGHT/2, GAME_WIDTH/2 - 10, "Terminazione del gioco in corso...");
    refresh();
    
    // Terminiamo tutti i processi coccodrillo
    for (int i = 0; i < NUM_CROC; i++) {
        if (croc_array[i].pid > 1) {  // Verifichiamo che sia un PID valido
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
    
    mvprintw(GAME_HEIGHT/2 + 1, GAME_WIDTH/2 - 15, "Tutti i processi sono stati terminati correttamente");
    refresh();
    sleep(1);  // Mostriamo il messaggio per un secondo
}

/**
 * Funzione che gestisce in modo ordinato la terminazione del gioco,
 * pulisce i processi, chiude ncurses e visualizza un messaggio di uscita
 * @param message messaggio da visualizzare all'uscita
 */
/**
 * Funzione che gestisce in modo ordinato la terminazione del gioco
 * @param pipe_fd_write il file descriptor in scrittura della pipe
 * @param pipe_fd_read il file descriptor in lettura della pipe
 * @param crocs array dei coccodrilli
 * @param bullets array dei proiettili
 * @param grenades array delle granate
 * @param frog struttura della rana
 * @param message messaggio da visualizzare all'uscita
 */
void exit_game(int pipe_fd_write, int pipe_fd_read, 
              Messaggio crocs[NUM_CROC], 
              Messaggio bullets[MAX_BULLETS], 
              Messaggio grenades[MAX_GRENADE], 
              Messaggio frog, 
              const char* message) {
    
    // Prima pulisci lo schermo
    clear();
    
    // Definisci l'ASCII art per "HAI PERSO"
    #define ALTEZZA_SPRITE 6
    wchar_t *spriteSconfitta[ALTEZZA_SPRITE] = {
        L"██╗░░██╗░█████╗░██╗  ██████╗░███████╗██████╗░░██████╗░█████╗░██╗",
        L"██║░░██║██╔══██╗██║  ██╔══██╗██╔════╝██╔══██╗██╔════╝██╔══██╗██║",
        L"███████║███████║██║  ██████╔╝█████╗░░██████╔╝╚█████╗░██║░░██║██║",
        L"██╔══██║██╔══██║██║  ██╔═══╝░██╔══╝░░██╔══██╗░╚═══██╗██║░░██║╚═╝",
        L"██║░░██║██║░░██║██║  ██║░░░░░███████╗██║░░██║██████╔╝╚█████╔╝██╗",
        L"╚═╝░░╚═╝╚═╝░░╚═╝╚═╝  ╚═╝░░░░░╚══════╝╚═╝░░╚═╝╚═════╝░░╚════╝░╚═╝",
    };
    
    // Calcola la posizione centrale per il testo
    int start_y = (GAME_HEIGHT - ALTEZZA_SPRITE) / 2;
    int start_x = (GAME_WIDTH - wcslen(spriteSconfitta[0])) / 2;
    
    // Imposta colorazione per il messaggio di Game Over
    attron(COLOR_PAIR(4));  // Rosa corallo per l'effetto drammatico
    
    // Stampa ogni riga dell'ASCII art
    for (int i = 0; i < ALTEZZA_SPRITE; i++) {
        mvaddwstr(start_y + i, start_x, spriteSconfitta[i]);
    }
    
    // Aggiungi il messaggio specifico sotto l'ASCII art
    mvprintw(start_y + ALTEZZA_SPRITE + 2, (GAME_WIDTH - strlen(message)) / 2, "%s", message);
    
    // Aggiungi istruzioni per uscire
    mvprintw(start_y + ALTEZZA_SPRITE + 4, (GAME_WIDTH - 23) / 2, "Premi un tasto per uscire");
    
    attroff(COLOR_PAIR(4));
    
    // Aggiorna lo schermo e attendi l'input dell'utente
    refresh();
    sleep(4);
    getch();  // Attende che l'utente prema un tasto
    
    // Terminazione ordinata
    terminate_all_processes(pipe_fd_write, crocs, bullets, grenades, frog);
    
    // Chiudiamo la pipe
    close(pipe_fd_write);
    close(pipe_fd_read);
    
    // Chiudiamo ncurses
    endwin();
    
    // Mostriamo il messaggio finale (nella console, dopo aver chiuso ncurses)
    printf("Gioco terminato.\n");
    
    // Usciamo dal programma
    exit(EXIT_SUCCESS);
}