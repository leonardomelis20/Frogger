#include "gestione_partita.h" 


bool end_game (int vite, int count_tane, Messaggio rana, pid_t pid_frog, pid_t *pid_crocs, pid_t bullets, pid_t grenades)
{
   
}



/**
 * funzione principale per gestire il timer della partita
 * gestisce un conto alla rovescia che dà al giocatore 30 secondi per entrare in una tana
 * disegna una barra del tempo che rappresenta il tempo rimanente
 * @param pipe_write estremità di scrittura della pipe per inviare messaggi
 * @return 0
 */
int main_timer(int pipe_write) {
    int time_remaining = 30; // 30 secondi iniziali
    Messaggio msg;
    
    // Inizializza la struttura del messaggio
    msg.oggetto = ID_TIMER;
    
    while (time_remaining > 0) {
        // Invia il tempo rimanente al processo principale
        msg.tempo_rimanente = time_remaining;
        
        if (write(pipe_write, &msg, sizeof(Messaggio)) == -1) {
            perror("Errore scrittura pipe nel timer");
            exit(EXIT_FAILURE);
        }
        
        // Attendi 1 secondo
        sleep(1);
        
        // Decrementa il tempo rimanente
        time_remaining--;
    }
    
    // Invia un messaggio di timeout
    msg.oggetto = TIMER_TIMEOUT;
    msg.tempo_rimanente = 0;
    
    if (write(pipe_write, &msg, sizeof(Messaggio)) == -1) {
        perror("Errore scrittura pipe nel timer (timeout)");
        exit(EXIT_FAILURE);
    }
    
    return 0;
}

/**
 * funzione per disegnare la barra del tempo nella parte superiore dello schermo
 * visualizza una barra colorata che rappresenta il tempo rimanente
 * utilizza diversi colori in base al tempo rimanente: verde > 20s, giallo > 10s, rosso <= 10s
 * @param seconds i secondi rimanenti da visualizzare
 */
void draw_time_bar(int seconds) {
    int bar_width = 50; // Larghezza della barra in caratteri
    int filled = (seconds * bar_width) / 30; // Calcola quanti caratteri riempire
    int color_pair;
    int bar_x = 5; // Posizione X nella safe zone in basso a sinistra
    int bar_y = SAFE_ZONE_DOWN + 2; // Posizione Y nella safe zone in basso
    
   // Cancella la barra precedente
    mvhline(bar_y, bar_x, ' ', bar_width + 15);
    
    // Scegli il colore in base al tempo rimanente
    if (seconds > 20) {
        color_pair = 3; // Verde
    } else if (seconds > 10) {
        color_pair = 4; // Giallo
    } else {
        color_pair = 5; // Rosso
    }
    
    // Scrivi il testo del tempo
    mvprintw(bar_y, bar_x, "Tempo: ");
    
    // Disegna la barra
    attron(COLOR_PAIR(color_pair));
    for (int i = 0; i < bar_width; i++) {
        if (i < filled) {
            mvaddch(bar_y, bar_x + 7 + i, '|');
        } else {
            mvaddch(bar_y, bar_x + 7 + i, ' ');
        }
    }
    attroff(COLOR_PAIR(color_pair));
    
   // Aggiungi i secondi rimanenti alla fine della barra
    mvprintw(bar_y, bar_x + 7 + bar_width + 1, "%2ds", seconds);
    //refresh();
}

/**
 * funzione per resettare il timer creando un nuovo processo timer
 * termina il processo timer esistente e ne crea uno nuovo che parte da 30 secondi
 * @param pipe_fd estremità di scrittura della pipe per comunicare
 * @param pid_timer indirizzo del pid del timer corrente da aggiornare
 * @return 0 in caso di successo
 */
int reset_timer(int pipe_fd, pid_t *pid_timer) {
    // Se c'è un timer in esecuzione, terminalo
    if (*pid_timer > 0) {
        kill(*pid_timer, SIGKILL);
        int status;
        waitpid(*pid_timer, &status, 0);
    }
    
    // Crea un nuovo processo timer
    *pid_timer = fork();
    
    if (*pid_timer == -1) {
        perror("Fork timer fallita");
        exit(EXIT_FAILURE);
    } else if (*pid_timer == 0) {
        // Processo figlio timer
        close(pipe_fd); // Chiude lettura della pipe
        main_timer(pipe_fd);
        exit(EXIT_SUCCESS);
    }
    
    return 0;
}


/*bool finePartita(WINDOW *finestraGioco, Oggetto ranocchio, int vite, bool buffer, int punteggio,
                 int difficolta, Oggetto tempo, Oggetto macchina[], Oggetto camion[], Oggetto tronco[], bool *partitaInCorso, bool partitaFinita, bool audio)
{
     se anche solo una delle condizioni di fine della partita viene rispettata si entra nell'if, che poi stampa la scritta giusta in base a
     quale di queste condizioni è vera. viene poi chiesto se si vuole fare una nuova partita e in base al valore restituito si prosegue.
     i processi vengono killati indipendentemente da questo dato che vengono ricreati all'avvio della partita 
    int i;
    bool riniziaPartita;
    if (ranocchio.id == q || vite == 0 || buffer == false || partitaFinita)
    {
        (*partitaInCorso) = false;
        werase(finestraGioco);

        if (vite == 0 || partitaFinita)
            gameOver(finestraGioco, punteggio);

        else if (buffer == false)
            vittoria(finestraGioco, punteggio);

        werase(finestraGioco);
        wrefresh(finestraGioco);

        riniziaPartita = pausaeNuovaPartita(finestraGioco, 2);

        if (!riniziaPartita)
        {
            werase(finestraGioco);
            wrefresh(finestraGioco);
            schermataFinale(finestraGioco);
        }

        for (i = 0; i < NUMERO_MACCHINE; i++)
            kill(macchina[i].pid, SIGKILL);

        for (i = 0; i < NUMERO_CAMION; i++)
            kill(camion[i].pid, SIGKILL);

        for (i = 0; i < NUMERO_TRONCHI + difficolta; i++)
            kill(tronco[i].pid, SIGKILL);

        kill(tempo.pid, SIGKILL);

        kill(ranocchio.pid, SIGKILL);

        if (audio)
            system("killall ffplay");
    }

    return riniziaPartita;
}
*/
