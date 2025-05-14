#include "utility.h"

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
    fprintf(file, "direzione: %d\n",direzione);
    fprintf(file, "---------------\n");

    fclose(file); // Chiudi il file
}