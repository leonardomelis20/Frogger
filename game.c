#include <stdio.h>
#include <curses.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <locale.h>

#include "rana.h"
#include "coccodrilli.h"
#include "strutture.h"
#include "disegni.h"
#include "collisioni.h"
#include "proiettili.h"
#include "granate.h"
#include "utility.h"
#include "gestione_partita.h"

void inizializza_schermo(); //per chiamare le funzioni ncurses


int speed[NUM_STREAMS];

int main(){
    
    srand(time(NULL));
    int pipe_fd[2];
    Messaggio coccodrilli[NUM_CROC];    //array di coccodrilli
    pid_t pid_rana, pid_coccodrillo[NUM_STREAMS*COCCODRILLI_X_FLUSSO], pid_bullet, pid_grenade_rx, pid_grenade_lx;
    Messaggio msg;
    int prev_x_rana = -1, prev_y_rana = -1; 
    int prev_x_cocc = -1, prev_y_cocc = -1;

    // Array di copie dei messaggi
    Messaggio frog_copy, croc_copy[NUM_CROC];
    Messaggio active_bullets[MAX_BULLETS];
    Messaggio active_grenades[MAX_GRENADE];
    
    
    
    int bullet_count = 0;  // Contatore dei proiettili attivi
    int grenade_count = 0; // Contatore delle granate attive
    
    int centro_y = MAP_HEIGHT - ALTEZZA_RANA;
    int centro_x = GAME_WIDTH / 2;
    int score = 0;

    TimerInfo game_timer;
    

    // Inizializza la rana
    frog_copy.oggetto = ID_RANA;
    frog_copy.x = centro_x;
    frog_copy.y = centro_y;
    frog_copy.on_croc = false;
    frog_copy.croc_index = -1;
    
    
    bool flag[NUM_BURROWS+1] = {false};
    bool flag_croc = true; //per i coccodrilli
    int direzione;
    int vite = 5;
    int status = 0;
    int i= 0; 
    int direzioni[NUM_CROC];
    int croc_index;
    int cont_bullets = 0;
    int count_burrows = 0;
    bool in_safe_zone = false;
    bool out_of_bounds = false;
    int new_x, new_y;
    int count[NUM_CROC];
    int manche = 1;
    

  
    
    
    // Aggiungi la variabile per tenere traccia del PID del timer
    
    
    // Avvia il timer
    //reset_timer(pipe_fd[WRITE], &pid_timer);
    
    inizializza_schermo();
    menu_iniziale();
   

    //getmaxyx(stdscr, y, x);
    box(stdscr, 0, 0); // Crea un bordo attorno alla finestra
    draw_burrows();
    draw_safety_zones();
    init_timer(&game_timer);
    draw_timer_bar(game_timer.seconds_left);
    //mvprintw(GAME_HEIGHT/2, GAME_WIDTH/2 - 10, "Caricamento in corso...");
    refresh();
    
    if (pipe(pipe_fd) == -1){
        perror ("Errore creazione pipe");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < NUM_STREAMS; i++) {
        switch (i) {
            case 0: speed[i] = 120000; break;
            case 1: speed[i] = 140000; break;
            case 2: speed[i] = 115000; break;
            case 3: speed[i] = 135000; break;
            case 4: speed[i] = 125000; break;
            case 5: speed[i] = 145000; break;
            case 6: speed[i] = 110000; break;
            case 7: speed[i] = 130000; break;
            case 8: speed[i] = 150000; break;
        }
    }
    for (int i = 0; i < NUM_CROC; i++)
    {
        if (i >= 9){
            speed[i] = speed[i-9];
        }   
    }
        
    // Inizializza l'array di proiettili
    for (int i = 0; i < MAX_BULLETS; i++) {
        active_bullets[i].oggetto = ID_BULLET;
        active_bullets[i].is_active = false;
        active_bullets[i].index = i;
        active_bullets[i].x = -100; // Posizione off screen
        active_bullets[i].y = -100;
        active_bullets[i].pid = -1;  // Nessun processo associato
    }


    // Inizializza l'array di granate
    for (int i = 0; i < MAX_GRENADE; i++) {
        active_grenades[i].oggetto = ID_GRENADE;
        active_grenades[i].is_active = false;
        active_grenades[i].index = i;
        active_grenades[i].x = -100; // Posizione off screen
        active_grenades[i].y = -100;
        active_grenades[i].pid = -1;  // Nessun processo associato
    }

    pid_rana = fork();
    if (pid_rana == -1){
        perror("Fork rana fallita");
        exit(EXIT_FAILURE);
    } else if (pid_rana == 0){
        close(pipe_fd[READ]);  //chiudo la pipe in lettura
        frog(pipe_fd[WRITE], flag, speed); //passo la pipe direttamente in scrittura
        exit(EXIT_SUCCESS);
    }

    

    
  
   
    if (rand() % 2 == 0){
        direzioni[0] = 1;
    } else {
        direzioni[0] = -1;
    }
    for (int i = 1; i < 9; i++) {
        direzioni[i] = -direzioni[i - 1];  // Alterna rispetto al precedente     
    }
    for (int i = 9; i < 18; i++){
        direzioni[i] = direzioni[i -9];
    }
     
        

    for(int i=0; i< NUM_CROC; i++){
        pid_coccodrillo[i]= fork();
        if (pid_coccodrillo[i] == -1){
            perror("Fork Coccodrillo fallita");
            exit(EXIT_FAILURE);
        } else if (pid_coccodrillo[i] == 0){
            close(pipe_fd[READ]);  //chiudo la pipe in lettura
            if (i >= 9){
                flag_croc = false;
            }
            main_croc(pipe_fd[WRITE], i, direzioni[i], speed[i], flag_croc); //passo la pipe direttamente in scritturap
            
            exit(EXIT_SUCCESS);
        }

    }

    for (int i = 0; i < NUM_CROC; i++) {
        croc_copy[i].index = i;  // Inizializzazione indici
        croc_copy[i].x = -100;   // Posizione off screen
        croc_copy[i].y = -100;
    }

    while (1) {
        // Leggi tutti i messaggi disponibili dalla pipe

        ssize_t r = read(pipe_fd[READ], &msg, sizeof(Messaggio));
        if (r == -1) {
            perror("Errore nella lettura dalla pipe");
            exit(EXIT_FAILURE);
        } else if (r == 0) {
            // Nessun messaggio disponibile
            continue;
        }

        correct_x_frog(frog_copy);

        //cancello la vecchia posizione della rana
        if (prev_x_rana != -1 && prev_y_rana != -1) {
            clear_frog(prev_x_rana, prev_y_rana);
        }


       draw_hearts(vite); // Disegna le vite
        attron(COLOR_PAIR(7));  // Celeste pastello per il punteggio
        mvprintw(40, 55, "PUNTI %d ", score);
        attroff(COLOR_PAIR(7));

        attron(COLOR_PAIR(8));  // Pesca pastello per le manche
        mvprintw(40, 70, "MANCHE %d ", manche);
        attroff(COLOR_PAIR(8));
        
        switch (msg.oggetto) {
            case ID_RANA:

            // Copia il messaggio
            // calcola nuova posizione
            new_x = frog_copy.x + msg.x;
            new_y = frog_copy.y + msg.y;

            // controlla se è nella safe zona
            in_safe_zone = (new_y >= 33 && new_y <= 39);

            //controllo boundaries
            out_of_bounds = (new_x < 0 || new_x >= GAME_WIDTH - LARGHEZZA_RANA || 
                                new_y < 0 || new_y >= MAP_HEIGHT - ALTEZZA_RANA);

            
            // aggiorno
            if (!out_of_bounds || in_safe_zone) {
                // se è dentro i bordi e nella safe zone muoviti
                if (in_safe_zone) {
                    // se è nella safe zone non muoverti in x
                    if (new_x >= 0 && new_x < GAME_WIDTH - LARGHEZZA_RANA) {
                        frog_copy.x = new_x;
                    }
                    
                    // controllo coordinate verticali nella safe zona
                    if (new_y >= 33 && new_y <= MAP_HEIGHT - ALTEZZA_RANA) {
                        frog_copy.y = new_y;
                    }
                } else {
                    //movimento normale fuori dalla safe zone
                    frog_copy.x = new_x;
                    frog_copy.y = new_y;
                }
            }

            // Verifica se la rana è su un coccodrillo e aggiorna le sue info
            frog_with_croc(&frog_copy, croc_copy);
            
            // Salva la nuova posizione per la prossima clear
            prev_x_rana = frog_copy.x;
            prev_y_rana = frog_copy.y;

            
            // Disegna la rana nella nuova posizione
            draw_frog(frog_copy.x, frog_copy.y);

            //se è fuori dallo schermo e fuori dalla safe zone perde vite e viene riposizionata
           /* if ((out_of_bounds && !in_safe_zone) || river(frog_copy)) {
                score += POINT_WATER;
                vite--;
                reset_timer(&game_timer);
                manche++;
                frog_copy.x = centro_x;
                frog_copy.y = centro_y;
                frog_copy.on_croc = false;
                frog_copy.croc_index = -1;
                if (vite <= 0) {
                    exit_game(pipe_fd[WRITE], pipe_fd[READ], croc_copy, active_bullets, active_grenades, frog_copy, "Hai perso tutte le vite. Game Over!", score);
                    terminate_all_processes(pipe_fd[WRITE], pipe_fd[READ], active_bullets, active_grenades, frog_copy); 
                   
    
                
                }
            }*/
            break;

        case ID_CROCODILE:
            //cancello
            clear_croc(msg);
            //aggiorno
            coccodrilli[msg.index] = msg;
            //disegno
            croc_copy[msg.index] = msg;
            croc_copy[msg.index].index = msg.index;
            prev_x_cocc = msg.x;
            prev_y_cocc = msg.y;


        
            //aggiorno la posizione della rana se è sopra il coccodrillo
            if (frog_copy.on_croc && frog_copy.croc_index == msg.index) {
                clear_frog(frog_copy.x, frog_copy.y); // cancella la posizione precedente
                
                //la rana si muove con il coccodrillo
                frog_copy.x += msg.direzione; // aggiorna la posizione verticale della rana

            
                // aggiorna anche le coordinate precedenti
                prev_x_rana = frog_copy.x;
                prev_y_rana = frog_copy.y;
            }
            // Se la rana era sopra questo coccodrillo ma ora non lo è più, azzera i flag

            
            draw_crocodile(msg.x, msg.y);
            
            if (!frog_copy.on_croc && 
                frog_copy.y >= msg.y && 
                frog_copy.y < msg.y + ALTEZZA_COCCODRILLO &&
                frog_copy.x + LARGHEZZA_RANA > msg.x && 
                frog_copy.x < msg.x + LARGHEZZA_COCCODRILLO) {
                
                frog_copy.on_croc = true;
                frog_copy.croc_index = msg.index;
                frog_copy.y = msg.y; 
                score += POINT_CROCODILE;
                
            } 
            
                       
            break;         
        case RESPAWN: 

        //log_coordinates(msg.x, msg.y, msg.direzione);
            int direzione =  0;
            direzione = msg.direzione;
            int indice = 0;
            indice = msg.index;
            int x = msg.x;
            kill(msg.pid, SIGKILL);
            waitpid(msg.pid, &status, 0); // aspetta che il processo muoia
            // Ricrea un nuovo processo coccodrillo con le stesse proprietà
            
            pid_t nuovo_pid = fork();
            flag_croc = true;
            if (nuovo_pid == -1 ){
                perror("errore fork ");
                exit(EXIT_FAILURE);
            } else if (nuovo_pid == 0) {
                close(pipe_fd[READ]);
                // nuovo processo figlio parte da capo
                main_croc(pipe_fd[WRITE], indice, direzione, speed[indice], flag_croc);
                exit(EXIT_SUCCESS);
            }
    
            
            break;    
       

// Modifica nella sezione CREATE_BULLET del main.c
// Questo codice rappresenta come dovrebbe essere implementato nel main.c

case CREATE_BULLET: {
    // Cerca una posizione libera nell'array dei proiettili
    int free_slot = -1;
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!active_bullets[i].is_active) {
            free_slot = i;
            break;
        }
    }
    
    if (free_slot != -1) {
        // Prima di creare un nuovo proiettile, verifica se il coccodrillo ha già un proiettile attivo
        bool already_has_bullet = false;
        for (int i = 0; i < MAX_BULLETS; i++) {
            if (active_bullets[i].is_active && active_bullets[i].index == msg.index) {
                already_has_bullet = true;
                break;
            }
        }
        
        // Se il coccodrillo non ha già un proiettile attivo, ne crea uno nuovo
        if (!already_has_bullet) {
            // Inizializza il proiettile
            active_bullets[free_slot].is_active = true;
            active_bullets[free_slot].x = msg.x;
            active_bullets[free_slot].y = msg.y;
            active_bullets[free_slot].direzione = msg.direzione;
            active_bullets[free_slot].oggetto = ID_BULLET;
            active_bullets[free_slot].index = msg.index;  // Indice del coccodrillo che ha sparato
            
            bullet_count++;
            
            // Crea il processo proiettile
            pid_bullet = fork();
            if (pid_bullet == -1) {
                perror("Fork proiettile fallita");
                exit(EXIT_FAILURE);
            } else if (pid_bullet == 0) {
                close(pipe_fd[READ]);
                main_bullet(pipe_fd[WRITE], msg);
                exit(EXIT_SUCCESS);
            }
            
            active_bullets[free_slot].pid = pid_bullet;
        }
    }
    break;
}

case ID_BULLET:
    // Aggiorna la posizione del proiettile nell'array
    
    if (msg.index >= 0 && msg.index < NUM_CROC) {
        int bullet_idx = -1;
        
        // Trova il proiettile con il pid corrispondente
        for (int i = 0; i < MAX_BULLETS; i++) {
            if (active_bullets[i].is_active && active_bullets[i].pid == msg.pid) {
                bullet_idx = i;
                break;
            }
        }
        
        if (bullet_idx != -1) {
            // Cancella vecchia posizione SOLO se la posizione è cambiata
        if (active_bullets[bullet_idx].x != msg.x || active_bullets[bullet_idx].y != msg.y) {
            clear_bullet(active_bullets[bullet_idx].x, active_bullets[bullet_idx].y);

            // Aggiorna la posizione del proiettile
            active_bullets[bullet_idx].x = msg.x;
            active_bullets[bullet_idx].y = msg.y;
            
            // Disegna il proiettile nella nuova posizione
            draw_bullet(msg.x, msg.y);
        }
            
            // Se il proiettile non è più attivo, rimuovilo e decrementare il contatore
            if (!msg.is_active) {
                active_bullets[bullet_idx].is_active = false;
                active_bullets[bullet_idx].x = -100;
                active_bullets[bullet_idx].y = -100;
                
                if (bullet_count > 0) {
                    bullet_count--;
                }
                
                // Termina il processo associato
                kill(msg.pid, SIGKILL);
                waitpid(msg.pid, &status, 0);
            } else {
                // Aggiorna la posizione del proiettile
                active_bullets[bullet_idx].x = msg.x;
                active_bullets[bullet_idx].y = msg.y;
                
                // Verifica se il proiettile è uscito dai bordi
                if (msg.x < 0 || msg.x >= GAME_WIDTH) {
                    // Il proiettile è uscito, uccidi il processo
                    kill(msg.pid, SIGKILL);
                    waitpid(msg.pid, &status, 0);
                    
                    // Marca il proiettile come inattivo
                    active_bullets[bullet_idx].is_active = false;
                    active_bullets[bullet_idx].x = -100;
                    active_bullets[bullet_idx].y = -100;
                    
                    // Decrementa il contatore dei proiettili attivi
                    if (bullet_count > 0) {
                        bullet_count--;
                    }
                } else {
                    // Disegna il proiettile nella nuova posizione
                    draw_bullet(msg.x, msg.y);
                    
                    // Controlla collisione con la rana
                    if (frog_copy.x < msg.x + 1 && frog_copy.x + LARGHEZZA_RANA > msg.x &&
                        frog_copy.y < msg.y + 1 && frog_copy.y + ALTEZZA_RANA > msg.y) {
                        // Collisione! La rana perde una vita
                        score += POINT_BULLETS;
                        vite--;
                        reset_timer(&game_timer);
                        manche++;
                        frog_copy.x = centro_x;
                        frog_copy.y = centro_y;
                        frog_copy.on_croc = false;
                        frog_copy.croc_index = -1;

                        // Disattiva il proiettile
                        kill(msg.pid, SIGKILL);
                        waitpid(msg.pid, &status, 0);
                        active_bullets[bullet_idx].is_active = false;
                        active_bullets[bullet_idx].x = -100;
                        active_bullets[bullet_idx].y = -100;
                        
                        // Decrementa il contatore dei proiettili attivi
                        if (bullet_count > 0) {
                            bullet_count--;
                        }
                        
                        if (vite <= 0) {
                            exit_game(pipe_fd[WRITE], pipe_fd[READ], croc_copy, active_bullets, active_grenades, frog_copy, "Hai perso tutte le vite. Game Over!");
                            
                            //terminate_all_processes(pipe_fd[WRITE], pipe_fd[READ], croc_copy, bulle, frog_copy);    
                           
                        }
                    }
                }
            }
        }
    }
    break;
           
case CREATE_GRENADE: {
    int free_slot = -1;
    // Cerchiamo due slot liberi consecutivi
    for (int i = 0; i < MAX_GRENADE - 1; i++) {
        if (!active_grenades[i].is_active && !active_grenades[i+1].is_active) {
            free_slot = i;
            break;
        }
    }
    
    if (free_slot != -1) {
        // Inizializza la granata sinistra
        active_grenades[free_slot].is_active = true;
        active_grenades[free_slot].x = frog_copy.x;
        active_grenades[free_slot].y = frog_copy.y + 1;
        active_grenades[free_slot].oggetto = ID_GRENADE;
        active_grenades[free_slot].direzione = -1;
        active_grenades[free_slot].index = free_slot;
        
        // Inizializza la granata destra
        active_grenades[free_slot+1].is_active = true;
        active_grenades[free_slot+1].x = frog_copy.x + 2;
        active_grenades[free_slot+1].y = frog_copy.y + 1;
        active_grenades[free_slot+1].oggetto = ID_GRENADE;
        active_grenades[free_slot+1].direzione = 1;
        active_grenades[free_slot+1].index = free_slot + 1;
        
        // Aggiorna il contatore delle granate attive
        grenade_count += 2;
        
        // Crea processo per granata sinistra
        pid_grenade_lx = fork();
        if (pid_grenade_lx == -1) {
            perror("Fork granata fallita");
            exit(EXIT_FAILURE);
        } else if (pid_grenade_lx == 0) {
            close(pipe_fd[READ]);
            msg.direzione = -1;
            msg.x = frog_copy.x;
            msg.y = frog_copy.y + 1;
            msg.index = free_slot;
            main_grenade(pipe_fd[WRITE], msg); 
            exit(EXIT_SUCCESS);
        }
        
        // Crea processo per granata destra
        pid_grenade_rx = fork();
        if (pid_grenade_rx == -1) {
            perror("Fork granata fallita");
            exit(EXIT_FAILURE);
        } else if (pid_grenade_rx == 0) {
            close(pipe_fd[READ]);
            msg.direzione = 1;
            msg.x = frog_copy.x + 2;
            msg.y = frog_copy.y + 1;
            msg.index = free_slot + 1;
            main_grenade(pipe_fd[WRITE], msg); 
            exit(EXIT_SUCCESS);
        }
        
        // Salva i PID dei processi
        active_grenades[free_slot].pid = pid_grenade_lx;
        active_grenades[free_slot+1].pid = pid_grenade_rx;
    }
    break;
}
          

case ID_GRENADE:
{
    // Trova la granata nell'array
    int grenade_idx = -1;
    for (int i = 0; i < MAX_GRENADE; i++) {
        if (active_grenades[i].pid == msg.pid) {
            grenade_idx = i;
            break;
        }
    }
    
    if (grenade_idx != -1) {
        // Cancella vecchia posizione SOLO se la posizione è cambiata
        if (active_grenades[grenade_idx].x != msg.x || active_grenades[grenade_idx].y != msg.y) {
            clear_grenade(active_grenades[grenade_idx].x, active_grenades[grenade_idx].y);
            
            // Aggiorna la posizione nell'array
            active_grenades[grenade_idx].x = msg.x;
            active_grenades[grenade_idx].y = msg.y;
            active_grenades[grenade_idx].is_active = true;
            
            // Disegna la granata nella nuova posizione
            draw_grenade(msg.x, msg.y);
        }
    }
    
    // Verifica se la granata è uscita dai bordi
    if (msg.x <= 0 || msg.x >= GAME_WIDTH) {
        // La granata è uscita, uccidi il processo
        kill(msg.pid, SIGKILL);
        waitpid(msg.pid, &status, 0);
        
        if (grenade_idx != -1) {
            // Assicurati che venga cancellata dallo schermo
            clear_grenade(active_grenades[grenade_idx].x, active_grenades[grenade_idx].y);
            
            active_grenades[grenade_idx].is_active = false;
            active_grenades[grenade_idx].x = -100;
            active_grenades[grenade_idx].y = -100;
            
            // Decrementa il contatore
            if (grenade_count > 0) {
                grenade_count--;
            }
        }
    }
    break;
}
} 
        //controlla se è dentro la tana oppure se entra in mezzo a due tane
        if (is_inside(frog_copy)){
            int num_tane = num_tana(frog_copy);
            //se la rana è dentro una tana
            if (flag[num_tane] == false && num_tane != 6){
                flag[num_tane] = true; //setto il flag a true per segnalare che non può più entrare in questa tana
                tane(frog_copy);  // Chiude graficamente la tana
                frog_copy.x = centro_x;
                frog_copy.y = centro_y;
                count_burrows++;
                score += POINT_BURROWS;
                if (count_burrows >= 5){
                   victory(pipe_fd[WRITE], pipe_fd[READ], croc_copy, active_bullets, active_grenades, frog_copy, "Hai vinto!");

                   //terminate_all_processes(pipe_fd[WRITE], pipe_fd[READ], active_bullets, active_grenades, frog_copy); 
                }
                reset_timer(&game_timer);
                
                // Respawna la rana
            } else{
                //se la tana è già occupata
                score += POINT_TAKEN_BURROWS;
                vite--;
                reset_timer(&game_timer);
                manche++;
                frog_copy.x = centro_x;
                frog_copy.y = centro_y;
                if (vite <= 0){
                  exit_game(pipe_fd[WRITE], pipe_fd[READ], croc_copy, active_bullets, active_grenades, frog_copy, "Hai perso tutte le vite. Game Over!");
                        //terminate_all_processes(pipe_fd[WRITE], pipe_fd[READ], active_bullets, active_grenades, frog_copy); 
                       
                }
                 refresh();
            }
        }        
       
        //disegno la rana
        draw_frog(frog_copy.x, frog_copy.y);
         draw_timer_bar(game_timer.seconds_left);

        
        // Disegna tutti i proiettili attivi
        for (int i = 0; i < MAX_BULLETS; i++) {
            if (active_bullets[i].is_active) {
                draw_bullet(active_bullets[i].x, active_bullets[i].y);
            }
        }
        for (int i = 0; i < MAX_GRENADE; i++) {
            if (active_grenades[i].is_active) {
                draw_grenade(active_grenades[i].x, active_grenades[i].y);
            }
        }

        if (collision_b_g(active_bullets, active_grenades, cont_bullets, grenade_count)){
            score += POINT_GRENADE;
        }


        if (update_timer(&game_timer)) {
    // Tempo scaduto
    score += POINT_TIME;
    vite--;
    frog_copy.x = centro_x;
    frog_copy.y = centro_y;
    frog_copy.on_croc = false;
    frog_copy.croc_index = -1;
    
    if (vite <= 0) {
        exit_game(pipe_fd[WRITE], pipe_fd[READ], croc_copy, active_bullets, active_grenades, frog_copy, "Hai perso tutte le vite. Game Over!");
             //terminate_all_processes(pipe_fd[WRITE], pipe_fd[READ], active_bullets, active_grenades, frog_copy); 
    }
    

    
    // Resetta il timer
    reset_timer(&game_timer);
    score += POINT_TIME;

}
 if (manche >= 5) {
    exit_game(pipe_fd[WRITE], pipe_fd[READ], croc_copy, active_bullets, active_grenades, frog_copy, "Hai perso tutte le vite. Game Over!");
    //terminate_all_processes(pipe_fd[WRITE], pipe_fd[READ], active_bullets, active_grenades, frog_copy); 
 }
    }       
    
    // Prima di uscire, assicuriamoci di chiudere tutti i processi
    terminate_all_processes(pipe_fd[WRITE], croc_copy, active_bullets, active_grenades, frog_copy);


    // Ora possiamo terminare ncurses
    endwin();

    printf("Gioco terminato con successo!\n");
    return 0;
}

