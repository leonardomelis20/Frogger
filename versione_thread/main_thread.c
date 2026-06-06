#include <stdio.h>
#include <curses.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <locale.h> //libreria per i caratteri wide 
#include <signal.h>

#include "rana_thread.h"
#include "coccodrilli_thread.h"
#include "strutture_thread.h"
#include "buffer_thread.h"
#include "disegni_thread.h"
#include "collisioni_thread.h"
#include "proiettili_thread.h"
#include "granate_thread.h"
#include "gestione_partita_thread.h"
#include "game_handlers_thread.h"
#include "sprites_thread.h"

void inizializza_schermo();
void cleanup_game_threads(Game_data* game_data);
void reset_game_state(Game_data* game_data);

static pthread_t music_thread_id = 0;
static volatile bool music_should_stop = false; 

void* music_thread_func(void* arg) {
    /*ciclo per riavviare la musica quando finisce*/
    while (!music_should_stop) {
        system("ffplay -nodisp -autoexit Jacques.wav 2>/dev/null");
        
        if (music_should_stop) break;
    
        usleep(100000);
    }

    return NULL;
}

void start_background_music() {
    if (music_thread_id == 0 && !music_should_stop) {
        music_should_stop = false;
        
        if (pthread_create(&music_thread_id, NULL, music_thread_func, NULL) == 0) {
            pthread_detach(music_thread_id);
            printf("Thread musica avviato\n");
        } else {
            perror("Errore creazione thread musica");
            music_thread_id = 0;
        }
    }
}

void stop_background_music() {
    if (music_thread_id != 0) {
        music_should_stop = true;
        
        system("pkill -f 'ffplay.*Jacques.wav' 2>/dev/null");
        
        usleep(500000);
        
        music_thread_id = 0;
        printf("Musica fermata\n");
    }
}

pthread_mutex_t screen_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t pause_mutex = PTHREAD_MUTEX_INITIALIZER;
int speed[NUM_STREAMS];

