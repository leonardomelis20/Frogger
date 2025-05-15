
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


void kill_everything (int pipe_fd, Messaggio croc[NUM_CROC], Messaggio bullet[MAX_BULLETS], Messaggio grenade[MAX_GRENADE], Messaggio frog) {
    


    for (int i = 0; i < NUM_CROC; i++) {
        kill(croc[i].pid, SIGKILL);
       
        //waitpid(croc[i].pid, NULL, 0);
    }

    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (bullet[i].is_active) {
            kill(bullet[i].pid, SIGKILL);
            //waitpid(bullet[i].pid, NULL, 0);
        }
    }
    
    for (int i = 0; i < MAX_GRENADE; i++)
    {
        if (grenade[i].is_active) {
            kill(grenade[i].pid, SIGKILL);
            //waitpid(grenade[i].pid, NULL, 0);
        }
    }

    kill(frog.pid, SIGKILL);
    //waitpid(frog.pid, NULL, 0);

}