// audio.c
#include "audio.h"
#include <stdlib.h>
#include <stdio.h>

void play_background_music(const char* music_file) {
    // Prima fermiamo qualsiasi musica che potrebbe essere in riproduzione
    //stop_background_music();
    
    // Costruiamo il comando per riprodurre la musica in background
    system("aplay -q ./Jacques.wav &> /dev/null &");
    
}

void stop_background_music() {
    // Fermiamo tutte le istanze di aplay in esecuzione
    system("pkill aplay");
}