int main() {
    srand(time(NULL));

    inizializza_schermo();
    
    Thread_args croc_args[NUM_CROC]; //argomenti coccodrilli
    int direzioni[NUM_CROC]; //array per le direzioni dei coccodrilli
    /*variabili per la barra del tempo*/
    int seconds_left = 0; 
    int bar_length = 0; 
    Messaggio msg; //messaggio
    /*variabili per le coordinate precedenti di rana e coccodrillo*/
    int prev_x_rana, prev_y_rana; 
    int prev_x_cocc, prev_y_cocc;
    int bullet_count, grenade_count; //variabili per tenere il conto di proiettili e granate attivi
    bool message_received; //falg per vedere se un messaggio è stato ricevuto
    int messages_processed; //variabile per sapere il numero di messaggio processati
    bool restart = menu_iniziale(); //variabile per riniziare la partita inizializzata alla funzione che si occupa delle indicazioni per l'inizio del gioco

    /*se l'utente non vuole inziare il gioco, esce e termina tutto*/
    if (!restart) {
        endwin();
        return 0;
    }
    
    system("ffplay -nodisp ./Jacques.wav 2> /dev/null &"); //per l'avvio della musica in background
    
    /*allochiamo e inizializziamo la struttura del gioco*/
    Game_data* game_data = malloc(sizeof(Game_data));
    if (!game_data) {
        perror("Errore allocazione Game_data");
        endwin();
        return 1;
    }
    
    init_game_data(game_data); //inizializziamo i campi di game data
    
    /*ciclo for che corre per il numero di flusso per inizializzare le velocità dei coccodrilli*/
    for (int i = 0; i < NUM_STREAMS; i++) {
        /*switch che si occupa, in base all'indice, di dare una velocità a quel flusso*/
        switch (i) {
            case 0: 
                speed[i] = 120000; 
                break;
            case 1: 
                speed[i] = 140000; 
                break;
            case 2: 
                speed[i] = 115000; 
                break;
            case 3: 
                speed[i] = 135000; 
                break;
            case 4: 
                speed[i] = 125000; 
                break;
            case 5: 
                speed[i] = 145000; 
                break;
            case 6: 
                speed[i] = 110000; 
                break;
            case 7: 
                speed[i] = 130000; 
                break;
            case 8: 
                speed[i] = 150000; 
                break;
        }
    }
    
    /*ciclo for che scorre per tutti i coccodrilli */
    for (int i = 0; i < NUM_CROC; i++) {
        /*per tutti i coccodrilli oltre l'indice 9, riusiamo la velocità*/
        if (i >= 9) {
            speed[i] = speed[i-9];
        }   
    }

    start_background_music(); 
    
    /*ciclo che continua fino a quando si preme s*/
    while (restart) {
        reset_game_state(game_data); //resettiamo lo stato del gioco
        
        
        /*inizializzazione dei thread degli oggetti*/
        pthread_t frog_tid;
        pthread_t croc_tids[NUM_CROC] = {0};
        pthread_t bullet_tids[MAX_BULLETS] = {0};
        pthread_t grenade_tids[MAX_GRENADE] = {0};
        
        Thread_args frog_args = {game_data->buffer, 0, 0, 0, true, game_data->flag, speed};
        
        /*inizializziamo le direzioni dei coccodrilli in modo alternato*/
        direzioni[0] = (rand() % 2 == 0) ? 1 : -1;
        for (int i = 1; i < 9; i++) {
            direzioni[i] = -direzioni[i - 1];
        }
        for (int i = 9; i < 18; i++) {
            direzioni[i] = direzioni[i - 9];
        }
        
        pthread_mutex_lock(&screen_mutex); //accesso esclusivo al mutex di ncurses
        /*settiamo lo schermo di gioco iniziale con le cose base */
        clear();
        box(stdscr, 0, 0);
        draw_burrows();
        draw_safety_zones();

        init_timer(&game_data->game_timer);
        seconds_left = game_data->game_timer.seconds_left;
        bar_length = (seconds_left * (TIMER_BAR_WIDTH - 2)) / TIMER_DURATION;
        attron(COLOR_PAIR(1));
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
        attroff(COLOR_PAIR(1));
        
        /*disegniamo la rana*/
        attron(COLOR_PAIR(1));
        for (int i = 0; i < ALTEZZA_RANA; i++) {
            mvprintw(game_data->frog_copy.y + i, game_data->frog_copy.x, "%s", spriteRana[i]);
        }
        attroff(COLOR_PAIR(1));
        
        refresh();

        pthread_mutex_unlock(&screen_mutex); //rilasciamo il mutex di ncurses
        
        
        /*creazione thread rana*/
        if (pthread_create(&frog_tid, NULL, frog_thread, &frog_args) == 0) {
            game_data->frog_copy.tid = frog_tid;
        }
        
        /*for per riempire l'array dei coccodrilli*/
        for (int i = 0; i < NUM_CROC; i++) {
            croc_args[i].buffer = game_data->buffer;
            croc_args[i].index = i;
            croc_args[i].direzione = direzioni[i];
            croc_args[i].velocita = speed[i];
            croc_args[i].flag = (i < 9) ? true : false;
            croc_args[i].game_flags = game_data->flag;
            croc_args[i].speed_array = speed;
            
            /*creiamo il thread dei coccodrilli*/
            if (pthread_create(&croc_tids[i], NULL, crocodile_thread, &croc_args[i]) == 0) {
                game_data->coccodrilli[i].tid = croc_tids[i];
            }
        }
        
        /*inizializziamo le variabili che serviranno per gli spostamenti*/
        prev_x_rana = -1, prev_y_rana = -1;
        prev_x_cocc = -1, prev_y_cocc = -1;
        bullet_count = 0, grenade_count = 0;
        
        /*ciclo principale dove avviene il gioco*/
        while (!game_data->game_over) {
            message_received = false;
        
            messages_processed = 0;

            /*prendiamo i messaggi dal buffer ed eseguiamo le funzioni corrispondenti*/
            while (buffer_try_get(game_data->buffer, &msg) && messages_processed < 50) {
                message_received = true;
                messages_processed++;
                
                switch (msg.oggetto) {
                    case ID_RANA:
                        handle_frog_message(game_data, &msg, &prev_x_rana, &prev_y_rana);
                        break;
                    case ID_CROCODILE:
                        handle_crocodile_message(game_data, &msg, &prev_x_cocc, &prev_y_cocc);
                        break;
                    case RESPAWN:
                        handle_respawn_message(game_data, &msg, croc_tids, croc_args, direzioni);
                        break;
                    case CREATE_BULLET:
                        handle_create_bullet(game_data, &msg, bullet_tids, &bullet_count);
                        break;
                    case ID_BULLET:
                        handle_bullet_message(game_data, &msg, bullet_tids, &bullet_count);
                        break;
                    case CREATE_GRENADE:
                        handle_create_grenade(game_data, &msg, grenade_tids, &grenade_count);
                        break;
                    case ID_GRENADE:
                        handle_grenade_message(game_data, &msg, grenade_tids, &grenade_count);
                        break;
                    case PAUSE:
                        handle_pause(game_data);
                        break;
                }
            }
    
            /*aggiorniamo il timer*/
            if (update_timer(&game_data->game_timer)) {
                handle_timeout(game_data);
                prev_x_rana = -1;
                prev_y_rana = -1;
            }
            
            /*controlliamo le collisioni*/
            pthread_mutex_lock(&game_data->bullets_mutex);
            pthread_mutex_lock(&game_data->grenades_mutex);
            if (collision_b_g(game_data->active_bullets, game_data->active_grenades, bullet_count, grenade_count)) {
                pthread_mutex_lock(&game_data->game_mutex);
                game_data->score += POINT_GRENADE;
                pthread_mutex_unlock(&game_data->game_mutex);
            }
            pthread_mutex_unlock(&game_data->grenades_mutex);
            pthread_mutex_unlock(&game_data->bullets_mutex);
            
            check_game_conditions(game_data); //controlliamo se si verificano la condizioni principali di gioco
            
            update_display(game_data); 
            
            if (message_received) {
                usleep(1000); 
            } else {
                usleep(5000); 
            }
        }
        
        cleanup_game_threads(game_data);//terminiamo i thread
        
        /*controlliamo le condizioni di fine gioco*/
        pthread_mutex_lock(&game_data->game_mutex);
        if (game_data->count_burrows >= NUM_BURROWS) {
            restart = victory(game_data);
        } else if (game_data->vite <= 0) {
            restart = exit_game(game_data);
        } else {
            restart = exit_game(game_data);
        }
        pthread_mutex_unlock(&game_data->game_mutex);
         stop_background_music();
    }
   
    
    /*puliamo tutti i mutex e liberiamo lo spazio*/
    destroy_game_data(game_data);
    free(game_data);
    
    endwin();
    printf("Gioco terminato con successo!\n");
    return 0;
}