void inizializza_schermo(){

    // Inizializza il supporto per i caratteri wide
    setlocale(LC_ALL, "");
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    timeout(100);
    curs_set(0);
    resize_term(GAME_HEIGHT, GAME_WIDTH);
    clear();

    start_color();
    
    // Verde fluo per la rana - RGB(0, 255, 0)
    init_color(1, 0, 1000, 0);
    init_pair(1, 1, COLOR_BLACK);
    
    // Blu elettrico per i coccodrilli - RGB(0, 140, 255)
    init_color(2, 0, 549, 1000);
    init_pair(2, 2, COLOR_BLACK);
    
    // Giallo fluo per i proiettili - RGB(255, 255, 0)
    init_color(3, 1000, 1000, 0);
    init_pair(3, 3, COLOR_BLACK);
    
    // Rosa fluo per le granate - RGB(255, 0, 255)
    init_color(4, 1000, 0, 1000);
    init_pair(4, 4, COLOR_BLACK);
    
    // Viola fluo per le tane - RGB(170, 0, 255)
    init_color(5, 667, 0, 1000);
    init_pair(5, 5, COLOR_BLACK);
    
    // Arancione fluo per le vite - RGB(255, 128, 0)
    init_color(6, 1000, 502, 0);
    init_pair(6, 6, COLOR_BLACK);
    
    // Azzurro fluo per il punteggio - RGB(0, 255, 255)
    init_color(7, 0, 1000, 1000);
    init_pair(7, 7, COLOR_BLACK);
    
    // Rosso fluo per le manche - RGB(255, 0, 0)
    init_color(8, 1000, 0, 0);
    init_pair(8, 8, COLOR_BLACK);
    
    // Colori per il fiume e aree di gioco
    init_color(9, 0, 1000, 700);  // Ciano fluo
    init_pair(9, 9, COLOR_BLACK);  // Per il fiume
    
    init_color(10, 800, 0, 1000);  // Viola-blu fluo
    init_pair(10, 10, COLOR_BLACK);  // Per le zone sicure
}



void log_coordinates(int pid, int frog_x, int crocodile_x, int direzione) {
    FILE *file = fopen("coordinates_log.txt", "a"); // Apri il file in modalità append
    if (file == NULL) {
        perror("Errore nell'apertura del file");
        return;
    }

    // Scrivi i valori nel file
    fprintf(file, "---------------\n");
     fprintf(file, "pid: %d\n",pid );
    fprintf(file, "x: %d\n",frog_x );
    fprintf(file, "y: %d\n",crocodile_x);
    fprintf(file, "velocita: %d\n",direzione);
    fprintf(file, "---------------\n");

    fclose(file); // Chiudi il file
}