// audio.c
#include "audio.h"
#include <stdlib.h>
#include <stdio.h>

void play_background_music(const char* music_file) {
    // Prima fermiamo qualsiasi musica che potrebbe essere in riproduzione
    //stop_background_music();
    
    // Costruiamo il comando per riprodurre la musica in background
    system("ffplay -nodisp ./Jacques.wav 2> /dev/null &");
    
}

void stop_background_music() {
    // Fermiamo tutte le istanze di aplay in esecuzione
    system("pkill -9 ffplay");
}