/**
 * funzione che resetta lo stato del gioco
 * @param game_data puntatore alla struttura dati del gioco
 */
void reset_game_state(Game_data* game_data) {
    pthread_mutex_lock(&game_data->game_mutex);
    
    /*reset variabili di gioco*/
    game_data->game_over = false;
    game_data->paused = false;
    game_data->vite = 5;
    game_data->score = 0;
    game_data->manche = 1;
    game_data->count_burrows = 0;
    
    /*reset flag delle tane*/
    for (int i = 0; i <= NUM_BURROWS; i++) {
        game_data->flag[i] = false;
    }
    
    /*reset posizione rana*/
    int centro_y = MAP_HEIGHT - ALTEZZA_RANA;
    int centro_x = GAME_WIDTH / 2;
    game_data->frog_copy.x = centro_x;
    game_data->frog_copy.y = centro_y;
    game_data->frog_copy.on_croc = false;
    game_data->frog_copy.croc_index = -1;
    game_data->frog_copy.is_active = true;
    game_data->frog_copy.tid = 0;
    
    /*reset array coccodrilli*/
    for (int i = 0; i < NUM_CROC; i++) {
        game_data->coccodrilli[i].tid = 0;
        game_data->coccodrilli[i].is_active = false;
        game_data->coccodrilli[i].x = -100;
        game_data->coccodrilli[i].y = -100;
    }
    
    /*reset array proiettili*/
    for (int i = 0; i < MAX_BULLETS; i++) {
        game_data->active_bullets[i].tid = 0;
        game_data->active_bullets[i].is_active = false;
        game_data->active_bullets[i].x = -100;
        game_data->active_bullets[i].y = -100;
    }
    /*reset array granate*/
    for (int i = 0; i < MAX_GRENADE; i++) {
        game_data->active_grenades[i].tid = 0;
        game_data->active_grenades[i].is_active = false;
        game_data->active_grenades[i].x = -100;
        game_data->active_grenades[i].y = -100;
    }
    
    /*reset tane*/
    for (int i = 0; i < NUM_BURROWS; i++) {
        game_data->closed_burrows[i].index = -1;
    }
    
    pthread_mutex_unlock(&game_data->game_mutex);
}

void inizializza_schermo() {
    setlocale(LC_ALL, "");
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    timeout(1);
    curs_set(0);
    nodelay(stdscr, TRUE); 
    resize_term(GAME_HEIGHT, GAME_WIDTH);
    
    clear();

    start_color();
    
    /*verde fluo per la rana*/
    init_color(1, 51 * (1000 / 255), 255 * (1000 / 255), 0);
    init_pair(1, 1, COLOR_BLACK);
    
    /*verde scuro per i coccodrilli*/
    init_color(2, 0, 153 * (1000 / 255), 0);
    init_pair(2, 2, COLOR_BLACK);
    
    /*giallo fluo per i proiettili*/
    init_color(3, 255 * (1000 / 255), 255 * (1000 / 255), 0);
    init_pair(3, 3, COLOR_BLACK);
    
    /*rosso per le granate e vite*/
    init_color(4, 255 * (1000 / 255), 0, 0);
    init_pair(4, 4, COLOR_BLACK);
    
    /*marrone per le tane*/
    init_color(5, 153 * (1000 / 255), 76 * (1000 / 255), 0);
    init_pair(5, 5, COLOR_BLACK);
    
    /*arancione fluo per le manche*/
    init_color(6, 255 * (1000 / 255), 128 * (1000 / 255), 0);
    init_pair(6, 6, COLOR_BLACK);
    
    /*azzurro fluo per il punteggio*/
    init_color(7, 0, 255 * (1000 / 255), 255 * (1000 / 255));
    init_pair(7, 7, COLOR_BLACK);
    
    /*ciano fluo per le regole*/
    init_color(8, 0, 1000, 700);
    init_pair(8, 8, COLOR_BLACK);
    
    /*celestino fluo per le zone sicure*/
    init_color(9, 800, 0, 1000);
    init_pair(9, 9, COLOR_BLACK);
}